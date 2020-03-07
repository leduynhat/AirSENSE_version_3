Lập trình nút nhấn với ESP32 bằng ESP-IDF
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

* Dưới đây là ví dụ đơn giản bật tắt LED sử dụng nút nhấn lập trình trên board ESP32-Wifi-Uno.

Chuẩn bị.
=========

+-------------------------------+--------------------------------------------+
| **Phần cứng**                 | **Link**                                   |
+===============================+============================================+
| Board ESP32-Wifi-Uno          | https://github.com/esp32vn/esp32-iot-uno   |
+-------------------------------+--------------------------------------------+

Kết nối.
========

    * Trên board ESP32-Wifi-Uno có đèn D3 nối với chân số GPIO23 và nút nhấn nối với chân GPIO18.

Demo
====
.. youtube:: https://www.youtube.com/watch?v=d7jUb5BKkmg

Hướng dẫn
=========

Dự án mẫu:
**********
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

		#define LED_GPIO 23
		#define BTN_GPIO 18

* Định nghĩa ``LED_GPIO 23`` là ``GPIO23``.
* Định nghĩa ``BTN_GPIO 18`` là ``GPIO18``.

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

		gpio_set_pull_mode (gpio_num_t gpio_num, gpio_pull_mode_t pull);

* Sử dụng chức năng này để configure GPIO pull mode, chẳng hạn như pull-up, pull-down.
* Hàm này có 2 đối số được truyền vào:
	* ``gpio_num_t gpio_num``: Lựa chon PIN
	* ``gpio_pull_mode_t pull``	: Lựa chon chế độ
		* ``GPIO_PULLUP_ONLY``: Pad pull up
		* ``GPIO_PULLDOWN_ONLY``: Pad pull down
		* ``GPIO_PULLUP_PULLDOWN``: Pad pull up and pull down
		* ``GPIO_FLOATING``: Pad floating

.. code:: cpp

    gpio_get_level (gpio_num_t gpio_num);

* Hàm này trả về mức logic giá trị đầu vào:
	* ``0``: nếu ngõ vào là mức thấp.
	* ``1``: nếu ngõ vào là mức cao.

.. code:: cpp

		gpio_set_direction (gpio_num_t gpio_num, gpio_mode_t mode);

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

		/* Button Example

		   This example code is in the Public Domain (or CC0 licensed, at your option.)

		   Unless required by applicable law or agreed to in writing, this
		   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
		   CONDITIONS OF ANY KIND, either express or implied.
		*/
		#include <stdio.h>
		#include "freertos/FreeRTOS.h"
		#include "freertos/task.h"
		#include "driver/gpio.h"

		#define LED_GPIO 23
		#define BTN_GPIO 18

		void button_task(void *pvParameter)
		{
		    /* Configure the IOMUX register for pad LED_GPIO, BTN_GPIO (some pads are
		       muxed to GPIO on reset already, but some default to other
		       functions and need to be switched to GPIO. Consult the
		       Technical Reference for a list of pads and their default
		       functions.)
		    */
		    gpio_pad_select_gpio(LED_GPIO);
		    gpio_pad_select_gpio(BTN_GPIO);

		    /* Set the GPIO as a push/pull output */
		    gpio_set_direction(LED_GPIO, GPIO_MODE_OUTPUT);

		    gpio_set_direction(BTN_GPIO, GPIO_MODE_INPUT);
		    gpio_set_pull_mode(BTN_GPIO, GPIO_PULLUP_ONLY);

		    while(1) {
		        if (gpio_get_level(BTN_GPIO) == 0) {
		        	gpio_set_level(LED_GPIO, 0);
		        }
		        else
		        	gpio_set_level(LED_GPIO, 1);
		    }
		}

		void app_main()
		{
		    xTaskCreate(&button_task, "button_task", 512, NULL, 5, NULL);
		}

Hướng dẫn config, nạp và debug chương trình:
********************************************

.. code:: cpp

    make menuconfig
    make flash
    make moniter

Lưu ý
=====
* Hướng dẫn cài đặt `ESP-IDF <https://esp-idf.readthedocs.io/en/latest/index.html>`_
* Nạp và Debug chương trình `xem tại đây <https://esp-idf.readthedocs.io/en/latest/index.html>`_
* Tài nguyên hệ thống xem `tại đây <https://github.com/espressif/esp-idf>`_
