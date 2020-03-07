WIFI
++++

Tổng quan
---------

Ví dụ
-----

**Dưới đây là ví dụ đơn giản về kết nối ESP32 vào một mạng Wifi**

Chuẩn bị

    +---------------------------------+-----------------------------------------------------------+
    |                                 | **Link**                                                  |
    +=================================+===========================================================+
    | Board ESP32 IoT Uno             | https://github.com/esp32vn/esp32-iot-uno                  |
    +---------------------------------+-----------------------------------------------------------+
    + Code mẫu                        | https://github.com/espressif/esp-idf-template             |
    +---------------------------------+-----------------------------------------------------------+

Sau khi clone ``esp-idf-template`` về vào truy cập theo đường dẫn ``esp-idf-template\main\`` và mở file ``main.c``. Sau đó thay đổi các giá trị:

    * **ssid** = {Tên Wifi}
    * **password** = {Mật khẩu của Wifi}

Save lại và thực hiện lệnh ``make flash`` để nạp chương trình vào ``ESP32``. Để xem kết quả dùng lệnh ``make monitor``, nếu kết nối thành công sẽ như hình dưới đây:

.. image:: ../_static/Wifi_connected.png
    :target: ../_static/Wifi_connected.png
    :align: center

API Reference
-------------

Header file
===========

* `esp-idf/components/esp32/include/esp32-wifi.h <https://github.com/espressif/esp-idf/blob/969f1bb/components/esp32/include/esp_wifi.h>`_

Một số hàm tham khảo
====================

.. c:function:: esp_err_t esp_wifi_init(wifi_init_config_t *config)

    Cài đặt Wifi Alloc cho các trình điều khiển wifi, chẳng hạn như cấu trúc điều khiển Wifi, bộ đệm RX/TX, cấu trúc Wifi NVS..., cũng như bắt đầu task Wifi

    Chú ý:
        1. API này phải được gọi đầu tiên trước khi các API khác được gọi
        2. Luôn sử dụng WIFI_INIT_CONFIG_DEFAULT để khởi tạo các giá trị mặc định. Nếu muốn thay đổi giá trị nào chỉ việc ghi đè lên giá trị mặc định, và nhớ field 'magic' của wifi_init_config_t luôn là WIFI_INIT_CONFIG_MAGIC!
    
    Return:
        * ESP_OK: Thành công
        * ESP_ERR_WIFI_NO_MEM
        * others: tham khảo mã lỗi trong esp_err.h
    
    Tham số:
        * ``config``: cung cấp cấu hình Wifi

.. c:function:: esp_err_t esp_wifi_deinit(void)

    Hủy bỏ cấu hình Wifi đã khởi tạo bằng esp_wifi_init và dừng task Wifi

    Chú ý:
        1. Lệnh này chỉ nên được gọi khi không muốn sử dụng Wifi nữa

    Return:
        ESP_OK: Thành công

.. c:function:: esp_err_t esp_wifi_set_mode(wifi_mode_t mode)

    Đưa Wifi vào chế độ hoạt động, có ba chế độ hoạt động là ``station``, ``soft-AP`` hoặc ``station+soft-AP``. Nếu không đề cập đến thì mặc định sẽ là ``soft-Ap``

    Trả về:
        * ESP_OK: Thành công
        * ESP_ERR_WIFI_NOT_INIT: Wifi không được khởi tạo bởi esp_wifi_init
        * ESP_ERR_WIFI_ARG: Đối số không hợp lệ
        * others: tham khảo mã lỗi trong esp_err.h

    Tham số:
        * ``mode``: lưu trữ chế độ hoạt động hiện tại của Wifi

.. c:function:: esp_err_t esp_wifi_start(void)

    Kích hoạt Wifi dược trên cấu hình đã thiết lập ở trên. Nếu ``mode`` là WIFI_MODE_STA sẽ tạo ra khối station control, nếu ``mode`` là WIFI_MODE_AP sẽ tạo ra khối soft-AP control, nếu ``mode`` là WIFI_MODE_APSTA sẽ tạo ra cả hai khối soft-AP và station control.

    Return:
        * ESP_OK: Thành công
        * ESP_ERR_WIFI_NOT_INIT: Wifi không được khởi tạo bởi esp_wifi_init
        * ESP_ERR_WIFI_ARG: Đối số không hợp lệ
        * ESP_ERR_WIFI_NO_MEM: tràn bộ nhớ
        * ESP_ERR_WIFI_CONN: lỗi thiết lập khối station hoặc soft-AP control sai
        * ESP_ERR_WIFI_FAIL: lỗi thiết lập khác

