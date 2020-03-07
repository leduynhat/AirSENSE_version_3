CHUYỂN ĐỔI TƯƠNG TỰ SANG SỐ
============================

Tổng quan
---------

ESP32 tích hợp 12-bit SAR (Successive Approximation Register) ADCs và hỗ trợ đo trên 18 chân analog. Một trong số các chân này được dùng để xây dựng bộ khuyếch đại có độ lợi khả trình (programmable gain amplifier) nhằm đo các tín hiệu analog yếu.

.. note::

    SAR (thanh ghi xấp xỉ liên tiếp)  sử dụng thuật toán tìm kiếm nhị phân giúp hội tụ tín hiệu đầu vào, từ đó giúp tăng dung lượng và tốc độ so sánh của ADC

Các API điểu khiển ADC hiện tại chỉ hổ trợ 9 kênh ADC1 (từ GPIO32 đến GPIO39).
Việc đo ADC bao gồm cấu hình ADC với độ chính sác mong muốn , cài đặt độ suy giảm, gọi hàm adc1_get_voltage() để lấy gía trị đo được.

Bạn cũng có thể đọc được gía trị từ cảm biến hiệu ứng Hall thông qua ADC1.

**Ví dụ**

Đọc gía trị điện áp ở kênh 0 của ADC1(GPIO26)

.. code:: cpp

    #include <driver/adc.h>

    ...

        adc1_config_width(ADC_WIDTH_12Bit);
        adc1_config_channel_atten(ADC1_CHANNEL_0,ADC_ATTEN_0db);
        int val = adc1_get_voltage(ADC1_CHANNEL_0);


Đọc tín hiệu từ  cảm biến hiệu ứng Hall

.. code:: cpp

    #include <driver/adc.h>

    ...

        adc1_config_width(ADC_WIDTH_12Bit);
        int val = hall_sensor_read();

Tất cả các gía trị đọc được đều có độ rộng 12 bit (từ 0-4095).

API Reference
-------------

**Một số hàm trong thư viện ``adc.h``**

.. code:: cpp

	esp_err_t  adc1_config_width(adc_bits_width_t width_bit)

Cấu hình độ rộng bit của bộ ADC1.
Cấu hình cho tất cả các kênh ADC1.
Gía trị trả về :
	- ``ESP_Ok`` : thành công
	- ``ESP_ERR_INVALID_ARG`` : lỗi đối số
Các đối số
	- ``width``: Độ rộng bit của ADC1.

****

.. code:: cpp

	esp_err_t adc1_config_channel_atten(adc1_channel_t channel, adc_atten_t atten)

Cấu hình điện áp lấy mẫu cho kênh .

Điện áp mặc định của ADC là 1.1V. Để có thể đọc được các điện áp cao hơn (bằng với điện áp tối đa của chân esp32 là 3.3v) thì cần phải cài đặc độ  suy giảm tín hiệu cho kênh ADC đó.

Chú ý:
	Hàm này cũng được dùng để cấu hình ngõ vào GPIO pin mux để kết nối với kênh ADC1. Vì thế nên hàm này cần được gọi trước khi gọi hàm ``adc_get_voltage()``.
	Khi VDD_A bằng 3v3:
		* Độ suy giảm 0dB (ADC_ATTEN_0db) cung cấp cho điện áp full-scale 1.1V.
		* Độ suy giảm 2.5dB (ADC_ATTEN_2_5db) cung cấp cho điện áp full-scale 1.5V.
		* Độ suy giảm 6dB (ADC_ATTEN_6db) cung cấp cho điện áp full-scale 2.2V.
		* Độ suy giảm 11dB (ADC_ATTEN_11db) cung cấp cho điện áp full-scale 3.9V.( xem chú ý bên dưới)
	chú ý: điện áp full_scale là điện áp tương ứng với mức đọc tối đa ( phụ thuộc vào cấu hình độ rộng bit của ADC1, gía trị này là : 4095 cho 12 bit,2047 cho 11 bit, 1023 cho 10 bit và 511 cho 9 bit).
	Với độ suy giảm 11dB, điện áp tối đa bị giới hạn bởi VDD_a (3.3V) chứ không phải là điện áp full_scale (3.9V).

Gía trị trả về:
	- ``ESP_OK`` : thành công.
	- ``ESP_ERR_INVALID_ARG`` : lỗi đối số
Các đối số:
	- ``channel`` : Kênh ADC được cấu hình
	- ``atten`` : Độ suy giảm.

****

.. code:: cpp

	int adc1_get_voltage(adc1_channel_t channel)

Đọc gía trị trên một kênh của ADC1
Chú ý:
	Gọi hàm ``adc1_config_width ()`` trước khi hàm này được gọi.
	Đối với một kênh nhất định, phải gọi hàm ``adc1_config_channel_atten (channel)`` trước khi hàm này được gọi.
Gía trị trả về :
	- ``-1`` :lỗi đối số.
	- ``gía trị khác`` : kênh ADC1 đang đọc.
Đối số:
	``channel``: Kênh ADC được cấu hình

****

.. code:: cpp

	void adc1_ulp_enable()

Khai báo ADC1 sử dụng `ULP <https://github.com/espressif/esp-idf/tree/master/components/ulp>`_

Hàm này khai báo lại ADC1 để nó có thể được điều khiển bơi ULP. chức năng của hàm này có thể hoàn nguyên bằng cách sử dụng hàm ``adc1_get_voltage.``

