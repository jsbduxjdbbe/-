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
#include "hi_timer.h"

#include "ohos_init.h"
#include "cmsis_os2.h"
#include "iot_gpio_ex.h"
#include "iot_gpio.h"
#include "iot_errno.h"
#include "hi_errno.h"
#include "pca9555.h"

#define DELAY_US    20
static volatile int g_buttonState = 0;

void OnFuncKeyPressed(char *arg)
{
    (void) arg;
    g_buttonState = 1;
}

void FuncKeyInit(void)
{
    IoTGpioRegisterIsrFunc(IOT_IO_NAME_GPIO_11, IOT_INT_TYPE_EDGE,
                           IOT_GPIO_EDGE_FALL_LEVEL_LOW, OnFuncKeyPressed, NULL);
    SetPCA9555GpioValue(PCA9555_PART0_IODIR, 0x1c);
}

void GetFunKeyState(void)
{
    uint8_t ext_io_state = 0;
    uint8_t ext_io_state_d = 0;
    uint8_t status;

    while (1) {
        if (g_buttonState == 1) {
            uint8_t diff;
            status = PCA9555I2CReadByte(&ext_io_state);
            if (status != IOT_SUCCESS) {
                printf("i2c error!\r\n");
                ext_io_state = 0;
                ext_io_state_d = 0;
                g_buttonState = 0;
                continue;
            }

            diff = ext_io_state ^ ext_io_state_d;
            if (diff == 0) {
                printf("diff = 0! state:%0X, %0X\r\n", ext_io_state, ext_io_state_d);
            }
            if ((diff & 0x04) && ((ext_io_state & 0x04) == 0)) {
                printf("button1 pressed,\r\n");
                L610UnInit();
            } else if ((diff & 0x08) && ((ext_io_state & 0x08) == 0)) {
                printf("button2 pressed \r\n");
                L610UnInit();
            } else if ((diff & 0x10) && ((ext_io_state & 0x10) == 0)) {
                printf("button3 pressed \r\n");
                L610UnInit();
            }
            status = PCA9555I2CReadByte(&ext_io_state);
            ext_io_state_d = ext_io_state;
            g_buttonState = 0;
        }
        usleep(DELAY_US);
    }
}

static void ButtonControl(void)
{
    printf("ButtonControl\r\n");
    PCA9555Init();
    SetPCA9555GpioValue(PCA9555_PART1_IODIR, PCA9555_OUTPUT);
    SetPCA9555GpioValue(PCA9555_PART1_OUTPUT, LED_OFF);
    FuncKeyInit();
    GetFunKeyState();
}

static void ButtonControlEntry(void)
{
    osThreadAttr_t attr;
    attr.name = "LedCntrolDemo";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = 1024;
    attr.priority = osPriorityNormal;
    if (osThreadNew((osThreadFunc_t)ButtonControl, NULL, &attr) == NULL) {
        printf("[LedExample] Failed to create LedTask!\n");
    }
}

APP_FEATURE_INIT(ButtonControlEntry);