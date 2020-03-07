/* Lowpower EVB Example

   For other examples please check:
   https://github.com/espressif/esp-iot-solution/tree/master/examples

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
 */

#ifndef _IOT_LOWPOWER_EVB_POWER_H_
#define _IOT_LOWPOWER_EVB_POWER_H_

#include "driver/gpio.h"

#ifdef __cplusplus

class CPowerCtrl
{
private:
    gpio_num_t io_num;
public:
    /**
     * @brief Constructor for power control class
     * @param io gpio number used to control power
     */
    CPowerCtrl(gpio_num_t io);

    /**
     * @brief Destructor function of power control class
     */
    ~CPowerCtrl();

    /**
     * @brief power on
     */
    void on();

    /**
     * @brief power off
     */
    void off();
};
#endif

#endif  