.. c:function:: esp_err_t esp_wifi_stop(void)

    Dừng Wifi lại và giải phóng các khối đã tạo ra.

    Return:
        * ESP_OK: Thành công
        * ESP_ERR_WIFI_NOT_INIT: Wifi không được khởi tạo bởi esp_wifi_init

.. c:function::esp_err_t esp_wifi_restore(void)

    Khôi phục các thiết lập vè giá trị mặc định
    Hàm này sẽ đặt lại các thiết lập được tạo ra bới các API:

        * esp_wifi_get_auto_connect,
        * esp_wifi_set_protocol,
        * esp_wifi_set_config related
        * esp_wifi_set_mode
    
    Trả về:
        * ESP_OK: Thành công
        * ESP_ERR_WIFI_NOT_INIT: Wifi không được khởi tạo bởi esp_wifi_init

.. c:function:: esp_err_t esp_wifi_connect(void)

    Kết nối station Wifi của ESP32 tới một AP

    Chú ý:
        1. Hàm này chỉ hoạt động khi ở chế độ ``station`` hoặc ``station+soft-AP``
        2. Nếu ESP32 kếu nối thành công tới một AP, sử dụng esp_wifi_disconnect để ngắt kết nối
    
    Return:
        * ESP_OK: Thành công
        * ESP_ERR_WIFI_NOT_INIT: Wifi không được khởi tạo bởi esp_wifi_init
        * ESP_ERR_WIFI_NOT_STARTED: Wifi chưa được kích hoạt bởi esp_wifi_start
        * ESP_ERR_WIFI_CONN: lỗi thiết lập khối station hoặc soft-AP control sai
        * ESP_ERR_WIFI_SSID: SSID của AP không chính xác

.. c:function::esp_err_t esp_wifi_disconnect(void)

    Ngắt kết nối Wifi tới AP

    Return:
        * ESP_OK: Thành công
        * ESP_ERR_WIFI_NOT_INIT: Wifi không được khởi tạo bởi esp_wifi_init
        * ESP_ERR_WIFI_NOT_STARTED: Wifi chưa được kích hoạt bởi esp_wifi_start
        * ESP_ERR_WIFI_FAIL: lỗi thiết lập khác

.. c:function:: esp_err_t esp_wifi_deauth_sta(uint16_t aid)

    Deauthenticate tất cả các trạm hoặc id được kết nối tới

    Return:
        * ESP_OK: Thành công
        * ESP_ERR_WIFI_NOT_INIT: Wifi không được khởi tạo bởi esp_wifi_init
        * ESP_ERR_WIFI_NOT_STARTED: Wifi chưa được kích hoạt bởi esp_wifi_start
        * ESP_ERR_WIFI_ARG: Đối số không hợp lệ
        * ESP_ERR_WIFI_MODE: chế độ không hợp lệ

    Tham số:
        * ``aid``: when ``aid`` là 0, deauthenticate tất cả các station, nếu không thì deauthenticate trạm có id được kết nối tới

.. c:function:: esp_err_t esp_wifi_scan_start(wifi_scan_config_t *config, bool block)

    Quét tất cả các AP xung quanh

    Chú ý:
        Nếu API này được gọi, thì các AP tìm thấy sẽ được lưu trong  bộ nhớ động được cấp phát và sẽ được giải phóng trong esp_wifi_scan_get_ap_records, do đó, hãy gọi esp_wifi_scan_get_ap_records để giải phóng bộ nhớ sau khi quét

    Chú ý:
        Giá trị tối đa của thời gian quét của cả chủ động và bị động là 1,5s. Các giá trị lớn hơn có thể làm cho statin ngắt kết nối tới AP và nó không được khuyến khích.
    
    Return:
        * ESP_OK: Thành công
        * ESP_ERR_WIFI_NOT_INIT: Wifi không được khởi tạo bởi esp_wifi_init
        * ESP_ERR_WIFI_NOT_STARTED: Wifi chưa được kích hoạt bởi esp_wifi_start
        * ESP_ERR_WIFI_TIMEOUT: Quá thời gian quét
        * others: tham khảo mã lỗi trong esp_err.h
    
    Tham số:
        * ``config``: cấu hình cho hoạt động quét
        * ``block``: Nếu block là true, API này sẽ chặn hàm gọi nó cho đến khi quét xong, nếu không sẽ quay lại ngay lập tức

