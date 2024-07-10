/* ADC的相关API接口 Related API interfaces of ADC */
#include "iot_adc.h"
#include "hi_adc.h"
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

#define IOT_PWM_PORT_PWM0   0
#define IOT_PWM_PORT_PWM1   1
#define IOT_PWM_PORT_PWM2   2
#define IOT_PWM_PORT_PWM3   3
#define IOT_FREQ            65535
#define IOT_DUTY            50

void Lth1550Init(void)
{
    /* 红外对管对应的GPIO，左：ADC0_GPIO12，右：ADC3_GPIO07 */
    /* GPIO corresponding to infrared tube, left: ADC0_ GPIO12, right: ADC3_ GPIO07 */
    IoTGpioInit(IOT_IO_NAME_GPIO_7);
    /* 设置GPIO07的管脚复用关系为GPIO */
    /* Set the pin reuse relationship of GPIO07 to GPIO */
    IoSetFunc(IOT_IO_NAME_GPIO_7, IOT_IO_FUNC_GPIO_7_GPIO);
    IoTGpioSetDir(IOT_IO_NAME_GPIO_7, IOT_GPIO_DIR_IN);
    IoTGpioInit(IOT_IO_NAME_GPIO_12);
    IoSetFunc(IOT_IO_NAME_GPIO_12, IOT_IO_FUNC_GPIO_12_GPIO);
    IoTGpioSetDir(IOT_IO_NAME_GPIO_12, IOT_GPIO_DIR_IN);
}

void GetInfraredData1(IotAdcChannelIndex IOT_ADC_CHANNEL_0,unsigned short datal)
{
    datal = 0;
    int ret = 0;
    /* ADC_Channal_6  自动识别模式  CNcomment:4次平均算法模式 CNend 0xff */
    /* ADC_ Channal_ 6 Automatic recognition mode CNcomment: 4 times average algorithm mode CNend 0xff */
    ret = AdcRead(IOT_ADC_CHANNEL_0, &datal, IOT_ADC_EQU_MODEL_4, IOT_ADC_CUR_BAIS_DEFAULT, 0xff);
    if (ret != IOT_SUCCESS) {
        printf("hi_adc_read failed\n");
    }
    printf("Left ADC value is %d \r\n", datal);
    if(datal<300)
    {
        LeftWheelForword();
    }
    else
    {
        LeftWheelStop();
    }
}

void GetInfraredData2(IotAdcChannelIndex IOT_ADC_CHANNEL_3,unsigned short datar)
{
    datar = 0;
    int ret = 0;
    /* ADC_Channal_6  自动识别模式  CNcomment:4次平均算法模式 CNend 0xff */
    /* ADC_ Channal_ 6 Automatic recognition mode CNcomment: 4 times average algorithm mode CNend 0xff */
    ret = AdcRead(IOT_ADC_CHANNEL_3, &datar, IOT_ADC_EQU_MODEL_4, IOT_ADC_CUR_BAIS_DEFAULT, 0xff);
    if (ret != IOT_SUCCESS) {
        printf("hi_adc_read failed\n");
    }
    printf("Left ADC value is %d \r\n", datar);
    if(datar<300)
    {
        RightWheelForword();
    }
    else
    {
        RightWheelStop();
    }
}



static void adcTask(void)
{
    Lth1550Init();
}

void ADCExampleEntry(void)
{
    osThreadAttr_t attr;
    IoTWatchDogDisable();
    attr.name = "adcTask";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = 5 * 1024; // 堆栈大小5*1024，stack size 5*1024
    attr.priority = osPriorityNormal;

    if (osThreadNew((osThreadFunc_t)adcTask, NULL, &attr) == NULL) {
        printf("[LSM6DSTask] Failed to create LSM6DSTask!\n");
    }
}

APP_FEATURE_INIT(ADCExampleEntry);
