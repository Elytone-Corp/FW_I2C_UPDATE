/******************************************************************************
 * Copyright (C) 2023 Maxim Integrated Products, Inc., All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL MAXIM INTEGRATED BE LIABLE FOR ANY CLAIM, DAMAGES
 * OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * Except as contained in this notice, the name of Maxim Integrated
 * Products, Inc. shall not be used except as stated in the Maxim Integrated
 * Products, Inc. Branding Policy.
 *
 * The mere transfer of this software does not imply any licenses
 * of trade secrets, proprietary technology, copyrights, patents,
 * trademarks, maskwork rights, or any other form of intellectual
 * property whatsoever. Maxim Integrated Products, Inc. retains all
 * ownership rights.
 *
 ******************************************************************************/

#include <stdio.h>
#include "mxc_device.h"
#include "mxc_sys.h"
#include "mxc_errors.h"
#include "mxc_assert.h"
#include "board.h"
#include "uart.h"
#include "gpio.h"
#include "mxc_pins.h"
#include "stdbool.h"


/***** Global Variables *****/
mxc_uart_regs_t *ConsoleUart = MXC_UART_GET_UART(CONSOLE_UART);

/* Button identifiers */
enum
{
  LIGHT_BUTTON_1,
  LIGHT_BUTTON_MAX
};

/*! \brief      Button position. */
typedef enum
{
  BTN_POS_INVALID,          /*!< Button position is invalid. */
  BTN_POS_DOWN,             /*!< Button position is depressed. */
  BTN_POS_UP                /*!< Button position is released. */
} BtnPos_t;

/*! \brief      Operational states. */
typedef enum
{
  BTN_STATE_UNINIT = 0,     /*!< Uninitialized state. */
  BTN_STATE_ERROR  = 0,     /*!< Error state. */
  BTN_STATE_READY           /*!< Ready state. */
} BtnState_t;

/*! \brief      Action callback signature. */
typedef void (*BtnActionCback_t)(uint8_t btnId, BtnPos_t state);

/***** Globals *****/
/*! \brief      Device control block. */
struct {
  BtnActionCback_t actionCback;                           /*!< Action call back functions. */
  BtnState_t       state;                                 /*!< State of the buttons. */
} BtnCb;

//extern uint32_t SystemCoreClock;

#define BUTTON_PORT MXC_GPIO0
#define BUTTON_PIN MXC_GPIO_PIN_7

// clang-format off
const mxc_gpio_cfg_t pb_pin[] = { { MXC_GPIO0, MXC_GPIO_PIN_9 , MXC_GPIO_FUNC_IN,
                                    MXC_GPIO_PAD_PULL_DOWN, MXC_GPIO_VSSEL_VDDIOH, MXC_GPIO_DRVSTR_0} };
const unsigned int num_pbs = (sizeof(pb_pin) / sizeof(mxc_gpio_cfg_t));

const mxc_gpio_cfg_t led_pin[] = { { MXC_GPIO0, MXC_GPIO_PIN_13, MXC_GPIO_FUNC_OUT,
                                     MXC_GPIO_PAD_NONE, MXC_GPIO_VSSEL_VDDIO, MXC_GPIO_DRVSTR_0 } };
const unsigned int num_leds = (sizeof(led_pin) / sizeof(mxc_gpio_cfg_t));
// clang-format on

/***** File Scope Variables *****/
//static void TotalBtnCback(uint8_t btnId, BtnPos_t state);
/******************************************************************************/
void mxc_assert(const char *expr, const char *file, int line)
{
    printf("MXC_ASSERT %s #%d: (%s)\n", file, line, expr);
    while (1) {}
}

/******************************************************************************/
/** 
 * NOTE: This weak definition is included to support Push Button interrupts in
 *       case the user does not define this interrupt handler in their application.
 **/
__weak void GPIO0_IRQHandler(void)
{
    MXC_GPIO_Handler(MXC_GPIO_GET_IDX(MXC_GPIO0));
}

/******************************************************************************/
int Board_Init(void)
{
    int err;
#if 1 //debug on evb only
    if ((err = Console_Init()) < E_NO_ERROR) {
        return err;
    }
    printf("[SYS] UART Init... Done\n");
#endif
//    BtnInit(TotalBtnCback);
//#if 0
//    ir_basic_init();
//#endif
//    if ((err = GPIO_Init()) != E_NO_ERROR) {
//		MXC_ASSERT_FAIL();
//		return err;
//	}
//    printf("[SYS] GPIO Init... Done\n");
////    GPIO_Out_Low(0);
////    printf("[SYS] AW9523 RST...[LOW]\n");
//    GPIO_Out_Low(0);
//    printf("[SYS] NAP1 Enable...[LOW]\n");
//
//#if 1
//    if ((err = Peri_Init_I2C()) < E_NO_ERROR) {
//		//		printf("[I2C] I2C Init Error...!\n");
//		return err;
//	}
////    Peri_Scan_I2C_Bus();
//
//#endif
//#if 0
//    if ((err = LED_Init()) != E_NO_ERROR) {
//        MXC_ASSERT_FAIL();
//        return err;
//    }
//#endif
////    GPIO_Out_High(0);
////    printf("[SYS] AW9523 RST...[High]\n");
//    if ((err = app_AW9523_Init()) < E_NO_ERROR) {
//    	printf("[APP] AW9523 Init Error...!\n");
//    	return err;
//    }
//    printf("[APP] AW9523 Init...Done\n");
//
//    app_AW9523_LED_R();

    return E_NO_ERROR;
}