.. c:function:: esp_err_t esp_wifi_scan_stop(void)

    Kết thúc quá trình quét
    
    Return:
        * ESP_OK: Thành công
        * ESP_ERR_WIFI_NOT_INIT: Wifi không được khởi tạo bởi esp_wifi_init
        * ESP_ERR_WIFI_NOT_STARTED: Wifi chưa được kích hoạt bởi esp_wifi_start

.. c:function:: esp_err_t esp_wifi_scan_get_ap_num(uint16_t *number)

    Lấy số lượng của các AP trong lần quét cuối cùng

    Chú ý:
        API này chỉ có thể gọi sau khi quét hoàn tất, nếu không có thể lấy sai giá trị.
    
    Return:
        * ESP_OK: Thành công
        * ESP_ERR_WIFI_NOT_INIT: Wifi không được khởi tạo bởi esp_wifi_init
        * ESP_ERR_WIFI_NOT_STARTED: Wifi chưa được kích hoạt bởi esp_wifi_start
        * ESP_ERR_WIFI_ARG: Đối số không hợp lệ

    Tham số:
    * ``number``: lưu số lượng các AP tìm thấy trong lần quét cuối cùng 

.. c:function:: esp_err_t esp_wifi_scan_get_ap_records(uint16_t *number, wifi_ap_record_t *ap_records)

    Lấy danh sách Ap tìm thấy trong lần quét cuối cùng

    Return:
        * ESP_OK: Thành công
        * ESP_ERR_WIFI_NOT_INIT: Wifi không được khởi tạo bởi esp_wifi_init
        * ESP_ERR_WIFI_NOT_STARTED: Wifi chưa được kích hoạt bởi esp_wifi_start
        * ESP_ERR_WIFI_ARG: Đối số không hợp lệ
        * ESP_ERR_WIFI_NO_MEM: tràn bộ nhớ

    Tham số:
        * ``number``: Là một tham số đầu vào, lưu trữ tối đa số AP ``ap_record`` có thể. Là một tham số đầu ra, nhận số AP thực tế mà API trả về
        * ``ap_records``: mảng wifi_ap_record_t chứa các API tìm thấy

.. c:function::esp_err_t esp_wifi_sta_get_ap_info(wifi_ap_record_t *ap_info)

    Lấy thông tin của AP mà ESP32 station kết nối tới.

    Return:
        * ESP_OK: Thành công
        * others: tham khảo mã lỗi trong esp_err.h

    Tham số:
        * ``ap_info``: wifi_ap_record_t chứa thông tin AP

.. c:function:: esp_err_t esp_wifi_set_ps(wifi_ps_type_t type)

    Thiết lập kiểu tiết kiệm năng lượng

    Chú ý:
        Mặc định kiểu tiết kiệm năng lượng là WIFI_PS_NONE    
    Return:
        ESP_ERR_WIFI_NOT_SUPPORT: chưa hỗ trợ
    
    Tham số:
        * ``type``: kiểu tiết kiệm năng lượng

.. c:function:: esp_err_t esp_wifi_get_ps(wifi_ps_type_t *type)

    Nhận kiểu tiết kiệm năng lượng

    Chú ý:
        Mặc định kiểu tiết kiệm năng lượng là WIFI_PS_NONE

    Return:
        ESP_ERR_WIFI_NOT_SUPPORT: chưa hỗ trợ
    
    Tham số:
        * ``type``: kiểu tiết kiệm năng lượng

