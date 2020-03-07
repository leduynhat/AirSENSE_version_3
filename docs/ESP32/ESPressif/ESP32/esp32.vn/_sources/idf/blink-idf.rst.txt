Lập trình chớp tắt LED với ESP32 bằng ESP-IDF
---------------------------------------------

Giới thiệu.
===========

* Trong ESP32 có tất cả 34 chân GPIO:
	* GPIO 00 - GPIO 19
	* GPIO 21 - GPIO 23
	* GPIO 25 - GPIO 27
	* GPIO 32 - GPIO 39

* Lưu ý:
	* ``Không bao gồm`` các chân ``20, 24, 28, 29, 30 và 31``.
	* Các chân ``GPIO34 - GPIO39`` chỉ thiết lập ở chế độ ``INPUT`` và không có chức năng pullup hoặc pulldown cho phần mềm.
	* Các chân ``GPIO06 - GPIO11`` thường được dùng để giao tiếp với thẻ nhớ ngoài thông qua giao thức SPI nên hạn chế sử dụng để thiết lập IO.

* Dưới đây là ví dụ đơn giản nhấp nháy LED sử dụng board ESP32-Wifi-Uno.

Chuẩn bị.
=========

+-------------------------------+--------------------------------------------+
| **Phần cứng**                 | **Link**                                   |
+===============================+============================================+
| Board ESP32-Wifi-Uno          | https://github.com/esp32vn/esp32-iot-uno   |
+-------------------------------+--------------------------------------------+

Kết nối.
========

    * Trên board ESP32-Wifi-Uno đã có đèn D3 nối với chân GPIO23.

Demo
====
.. youtube:: https://www.youtube.com/watch?v=E0ZU68mlbj4

Hướng dẫn
=========

Tải dự án mẫu:
**************
.. code:: bash

    git clone https://github.com/espressif/esp-idf.git

Include
*******
.. code:: cpp

    #include <stdio.h>
    #include "freertos/FreeRTOS.h"
    #include "freertos/task.h"
    #include "driver/gpio.h"

* ``driver/gpio.h``: Bao gồm cấu hình đầu vào và đầu ra với mục đích cơ bản.

Define
******
.. code:: cpp

    #define BLINK_GPIO 23

* Định nghĩa ``BLINK_GPIO`` là ``GPIO23``.

GPIO
****
.. code:: cpp

    gpio_pad_select_gpio (uint8_t gpio_num);

* Chọn pad làm chức năng GPIO từ IOMUX.

.. code:: cpp

		gpio_set_direction (gpio_num_t gpio_num, gpio_mode_t mode);

* Định hướng GPIO, chẳng hạn như output only, input only, output and input.
* Có 2 đối số được truyền vào hàm:
	* ``gpio_num_t gpio_num``: Lựa chon PIN
		*	``GPIO_NUM_0`` ... ``GPIO_NUM_39``  hoặc ``0`` ... ``39``.
	* ``gpio_mode_t mode``	: Lựa chọn Mode
		* ``GPIO_MODE_INPUT``: input only
		* ``GPIO_MODE_OUTPUT``: output only mode
		* ``GPIO_MODE_OUTPUT_OD``: output only with open-drain mode
		* ``GPIO_MODE_INPUT_OUTPUT_OD``: output and input with open-drain mode
		* ``GPIO_MODE_INPUT_OUTPUT``: output and input mode

.. code:: cpp

    gpio_set_level (gpio_num_t gpio_num, uint32_t level);
* Thiết lập mức (LOW hoặc HIGH) cho GPIO.
* Có 2 đối số được truyền vào hàm:
	* ``gpio_num_t gpio_num``: Lựa chon PIN
		*	``GPIO_NUM_0`` ... ``GPIO_NUM_39``  hoặc ``0`` ... ``39``.
	* ``uint32_t level``	: Lựa chọn mức logic
		* ``0``: Mức thấp
		* ``1``: Mức cao

Make file:
**********
.. code:: bash

    PROJECT_NAME := myProject
    include $(IDF_PATH)/make/project.mk

* ``PROJECT_NAME := myProject`` : Tạo ra một mã nhị phân với tên này tức là - myProject.bin, myProject.elf.

Lập trình
=========
    Bây giờ, bạn có thể xem code hoàn chỉnh.

.. code:: cpp

    /* Blink Example
       This example code is in the Public Domain (or CC0 licensed, at your option.)
       Unless required by applicable law or agreed to in writing, this
       software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
       CONDITIONS OF ANY KIND, either express or implied.
    */
    #include <stdio.h>
    #include "freertos/FreeRTOS.h"
    #include "freertos/task.h"
    #include "driver/gpio.h"

    #define BLINK_GPIO 23

    void blink_task(void *pvParameter)
    {
        /* Configure the IOMUX register for pad BLINK_GPIO (some pads are
           muxed to GPIO on reset already, but some default to other
           functions and need to be switched to GPIO. Consult the
           Technical Reference for a list of pads and their default
           functions.)
        */
        gpio_pad_select_gpio(BLINK_GPIO);
        /* Set the GPIO as a push/pull output */
        gpio_set_direction(BLINK_GPIO, GPIO_MODE_OUTPUT);
        while(1) {
            /* Blink off (output low) */
            gpio_set_level(BLINK_GPIO, 0);
            vTaskDelay(1000 / portTICK_PERIOD_MS);
            /* Blink on (output high) */
            gpio_set_level(BLINK_GPIO, 1);
            vTaskDelay(1000 / portTICK_PERIOD_MS);
        }
    }

    void app_main()
    {
        xTaskCreate(&blink_task, "blink_task", 512, NULL, 5, NULL);
    }

Lưu ý
=====
* Hướng dẫn cài đặt `ESP-IDF <https://esp-idf.readthedocs.io/en/latest/index.html>`_
* Nạp và Debug chương trình `xem tại đây <https://esp-idf.readthedocs.io/en/latest/index.html>`_
* Tài nguyên hệ thống xem `tại đây <https://github.com/espressif/esp-idf>`_
