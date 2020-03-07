Lập trình "Hello World" với ESP32 IDF
-------------------------------------

Giới thiệu
==========
Espressif Internet Development Framework (ESP-IDF) sử dụng FreeRTOS để tận dụng tốt hơn hai bộ xử lý tốc độ cao và quản lý nhiều thiết bị ngoại vi được cài sẵn. Nó được thực hiện bằng cách tạo các tác vụ. Hãy bắt đầu bằng chương trình "Hello world" để hiểu rõ hơn.

Chương trình Hello world sau mỗi 10 giây in ra một chuỗi "Hello world" và hiển thị trên terminal máy tính xuất từ cổng UART của ESP32.

Demo
====
.. youtube:: https://www.youtube.com/watch?v=SxPDVPu8tug

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

Include thư viện
****************
.. code:: cpp

    #include <stdio.h>
    #include "freertos/FreeRTOS.h"
    #include "freertos/task.h"
    #include "esp_system.h"

* ``stdio.h``: Cung cấp cốt lõi của những khả năng nhập trong C. Tập tin này bao gồm họ hàm printf.
* ``freertos/FreeRTOS.h``: Thư viện này bao gồm các thiết lập cấu hình yêu cầu để chạy freeRTOS trên ESP32.
* ``freertos/task.h``: Cung cấp chức năng đa nhiệm. (Chúng tôi sẽ làm đa nhiệm ở các ví dụ sau)
* ``esp_system.h``: Bao gồm cấu hình các thiết bị ngoại vi trong hệ thống ESP. Chức năng của nó như là hệ thống khởi tạo.

Một dự án trông như thế này:

.. code:: bash

    - myProject/
                     - Makefile
                     - sdkconfig
                     - components/ - component1/ - component.mk
                                                 - Kconfig
                                                 - src1.c
                                   - component2/ - component.mk
                                                 - Kconfig
                                                 - src1.c
                                                 - include/ - component2.h
                     - main/       - src1.c
                                   - src2.c
                                   - component.mk

                     - build/

Ví dụ "myProject" chứa các yếu tố sau:

* Một Makefile dự án cấp cao nhất. Makefile này thiết lập biến PROJECT_NAME và (tùy ý) xác định các biến tạo khác trên toàn dự án. Nó bao gồm cốt lõi $(IDF_PATH)/make/project.mk makefile mà thực hiện phần còn lại của hệ thống xây dựng ESP-IDF.
* "Sdkconfig" tập tin cấu hình dự án. Tập tin này được tạo ra / cập nhật khi "make menuconfig" chạy, và giữ cấu hình cho tất cả các thành phần trong dự án (bao gồm esp-idf). Tập tin "sdkconfig" có thể hoặc không thể được thêm vào hệ thống kiểm soát nguồn của dự án.
* Tùy chọn "components" thư mục chứa các thành phần là một phần của dự án. Một dự án không phải chứa các thành phần tùy chỉnh của loại này, nhưng có thể hữu ích cho việc cấu trúc mã tái sử dụng hoặc bao gồm các thành phần bên thứ ba không thuộc ESP-IDF.
* "Main" thư mục là một đặc biệt "pseudo-component" có chứa mã nguồn cho dự án chính nó. "Main" là một tên mặc định, biến SRCDIRS Makefile mặc định này nhưng có thể được đặt để tìm các thành phần giả trong các thư mục khác.
* "build" thư mục là nơi sản xuất xây dựng được tạo ra. Sau khi chạy quá trình make, thư mục này sẽ chứa các tệp đối tượng tạm thời và các thư viện cũng như tệp tin đầu ra nhị phân cuối cùng. Thư mục này thường không được thêm vào kiểm soát nguồn hoặc phân phối với mã nguồn dự án.

Hướng dẫn sửa và tạo make file:
*******************************
.. code:: bash

    PROJECT_NAME := myProject
    include $(IDF_PATH)/make/project.mk

* ``PROJECT_NAME := myProject`` : Tạo ra một mã nhị phân với tên này tức là - myProject.bin, myProject.elf.

Hướng dẫn config, nạp và debug chương trình:
********************************************

.. code:: cpp

    cd ~/esp-idf/examples/get-started/hello_world
    make menuconfig
    make flash
    make moniter