.. c:function:: esp_err_t esp_wifi_set_protocol(wifi_interface_t ifx, uint8_t protocol_bitmap)

    Thiết lập giao thức kiểu specificed interface 
    Giao thức mặc định là (WIFI_PROTOCOL_11B | WIFI_PROTOCOL_11G | WIFI_PROTOCOL_11N)

    Chú ý:
        Hiện tại chỉ hỗ trợ 802.11b hoặc 802.11bg hoặc 802.11bgn
    
    Return:
        * ESP_OK: Thành công
        * ESP_ERR_WIFI_NOT_INIT: Wifi không được khởi tạo bởi esp_wifi_init
        * ESP_ERR_WIFI_IF: giao thức không hợp lệ
        * others: tham khảo mã lỗi trong esp_err.h

    Tham số:
        * ``ix``: giao diện
        * ``protocol_bitmap``: Wifi protocol bitmap

.. c:function:: esp_err_t esp_wifi_get_protocol(wifi_interface_t ifx, uint8_t *protocol_bitmap)
    
    Lấy protocol bitmap hiện tại của specificed interface

    Return:
        * ESP_OK: Thành công
        * ESP_ERR_WIFI_NOT_INIT: Wifi không được khởi tạo bởi esp_wifi_init
        * ESP_ERR_WIFI_IF: giao diện không hợp lệ
        * ESP_ERR_WIFI_ARG: Đối số không hợp lệ
        * others: tham khảo mã lỗi trong esp_err.h
    
    Tham số:
        * ``ix``: giao diện
        * ``protocol_bitmap``: Wifi protocol bitmap

.. c:function:: esp_err_t esp_wifi_set_bandwidth(wifi_interface_t ifx, wifi_bandwidth_t bw)

    Thiết lập băng thông của giao thức ESP32 được chỉ định

    Chú ý:
        1. API trả về false nếu cố gắng cấu hình một giao thức không cho phép
        2. WIFI_BW_HT40 chỉ hỗ trợ khi giao thức hỗ trợ 11N

    Return:
        * ESP_OK: Thành công
        * ESP_ERR_WIFI_NOT_INIT: Wifi không được khởi tạo bởi esp_wifi_init
        * ESP_ERR_WIFI_IF: giao diện không hợp lệ
        * ESP_ERR_WIFI_ARG: Đối số không hợp lệ
        * others: tham khảo mã lỗi trong esp_err.h
    
    Tham số:
        * ``ifx``: giao thức được thiết lập
        * ``bw``: băng thông

.. c:function:: esp_err_t esp_wifi_get_bandwidth(wifi_interface_t ifx, wifi_bandwidth_t *bw)

    Nhận băng thông giao thức ESP32 được chỉ định

    Chú ý:
        API trả về false nếu cố gắng cấu hình một giao thức không cho phép
    
    Return:
        * ESP_OK: Thành công
        * ESP_ERR_WIFI_NOT_INIT: Wifi không được khởi tạo bởi esp_wifi_init
        * ESP_ERR_WIFI_IF: giao diện không hợp lệ
        * ESP_ERR_WIFI_ARG: Đối số không hợp lệ

    Tham số:
        * ``ifx``: giao thức được thiết lập
        * ``bw``: băng thông

.. c:function:: esp_err_t esp_wifi_set_channel(uint8_t primary, wifi_second_chan_t second)

    Thiết lập các kênh chính/phụ của ESP32

    Chú ý:
        1. Đây là một API đặc biệt dành cho sniffer
        2. API này nên được gọi sau esp_wifi_start() hoặc esp_with_set_promiscuous()
    
    Return:
        * ESP_OK: Thành công
        * ESP_ERR_WIFI_NOT_INIT: Wifi không được khởi tạo bởi esp_wifi_init
        * ESP_ERR_WIFI_IF: giao diện không hợp lệ
        * ESP_ERR_WIFI_ARG: Đối số không hợp lệ
    
    Tham số:
        * ``primary``: với HT20, primary là một nhóm kênh; với HT40, primary là kênh chính
        * ``second``: với HT20, second được bỏ qua; với HT40 second là kênh phụ

