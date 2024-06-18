
cmake_minimum_required(VERSION 3.12)

include(pico-sdk/pico_sdk_init.cmake)
include(pico-sdk/external/pico_sdk_import.cmake)

project(hello_rtos_world VERSION "0.6.4" LANGUAGES C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -DDEBUG_MODE")

include_directories(src)


pico_sdk_init()

add_executable(hello_world
    src/main.c
    src/dht.c
    src/actuador.c
    src/ds18b20.c
)

target_compile_definitions(hello_world  PRIVATE PICO_MALLOC_PANIC=0)

add_library(FreeRTOS STATIC 
    #FreeRTOS-Kernel/croutine.c
    FreeRTOS-Kernel/event_groups.c
    FreeRTOS-Kernel/list.c
    FreeRTOS-Kernel/queue.c
    FreeRTOS-Kernel/stream_buffer.c
    FreeRTOS-Kernel/tasks.c
    FreeRTOS-Kernel/timers.c

    # FreeRTOS-Kernel/portable/GCC/ARM_CM0/port.c
    # FreeRTOS-Kernel/portable/GCC/ARM_CM0/portasm.c
    FreeRTOS-Kernel/portable/MemMang/heap_3.c
    src/port.c
)

target_include_directories(FreeRTOS PUBLIC FreeRTOS-Kernel/include src/ FreeRTOS-Kernel/portable/GCC/ARM_CM0)

target_link_libraries(hello_world PRIVATE
    FreeRTOS
    pico_stdlib
)

pico_add_extra_outputs(hello_world)
pico_enable_stdio_usb(hello_world 1)
pico_enable_stdio_uart(hello_world 0)