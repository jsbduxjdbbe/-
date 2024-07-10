/*
 * Copyright (c) 2022 HiSilicon (Shanghai) Technologies CO., LIMITED.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include "iot_adc.h"
#include "hi_adc.h"
#include "iot_gpio_ex.h"
#include "ohos_init.h"
#include "cmsis_os2.h"
#include "iot_gpio.h"
#include "iot_uart.h"
#include "hi_uart.h"
#include "iot_watchdog.h"
#include "iot_errno.h"

#define UART_BUFF_SIZE 100
#define U_SLEEP_TIME   100000

const char *data1 = "ATI\r\n";
const char *data2 = "AT+MIPCALL?\r\n";
const char *data3 = "AT+MIPCALL=1\r\n";
const char *data4 = "AT+TCDEVINFOSET=1,\"MZE79HARLW\",\"smart_car\",\"OnfpCwniXuFDIGmDy6SabQ==\"\r\n";
const char *data5 = "AT+TCMQTTCONN=1,20000,240,1,1\r\n";
const char *data6 = "AT+TCMQTTSUB=\"$thing/down/property/MZE79HARLW/smart_car\",1\r\n";
const char *data7 = "AT+TCMQTTPUB=\"$thing/up/property/MZE79HARLW/smart_car\",1,\"{\\\"method\\\":\\\"report\\\",\\\"clientToken\\\":\\\"123\\\",\\\"params\\\":{\\\"load_status\\\":1}}\"\r\n";
const char *data8 = "AT+TCMQTTPUB=\"$thing/up/property/MZE79HARLW/smart_car\",1,\"{\\\"method\\\":\\\"report\\\",\\\"clientToken\\\":\\\"123\\\",\\\"params\\\":{\\\"load_status\\\":0}}\"\r\n";
const char *data9 = "AT+TCMQTTPUB=\"$thing/up/property/MZE79HARLW/smart_car\",1,\"{\\\"method\\\":\\\"report\\\",\\\"clientToken\\\":\\\"123\\\",\\\"params\\\":{\\\"route\\\":0}}\"\r\n";
const char *data10 = "AT+TCMQTTDISCONN\r\n";
const char *data11 = "AT+MIPCALL=0\r\n";
const char *data13 ="AT+TCMQTTPUB=\"$thing/up/property/MZE79HARLW/smart_car\",1,\"{\\\"method\\\":\\\"report\\\",\\\"clientToken\\\":\\\"123\\\",\\\"params\\\":{\\\"running_status\\\":6}}\"\r\n";

void Uart1GpioInit(void)
{
    IoTGpioInit(IOT_IO_NAME_GPIO_0);
    // 设置GPIO0的管脚复用关系为UART1_TX Set the pin reuse relationship of GPIO0 to UART1_ TX
    IoSetFunc(IOT_IO_NAME_GPIO_0, IOT_IO_FUNC_GPIO_0_UART1_TXD);
    IoTGpioInit(IOT_IO_NAME_GPIO_1);
    // 设置GPIO1的管脚复用关系为UART1_RX Set the pin reuse relationship of GPIO1 to UART1_ RX
    IoSetFunc(IOT_IO_NAME_GPIO_1, IOT_IO_FUNC_GPIO_1_UART1_RXD);
}

void Uart1Config(void)
{
    uint32_t ret;
    /* 初始化UART配置，波特率 9600，数据bit为8,停止位1，奇偶校验为NONE */
    /* Initialize UART configuration, baud rate is 9600, data bit is 8, stop bit is 1, parity is NONE */
    IotUartAttribute uart_attr = {
        .baudRate = 115200,
        .dataBits = 8,
        .stopBits = 1,
        .parity = 0,
    };
    ret = IoTUartInit(HI_UART_IDX_1, &uart_attr);
    if (ret != IOT_SUCCESS) {
        printf("Init Uart1 Falied Error No : %d\n", ret);
        return;
    }
}

void SingleRoad(void)
{
    int i=0;
    unsigned short datal;
    unsigned short datar;
    uint32_t len = 0;
    unsigned char uartReadBuff[UART_BUFF_SIZE] = {0};
    while (1) 
    {
        loop:
        len = IoTUartRead(HI_UART_IDX_1, uartReadBuff, UART_BUFF_SIZE);
        GetInfraredData1(IOT_ADC_CHANNEL_0,datal);
        GetInfraredData2(IOT_ADC_CHANNEL_3,datar);
        usleep(2000); // wait 20 us
        if (strstr(uartReadBuff, ":7") != NULL)
        {
            i++;
            break;
        }
    }
    if(i==1)
    {
        LeftWheelStop();
        RightWheelStop();
        IoTUartWrite(HI_UART_IDX_1, (unsigned char*)data13, strlen(data13));
        IoTUartWrite(HI_UART_IDX_1, (unsigned char*)data7, strlen(data7));
        TaskMsleep(500);
        EngineTurnRight();
        TaskMsleep(1000);
        RegressMiddle();
        IoTUartWrite(HI_UART_IDX_1, (unsigned char*)data8, strlen(data8));
        goto loop;
    }
    else if(i==2)
    {
        IoTUartWrite(HI_UART_IDX_1, (unsigned char*)data13, strlen(data13));
        LeftWheelStop();
        RightWheelStop();
        IoTUartWrite(HI_UART_IDX_1, (unsigned char*)data9, strlen(data9));
        TaskMsleep(5000);
        park();
    }
}