.. c:function:: esp_err_t esp_wifi_get_channel(uint8_t *primary, wifi_second_chan_t *second)

    Nhận kênh chính/phụ của ESP32

    Chú ý:
        API trả về false nếu cố gắng cấu hình một giao thức không cho phép

    Return:
        * ESP_OK: Thành công
        * ESP_ERR_WIFI_NOT_INIT: Wifi không được khởi tạo bởi esp_wifi_init
        * ESP_ERR_WIFI_ARG: Đối số không hợp lệ
    
    Tham số:
        * ``primary``: với HT20, primary là một nhóm kênh; với HT40, primary là kênh chính
        * ``second``: với HT20, second được bỏ qua; với HT40 second là kênh phụ

.. c:function:: esp_err_t esp_wifi_set_country(wifi_country_t country)

    Thiết lập mã vùng
    Mặc định là WIFI_COUNTRY_CN

    Return:
        * ESP_OK: Thành công
        * ESP_ERR_WIFI_NOT_INIT: Wifi không được khởi tạo bởi esp_wifi_init
        * ESP_ERR_WIFI_ARG: Đối số không hợp lệ
        * others: tham khảo mã lỗi trong esp_err.h

    Tham số:
        * ``country``: mã vùng

.. c:function:: esp_err_t esp_wifi_get_country(wifi_country_t *country)

    Lấy mã vùng

    Return:
        * ESP_OK: Thành công
        * ESP_ERR_WIFI_NOT_INIT: Wifi không được khởi tạo bởi esp_wifi_init
        * ESP_ERR_WIFI_ARG: Đối số không hợp lệ
    
    Tham số:
        * ``country``: lưu trữ mã vùng

.. c:function:: esp_err_t esp_wifi_set_mac(wifi_interface_t ifx, uint8_t mac[6])

    Thiết lập địa chỉ MAC của ESP32 Wifi statin hoặc soft-AP

    Chú ý:
        1. API này chỉ có thể được gọi khi giao thức bị vô hiệu hóa
        2. ESP32 soft-AP và station phải có địa chỉ MAC riêng biệt
        3. Bit 0 của byte đầu tiền trong địa chỉ MAC không thể là 1. Ví dụ, "1a:XX:XX:XX:XX:XX" đúng, "15:XX:XX:XX:XX:XX" sai

    Return:
        * ESP_OK: Thành công
        * ESP_ERR_WIFI_NOT_INIT: Wifi không được khởi tạo bởi esp_wifi_init
        * ESP_ERR_WIFI_IF: giao diện không hợp lệ
        * ESP_ERR_WIFI_ARG: Đối số không hợp lệ
        * ESP_ERR_WIFI_MAC: địa chỉ MAC không hợp lệ
        * ESP_ERR_WIFI_MODE: chế độ không hợp lệ
        * others: tham khảo mã lỗi trong esp_err.h

    Tham số:
        * ``ifx``: giao thức
        * ``mac``: địa chỉ MAC
.. c:function:: esp_err_t esp_wifi_get_mac(wifi_interface_t ifx, uint8_t mac[6])
    
    Lấy địa chỉ MAC của specificed interface

    Return:
        * ESP_OK: Thành công
        * ESP_ERR_WIFI_NOT_INIT: Wifi không được khởi tạo bởi esp_wifi_init
        * ESP_ERR_WIFI_IF: giao diện không hợp lệ
        * ESP_ERR_WIFI_ARG: Đối số không hợp lệ

    Tham số:
        * ``ifx``: giao thức
        * ``mac``: địa chỉ MAC

.. c:function:: esp_err_t esp_wifi_set_promiscuous_rx_cb(wifi_promiscuous_cb_t cb)

    Chức năng gọi lại thanh ghi RX trong chế độ promiscuous 
    Mỗi khi nhận được một packet, API sẽ được gọi ra 

    Return:
        * ESP_OK: Thành công
        * ESP_ERR_WIFI_NOT_INIT: Wifi không được khởi tạo bởi esp_wifi_init

    Tham số:
        * ``cb``: callback

.. c:function:: esp_err_t esp_wifi_set_promiscuous(bool en)

    Cho phép chế độ promiscuous

    Return:
        * ESP_OK: Thành công
        * ESP_ERR_WIFI_NOT_INIT: Wifi không được khởi tạo bởi esp_wifi_init

    Tham số:
        * ``en``: false - disable, true - enable

