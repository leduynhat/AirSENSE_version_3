CHUYỂN ĐỔI SỐ SANG TƯƠNG TỰ
===========================

Tổng quan
----------

ESP32 có 2 kênh DAC 8 bit là GPIO25 ( kênh 1) và GPIO26 (kênh 2)

Bộ điều khiển DAC cho phép các kênh này được thiết lập điện áp tùy ý.

Các kênh DAC này cũng được dùng để điều khiển với kiểu DMA được viết với dữ liệu mẫu thông qua ``I2S driver``  khi sử dụng chế độ ``build-in DAC``

API Reference
-------------

**Thư viện**

``#include <driver/dac.h>``

**Các hàm trong thư viện dac.h**

.. code:: cpp
	
	esp_err_t dac_output_voltage(dac_channel_t channel, uint8_t dac_value)

Dùng để thiết lập điện áp ngõ ra DAC.

Ngõ ra DAC 8-bit nên gía trị tối đa là 255 tương đương với VDD.

**chú ý:** cần phải khai báo chân DAC trước khi gọi hàm này. Kênh DAC 1 ứng với chân GPIO25 và DAC2 ứng với chân GPIO26.

**Gía trị trả về**

	* ``ESP_OK`` : khai báo thành công
	* ``ESP_ERR_INVALID_ARG`` : lỗi đối số

**Các đối số**

	* ``chanel`` : Kênh DAC được cấu hình
	* ``dac_value`` : gía trị ngõ ra DAC

****

.. code:: cpp

	esp_err_t dac_output_enable(dac_channel_t channel)

Cho phép ngõ ra DAC

**chú ý:** DAC kênh 1 tương ứng với GPIO25, còn DAC kênh 2 tương ứng với GPIO26.Kênh I2S bên trái sẽ được ánh xạ tới DAC kênh 2,kênh I2S  bên phải sẽ được ánh xạ tới DAC kênh 1. 

	* ``channel``: lựa chọn kênh DAC

****

.. code:: cpp

	esp_err_t dac_output_disable(dac_channel_t channel)

Vô hiệu hóa ngõ ra kênh DAC.

**Các đối số**

	* ``channel``: lựa chọn kênh DAC

****

.. code:: cpp

	esp_err_t dac_i2s_enable()

Cho phép dữ liệu ngõ ra DAC từ I2S

****

.. code:: cpp

	esp_err_t dac_i2s_disable()

Vô hiệu hóa dữ liệu ngõ ra DAC từ I2S

**CÁC ĐỐI SỐ**

.. code:: cpp

	enum dac_channel_t

**Gía trị**
	* ``DAC_CHANNEL_1 = 1`` :lựa chọn DAC kênh 1 (GPIO25)
	* ``DAC_CHANNEL_2`` :lựa chọn DAC kênh 1 (GPIO26)
	* ``DAC_CHANNEL_MAX``

Ví dụ minh họa
--------------

Yêu cầu: Cài đặt DAC kênh 1 (GPIO25) với mức điện áp ngõ ra xấp xỉ 0,78xVDD_A (VDD*200/255) và DAC kênh 2 (GPIO26) với mức điện áp ngõ ra xấp xỉ 0,39xVDD_A (VDD*100/255). Hai điện áp này được đo bằng ADC kênh 0 (GPIO36).

**Chuẩn bị**

  +--------------------+----------------------------------------------------------+
  | **Tên board mạch** | **Link**                                                 |
  +====================+==========================================================+
  | Board IoT Wifi Uno | https://github.com/esp32vn/esp32-iot-uno                 |
  +--------------------+----------------------------------------------------------+

**Đấu nối:**

Ta sẽ lần lượt kết nối chân GPIO 36 với chân GPIO 25 và GPIO26.

**Lập trình**

    bạn có thể xem code hoàn chỉnh.

.. code:: cpp

  #include <stdio.h>
  #include <stdlib.h>
  #include "freertos/FreeRTOS.h"
  #include "freertos/task.h"
  #include "esp_wifi.h"
  #include "esp_event_loop.h"
  #include "esp_system.h"
  #include "nvs_flash.h"
  #include "driver/dac.h"
  #include "driver/adc.h"

  void dac_out_task(void *pvParameters)
  {
    static uint8_t i = 0;
    while (1) {     dac_out_voltage(DAC_CHANNEL_1,200);
    dac_out_voltage(DAC_CHANNEL_2,100);

      printf("ESP32 ADC1_CH0 (IO36) = %d\n", adc1_get_voltage(ADC1_CHANNEL_0));
      vTaskDelay(4000 / portTICK_PERIOD_MS);
    }
  }

  void app_main()
  {
    nvs_flash_init();

    adc1_config_width(ADC_WIDTH_12Bit);
    adc1_config_channel_atten(ADC1_CHANNEL_0,ADC_ATTEN_11db);

    printf("Welcome to Noduino Quantum\r\n");
    printf("Try to output a voltage through GPIO25/26... \r\n");
    xTaskCreatePinnedToCore(&dac_out_task, "dac_out_task", 2048, NULL, 5,
        NULL, 0);
  }

**Hướng dẫn sửa và tạo make file:**

.. code:: cpp

  PROJECT_NAME := myProject
  include $(IDF_PATH)/make/project.mk

* PROJECT_NAME := myProject : Tạo ra một mã nhị phân với tên này tức là - myProject.bin, myProject.elf.

**Hướng dẫn config, nạp và debug chương trình**

.. code:: cpp

  cd (đường dẫn đến thư mục chứa project)  vd:cd ~/esp/esp-idf/exambles/peripherals/dac
  make menuconfig
  make flash
  make moniter

**Demo**

Lưu ý
-----

* Hướng dẫn cài đặt `ESP-IDF <https://esp-idf.readthedocs.io/en/latest/index.html>`_
* Nạp và Debug chương trình `xem tại đây <https://esp-idf.readthedocs.io/en/latest/index.html>`_
* Tài nguyên hệ thống xem `tại đây <https://github.com/espressif/esp-idf>`_
