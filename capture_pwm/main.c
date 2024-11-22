

//*********************************STABLE_CODE_STABLE_PWM_DETECT & CLASSIFICATION**************************************



//
//#include <stdio.h>
//#include "mxc_device.h"
//#include "gpio.h"
//#include "tmr.h"
//#include "nvic_table.h"
//#include "mxc_delay.h"
//#include "uart.h"
//
//#define PWM_INPUT_PIN      13  // This is P0_2, connected to the PWM signal
//#define TMR_IDX            0  // Use Timer 0
//
//volatile uint32_t risingEdge1 = 0, fallingEdge = 0, period = 0;
//volatile uint32_t highTime = 0;
//volatile float dutyCycle = 0.0;
//volatile int measurementReady = 0;
//volatile int currentClass = -1;
//
//void PWM_Handler(void* cbdata);
//
//void GPIO_Init(void) {
//    mxc_gpio_cfg_t gpio_cfg_pwm = {
//        .port = MXC_GPIO0,
//        .mask = (1 << PWM_INPUT_PIN),
//        .pad = MXC_GPIO_PAD_NONE,
//        .func = MXC_GPIO_FUNC_IN
//    };
//    MXC_GPIO_Config(&gpio_cfg_pwm);
//
//    // Register and enable interrupts
//    MXC_GPIO_RegisterCallback(&gpio_cfg_pwm, PWM_Handler, NULL);
//    MXC_GPIO_IntConfig(&gpio_cfg_pwm, MXC_GPIO_INT_BOTH);
//    MXC_GPIO_EnableInt(gpio_cfg_pwm.port, gpio_cfg_pwm.mask);
//
//    NVIC_EnableIRQ(MXC_GPIO_GET_IRQ(MXC_GPIO_GET_IDX(MXC_GPIO0)));
//}
//
//void Timer_Init(void) {
//    mxc_tmr_cfg_t tmr_cfg;
//
//    MXC_TMR_Stop(MXC_TMR_GET_TMR(TMR_IDX));
//
//    tmr_cfg.pres = MXC_TMR_PRES_1;
//    tmr_cfg.mode = MXC_TMR_MODE_CONTINUOUS;
//    tmr_cfg.cmp_cnt = 0;
//    tmr_cfg.pol = 0;
//
//    MXC_TMR_Init(MXC_TMR_GET_TMR(TMR_IDX), &tmr_cfg);
//    MXC_TMR_Start(MXC_TMR_GET_TMR(TMR_IDX));
//}
//
//void PWM_Handler(void* cbdata) {
//    uint32_t current_time = MXC_TMR_GetCount(MXC_TMR_GET_TMR(TMR_IDX));
//
//    if (MXC_GPIO_InGet(MXC_GPIO0, (1 << PWM_INPUT_PIN))) {
//        if (risingEdge1 == 0) {
//            risingEdge1 = current_time;
//        } else {
//            period = current_time - risingEdge1;
//            risingEdge1 = current_time;
//            measurementReady = 1;
//        }
//    } else {
//        fallingEdge = current_time;
//        highTime = fallingEdge - risingEdge1;
//    }
//}
//
//void Process_PWM(void) {
//    if (measurementReady) {
//        measurementReady = 0;
//
//        dutyCycle = ((float)highTime / period) * 100.0;
//
//        // Handle possible high duty cycles (near 90%)
//        if (dutyCycle > 95.0) {
//            dutyCycle = 90.0;
//        }
//
//        // Define ranges based on the new logic (I used 4 units)
//        const float requiredDutyCycleRanges[4][2] = {
//            {23.0, 28.0},  // 25% duty cycle range
//            {48.0, 53.0},  // 50% duty cycle range
//            {73.0, 78.0},  // 75% duty cycle range
//            {88.0, 92.0}   // 90% duty cycle range
//        };
//
//        // Classification logic
//        for (int i = 0; i < 4; i++) {
//            if (dutyCycle >= requiredDutyCycleRanges[i][0] && dutyCycle <= requiredDutyCycleRanges[i][1]) {
//                if (i != currentClass) {
//                    float classifiedDuty = (i + 1) * 22.5;
//                    if (i == 0) classifiedDuty = 25;
//                    if (i == 1) classifiedDuty = 50;
//                    if (i == 2) classifiedDuty = 75;
//                    if (i == 3) classifiedDuty = 90;
//
//                    //printf("Measured Duty Cycle: %.2f%%\n", dutyCycle);
//                    printf("----------  ");
//                    printf("Captured Duty Cycle =====>> %.0f%% duty cycle\n", classifiedDuty);
//                    currentClass = i;
//                }
//                break;
//            }
//        }
//    }
//}
//
//int main(void) {
//    GPIO_Init();
//    Timer_Init();
//
//    // Initial classification right after start
//    printf("STARING_____ AT :--> \n\t");
//    Process_PWM();
//
//    while (1) {
//        Process_PWM();
//        MXC_Delay(MXC_DELAY_MSEC(250));
//    }
//
//    return 0;
//}


