#include <stdio.h>
#include "pico/stdlib.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#if 0
// Simple led task
void vTaskCode( void * pvParameters )
{
    /* The parameter value is expected to be 1 as 1 is passed in the
    pvParameters value in the call to xTaskCreate() below. 
    configASSERT( ( ( uint32_t ) pvParameters ) == 1 );
    */
    for( ;; ) {
        gpio_put(PICO_DEFAULT_LED_PIN, 1);
        vTaskDelay(pdMS_TO_TICKS(1000));
        gpio_put(PICO_DEFAULT_LED_PIN, 0);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

int main() {
    stdio_init_all();

#ifdef PICO_DEFAULT_LED_PIN
    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);
#endif

    TaskHandle_t xHandle = NULL;
    /* Create the task, storing the handle. */
    BaseType_t xReturned = xTaskCreate(
        vTaskCode,       /* Function that implements the task. */
        "Blinky task",   /* Text name for the task. */
        512,             /* Stack size in words, not bytes. */
        ( void * ) 1,    /* Parameter passed into the task. */
        tskIDLE_PRIORITY,/* Priority at which the task is created. */
        &xHandle );   

    vTaskStartScheduler();
    __builtin_unreachable();
}
#endif

#if 0
// Two tasks with same code but different priorities
void vTaskCode( void * pvParameters ) {
    int taskId = *((int *)pvParameters);
    for( ;; ) {
        printf("Task %d task\n", taskId);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

int main() {
    stdio_init_all();
  
    TaskHandle_t xHandle1 = NULL;
    TaskHandle_t xHandle2 = NULL;
    /* Create 2 task */
    int tid1 = 1, tid2 = 2;
    BaseType_t xReturned1 = xTaskCreate( vTaskCode, "talking task1", 512, ( void * ) &tid1, 1, &xHandle1 );  
    BaseType_t xReturned2 = xTaskCreate( vTaskCode, "talking task2", 512, ( void * ) &tid2, 1, &xHandle2 );
    vTaskStartScheduler();
    __builtin_unreachable();
}
#endif

#if 0
// Two tasks with different yielding
void vTaskYielding( void * pvParameters ) {
    int taskId = *((int *)pvParameters);
    for( ;; ) {
        printf("doing something and sleeping ID %d\n", taskId);
        vTaskDelay(pdMS_TO_TICKS(1000));
        printf("Doing more and yielding ID %d\n", taskId);
        taskYIELD();
    }
}

void vTaskUnfriendly( void * pvParameters ) {
    int taskId = *((int *)pvParameters);
    for( ;; ) {
        printf("doing something and busy waiting ID %d\n", taskId);
        uint64_t start_ticks = time_us_64();
        uint64_t end_ticks = start_ticks + 2000000;
        while (time_us_64() < end_ticks) {
            tight_loop_contents();
        }
    }
}

int main() {
    stdio_init_all();
  
    TaskHandle_t xHandle1 = NULL;
    TaskHandle_t xHandle2 = NULL;
     /* Create 2 task */
    int tid1 = 1, tid2 = 2;
    BaseType_t xReturned1 = xTaskCreate( vTaskYielding, "Yielding Task", 512, ( void * ) &tid1, 2, &xHandle1 );  
    BaseType_t xReturned2 = xTaskCreate( vTaskUnfriendly, "Unfriendly Task", 512, ( void * ) &tid2, 1, &xHandle2 );
    vTaskStartScheduler();
    __builtin_unreachable();
}
#endif

#if 0
SemaphoreHandle_t xSemaphore;

// Two tasks sharing resources
void vTaskSem( void * pvParameters ) {
    int taskId = *((int *)pvParameters);
    for( ;; ) {
        printf("Running Task ID %d\n", taskId);
        if (xSemaphoreTake(xSemaphore, pdMS_TO_TICKS(100) ) != pdTRUE) {
            printf("Task %d failed to capture semaphore\n", taskId);
        } else {
            printf("Task %d doing some job\n", taskId);
            vTaskDelay(pdMS_TO_TICKS(10));

            printf("Task %d returning semaphore\n", taskId);
            xSemaphoreGive(xSemaphore);
        }

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

int main() {
    stdio_init_all();
  
    TaskHandle_t xHandle1 = NULL;
    TaskHandle_t xHandle2 = NULL;

    vSemaphoreCreateBinary( xSemaphore );
    printf("Semaphore creation was %s\n", xSemaphore == NULL ? "failed": "succesful");

     /* Create 2 task */
     int tid1 = 1, tid2 = 2;
    BaseType_t xReturned1 = xTaskCreate( vTaskSem, "Yielding Task 1", 512, ( void * ) &tid1, 1, &xHandle1 );
    BaseType_t xReturned2 = xTaskCreate( vTaskSem, "Yielding Task 2", 512, ( void * ) &tid2, 1, &xHandle2 );  
    vTaskStartScheduler();
    __builtin_unreachable();
}
#endif

#if 0
// One greedy task to rule them all
void vTaskFriendly( void * pvParameters ) {
    int taskId = *((int *)pvParameters);
    for( ;; ) {
        printf("Doing something and sleeping ID %d\n", taskId);
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

void vTaskUnfriendly( void * pvParameters ) {
    int taskId = *((int *)pvParameters);
    for( ;; ) {
        UBaseType_t priority = uxTaskPriorityGet(NULL);
        printf("Doing something with priority %d and busy waiting ID %d\n\n", priority, taskId);
        uint64_t start_ticks = time_us_64();
        uint64_t end_ticks = start_ticks + 2000000;
        while (time_us_64() < end_ticks) {
            tight_loop_contents();
        }
        vTaskPrioritySet(NULL, (priority+1) % configMAX_PRIORITIES);
    }
}

int main() {
    stdio_init_all();
  
    TaskHandle_t xHandle1 = NULL, xHandle2 = NULL, xHandle3 = NULL, xHandle4 = NULL, xHandle5 = NULL;
     /* Create 2 task */
    int tid1 = 1, tid2 = 2, tid3 = 3, tid4 = 4, tid5 = 5;
    BaseType_t xReturned1 = xTaskCreate( vTaskUnfriendly, "Unfriendly Task", 512, ( void * ) &tid1, 0, &xHandle1 );
    BaseType_t xReturned2 = xTaskCreate( vTaskFriendly, "Friendly Task", 512, ( void * ) &tid2, 1, &xHandle2 );
    BaseType_t xReturned3 = xTaskCreate( vTaskFriendly, "Friendly Task", 512, ( void * ) &tid3, 2, &xHandle3 ); 
    BaseType_t xReturned4 = xTaskCreate( vTaskFriendly, "Friendly Task", 512, ( void * ) &tid4, 3, &xHandle4 );   
    BaseType_t xReturned5 = xTaskCreate( vTaskFriendly, "Friendly Task", 512, ( void * ) &tid5, 4, &xHandle5 ); 
    
    vTaskStartScheduler();
    __builtin_unreachable();
}
#endif

#if 0
// Two tasks delaying
void vTaskDelaying( void * pvParameters ) {
    int taskId = *((int *)pvParameters);
    for( ;; ) {
        printf("ID %d, delay every 1000 ms, systick %d uS\n", taskId, xTaskGetTickCount());
        vTaskDelay(pdMS_TO_TICKS(1000));
        busy_wait_ms(100);
    }
}

void vTaskDelayingUntil( void * pvParameters ) {
    int taskId = *((int *)pvParameters);
    TickType_t xLastWakeTime = xTaskGetTickCount();
    for( ;; ) {
        printf("ID %d, delay until every 1000 ms, systick %d uS\n", taskId, xTaskGetTickCount());
        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(1000));
        busy_wait_ms(100);
    }
}

    int main() {
    stdio_init_all();
  
    TaskHandle_t xHandle1 = NULL, xHandle2 = NULL;
     /* Create 2 task */
    int tid1 = 1, tid2 = 2;
    BaseType_t xReturned1 = xTaskCreate( vTaskDelaying, "vTaskDelaying", 512, ( void * ) &tid1, 0, &xHandle1 );
    BaseType_t xReturned2 = xTaskCreate( vTaskDelayingUntil, "vTaskDelayingUntil", 512, ( void * ) &tid2, 1, &xHandle2 );
    
    vTaskStartScheduler();
    __builtin_unreachable();
}
#endif

#if 0
// Master task spwans and destroy 
void vTaskRunner( void * pvParameters ) {
    int taskId = *((int *)pvParameters);
    for( ;; ) {
        printf("ID %d, delay every 1000 ms\n", taskId);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

#define MAX_TASKS 500
#define PICO_MALLOC_PANIC 0
TaskHandle_t xListOfTasks[MAX_TASKS];

void vTaskMaster( void * pvParameters ) {
    int taskId = *((int *)pvParameters);
    int amountOfTasks = 0;
    bool createDestroy = true;
    for( ;; ) {
        printf("Master Task ID %d, detects %d created tasks\n", taskId, uxTaskGetNumberOfTasks());
        if (createDestroy) {
            if (amountOfTasks < MAX_TASKS) {
                amountOfTasks++;
                printf("Create new runner Task %d\n", amountOfTasks);
                BaseType_t xReturned1 = xTaskCreate(vTaskRunner, "vTaskRunner", 256, (void *) &amountOfTasks, 1, &xListOfTasks[amountOfTasks - 1]);
                if (xReturned1 != pdPASS) {
                    amountOfTasks--;
                    printf("Created all task allowed by memory %d\n", amountOfTasks);
                    createDestroy = false;
                    vTaskDelay(pdMS_TO_TICKS(2000));
                }
            } else {
                printf("Created all task defined in array %d\n", MAX_TASKS);
                vTaskDelay(pdMS_TO_TICKS(2000));
                createDestroy = false;
            }  
        } else if (amountOfTasks > 0){
            TaskHandle_t handle = xListOfTasks[amountOfTasks - 1];
            printf("Destroy created runner Task %d\n", amountOfTasks);
            vTaskDelete(handle);
            amountOfTasks--;
        } else {
            createDestroy = true;
        }
        
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

int main() {
    stdio_init_all();
  
    TaskHandle_t xHandle = NULL;
    int tid1 = 1;
    BaseType_t xReturned1 = xTaskCreate(vTaskMaster, "vTaskMaster", 512, (void *) &tid1, 4, &xHandle);
    
    vTaskStartScheduler();
    __builtin_unreachable();
}
#endif

#if 1
// Two publishers tasks messaging with a Queue to a Consumer task
QueueHandle_t xQueue;
void vTaskPublisher( void * pvParameters ) {
    int taskId = *((int *)pvParameters);
    for( ;; ) {
        const char* msg = taskId == 2 ? "Message from Task 2" :"Message from Task 3";
        printf("TaskId %d sending msg: %s\n", taskId, msg);
        BaseType_t res =  xQueueSend(xQueue, (void *)&msg, portMAX_DELAY);
        if (res != pdPASS) {
            printf("TaskId %d failed to send msg\n", taskId);
        }
        vTaskDelay(pdMS_TO_TICKS(250));
    }
}

void vTaskConsumer( void * pvParameters ) {
    int taskId = *((int *)pvParameters);
    const char* msg;
    for( ;; ) {
        vTaskDelay(pdMS_TO_TICKS(500));
        BaseType_t res = xQueueReceive(xQueue, &msg, portMAX_DELAY);
        if (res != pdPASS) {
            printf("TaskId %d failed to receive msg\n", taskId);
        } else {
            printf("Consumer received: %s\n", msg);
        }
    }
}
#define MESSAGE_COUNT 32
int main() {
    stdio_init_all();
  
    xQueue = xQueueCreate(MESSAGE_COUNT, sizeof(const char*));
    printf("Queue creation was %s\n", xQueue == NULL ? "failed": "succesful");

     /* Create 2 task */
    int tid1 = 1, tid2 = 2, tid3 = 3;
    TaskHandle_t xHandle1 = NULL, xHandle2 = NULL, xHandle3 = NULL;
    BaseType_t xReturned1 = xTaskCreate(vTaskConsumer, "Consumer Task "  , 512, (void *) &tid1, 3, &xHandle1);
    BaseType_t xReturned2 = xTaskCreate(vTaskPublisher, "Publisher Task 1", 512, (void *) &tid2, 2, &xHandle2);
    BaseType_t xReturned3 = xTaskCreate(vTaskPublisher, "Publisher Task 2", 512, (void *) &tid3, 2, &xHandle3);  
    vTaskStartScheduler();
    __builtin_unreachable();
}
#endif
/*
int fib(int val) {
    return val;
}

typedef struct {
    char val;
    int  result;
} myObject;

void vTaskFib(void * pvParameters) {
    // int *inVal = (int *)pvParameters;
    // int *outVal = (int *)pvParameters + 1;
    int cnt =0;
    for( ;; ) {
        
      int val  = fib(cnt++);
      printf("fib de %d es %d\n", cnt, val);
    }
}

 BaseType_t xReturned1 = xTaskCreate(vTaskConsumer, "Task "  , 1024, (void *) &tid1, 3, &xHandle1);
*/