Lưu ý rằng cần phải gọi hàm ``adc1_config_channel_atten`` , ``adc1_config_width`` để định cấu hình kênh ADC1, trước khi khai báo ADC1 được sử dụng ULP.

****

.. code:: cpp

	int hall_sensor_read()

DÙng để đọc cảm biến hiệu ứng Hall.

chú ý:
	cảm biến Hall sử dụng từ kênh 0 đến kênh 3 của ADC1 và không được khai báo cấu hình các kênh này để sử dụng như các kênh ADC.
	Module ADC1 phải được bật bằng cách gọi hàm ``adc1_config_width ()`` trước khi gọi hàm ``hall_sensor_read ()`` . ADC1 nên được cấu hình độ rộng 12 bit vì các giá trị của cảm biến của hall khá thấp và không bao gồm đủ độ rộng của ADC.
**Gía trị trả về** : cảm biến Hall đang được đọc.

**Các đối số**


.. code:: cpp

	enum adc_atten_t

Gía trị:
	- ``ADC_ATTEN_0db = 0`` : Điện áp đo được tối đa là 1.1V.
	- ``ADC_ATTEN_2_5db = 1`` : Điện áp đo được tối đa là 1.5V
	- ``ADC_ATTEN_6db = 2`` : Điện áp đo được tối đa là 2.2V
	- ``ADC_ATTEN_11db = 3`` : Điện áp đo được tối đa là 3.3V

****

.. code:: cpp

	enum adc_bit_width_t

Gía trị:
	- ``ADC_WIDTH_9Bit = 0`` :ADC độ rộng 9bit.
	- ``ADC_WIDTH_10Bit = 1`` :ADC độ rộng 10bit.
	- ``ADC_WIDTH_11Bit = 2`` :ADC độ rộng 11bit.
	- ``ADC_WIDTH_12Bit = 3`` :ADC độ rộng 12bit.

****

.. code:: cpp

	enum adc1_channel_t

Gía trị:
	-``ADC1_CHANNEL_0 = 0``     : ADC1 kênh 0 (GPIO36)

	-``ADC1_CHANNEL_1``	    : ADC1 kênh 1 (GPIO37)

	-``ADC1_CHANNEL_2``	    : ADC1 kênh 2 (GPIO38)

	-``ADC1_CHANNEL_3``	    : ADC1 kênh 3 (GPIO39)

	-``ADC1_CHANNEL_4``	    : ADC1 kênh 4 (GPIO32)

	-``ADC1_CHANNEL_5``	    : ADC1 kênh 5 (GPIO33)

	-``ADC1_CHANNEL_6``	    : ADC1 kênh 6 (GPIO34)

	-``ADC1_CHANNEL_7``     : ADC1 kênh 7 (GPIO35)

	-``ADC1_CHANNEL_MAX``

Ví dụ
-----

Chúng ta sẽ thực hiện Demo một chương trình mẫu trong thư mục esp-idf (theo đường dẫn thư mục ``~esp/esp-idf/examples/peripherals/adc``). CHương trình sẽ thực hiện việc đọc ADC ở kênh 0 (GPIO 36) và in ra gía trị đọc được.

lưu ý: thay đổi ``ADC1_TEST_CHANNEL (4)`` thành ``ADC1_TEST_CHANNEL (0)``

**Chuẩn bị**

  +--------------------+----------------------------------------------------------+
  | **Tên board mạch** | **Link**                                                 |
  +====================+==========================================================+
  | Board IoT Wifi Uno | https://github.com/esp32vn/esp32-iot-uno                 |
  +--------------------+----------------------------------------------------------+

**Đấu nối**

Ta kết nối chân GPIO36 của Esp32 Uno với chân nguồn 3v3 ( hoặc bất kì chân nào có tín hiệu ).

**Code**

.. code:: cpp

	#include <stdio.h>
	#include <string.h>
	#include <stdlib.h>
	#include "freertos/FreeRTOS.h"
	#include "freertos/task.h"
	#include "freertos/queue.h"
	#include "driver/gpio.h"
	#include "driver/adc.h"

	#define ADC1_TEST_CHANNEL (0)

	void adc1task(void* arg)
	{
	    // initialize ADC
	    adc1_config_width(ADC_WIDTH_12Bit);
	    adc1_config_channel_atten(ADC1_TEST_CHANNEL,ADC_ATTEN_11db);
	    while(1){
	        printf("The adc1 value:%d\n",adc1_get_voltage(ADC1_TEST_CHANNEL));
	        vTaskDelay(1000/portTICK_PERIOD_MS);
	    }
	}

	void app_main()
	{
	    xTaskCreate(adc1task, "adc1task", 1024*3, NULL, 10, NULL);
	}

**Hướng dẫn config, nạp, debug chương trinh**

Chạy các lệnh dưới đây trên terminal

.. code:: cpp

	$cd ~/esp/esp-idf/examples/peripherals/adc
	$make flash
	$make moniter

**Demo**


Lưu ý
-----
* Hướng dẫn cài đặt `ESP-IDF <https://esp-idf.readthedocs.io/en/latest/index.html>`_
* Nạp và Debug chương trình `xem tại đây <https://esp-idf.readthedocs.io/en/latest/index.html>`_
* Tài nguyên hệ thống xem `tại đây <https://github.com/espressif/esp-idf>`_