//==================================================================================================================


#include <stdio.h>
#include <math.h>
#include "mxc_device.h"
#include "gpio.h"
#include "tmr.h"
#include "nvic_table.h"
#include "uart.h"
#include "mxc_delay.h"

#define PWM_INPUT_PIN      13  // This is P0_2, connected to the PWM signal
#define TMR_IDX            0   // Use Timer 0

#define BUFFER_SIZE        14  // Define the buffer size as 14
#define PATTERN_SIZE       12  // Size of the classification patterns

#define DUTY_25_LOWER      23.0f
#define DUTY_25_UPPER      28.0f
#define DUTY_50_LOWER      48.0f
#define DUTY_50_UPPER      53.0f
#define DUTY_75_LOWER      73.0f
#define DUTY_75_UPPER      78.0f
#define DUTY_90_LOWER      88.0f
#define DUTY_90_UPPER      92.0f

#define TOLERANCE          8   // 5% tolerance for duty cycle comparisons
#define DTW_THRESHOLD      150  // Adjusted threshold for DTW distance

#define NEW_CLASSES 4  // Number of new classes (Class 5 to Class 8)

volatile uint32_t risingEdge1 = 0, fallingEdge = 0, period = 0;
volatile uint32_t highTime = 0;
volatile float dutyCycle = 0.0;
volatile int measurementReady = 0;

volatile int buffer[BUFFER_SIZE] = {0};  // Sequential buffer with length 14
volatile int bufferIndex = 0;            // Index to track the current position in the buffer
volatile int bufferReady = 0;            // Flag to indicate when the buffer is full
volatile int detecting = 0;              // Flag to indicate if we're in the detecting phase

int class_patterns[NEW_CLASSES][PATTERN_SIZE] = {
    {25, 50, 50, 50, 90, 50, 75, 75, 25, 50, 50, 50}, // Class 5
    {25, 50, 50, 75, 90, 50, 75, 75, 25, 50, 50, 75}, // Class 6
    {25, 50, 75, 50, 90, 50, 75, 75, 25, 50, 75, 50}, // Class 7
    {25, 50, 75, 75, 90, 50, 75, 75, 25, 50, 75, 75}  // Class 8
};

void GPIO_Init(void);
void Timer_Init(void);
void PWM_Handler(void* cbdata);
void Process_PWM(void);
float calculate_dtw_distance(int seq1[], int len1, int seq2[], int len2);
void classify_and_reset_buffer(void);
void reset_buffer(void);

void GPIO_Init(void) {
    mxc_gpio_cfg_t gpio_cfg_pwm = {
        .port = MXC_GPIO0,
        .mask = (1 << PWM_INPUT_PIN),
        .pad = MXC_GPIO_PAD_NONE,
        .func = MXC_GPIO_FUNC_IN
    };
    MXC_GPIO_Config(&gpio_cfg_pwm);

    // Register and enable interrupts
    MXC_GPIO_RegisterCallback(&gpio_cfg_pwm, PWM_Handler, NULL);
    MXC_GPIO_IntConfig(&gpio_cfg_pwm, MXC_GPIO_INT_BOTH);
    MXC_GPIO_EnableInt(gpio_cfg_pwm.port, gpio_cfg_pwm.mask);

    NVIC_EnableIRQ(MXC_GPIO_GET_IRQ(MXC_GPIO_GET_IDX(MXC_GPIO0)));
}

