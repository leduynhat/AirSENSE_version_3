ESP32 Development Hardware
==========================

Openhardware ``ESP32-IoT-Uno`` :

Hardware features
*****************

- ESP32 WiFi, Bluetooth LE SoC - 240Mhz - Module ESP-WROOM-32
- Automatic select 3 power sources (DC6-28V, USB and Battery)
- Auto download Flash mode
- Integrated SDCARD slot (support 1-bit mode)
- Open hardware design with KiCad, CC-BY-SA license.
- I2C OLED display header
- Lithium-Ion Battery Charger
- 1 Reset button, 1 programable button
- 1 Power LED, 1 programable LED, 1 Charger LED
- Compatible with Shields for ESP32 in the future (Gateway - GSM/GPRS/GPS and Lora Shield, Connectivity - CAN, RS485, RS232 Shield, Audio Shield, ...)

Github repository
*****************

- `https://github.com/esp32vn/esp32-iot-uno <https://github.com/esp32vn/esp32-iot-uno>`_

Pinout
******

.. image:: _static/Esp32-boad__1_.png
   :width: 800

Mạch nguyên lý
**************

.. image:: _static/sch.png
   :width: 800

.. toctree::
   :caption: Phần cứng
   :maxdepth: 2

   Phần cứng <hardware/index>

.. toctree::
   :caption: ESP32 Arduino
   :maxdepth: 3

   Arduino cơ bản <arduino/index>

.. toctree::
   :caption: ESP32 IDF
   :maxdepth: 3

   ESP-IDF cơ bản <idf/index>
   Thẻ nhớ <idf/sdcard>

.. toctree::
   :caption: Network/Protocol
   :maxdepth: 2


.. toctree::
   :caption: API
   :maxdepth: 1

.. toctree::
   :caption: Tài liệu ESP32

.. toctree::
   :caption: Contributing
   :maxdepth: 1

   Hướng dẫn đóng góp <contributing>



Indices
=======

* :ref:`genindex`