* ``make menuconfig``: Câu lệnh này sẽ hiển hiện ra một menu để cấu hình ESP32 như: Lựa chọn com port, lựa chọn tốc độ baud rate, ...
* ``make flash``: Câu lệnh này biên dịch và đổ chương trình xuống esp32
* ``make monitor``: Câu lệnh này cho phép nạp và debug chương trình
* ``make simple_monitor``: Câu lệnh này cho phép debug chương trình
* ``make help``: Còn nhiều lệnh khác xem trong đây.

Hàm app_main()
**************

``app_main()`` được thực thi sau khi hoàn tất quá trình khởi động chip ESP32.

.. code:: cpp

    void app_main()
    {
        xTaskCreate(&hello_task, "hello_task", 2048, NULL, 5, NULL);
    }

``xTaskCreate()`` để khởi tạo Task, Khi gọi hàm này thì Task mới thực sự được tạo ra.

.. code:: cpp

    xTaskCreate(TaskFunction_t pxTaskCode, const char * pcName, const uint16_t usStackDepth, void *pvParameters, UBaseType_t uxPriority, TaskHandle_t *pxCreatedTask);

* ``pvTaskCode``: con trỏ tới hàm task.
* ``pcName``: là tên đặt cho task.
* ``usStackDepth``: Bộ nhớ stack sẽ được cấp phát cho task, phụ thuộc vào bộ nhớ biến cục bộ định nghĩa trong task và số lần gọi hàm.
* ``pvParameters``: Context đưa vào argument của task.
* ``uxPriority``: giá trị ưu tiên của Task.
* ``pxCreatedTask``: Reference để điều khiển task.

Ngoài ra chúng ta cũng có thể sử dụng hàm này:

.. code:: cpp

    xTaskCreatePinnedToCore(TaskFunction_t pxTaskCode, const char * pcName, const uint16_t usStackDepth, void *pvParameters, UBaseType_t uxPriority, TaskHandle_t *pxCreatedTask, const BasType_t xCoreID)

Hàm ``xTaskCreatePinnedToCore()`` hoạt động giống như xTaskCreate() nhưng có thêm một đối số xCoreID cho phép lựa chọn core vì ESP32 có 2 core.

Ví dụ: Khởi tạo task hoạt động ở core 0

.. code:: cpp

    void app_main()
    {
        xTaskCreatePinnedToCore(&hello_task, "hello_task", 2048, NULL, 5, NULL, 0);
    }


Tác vụ
******

Các chức năng được gọi là từ nhiệm vụ tạo ra ở trên là một chức năng đơn giản như hình dưới đây. Nó chỉ đơn giản là in chuỗi để UART. Dòng in được cấu hình để UART0 ESP32.

.. code:: cpp

    void hello_task(void *pvParameter)
    {
        printf("Hello world!\n");
        for (int i = 10; i >= 0; i--) {
            printf("Restarting in %d seconds...\n", i);
            vTaskDelay(1000 / portTICK_RATE_MS);
        }
        printf("Restarting now.\n");
        esp_restart();
    }

Lập trình
=========
    Bây giờ, bạn có thể xem code hoàn chỉnh.

.. code:: cpp

    /* Hello World Example

       This example code is in the Public Domain (or CC0 licensed, at your option.)

       Unless required by applicable law or agreed to in writing, this
       software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
       CONDITIONS OF ANY KIND, either express or implied.
    */
    #include <stdio.h>
    #include "freertos/FreeRTOS.h"
    #include "freertos/task.h"
    #include "esp_system.h"

    void hello_task(void *pvParameter)
    {
        printf("Hello world!\n");
        for (int i = 10; i >= 0; i--) {
            printf("Restarting in %d seconds...\n", i);
            vTaskDelay(1000 / portTICK_RATE_MS);
        }
        printf("Restarting now.\n");
        fflush(stdout);
        esp_restart();
    }

    void app_main()
    {
        xTaskCreate(&hello_task, "hello_task", 2048, NULL, 5, NULL);
    }

Lưu ý
=====
* Hướng dẫn cài đặt `ESP-IDF <https://esp-idf.readthedocs.io/en/latest/index.html>`_
* Nạp và Debug chương trình `xem tại đây <https://esp-idf.readthedocs.io/en/latest/index.html>`_
* Tài nguyên hệ thống xem `tại đây <https://github.com/espressif/esp-idf>`_