void DoubleRoad(void)
{
    int i=0;
    unsigned short datal;
    unsigned short datar;
    uint32_t len = 0;
    unsigned char uartReadBuff[UART_BUFF_SIZE] = {0};
    while (1) 
    {
        loop:
        len = IoTUartRead(HI_UART_IDX_1, uartReadBuff, UART_BUFF_SIZE);
        GetInfraredData1(IOT_ADC_CHANNEL_0,datal);
        GetInfraredData2(IOT_ADC_CHANNEL_3,datar);
        usleep(2000); // wait 20 us
        if (strstr(uartReadBuff, ":7") != NULL)
        {
            i++;
            break;
        }
    }
    if(i==1)
    {
        LeftWheelStop();
        RightWheelStop();
        IoTUartWrite(HI_UART_IDX_1, (unsigned char*)data13, strlen(data13));
        IoTUartWrite(HI_UART_IDX_1, (unsigned char*)data7, strlen(data7));
        TaskMsleep(500);
        EngineTurnRight();
        TaskMsleep(1000);
        RegressMiddle();
        IoTUartWrite(HI_UART_IDX_1, (unsigned char*)data8, strlen(data8));
        goto loop;
    }
    else if(i==2)
    {
        LeftWheelStop();
        RightWheelStop();
        IoTUartWrite(HI_UART_IDX_1, (unsigned char*)data13, strlen(data13));
        IoTUartWrite(HI_UART_IDX_1, (unsigned char*)data7, strlen(data7));
        TaskMsleep(500);
        EngineTurnRight();
        TaskMsleep(1000);
        RegressMiddle();
        IoTUartWrite(HI_UART_IDX_1, (unsigned char*)data8, strlen(data8));
        goto loop;
    }
    else if(i==3)
    {
        IoTUartWrite(HI_UART_IDX_1, (unsigned char*)data13, strlen(data13));
        LeftWheelStop();
        RightWheelStop();
        IoTUartWrite(HI_UART_IDX_1, (unsigned char*)data9, strlen(data9));
        park();
    }
}

void L610Init()
{
    
    IoTUartWrite(HI_UART_IDX_1, (unsigned char*)data1, strlen(data1));
    TaskMsleep(500);
    IoTUartWrite(HI_UART_IDX_1, (unsigned char*)data2, strlen(data2));
    TaskMsleep(500);
    IoTUartWrite(HI_UART_IDX_1, (unsigned char*)data3, strlen(data3));
    TaskMsleep(500);
    IoTUartWrite(HI_UART_IDX_1, (unsigned char*)data4, strlen(data4));
    TaskMsleep(500);
    IoTUartWrite(HI_UART_IDX_1, (unsigned char*)data5, strlen(data5));
    TaskMsleep(500);
    IoTUartWrite(HI_UART_IDX_1, (unsigned char*)data6, strlen(data6));
    TaskMsleep(500);
}

void L610UnInit(void)
{
    IoTUartWrite(HI_UART_IDX_1, (unsigned char*)data10, strlen(data10));
    TaskMsleep(500);
    IoTUartWrite(HI_UART_IDX_1, (unsigned char*)data11, strlen(data11));
    TaskMsleep(500);
}

void park(void)
{   
    RegressMiddle();
    LeftWheelForword();
    RightWheelForword();
    TaskMsleep(500);
    LeftWheelStop();
    TaskMsleep(1000);
    RightWheelStop();
}

void unpark(void)
{
    EngineTurnLeft();
    RightWheelBackword();
    TaskMsleep(1000);
    LeftWheelBackword();
    TaskMsleep(500);
    LeftWheelStop();
    IoTUartWrite(HI_UART_IDX_1, (unsigned char*)data7, strlen(data7));
    TaskMsleep(1000);
    RegressMiddle();
    IoTUartWrite(HI_UART_IDX_1, (unsigned char*)data8, strlen(data8));
    RightWheelStop();
}

static void UartTask(void)
{
    uint32_t len = 0;
    unsigned char uartReadBuff[UART_BUFF_SIZE] = {0};
    
    // 对UART1的一些初始化 Some initialization of UART1
    Uart1GpioInit();
    // 对UART1参数的一些配置 Some configurations of UART1 parameters
    Uart1Config();
    L610Init();

    while (1) {
        len = IoTUartRead(HI_UART_IDX_1, uartReadBuff, UART_BUFF_SIZE);
        if (strstr(uartReadBuff, ":1") != NULL)
        {
            RegressMiddle();
            IoTUartWrite(HI_UART_IDX_1, (unsigned char*)data7, strlen(data7));
            TaskMsleep(1000);
            IoTUartWrite(HI_UART_IDX_1, (unsigned char*)data8, strlen(data8));
            SingleRoad();
        }
        else if(strstr(uartReadBuff, ":2") != NULL)
        {
            unpark();
            DoubleRoad();
        }
        usleep(U_SLEEP_TIME);
    }
}

void UartExampleEntry(void)
{
    osThreadAttr_t attr;
    IoTWatchDogDisable();

    attr.name = "UartTask";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = 5 * 1024; // 任务栈大小*1024 stack size 5*1024
    attr.priority = osPriorityNormal;

    if (osThreadNew((osThreadFunc_t)UartTask, NULL, &attr) == NULL) {
        printf("[UartTask] Failed to create UartTask!\n");
    }
}

APP_FEATURE_INIT(UartExampleEntry);
