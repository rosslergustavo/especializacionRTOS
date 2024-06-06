#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#if 1
// Two Cores Runnning Freertos
QueueHandle_t xQueue;
void vTaskRunner( void * pvParameters ) {
    int coreId = ((int *)pvParameters)[0];
    int taskId = ((int *)pvParameters)[1];
    for( ;; ) {
        printf("Core %d, TaskId %d running\n", coreId, taskId);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void core1_main() {
    // /* Create task */
    #if 0
    int coreTid[2] = {1, 2};
    TaskHandle_t xHandle2 = NULL;
    BaseType_t xReturned2 = xTaskCreate(vTaskRunner, "Runner Task", 512, (void *) &coreTid, 3, &xHandle2);

    vTaskStartScheduler();
    __builtin_unreachable();
    #else 
    for (;;) {
        printf("Core %d, TaskId %d running\n", coreTid[0], coreTid[1]);
        sleep_ms(1000);
    }
    #endif
}

int main() {
    stdio_init_all();

    /* Create task */
    int coreTid[2] = {0, 1};
    TaskHandle_t xHandle1 = NULL;
    BaseType_t xReturned1 = xTaskCreate(vTaskRunner, "Runner Task", 512, (void *) &coreTid, 3, &xHandle1);

    //Start seconc core
    multicore_launch_core1(core1_main);

    vTaskStartScheduler();
    __builtin_unreachable();
}
#endif