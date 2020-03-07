Kết nối phần cứng và cài đặt
----------------------------
Kết nối phần cứng
=================
Kết nối USB với Board mạch **ESP32 WiFi Uno** như hình bên dưới, đảm bảo đèn LED nguồn sáng.

.. image:: ../_static/connection-board.jpg
    :target: ../_static/connection-board.jpg
    :width: 800

Cài đặt USB Driver
==================
Tùy thuộc từng hệ điều hành mà chọn và tải bản cài đặt `CP210x USB to UART`_ phù hợp.


Cấu hình kết nối
=================

* Sau khi cài đặt xong và kết nối, sẽ xuất hiện cổng COM ảo trên máy tính (Tùy từng loại hệ điều hành mà có những tên cổng như: ``COM1, COM2 ...`` đối với Windows, ``/dev/tty.wchusbserial1420`` trên Mac OS), ``/dev/ttyUSB0`` trên Ubuntu Linux)

* Mở Arduino IDE và lựa chọn board (tham khảo cấu hình kết nối như hình dưới):

    * Board: ``ESP32vn IoT Uno``
    * Flash Frequency: ``80MHz``
    * Port: chọn cổng khi gắn thiết bị vào sẽ thấy xuất hiện
    * Upload speed: Chọn cao nhất, nếu nạp không được chọn thấp dần

.. image:: ../_static/setup-board.png
    :target: ../_static/setup-board.png
    :width: 800

.. _`CP210x USB to UART`: http://www.silabs.com/products/development-tools/software/usb-to-uart-bridge-vcp-drivers