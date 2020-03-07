Arduino cơ bản
--------------

Có thể tìm thấy hướng dẫn cài đặt Tiếng Anh và được cập nhật tại `Arduino for ESP32`_


Chuẩn bị
********

- `Arduino 1.6.8`_ (hay mới hơn, nếu bạn biết bạn đang làm gì)
- git
- python 2.7
- terminal, console, or command prompt (phụ thuộc hệ điều hành của bạn)
- Kết nối Internet

Hướng dẫn
*********

Cài đặt cho Debian/Ubuntu Linux
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

- Clone repository đến thư mục ``hardware/esp32com``. Hoặc bạn có thể clone vào nơi nào đó và tạo symlink bằng ``ln -s``, nếu Hệ điều hành hỗ trợ::

    cd hardware
    mkdir esp32com
    cd esp32com
    git clone https://github.com/espressif/arduino-esp32

Cấu trúc dự án sau khi bạn thực hiện xong::

    Arduino
        |- hardware
        |- esp32com
            |- arduino-esp32
                |- cores
                |- doc
                |- libraries
                |- package
                |- tools
                |- variants
                |- package.json
                |- platform.txt
                |- programmers.txt
                |- README.md
                |- boards.txt
                |- component.mk
                |- Kconfig
                `- Makefile.projbuild

- Tải binary tools::

    cd tools/
    python get.py

- Khởi động lại Arduino IDE.

Cài đặt cho Mac
^^^^^^^^^^^^^^^

- Mở ``terminal``, copy và past các command sau ::

    mkdir -p ~/Documents/Arduino/hardware/espressif && \
    cd ~/Documents/Arduino/hardware/espressif && \
    git clone https://github.com/espressif/arduino-esp32.git esp32 && \
    cd esp32/tools/ && \
    python get.py

- Khởi động lại Arduino IDE.

Cài đặt cho Windows
^^^^^^^^^^^^^^^^^^^

- Mở ``terminal`` và cd đến thư mục Arduino. Nó có thể là thư mục *sketchbook* (thường là ``<Documents>/Arduino``), hay thư mục của Ứng dụng Arduino, tùy bạn chọn.

- Clone/download repository https://github.com/espressif/arduino-esp32.git này đến thư mục ``hardware/esp32com`` 

- Thường là ``C:/Users/[YOUR_USER_NAME]/Documents/Arduino/hardware/espressif/esp32``

- Hoặc thay đổi *sketchbook* trên phù hợp với thư mục đã cài đặt Ứng dụng Arduino phù hợp trên máy. 

- Mở ``C:/Users/[YOUR_USER_NAME]/Documents/Arduino/hardware/esp32/tools`` và double-click vào ``get.exe``

.. image:: ../_static/win-gui-get-exe.png
    :target: _static/win-gui-get-exe.png
    :width: 800

- Khi ``get.exe`` thực hiện xong, cấu trúc có thể thấy như sau: 

.. image:: ../_static/win-gui-get-exe2.png
    :target: _static/win-gui-get-exe2.png
    :width: 800

- Khởi động lại Arduino IDE.

.. _`Arduino for ESP32`: https://github.com/espressif/arduino-esp32
.. _`Arduino 1.6.8`: https://www.arduino.cc/en/Main/OldSoftwareReleases#previous