.. c:function:: esp_err_t esp_wifi_get_promiscuous(bool *en)

    Chuyển chế độ promiscuous

    Return:
        * ESP_OK: Thành công
        * ESP_ERR_WIFI_NOT_INIT: Wifi không được khởi tạo bởi esp_wifi_init
        * ESP_ERR_WIFI_ARG: Đối số không hợp lệ
    
    Tham số:
        * ``en``: lưu trạng thái hiện tại của chế độ promiscuous

.. c:function:: esp_err_t esp_wifi_set_promiscuous_filter(const wifi_promiscuous_filter_t *filter)

    Kích hoạt bộ lọc promiscuous
    
    Chú ý:
        Mặc định sẽ lọc hết tất cả trừ gói WIFI_PKT_MISC

    Return:
        * ESP_OK: Thành công
        * ESP_ERR_WIFI_NOT_INIT: Wifi không được khởi tạo bởi esp_wifi_init
    
    Tham số:
        * ``filter``: Loại gói tin được lọc bở promiscuous

.. c:function:: esp_err_t esp_wifi_get_promiscuous_filter(wifi_promiscuous_filter_t *filter)

    Lấy bộ lọc promiscuous

    Return:
        * ESP_OK: Thành công
        * ESP_ERR_WIFI_NOT_INIT: Wifi không được khởi tạo bởi esp_wifi_init
        * ESP_ERR_WIFI_ARG: Đối số không hợp lệ

    Tham số:
        * ``filter``: lưu trạng thái hiện tại của bộ lọc promiscuous

.. c:function:: esp_err_t esp_wifi_set_config(wifi_interface_t ifx, wifi_config_t *conf)

    Thiết lập cấu hình của ESP32 station hoặc soft-AP

    Chú ý:
        1. API này chỉ có thể gọi khi specificed interface được kích hoạt, nếu không thì API sẽ báo lỗi
        2. Để cấu hình station, bssid_set phải đặt là 0; và chỉ có thể lên 1 khi người dùng cần kiểm tra địa chỉ MAC của AP
        3. ESP32 bị giới hạn chỉ một kệnh, vì vậy khi ở chế độ soft-AP+station softAP sẽ tự điều chỉnh kênh tự động giống với ESP32 station

    Return:
        * ESP_OK: Thành công
        * ESP_ERR_WIFI_NOT_INIT: Wifi không được khởi tạo bởi esp_wifi_init
        * ESP_ERR_WIFI_ARG: Đối số không hợp lệ
        * ESP_ERR_WIFI_IF: giao thức không hợp lệ
        * ESP_ERR_WIFI_MODE: chế độ không hợp lệ
        * ESP_ERR_WIFI_PASSWORD: Mật khẩu không hợp lệ
        * ESP_ERR_NVS: Lỗi NVS nội bộ
        * others: tham khảo mã lỗi trong esp_err.h

    Tham số:
        * ``ifx``: giao thức
        * ``conf``: cấu hình station hoặc soft-AP

.. c:function:: esp_err_t esp_wifi_get_config(wifi_interface_t ifx, wifi_config_t *conf)

    Lấy cấu hình của specificed interface

    Return:
        * ESP_OK: Thành công
        * ESP_ERR_WIFI_NOT_INIT: Wifi không được khởi tạo bởi esp_wifi_init
        * ESP_ERR_WIFI_ARG: Đối số không hợp lệ
        * ESP_ERR_WIFI_IF: giao thức không hợp lệ
    
    Tham số:
        * ``ifx``: giao thức
        * ``conf``: cấu hình station hoặc soft-AP

.. c:function::  esp_err_t esp_wifi_ap_get_sta_list(wifi_sta_list_t *sta)

    Lấy STAs kết nối với soft-AP

    Chú ý:
        SSC only API

    Return:
        * ESP_OK: Thành công
        * ESP_ERR_WIFI_NOT_INIT: Wifi không được khởi tạo bởi esp_wifi_init
        * ESP_ERR_WIFI_ARG: Đối số không hợp lệ
        * ESP_ERR_WIFI_MODE: chế độ không hợp lệ
        * ESP_ERR_WIFI_CONN: lỗi thiết lập khối station hoặc soft-AP control sai

    Tham số:
        * ``storage``: kiểu lưu trữ