void Timer_Init(void) {
    mxc_tmr_cfg_t tmr_cfg;

    MXC_TMR_Stop(MXC_TMR_GET_TMR(TMR_IDX));

    tmr_cfg.pres = MXC_TMR_PRES_1; // No prescaler
    tmr_cfg.mode = MXC_TMR_MODE_CONTINUOUS;
    tmr_cfg.cmp_cnt = 0;
    tmr_cfg.pol = 0;

    MXC_TMR_Init(MXC_TMR_GET_TMR(TMR_IDX), &tmr_cfg);
    MXC_TMR_Start(MXC_TMR_GET_TMR(TMR_IDX));
}


void PWM_Handler(void* cbdata) {
    uint32_t current_time = MXC_TMR_GetCount(MXC_TMR_GET_TMR(TMR_IDX));

    if (MXC_GPIO_InGet(MXC_GPIO0, (1 << PWM_INPUT_PIN))) {
        if (risingEdge1 == 0) {
            risingEdge1 = current_time;
        } else {
            period = current_time - risingEdge1;
            risingEdge1 = current_time;
            measurementReady = 1;
        }
    } else {
        fallingEdge = current_time;
        highTime = fallingEdge - risingEdge1;
    }
}


void Process_PWM(void) {
    if (measurementReady && !bufferReady) {
        measurementReady = 0;

        dutyCycle = ((float)highTime / period) * 100.0;

        // Handle possible high duty cycles (near 90%)
        if (dutyCycle > 95.0) {
            dutyCycle = 90.0;
        }

        if (dutyCycle >= DUTY_90_LOWER - TOLERANCE && dutyCycle <= DUTY_90_UPPER + TOLERANCE && !detecting) {
            detecting = 1;
            buffer[0] = 90;
            bufferIndex = 1;
        }

        if (detecting && bufferIndex < BUFFER_SIZE) {
            if (dutyCycle >= DUTY_25_LOWER - TOLERANCE && dutyCycle <= DUTY_25_UPPER + TOLERANCE) {
                buffer[bufferIndex++] = 25;
            } else if (dutyCycle >= DUTY_50_LOWER - TOLERANCE && dutyCycle <= DUTY_50_UPPER + TOLERANCE) {
                buffer[bufferIndex++] = 50;
            } else if (dutyCycle >= DUTY_75_LOWER - TOLERANCE && dutyCycle <= DUTY_75_UPPER + TOLERANCE) {
                buffer[bufferIndex++] = 75;
            }

            if (bufferIndex == BUFFER_SIZE) {
                bufferReady = 1;
            }
        }
    }
}

float calculate_dtw_distance(int seq1[], int len1, int seq2[], int len2) {
    float dtw[len1 + 1][len2 + 1];
    const float infinity = 1e10f;

    // Initialize DTW matrix
    for (int i = 0; i <= len1; i++) {
        for (int j = 0; j <= len2; j++) {
            dtw[i][j] = infinity;
        }
    }
    dtw[0][0] = 0;

    // Compute DTW
    for (int i = 1; i <= len1; i++) {
        for (int j = 1; j <= len2; j++) {
            float cost = fabs(seq1[i - 1] - seq2[j - 1]);
            dtw[i][j] = cost + fminf(dtw[i - 1][j], fminf(dtw[i][j - 1], dtw[i - 1][j - 1]));
        }
    }

    return dtw[len1][len2];
}

void classify_and_reset_buffer() {
    if (!bufferReady) {
        return;
    }

    // Print the buffer content
    printf("Buffer content: ");
    for (int i = 0; i < BUFFER_SIZE; i++) {
        printf("%d%% ", buffer[i]);
    }
    printf("\n");

    // Count occurrences of each duty cycle type
    int count_25 = 0, count_50 = 0, count_75 = 0, count_90 = 0;
    for (int i = 0; i < BUFFER_SIZE; i++) {
        if (buffer[i] == 25) count_25++;
        if (buffer[i] == 50) count_50++;
        if (buffer[i] == 75) count_75++;
        if (buffer[i] == 90) count_90++;
    }

    // Class 4 Detection
    if (buffer[0] == 90 && buffer[9] == 75 && count_25 >= 8) {
        printf("\t--------Classified as Class 4--------\n");
        reset_buffer();
        return;
    }

    // Position-based DTW for other classes
    float min_dtw_distance = 1e10f;
    int matched_class = -1;

    for (int i = 0; i < NEW_CLASSES; i++) {
        float dtw_distance = calculate_dtw_distance(buffer, BUFFER_SIZE, class_patterns[i], PATTERN_SIZE);
        printf("DTW Distance to Class %d: %.2f\n", i + 5, dtw_distance);

        if (dtw_distance < min_dtw_distance) {
            min_dtw_distance = dtw_distance;
            matched_class = i + 5;
        }
    }

    // Apply classification based on the closest DTW distance
    if (min_dtw_distance < DTW_THRESHOLD) {
        printf("\t--------Classified as Class %d--------\n", matched_class);
    } else {
        printf("\t*******No matching class detected.**********\n");
    }

    reset_buffer();
}

