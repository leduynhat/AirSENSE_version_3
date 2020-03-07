Lập trình GPIO với  ESP32 bằng Arduino IDE.
-------------------------------------------

Giới thiệu.
===========

* Trong ESP32 có tất cả 34 chân GPIO:
	* GPIO 00 - GPIO 19
	* GPIO 21 - GPIO 23
	* GPIO 25 - GPIO 27
	* GPIO 32 - GPIO 39
* Lưu ý:
	* ``Không bao gồm`` các chân ``20, 24, 28, 29, 30 và 31``.
	* Các chân ``GPIO 34 - GPIO39`` chỉ thiết lập ở chế độ ``INPUT``.
	* Các chân ``GPIO 06 - GPIO 11`` thường được dùng để giao tiếp với thẻ nhớ ngoài thông qua giao thức SPI nên hạn chế sử dụng để thiết lập IO.
* Dưới đây là ví dụ đơn giản về thiết lập IO bằng Arduino để nhấp nháy LED cho ESP32 sử dụng board ESP32-Wifi-Uno.

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

Chương trình.
=============
Chạy chương trình Arduino IDE lên và copy toàn bộ code dưới đây vào và save với lại với một tên bất kì.

.. code:: cpp

		int LED = 23;
		int inPin = 18;
		int val = 0;

		void setup(){
			pinMode(LED, OUTPUT);
			pinMode(inPin, INPUT_PULLUP);
		}

		void loop(){
			while(digitalRead(inPin)==val){
				delay(200);
				digitalWrite(LED, LOW);
				val = digitalRead(inPin);
				delay(200);
				digitalWrite(LED, HIGH);
			}
		}

**Lưu ý**
	* Khi chân được thiết lập ``INPUT`` không nối với gì thì mặc định khi trả về sẽ là mức ``HIGH``.
	* Đèn D3 trên board ESP32-Wifi-Uno sẽ sáng khi chân ``OUTPUT`` ở mức ``LOW``.

Nạp chương trình.
=================

Kết nối ``ESP32 IoT Wifi Uno`` với máy tính và thực hiện theo `hướng dẫn tại đây. <https://esp32.vn/hardware/connection.html#cau-hinh-ket-noi>`_

Kết luận.
=========

	* Các chân GPIO trên ESP32 là khá ít và đặc biệt trên board ESP32-Wifi-Uno sẽ còn ít hơn nữa, nên khi muốn điều khiển nhiều hơn thì có thể mở rộng thêm bằng các IC như PCF8574 hoặc MCP23017...
	* Ngoài chức năng xuất các tín hiệu vào ra ở các chân GPIO, thì vẫn còn chức năng đó là xử lý ngắt (interrupt handling). Về việc tìm hiểu chức năng thì sẽ được giới thiệu ở bài viết sau.