.. c:function:: esp_err_t esp_wifi_set_auto_connect(bool en)

    Thiết lập tự động kết nối. Giá trị mặc định là true

    Return:
        * ESP_OK: Thành công
        * ESP_ERR_WIFI_NOT_INIT: Wifi không được khởi tạo bởi esp_wifi_init
        * ESP_ERR_WIFI_MODE: chế độ không hợp lệ
    
    Tham số:
        * ``en``: true - cho phép tự động kết nối / false - vô hiệu hóa tự động kết nối

.. c:function:: esp_err_t esp_wifi_get_auto_connect(bool *en)
    
    Lấy bit cờ tự động kết nối

    Return:
        * ESP_OK: Thành công
        * ESP_ERR_WIFI_NOT_INIT: Wifi không được khởi tạo bởi esp_wifi_init
        * ESP_ERR_WIFI_ARG: Đối số không hợp lệ
    
    Tham số:
        * ``en``: lưu cấu hình tự động kết nối hiện tại

.. c:function:: esp_err_t esp_wifi_set_vendor_ie(bool enable, wifi_vendor_ie_type_t type, wifi_vendor_ie_id_t idx, const void *vnd_ie)

    Đặt 802.11 Vendor-specific 

    Return:
        * ESP_OK: Thành công
        * ESP_ERR_WIFI_NOT_INIT: Wifi không được khởi tạo bởi esp_wifi_init
        * ESP_ERR_WIFI_ARG: Đối số không hợp lệ, bao gồm nếu byte đầu tiên của vnd_ie không phải WIFI_VENDOR_IE_ELEMENT_ID (0xDD) hoặc byte thứ hai có độ dài không hợp lệ
        * ESP_ERR_WIFI_NO_MEM: tràn bộ nhớ

    Tham số:
        * ``enable``: Nếu true, specificed IE được cho phép. Nếu flase, specificed IE bị loại bỏ
        * ``type``: Thông tin kiểu Element. Xác định loại khung kết nối với IE
        * ``idx``: Để thiết lập hoặc xóa bỏ. Mỗi loại IE có thể kết nối tối đa hai yếu tố (0 và 1)

Stuctures
=========

.. c:function:: struct wifi_init_config_t

    Các tham số cấu hình Wifi bỏ qua để gọi esp_wifi_init 
    
    **Public Members**

    system_event_handler_t event_handler
        Xử lý sự kiện Wifi
    wpa_crypto_funcs_t wpa_crypto_funcs
        Mật khẩu Wifi khi kết nối
    int static_rx_buf_num
        Bộ đệm RX tĩnh
    int dynamic_rx_buf_num
        Bộ đệm RX động
    int tx_buf_type
        Kiểu bộ đệm TX
    int static_tx_buf_num
        Bộ đệm TX tĩnh
    int dynamic_tx_buf_num
        Bộ đệm TX động
    int ampdu_enable
        Bit cờ bật tính năng AMPDU
    int nvs_enable
        Bit cờ cho phép NVS
    int nano_enable
        Bit cờ cho phép tùy chọn Nano cho printf / scan
    int tx_ba_win
        Kích thước cửa sổ Wifi Bock ACK RX
    int tx_ba_win
        Kích thước cửa sổ Wifi Bock ACK TX
    int magic
        Khởi tạo Magic number

Macros
======

.. c:function:: ESP_ERR_WIFI_OK ESP_OK

    Không có lỗi

.. c:function:: ESP_ERR_WIFI_FAIL ESP_FAIL

    Mã lỗi chung

.. c:function:: ESP_ERR_WIFI_NO_MEM ESP_ERR_NO_MEM

    Không đủ bộ nhớ

.. c:function:: ESP_ERR_WIFI_ARG ESP_ERR_INVALID_ARG

    Đối số không hợp lệ

.. c:function:: ESP_ERR_WIFI_NOT_SUPPORT ESP_ERR_NOT_SUPPORTED

    Cho biết API chưa được hỗ trợ

.. c:function::  ESP_ERR_WIFI_NOT_INIT (ESP_ERR_WIFI_BASE + 1)

    Wifi driver chưa được cài đặt bởi esp_wifi_init