/******************************************************************************/
int Console_Init(void)
{
    int err;

    if ((err = MXC_UART_Init(ConsoleUart, CONSOLE_BAUD, MAP_C)) != E_NO_ERROR) {
        return err;
    }

    return E_NO_ERROR;
}

int Console_Shutdown(void)
{
    int err;

    if ((err = MXC_UART_Shutdown(ConsoleUart)) != E_NO_ERROR) {
        return err;
    }

    return E_NO_ERROR;
}


//extern SYS_MODE_T cur_pwr_status;
//extern SYS_MODE_T new_pwr_status;
uint8_t flag_last_sta = 0;

//void TotalBtnCback(uint8_t btnId, BtnPos_t state)
//{
//	if(flag_last_sta != state){
//		flag_last_sta = state;
//	}else{
//		return;
//	}
////	printf("S = %d\n",state);
//#if 1
//
//	if (state == BTN_POS_UP) {
////			printf("\nB-UP");
//
//	} else if (state == BTN_POS_DOWN) {
////		printf("\n pwr_sta: %d", cur_pwr_status);
//		if (cur_pwr_status == SYS_PWR_OFF) {
//			new_pwr_status = SYS_PWR_ON;
//		} else {
//			new_pwr_status = SYS_PWR_OFF;
//		}
//	}

//#endif
//	PB_IntClear(0);
//  /* Only alert application of button press and not release. */
//  if ((btnId < LIGHT_BUTTON_MAX) && (state == BTN_POS_DOWN))
//  {
//  }
//}

/*************************************************************************************************/
/*!
 *  \brief      Button callback handler.
 *
 *  \param      pb        gpio_cfg_t Pointer to the button that was pressed.
 *
 *  \return     None.
 *
 *  Figure out which button interrupted and if it's pressed or depressed.
 */
/*************************************************************************************************/
//void BtnCallback(void* pb)
//{
//  unsigned i;
//  int button = -1;
//  BtnPos_t btnState = BTN_POS_UP;
//
//  /* Figure out which button caused this interrupt */
//  for(i = 0; i < num_pbs; i++) {
//
//    /* This is the button */
//    if((void*)&pb_pin[i] == pb) {
//      button = (int)i;
//      if(PB_Get(i)) {
//        btnState = BTN_POS_DOWN;
////        _u32BTNPressingCnt = 3000;
////        _bSBTNPressingFinish = false;
//      } else {
//        btnState = BTN_POS_UP;
//      }
//      break;
//    }
//  }
//  //printf("BTN State:%d\n",btnState);
//
//  /* We didn't find the button */
//  if(button == -1) {
//    return;
//  }
//
//  /* App UI btn index starts at 1 */
//  button++;
//
//  /* Call the registered callback */
//  if(BtnCb.actionCback != NULL) {
//    BtnCb.actionCback((uint8_t)button, btnState);
//  }
//}

/*************************************************************************************************/
/*!
 *  \brief      Initialize buttons.
 *
 *  \param      actCback    Button event callback (called in ISR context).
 *
 *  \return     None.
 *
 *  Initialize buttons corresponding to set ordinal bit position in btnMask.
 */
/*************************************************************************************************/
//void BtnInit(BtnActionCback_t actCback)
//{
//  unsigned i;
//
//  PB_Init();
//
//  /* Initialize callback */
//  BtnCb.actionCback = actCback;
//
//  /* Register the callback for all of the buttons */
//  for(i = 0; i < num_pbs; i++) {
//    PB_RegisterCallbackRiseFall(i, BtnCallback);
//    PB_IntEnable(i);
//  }
//#if 0 //for button sleep mode
//  MXC_LP_EnableGPIOWakeup((mxc_gpio_cfg_t *)&pb_pin[0]);
//  MXC_GPIO_SetWakeEn(pb_pin[0].port, pb_pin[0].mask);
//#endif
//  printf("[SYS] Button Init... Done\n");
//  BtnCb.state = BTN_STATE_READY;
//}

/******************************************************************************/
void NMI_Handler(void)
{
    __NOP();
}