void reset_buffer() {
    for (int i = 0; i < BUFFER_SIZE; i++) {
        buffer[i] = 0;
    }
    bufferIndex = 0;
    bufferReady = 0;
    detecting = 0;
}

int main(void) {
    GPIO_Init();
    Timer_Init();

    printf("Starting PWM Capture...\n");

    while (1) {
        if (bufferReady) {
            classify_and_reset_buffer();
        }

        Process_PWM();
        MXC_Delay(MXC_DELAY_USEC(5)); // Adjust delay to match duty cycle period
    }

    return 0;
}



//==============================================================================================================

//#include <stdio.h>
//#include <math.h>
//#include "mxc_device.h"
//#include "gpio.h"
//#include "tmr.h"
//#include "nvic_table.h"
//#include "uart.h"
//#include "board.h"
//#include "mxc_delay.h"
//
//#define PWM_INPUT_PIN      13  // P0_13 connected to the PWM signal
//#define TMR_IDX            0   // Use Timer 0
//
//#define BUFFER_SIZE        14  // Define the buffer size as 14
//#define PATTERN_SIZE       12  // Size of the new classification patterns
//
//// Define thresholds with slightly wider ranges to accommodate signal variations
//#define DUTY_25_LOWER      23.0f
//#define DUTY_25_UPPER      28.0f
//#define DUTY_50_LOWER      48.0f
//#define DUTY_50_UPPER      53.0f
//#define DUTY_75_LOWER      73.0f
//#define DUTY_75_UPPER      78.0f
//#define DUTY_90_LOWER      88.0f
//#define DUTY_90_UPPER      92.0f
//
//// Define a tolerance value to allow for flexibility in comparisons
//#define TOLERANCE          5   // 5% tolerance for duty cycle comparisons
//
//volatile int buffer[BUFFER_SIZE] = {0};  // Sequential buffer with length 14
//volatile int bufferIndex = 0;            // Index to track the current position in the buffer
//volatile int bufferReady = 0;            // Flag to indicate when the buffer is full
//volatile int detecting = 0;              // Flag to indicate if we're in the detecting phase
//
//volatile uint32_t risingEdge1 = 0, fallingEdge = 0, period = 0;
//volatile uint32_t highTime = 0;
//volatile float dutyCycle = 0.0;
//volatile int measurementReady = 0;
//
//void PWM_Handler(void* cbdata);
//
//void GPIO_Init(void) {
//    mxc_gpio_cfg_t gpio_cfg_pwm = {
//        .port = MXC_GPIO0,
//        .mask = (1 << PWM_INPUT_PIN),
//        .pad = MXC_GPIO_PAD_NONE,
//        .func = MXC_GPIO_FUNC_IN
//    };
//    MXC_GPIO_Config(&gpio_cfg_pwm);
//
//    // Register and enable interrupts
//    MXC_GPIO_RegisterCallback(&gpio_cfg_pwm, PWM_Handler, NULL);
//    MXC_GPIO_IntConfig(&gpio_cfg_pwm, MXC_GPIO_INT_BOTH);
//    MXC_GPIO_EnableInt(gpio_cfg_pwm.port, gpio_cfg_pwm.mask);
//
//    NVIC_EnableIRQ(MXC_GPIO_GET_IRQ(MXC_GPIO_GET_IDX(MXC_GPIO0)));
//}
//
//void Timer_Init(void) {
//    mxc_tmr_cfg_t tmr_cfg;
//
//    MXC_TMR_Stop(MXC_TMR_GET_TMR(TMR_IDX));
//
//    tmr_cfg.pres = MXC_TMR_PRES_1;
//    tmr_cfg.mode = MXC_TMR_MODE_CONTINUOUS;
//    tmr_cfg.cmp_cnt = 0;
//    tmr_cfg.pol = 0;
//
//    MXC_TMR_Init(MXC_TMR_GET_TMR(TMR_IDX), &tmr_cfg);
//    MXC_TMR_Start(MXC_TMR_GET_TMR(TMR_IDX));
//}
//
//void PWM_Handler(void* cbdata) {
//    uint32_t current_time = MXC_TMR_GetCount(MXC_TMR_GET_TMR(TMR_IDX));
//
//    if (MXC_GPIO_InGet(MXC_GPIO0, (1 << PWM_INPUT_PIN))) {
//        if (risingEdge1 == 0) {
//            risingEdge1 = current_time;
//        } else {
//            period = current_time - risingEdge1;
//            risingEdge1 = current_time;
//            measurementReady = 1;
//        }
//    } else {
//        fallingEdge = current_time;
//        highTime = fallingEdge - risingEdge1;
//    }
//}
//
//// Function to calculate the DTW distance with weighted segments
//double calculate_weighted_dtw(int *sequence1, int *sequence2, int length1, int length2, double *weights) {
//    double dtw[length1+1][length2+1];
//
//    // Initialize the DTW array
//    for (int i = 0; i <= length1; i++) {
//        for (int j = 0; j <= length2; j++) {
//            dtw[i][j] = INFINITY;
//        }
//    }
//    dtw[0][0] = 0;
//
//    // Calculate the DTW distance with weights
//    for (int i = 1; i <= length1; i++) {
//        for (int j = 1; j <= length2; j++) {
//            double cost = pow((sequence1[i-1] - sequence2[j-1]), 2) * weights[i-1]; // Squared difference with weight
//            dtw[i][j] = cost + fmin(fmin(dtw[i-1][j],    // Insertion
//                                         dtw[i][j-1]),   // Deletion
//                                         dtw[i-1][j-1]); // Match
//        }
//    }
//
//    return dtw[length1][length2];
//}
//
//// Function to filter out the ACK pattern (50, 75, 75)
//void filter_ack_pattern() {
//    // Look for the ACK pattern (50, 75, 75) in the buffer
//    for (int i = 0; i < BUFFER_SIZE - 2; i++) {
//        if (buffer[i] == 50 && buffer[i+1] == 75 && buffer[i+2] == 75) {
//            // Shift the remaining elements left by 3 positions
//            for (int j = i; j < BUFFER_SIZE - 3; j++) {
//                buffer[j] = buffer[j + 3];
//            }
//            // Clear the last three positions
//            buffer[BUFFER_SIZE - 3] = 0;
//            buffer[BUFFER_SIZE - 2] = 0;
//            buffer[BUFFER_SIZE - 1] = 0;
//            break;
//        }
//    }
//}
//
//// Function to classify the buffer using segmented DTW with debug output
//void classify_and_reset_buffer() {
//    // Filter the ACK pattern from the buffer
//    filter_ack_pattern();
//
//    // Predefined patterns for different classes (without ACK pattern)
//    int class4_pattern[PATTERN_SIZE] = {25, 25, 25, 25, 90, 50, 75, 75, 25, 25, 25, 25};
//    int class5_pattern[PATTERN_SIZE] = {25, 50, 50, 50, 90, 50, 75, 75, 25, 50, 50, 50};
//    int class6_pattern[PATTERN_SIZE] = {25, 50, 50, 75, 90, 50, 75, 75, 25, 50, 50, 75};
//    int class7_pattern[PATTERN_SIZE] = {25, 50, 75, 50, 90, 50, 75, 75, 25, 50, 75, 50};
//    int class8_pattern[PATTERN_SIZE] = {25, 50, 75, 75, 90, 50, 75, 75, 25, 50, 75, 75};
//
//    // Weights focusing on critical differences
//    double weights[PATTERN_SIZE] = {1.0, 1.0, 1.2, 1.5, 2.0, 1.5, 1.5, 1.2, 1.0, 1.0, 1.2, 1.5};
//
//    // Calculate weighted DTW for the buffer
//    double dtw_class4 = calculate_weighted_dtw(buffer, class4_pattern, PATTERN_SIZE, PATTERN_SIZE, weights);
//    double dtw_class5 = calculate_weighted_dtw(buffer, class5_pattern, PATTERN_SIZE, PATTERN_SIZE, weights);
//    double dtw_class6 = calculate_weighted_dtw(buffer, class6_pattern, PATTERN_SIZE, PATTERN_SIZE, weights);
//    double dtw_class7 = calculate_weighted_dtw(buffer, class7_pattern, PATTERN_SIZE, PATTERN_SIZE, weights);
//    double dtw_class8 = calculate_weighted_dtw(buffer, class8_pattern, PATTERN_SIZE, PATTERN_SIZE, weights);
//
//    // Debugging: Print the DTW distances
//    printf("DTW Distances: Class 4: %.2f, Class 5: %.2f, Class 6: %.2f, Class 7: %.2f, Class 8: %.2f\n",
//           dtw_class4, dtw_class5, dtw_class6, dtw_class7, dtw_class8);
//
//    // Determine the class with the minimum DTW distance
//    double min_dtw = dtw_class4;
//    int detectedClass = 4;
//
//    if (dtw_class5 < min_dtw) {
//        min_dtw = dtw_class5;
//        detectedClass = 5;
//    }
//    if (dtw_class6 < min_dtw) {
//        min_dtw = dtw_class6;
//        detectedClass = 6;
//    }
//    if (dtw_class7 < min_dtw) {
//        min_dtw = dtw_class7;
//        detectedClass = 7;
//    }
//    if (dtw_class8 < min_dtw) {
//        min_dtw = dtw_class8;
//        detectedClass = 8;
//    }
//
//    // Output the result
//    printf("\t--------Classified as Class %d--------\n", detectedClass);
//
//    // Print the buffer for debugging
//    printf("Filtered Buffer contents: ");
//    for (int i = 0; i < BUFFER_SIZE; i++) {
//        printf("%d ", buffer[i]);
//    }
//    printf("\n");
//
//    // Reset the buffer and index for the next window
//    for (int i = 0; i < BUFFER_SIZE; i++) {
//        buffer[i] = 0;
//    }
//    bufferIndex = 0;
//    bufferReady = 0;  // Reset the flag
//    detecting = 0;    // Go back to waiting for the next trigger
//}
//
//void Process_PWM(void) {
//    if (measurementReady && !bufferReady) {
//        measurementReady = 0;
//
//        dutyCycle = ((float)highTime / period) * 100.0;
//
//        // Use the unique number (90%) as a trigger to start filling the buffer
//        if (dutyCycle >= DUTY_90_LOWER - TOLERANCE && dutyCycle <= DUTY_90_UPPER + TOLERANCE && !detecting) {
//            printf("Detected 90%% duty cycle. Starting buffer...\n");
//            detecting = 1;  // Start detecting subsequent events
//            buffer[0] = 90; // Store the 90% as the first entry
//            bufferIndex = 1;
//        }
//
//        // Continue filling the buffer once detecting is triggered
//        if (detecting && bufferIndex < BUFFER_SIZE) {
//            if (dutyCycle >= DUTY_25_LOWER - TOLERANCE && dutyCycle <= DUTY_25_UPPER + TOLERANCE) {
//                buffer[bufferIndex++] = 25;  // 25% duty cycle
//                printf("Buffer[%d] = 25\n", bufferIndex - 1);  // Debug output
//            } else if (dutyCycle >= DUTY_50_LOWER - TOLERANCE && dutyCycle <= DUTY_50_UPPER + TOLERANCE) {
//                buffer[bufferIndex++] = 50;  // 50% duty cycle
//                printf("Buffer[%d] = 50\n", bufferIndex - 1);  // Debug output
//            } else if (dutyCycle >= DUTY_75_LOWER - TOLERANCE && dutyCycle <= DUTY_75_UPPER + TOLERANCE) {
//                buffer[bufferIndex++] = 75;  // 75% duty cycle
//                printf("Buffer[%d] = 75\n", bufferIndex - 1);  // Debug output
//            }
//
//            // If buffer is full, set bufferReady flag for classification
//            if (bufferIndex == BUFFER_SIZE) {
//                bufferReady = 1;
//            }
//        }
//    }
//}
//
//int main(void) {
//    MXC_Delay(MXC_DELAY_MSEC(2));
//    GPIO_Init();
//    Timer_Init();
//
//    // Main loop
//    while (1) {
//        if (bufferReady) {
//            classify_and_reset_buffer();
//        }
//
//        Process_PWM();
//        MXC_Delay(MXC_DELAY_MSEC(2));
//    }
//
//    return 0;
//}
