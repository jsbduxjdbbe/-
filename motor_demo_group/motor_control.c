#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include "hi_timer.h"
#include "hi_io.h"
#include "iot_watchdog.h"
#include "ohos_init.h"
#include "cmsis_os2.h"
#include "iot_gpio_ex.h"
#include "iot_gpio.h"LeftWheelStop
#include "iot_errno.h"
#include "hi_errno.h"
#include "pca9555.h"
#include "iot_pwm.h"

#define IOT_PWM_PORT_PWM0   0
#define IOT_PWM_PORT_PWM1   1
#define IOT_PWM_PORT_PWM2   2
#define IOT_PWM_PORT_PWM3   3
#define IOT_FREQ            65535
#define IOT_DUTY            27


void LeftWheelForword(void)
{
    IoTPwmStart(IOT_PWM_PORT_PWM2, IOT_DUTY, IOT_FREQ);
    IoTPwmStop(IOT_PWM_PORT_PWM3);
}

void LeftWheelBackword(void)
{
    IoSetFunc(IOT_IO_NAME_GPIO_12, IOT_IO_FUNC_GPIO_12_GPIO);
    IoSetFunc(IOT_IO_NAME_GPIO_6, IOT_IO_FUNC_GPIO_6_PWM3_OUT);
    IoTPwmStart(IOT_PWM_PORT_PWM3, IOT_DUTY, IOT_FREQ);
    IoTPwmStop(IOT_PWM_PORT_PWM2);
}

void LeftWheelStop(void)
{
    IoTPwmStop(IOT_PWM_PORT_PWM2);
    IoTPwmStop(IOT_PWM_PORT_PWM3);
}

void RightWheelForword(void)
{
    IoTPwmStart(IOT_PWM_PORT_PWM1, IOT_DUTY, IOT_FREQ);
    IoTPwmStop(IOT_PWM_PORT_PWM0);
}

void RightWheelBackword(void)
{
    IoTPwmStart(IOT_PWM_PORT_PWM0, IOT_DUTY, IOT_FREQ);
    IoTPwmStop(IOT_PWM_PORT_PWM1);
}

void RightWheelStop(void)
{
    IoTPwmStop(IOT_PWM_PORT_PWM0);
    IoTPwmStop(IOT_PWM_PORT_PWM1);
}

void GA12N20Init(void)
{
    // 左电机GPIO5,GPIO6初始化 Initialization of left motor GPIO5 and GPIO6
    IoTGpioInit(IOT_IO_NAME_GPIO_5);
    IoTGpioInit(IOT_IO_NAME_GPIO_6);
    // 右电机GPIO9, GPIO10初始化 Right motor GPIO9, GPIO10 initialization
    IoTGpioInit(IOT_IO_NAME_GPIO_9);
    IoTGpioInit(IOT_IO_NAME_GPIO_10);

    // 设置GPIO5的管脚复用关系为PWM2输出 Set the pin multiplexing relationship of GPIO5 to PWM2 output
    IoSetFunc(IOT_IO_NAME_GPIO_5, IOT_IO_FUNC_GPIO_5_PWM2_OUT);
    // 设置GPIO6的管脚复用关系为PWM3输出 Set the pin multiplexing relationship of GPIO6 to PWM3 output
    IoSetFunc(IOT_IO_NAME_GPIO_6, IOT_IO_FUNC_GPIO_6_PWM3_OUT);
    // 设置GPIO9的管脚复用关系为PWM0输出 Set the pin multiplexing relationship of GPIO9 to PWM0 output
    IoSetFunc(IOT_IO_NAME_GPIO_9, IOT_IO_FUNC_GPIO_9_PWM0_OUT);
    // 设置GPIO10的管脚复用关系为PWM01输出 Set the pin multiplexing relationship of GPIO10 to PWM01 output
    IoSetFunc(IOT_IO_NAME_GPIO_10, IOT_IO_FUNC_GPIO_10_PWM1_OUT);

    // GPIO5方向设置为输出 GPIO5 direction set to output
    IoTGpioSetDir(IOT_IO_NAME_GPIO_5, IOT_GPIO_DIR_OUT);
    // GPIO6方向设置为输出 GPIO6 direction set to output
    IoTGpioSetDir(IOT_IO_NAME_GPIO_6, IOT_GPIO_DIR_OUT);
    // GPIO9方向设置为输出 GPIO9 direction set to output
    IoTGpioSetDir(IOT_IO_NAME_GPIO_9, IOT_GPIO_DIR_OUT);
    // GPIO10方向设置为输出 GPIO10 direction set to output
    IoTGpioSetDir(IOT_IO_NAME_GPIO_10, IOT_GPIO_DIR_OUT);
    // 初始化PWM2 Initialize PWM2
    IoTPwmInit(IOT_PWM_PORT_PWM2);
    // 初始化PWM3 Initialize PWM3
    IoTPwmInit(IOT_PWM_PORT_PWM3);
    // 初始化PWM0 Initialize PWM0
    IoTPwmInit(IOT_PWM_PORT_PWM0);
    // 初始化PWM1 Initialize PWM1
    IoTPwmInit(IOT_PWM_PORT_PWM1);
    // 先使两个电机处于停止状态 motors stop
    RightWheelStop();
    LeftWheelStop();
}

void GA12N205Task(void)
{
    // 初始化电机模块 Initialize the motor module
    GA12N20Init();
}

void GA12N20SampleEntry(void)
{
    osThreadAttr_t attr;
    IoTWatchDogDisable();
    attr.name = "GA12N205Task";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = 1024 * 5; // 堆栈大小为1024*5,stack size 1024*5
    attr.priority = osPriorityNormal;

    if (osThreadNew((osThreadFunc_t)GA12N205Task, NULL, &attr) == NULL) {
        printf("[GA12N205Task] Failed to create Hcsr04SampleTask!\n");
    }
}

APP_FEATURE_INIT(GA12N20SampleEntry);