.. c:function:: ESP_ERR_WIFI_NOT_STARTED (ESP_ERR_WIFI_BASE + 2)

    Wifi driver chưa được bắt đầu bởi esp_wifi_start
    
.. c:function:: ESP_ERR_WIFI_NOT_STOPPED (ESP_ERR_WIFI_BASE + 3)

    Wifi driver chưa được dừng bởi esp_wifi_stop

.. c:function:: ESP_ERR_WIFI_IF (ESP_ERR_WIFI_BASE + 4)

    Giao thức lỗi

.. c:function:: ESP_ERR_WIFI_MODE (ESP_ERR_WIFI_BASE + 5)

    Chế độ Wifi lỗi

.. c:function:: ESP_ERR_WIFI_STATE (ESP_ERR_WIFI_BASE + 6)

    Lỗi trạng thái nội bộ WiFi

.. c:function:: ESP_ERR_WIFI_CONN (ESP_ERR_WIFI_BASE + 7)

    Khối kiểm soát nội bộ Wifi của station hoặc soft-AP lỗi

.. c:function:: ESP_ERR_WIFI_NVS (ESP_ERR_WIFI_BASE + 8)

    Lỗi nội bộ của NVS WiFi

.. c:function:: ESP_ERR_WIFI_MAC (ESP_ERR_WIFI_BASE + 9)

    Địa chỉ MAC không có hợp lệ
    
.. c:function:: ESP_ERR_WIFI_SSID (ESP_ERR_WIFI_BASE + 10)

    SSID không hợp lệ

.. c:function:: ESP_ERR_WIFI_PASSWORD (ESP_ERR_WIFI_BASE + 11)

    Mật khẩu không hợp lệ

.. c:function:: ESP_ERR_WIFI_TIMEOUT (ESP_ERR_WIFI_BASE + 12)

    Lỗi timeout

.. c:function:: ESP_ERR_WIFI_WAKE_FAIL (ESP_ERR_WIFI_BASE + 13)

    Wifi đang trong trạng thái ngủ (RF closed) và thức dậy bị lỗi

.. c:function:: WIFI_STATIC_TX_BUFFER_NUM 0

.. c:function:: WIFI_DYNAMIC_TX_BUFFER_NUM 0

.. c:function:: WIFI_AMPDU_ENABLED 0

.. c:function:: WIFI_NVS_ENABLED

.. c:function:: WIFI_NANO_FORMAT_ENABLED 0

.. c:function:: WIFI_INIT_CONFIG_MAGIC 0x1F2F3F4F

.. c:function:: WIFI_INIT_CONFIG_DEFAULT {0}; _Static_assert(0, "please enable wifi in menuconfig to use esp_wifi.h");

Type Definitions
================

.. c:function:: typedef void (*wifi_promiscuous_cb_t)(void *buf, wifi_promiscuous_pkt_type_t type)

    Chức năng gọi lại RX ở chế độ promiscuous. Mỗi khi nhận được gói tin, chức năng gọi lại sẽ được gọi.

    Tham số:
        * ``buf``: Data đã được nhận. Kiểu dữ liệu trong bộ đệm (wifi_promiscuous_pkt_t hoặc wifi_pkt_rx_ctrl_t) được chỉ ra bởi tham số 'type'
        * ``type``: kiểu gói promiscuous promiscuous

.. c:function:: typedef void (*esp_vendor_ie_cb_t)(void *ctx, wifi_vendor_ie_type_t type, const uint8_t sa[6], const vendor_ie_data_t *vnd_ie, int rssi)

    Chức năng của signature để nhận lại thông tin chi tiết của Vendor-specific

    Tham số:
        * ``ctx``: Đối số context, Như được truyền đến esp_wifi_set_vendor_ie_cb () khi đăng ký gọi lại.
        * ``type``: Kiểu phần tử thông tin, dựa trên kiểu frame nhận được
        * ``sa``: Địa chỉ 802.11 nguồn
        * ``vnd_ie``: Con trỏ đến các dữ liệu yếu tố nhà cung cấp cụ thể nhận được.
        * ``rssl``: Nhận chỉ báo cường độ tín hiệu.