#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include "hi_timer.h"
#include "hi_io.h"
#include "iot_watchdog.h"
#include "ohos_init.h"
#include "cmsis_os2.h"
#include "iot_gpio_ex.h"
#include "iot_gpio.h"
#include "iot_errno.h"
#include "hi_errno.h"
#include "pca9555.h"
#include "iot_pwm.h"

#define  COUNT   10
#define  FREQ_TIME    20000


void SetAngle(unsigned int duty)
{
    unsigned int time = FREQ_TIME;

    IoTGpioSetOutputVal(IOT_IO_NAME_GPIO_2, IOT_GPIO_VALUE1);
    hi_udelay(duty);
    IoTGpioSetOutputVal(IOT_IO_NAME_GPIO_2, IOT_GPIO_VALUE0);
    hi_udelay(time - duty);
}

/* The steering gear is centered
 * 1、依据角度与脉冲的关系，设置高电平时间为1500微秒
 * 2、不断地发送信号，控制舵机居中
*/
void RegressMiddle(void)
{
    unsigned int angle = 2000;
    for (int i = 0; i < COUNT; i++) {
        SetAngle(angle);
        TaskMsleep(10);
    }
}

/* Turn 90 degrees to the right of the steering gear
 * 1、依据角度与脉冲的关系，设置高电平时间为500微秒
 * 2、不断地发送信号，控制舵机向右旋转90度
*/
/*  Steering gear turn right */
void EngineTurnRight(void)
{
    unsigned int angle = 1200;
    for (int i = 0; i < COUNT; i++) {
        SetAngle(angle);
        TaskMsleep(10);
    }
}

/* Turn 90 degrees to the left of the steering gear
 * 1、依据角度与脉冲的关系，设置高电平时间为2500微秒
 * 2、不断地发送信号，控制舵机向左旋转90度
*/
/* Steering gear turn left */
void EngineTurnLeft(void)
{
    unsigned int angle = 3400;
    for (int i = 0; i < COUNT; i++) {
        SetAngle(angle);
        TaskMsleep(10);
    }
}

void S92RInit(void)
{
    IoTGpioInit(IOT_IO_NAME_GPIO_2);
    IoSetFunc(IOT_IO_NAME_GPIO_2, IOT_IO_FUNC_GPIO_2_GPIO);
    IoTGpioSetDir(IOT_IO_NAME_GPIO_2, IOT_GPIO_DIR_OUT);
}

void Sg92RTask(void)
{
    unsigned int time = 200;
    S92RInit();
}

void Sg92rSampleEntry(void)
{
    osThreadAttr_t attr;
    IoTWatchDogDisable();
    attr.name = "Sg92RTask";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = 1024 * 5; // 堆栈大小为1024*5 stack size 5*1024
    attr.priority = osPriorityNormal;

    if (osThreadNew((osThreadFunc_t)Sg92RTask, NULL, &attr) == NULL) {
        printf("[Sg92RTask] Failed to create Hcsr04SampleTask!\n");
    }
}

APP_FEATURE_INIT(Sg92rSampleEntry);