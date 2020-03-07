*******
Cài đặt
*******

Để phát triển một chương trình cho ESP32 thì bạn cần phải có:

* **Máy tính** đang chạy hệ điều hành Windows, Mac hay Linux.
* **Toonchain** để xây dựng một **chương trình** cho ESP32.
* **ESP-IDF** bao gồm các API cho ESP32 và các lệnh để khởi chạy Toolchain.
* Một trình soạn thảo để viết **chương trình** bằng ngôn ngữ **C**.
* Một board **ESP32**.

.. note::

    Trong loạt bài hướng dẫn ở esp32.vn sẽ sử dụng board ``ESP32-Wifi-Uno``.

Cài đặt Toolchain
=================

Tùy thuộc vào **hệ điều hành** mà máy tính bạn đang sử dụng sẽ có các cách cài đặt khác nhau.

Windowns
--------

Trong Windows chưa sẵn có một môi trường nào để có thể cài đặt **toolchain**. Vì vậy trước khi cài đặt **toolchain** thì cần phải tạo ra môi trường phù hợp, cụ thể hơn ở đây sẽ sử dụng MSYS2_. Để cài đặt tất cả mọi thứ từ MSYS2 và **toolchain** bạn chỉ cần tải một tệp nén từ dl.espressì.com:

https://dl.espressif.com/dl/esp32_win32_msys2_environment_and_toolchain-20170330.zip

Sau khi tải về, giải nén và bỏ vào ổ ``C:\`` (hoặc bất kì đâu bạn muốn), ở đó sẽ có thư mục ``msys32``.

Truy cập vào thư mục ``msys32`` và tìm đến tập tin ``mingw32.exe``. Sẽ mở ra một cửa sổ, tại đây sẽ cung cấp một môi trường gọi là ``bash shell``.



.. _MSYS2: http://www.msys2.org/

MacOS
-----

Trước khi cài đặt toolchain bạn mở terminal và chạy các lệnh sau:

* Cài đặt pip::

    sudo easy_install pip

* Cài đặt pyserial::

    sudo pip install pyserial

Sau khi hoàn thành cài đặt pyserial tiến hành download ESP32 toolchain cho macOS tại:

https://dl.espressif.com/dl/xtensa-esp32-elf-osx-1.22.0-61-gab8375a-5.2.0.tar.gz

Sau khi tải xong, mở terminal và chạy các lệnh dưới đây::
    
    mkdir -p ~/esp
    cd ~/esp
    tar -xzf ~/Downloads/xtensa-esp32-elf-osx-1.22.0-61-gab8375a-5.2.0.tar.gz

Sau khi chạy xong các lệnh trên thì toolchain sẽ được giải nén vào trong ``~/esp/xtensa-esp32-elf/``.

Để sử dụng được, copy dòng sao vào trong ``~/.profile``::

    export PATH=$PATH:$HOME/esp/xtensa-esp32-elf/bin

Sau khi copy vào ``~/.profile`` thì ``xtensa--esp32-elf`` sẽ có sẵn trong terminal khi nó được mở lên. Để sử dụng nó chỉ khi nào cần thì thêm dòng sau vào ``~/.profile`` thay vì dòng trên::
        
    alias get_esp32="export PATH=$PATH:$HOME/esp/xtensa-esp32-elf/bin".

Khi nào cần sử dụng toolchain chỉ cần gõ lệnh ``get_esp32`` và toolchain sẽ được thêm vào ``PATH``.

Linux
-----

Để biên dịch được với **ESP-IDF** cần phải cài đặt các gói sau:

- CentOS 7::

    sudo yum install git wget make ncurses-devel flex bison gperf python pyserial

- Ubuntu and Debian::

    sudo apt-get install git wget make libncurses-dev flex bison gperf python python-serial

- Arch::

    sudo pacman -S --needed gcc git make ncurses flex bison gperf python2-pyserial

Download ESP32 toolchain cho Linux tại:

- Linux 64-bit:

    https://dl.espressif.com/dl/xtensa-esp32-elf-linux64-1.22.0-61-gab8375a-5.2.0.tar.gz

- Linux 32-bit:

    https://dl.espressif.com/dl/xtensa-esp32-elf-linux32-1.22.0-61-gab8375a-5.2.0.tar.gz

Sau khi download xong, mở terminal và chạy các lệnh sau::
    
    mkdir -p ~/esp
    cd ~/esp
    tar -xzf ~/Downloads/xtensa-esp32-elf-linux64-1.22.0-61-gab8375a-5.2.0.tar.gz

Toolchain sẽ được giải nén trong thư mục ``~/esp/xtensa-esp32-elf//``

Để sử dụng được, cần cập nhật ``PATH`` trong ``/.profile``:

- Để ``xtensa-esp32-elf`` luôn có sẵn trong cửa sổ terminal, thêm dòng dưới đây vào tệp ``/.profile``::
    
    export PATH=$PATH:$HOME/esp/xtensa-esp32-elf/bin

- Hoặc sử dụng ``xtensa-esp32-elf`` chỉ khi nào cần thì thêm dòng dưới vào ``/.profile``::

    alias get_esp32="export PATH=$PATH:$HOME/esp/xtensa-esp32-elf/bin"

  Mỗi khi cần sử dụng bạn chỉ cần gõ lệnh get_esp32 thì toolchain sẽ được thêm vào ``PATH``.

.. note::
    
    Nếu bạn thiết lập ``bin/bash`` là login shell, và cả hai tệp ``.bash_profile`` và ``.profile`` cùng tồn tại thì bạn nên thêm vào file ``.bash_profile``.

.. note::
    
    Sau khi thêm vào ``/.profile`` nhớ **log out** và **log in** lại để ghi nhận sự thay đổi.

Tải ESP-IDF
===========

Sau khi đã thiết lập xong toolchain, bạn cũng sẽ cần các API/thư viện dành cho ESP32. Tất cả được cung cấp bởi `Espressif tại ESP-IDF repository <https://github.com/espressif/esp-idf>`_. Để có được, mở teminal và di chuyển tới nơi bạn muốn đặt ESP-IDF và clone nó về bằng dòng lệnh ``git clone``:

Ví dụ::
    
    cd ~/esp
    git clone --recursive https://github.com/espressif/esp-idf.git
    
.. note::
    
    Đừng quên ``--recursive`` trong lệnh ``git clone``. Nếu bạn đã clone ESP-IDF mà không có ``--recursive`` trong đó, chạy một lệnh khác để lấy tất cả Submodules::

        cd ~/esp/esp-idf
        git submodule update --init

.. note::

    Trong khi clone Submodules trong Windows, có thể sẽ báo ``':not a valied identifier...``. Hãy bỏ qua nó và tiếp tục, việc clone về sẽ thành công mà không có bất cứ lỗi nào.

ADD IDF_PATH
============


Để bảo vệ các thiết lập của ``IDF_PATH`` khỏi việc khởi động lại hệ điều hành, cần làm theo các hướng dẫn sau đây.

Windows
-------

1. Truy cập vào ``~/msys32/etc/profile.d/`` và tạo một file mới với tên ``export_idf_path.sh``
2. Xác định đường dẫn tới thư mục ESP-IDF, đường dẫn sẽ có dạng như ``C:\msys32\home\user-name\esp\esp-idf.
3. Thêm dòng sau vào trong ``export_idf_path.sh`` đã tạo ở trên::
    
    export IDF_PATH="C:/msys32/home/user-name/esp/esp-idf"

.. note::

    Thay đổi ``user-name`` phù hợp với máy tính của bạn.

4. Lưu lại
5. Đóng cửa sổ MSYS32 và mở lênh lại. Kiểm tra xem ``IDF_PATH`` đã được thiết lập chưa bằng cách::

    printenv IDF_PATH

   Đường dẫn nhập vào trước đó sẽ được hiện ra nếu tất cả các bước bạn làm chính xác.

Nếu bạn không muốn lưu thiết lập ``IDF_PATH`` thì bạn có thể thiết lập thủ công mỗi khi mở cửa sổ MSYS2::

    export IDF_PATH="C:/msys32/home/user-name/esp/esp-idf"

Sau khi đã làm xong tới đây bạn đã thiết lập xong tất cả mọi thứ để máy tính có thể làm việc được với ESP32. Chúc các bạn thành công!!

Linux and MacOS
---------------

Thiết lập ``IDF_PATH`` bằng cách thêm dòng sau vào ``~/.profile``::

    export IDF_PATH=~/esp/esp-idf

Log off và log in lại để việc thay đổi có hiệu lực.

.. note::
    
    Nếu bạn thiết lập ``bin/bash`` là login shell, và cả hai tệp ``.bash_profile`` và ``.profile`` cùng tồn tại thì bạn nên thêm vào file ``.bash_profile``.

Chạy lệnh sau trong terminal để kiểm tra ``IDF_PATH`` đã được thiết lập::

    printenv IDF_PATH

Đường dẫn thêm vào trước đó sẽ được hiện ra nếu tất cả các bước bạn làm chính xác.

Nếu bạn không muốn lưu thiết lập ``IDF_PATH`` thì bạn có thể thiết lập thủ công trong cửa sổ terminal mỗi khi mở khởi động lại máy tính hoặc khi log out::

    export IDF_PATH=~/esp/esp-idf

Sau khi đã làm xong tới đây bạn đã thiết lập xong tất cả mọi thứ để máy tính có thể làm việc được với ESP32. Chúc các bạn thành công!!
