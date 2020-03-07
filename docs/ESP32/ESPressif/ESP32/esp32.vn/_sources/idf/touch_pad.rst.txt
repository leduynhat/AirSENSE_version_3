Lập trình cảm ứng với ESP32 IDF
-------------------------------

Giới thiệu
==========

* ESP32 hỗ trợ đọc lên đến 10 bộ cảm biến cảm ứng điện dung (T0 - T9), được kết nối với các chân GPIO cụ thể:

  * ``TOUCH_PAD_NUM0``: Touch pad channel 0 là ``GPIO4``
  * ``TOUCH_PAD_NUM1``: Touch pad channel 0 là ``GPIO0``
  * ``TOUCH_PAD_NUM2``: Touch pad channel 0 là ``GPIO2``
  * ``TOUCH_PAD_NUM3``: Touch pad channel 0 là ``GPIO15``
  * ``TOUCH_PAD_NUM4``: Touch pad channel 0 là ``GPIO13``
  * ``TOUCH_PAD_NUM5``: Touch pad channel 0 là ``GPIO12``
  * ``TOUCH_PAD_NUM6``: Touch pad channel 0 là ``GPIO14``
  * ``TOUCH_PAD_NUM7``: Touch pad channel 0 là ``GPIO27``
  * ``TOUCH_PAD_NUM8``: Touch pad channel 0 là ``GPIO33``
  * ``TOUCH_PAD_NUM9``: Touch pad channel 0 là ``GPIO32``

* Dưới đây là ví dụ đọc và hiển thị giá trị từ các cảm biến cảm ứng điện dung được kết nối với kênh T0 ``GPIO4`` lập trình trên board ESP32-Wifi-Uno.

* Khi đã được cấu hình, ESP32 liên tục đo điện dung của cảm biến cảm ứng. Điện dung lớn khi ngón tay chạm vào Touch PAD và giá trị đo sẽ nhỏ. Trong trường hợp ngược lại, khi không được chạm, giá trị điện dung nhỏ và giá trị đo lớn.

Chuẩn bị
========

+-------------------------------+--------------------------------------------+
| **Phần cứng**                 | **Link**                                   |
+===============================+============================================+
| Board ESP32-Wifi-Uno          | https://github.com/esp32vn/esp32-iot-uno   |
+-------------------------------+--------------------------------------------+

Hướng dẫn
=========

Tải dự án mẫu:
**************
.. code:: bash

    git clone https://github.com/espressif/esp-idf.git

Include
*******
.. code:: cpp

    #include "freertos/FreeRTOS.h"
    #include "freertos/task.h"
    #include "driver/touch_pad.h"

* ``freertos/FreeRTOS.h``: Thư viện này bao gồm các thiết lập cấu hình yêu cầu để chạy freeRTOS.
* ``freertos/task.h``: Cung cấp chức năng đa nhiệm.
* ``driver/touch_pad.h``: Bao gồm cấu hình, thiết lập liên quan đến cảm ứng. Chức năng của nó như là hệ thống khởi tạo.

API
***
.. code:: cpp

		touch_pad_init();

* Chức năng khởi tạo module touch pad , kích hoạt module cảm ứng hoạt động.

.. code:: cpp

		touch_pad_read(touch_pad_t touch_num, uint16_t * touch_value);

* Có 1 đối số được truyền vào hàm và 1 con trỏ trả về giá trị:

  * ``touch_pad_t touch_num``: Lựa chon kênh cảm ứng

    *	``TOUCH_PAD_NUM0`` ... ``TOUCH_PAD_NUM9``  hoặc ``0`` ... ``9``.

  * ``uint16_t * touch_value``	: Giá trị trả về  kiểu số nguyên 16 bit của cảm biến.

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

    /* Touch Pad Read Example

      This example code is in the Public Domain (or CC0 licensed, at your option.)

      Unless required by applicable law or agreed to in writing, this
      software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
      CONDITIONS OF ANY KIND, either express or implied.
    */
    #include <stdio.h>
    #include "freertos/FreeRTOS.h"
    #include "freertos/task.h"

    #include "driver/touch_pad.h"

    /*
      Read values sensed at T0 (GPIO4) available touch pads.
      Print out values in a loop on a serial monitor.
    */
    static void tp_example_read_task(void *pvParameter)
    {
      while (1) {
          uint16_t touch_value;

          ESP_ERROR_CHECK(touch_pad_read(TOUCH_PAD_NUM0, &touch_value));
          printf("T:%4d \n", touch_value);
          vTaskDelay(500 / portTICK_PERIOD_MS);
      }
    }

    void app_main()
    {
      // Initialize touch pad peripheral
      touch_pad_init();

      // Start task to read values sensed by pads
      xTaskCreate(&tp_example_read_task, "touch_pad_read_task", 2048, NULL, 5, NULL);
    }

Hướng dẫn config, nạp và debug chương trình:
********************************************

.. code:: cpp

    make menuconfig
    make flash
    make moniter

Hiển thị một số giá trị thu được trên terminal:
***********************************************

.. code:: cpp

    T0:1043
    T0:1107
    T0:1109
    T0:17
    T0:19
    T0:11
    T0:999
    T0:1104
    T0:1103
    T0:1102

Lưu ý
=====
* Hướng dẫn cài đặt `ESP-IDF <https://esp-idf.readthedocs.io/en/latest/index.html>`_
* Nạp và Debug chương trình `xem tại đây <https://esp-idf.readthedocs.io/en/latest/index.html>`_
* Tài nguyên hệ thống xem `tại đây <https://github.com/espressif/esp-idf>`_
