I2S
===

TỔNG QUAN
--------

ESP32 bao gồm hai thiết bị ngoại vi I2S . Các thiết bị ngoại vi này có thể được khai báo dữ liệu ngõ vào, ngõ ra thông qua ``I2S Driver``.
Thiết bị ngoại vi I2S hỗ trợ DMA nghĩa là nó có thể truyền dữ liệu mẫu liên tục mà không yêu cầu mỗi mẫu phải được đọc hoặc viết bởi CPU.
Ngõ ra I2S cũng có thể được định tuyến trực tiếp bởi DAC ( Bộ chuyển đổi số sang tương tự  - GPIO25/GPIO26) để tạo ngõ ra tương tự trực tiếp chứ không cần thông qua bộ giải mã I2S bên ngoại.

****
 
**MỘT SỐ VÍ DỤ VỀ I2S**

Các ví dụ về I2S đều có ở esp-idf: `peripherals/i2s <https://github.com/espressif/esp-idf/tree/master/examples/peripherals/i2s>`_

Ví dụ ngắn về khai báo I2S:

.. code:: cpp

    #include "driver/i2s.h"
    #include "freertos/queue.h"

    static const int i2s_num = 0; // i2s port number

    static const i2s_config_t i2s_config = {
         .mode = I2S_MODE_MASTER | I2S_MODE_TX,
         .sample_rate = 44100,
         .bits_per_sample = 16,
         .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
         .communication_format = I2S_COMM_FORMAT_I2S | I2S_COMM_FORMAT_I2S_MSB,
         .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1, // high interrupt priority
         .dma_buf_count = 8,
         .dma_buf_len = 64
    };

    static const i2s_pin_config_t pin_config = {
        .bck_io_num = 26,
        .ws_io_num = 25,
        .data_out_num = 22,
        .data_in_num = I2S_PIN_NO_CHANGE
    };

    ...

        i2s_driver_install(i2s_num, &i2s_config, 0, NULL);   //install and start i2s driver

        i2s_set_pin(i2s_num, &pin_config);

        i2s_set_sample_rates(i2s_num, 22050); //set sample rates

        i2s_driver_uninstall(i2s_num); //stop & destroy i2s driver

Ví dụ ngắn về khai báo I2S để sử dụng DAC nội cho ngõ ra tương tự

.. code:: cpp

    #include "driver/i2s.h"
    #include "freertos/queue.h"

    static const int i2s_num = 0; // i2s port number

    static const i2s_config_t i2s_config = {
         .mode = I2S_MODE_MASTER | I2S_MODE_TX | I2S_MODE_DAC_BUILT_IN,
         .sample_rate = 44100,
         .bits_per_sample = 16, /* the DAC module will only take the 8bits from MSB */
         .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
         .communication_format = I2S_COMM_FORMAT_I2S_MSB,
         .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1, // high interrupt priority
         .dma_buf_count = 8,
         .dma_buf_len = 64
    };

    ...

        i2s_driver_install(i2s_num, &i2s_config, 0, NULL);   //install and start i2s driver

        i2s_set_pin(i2s_num, NULL); //for internal DAC, this will enable both of the internal channels
        
        //You can call i2s_set_dac_mode to set built-in DAC output mode.
        //i2s_set_dac_mode(I2S_DAC_CHANNEL_BOTH_EN); 

        i2s_set_sample_rates(i2s_num, 22050); //set sample rates

        i2s_driver_uninstall(i2s_num); //stop & destroy i2s driver

API Reference
-------------

Thư viện : `driver/include/driver/i2s.h <https://github.com/espressif/esp-idf/blob/fa7d53e/components/driver/include/driver/i2s.h>`_

**CÁC HÀM TRONG THƯ VIỆN I2S**

.. c:function:: esp_err_t i2s_set_pin(i2s_port_t i2s_num, const i2s_pin_config_t *pin)

Cấu hình số chân I2S

Trong cấu trúc khai báo cấu hình chân, đặt ``I2S_PIN_NO_CHANGE`` cho bất kỳ chân nào không nên thay đổi cấu hình hiện tại.

**Chú ý :**
Các tín hiệu ngõ ra ngoại vi I2S có thể kết nối tới nhiều GPIO nhưng tín hiệu ngõ vào ngoại vi I2S chỉ có thể kết nối với một GPIO

**Các đối số**

    - ``i2s_num`` : **I2S_NUM_0** hoặc **I2S_NUM_1**
    - ``pin`` : Cấu trúc chân I2S hoặc **NULL** để cấu hình chân DAC nội 2 kênh 8 bit (GPIO 25 và GPIO26)

**Chú ý:** Nếu chân được cấu hình là **NULL** thì hàm này sẽ khởi tạo mặc định cả hai kênh DAC. Nếu bạn chỉ muốn khởi tạo một trong 2 kênh DAC thì bạn có thể thay thế bằng hàm ``i2s_set_dac_mode`` .

**Giá trị trả về**

    - ``ESP_OK`` :thành công.
    - ``ESP_FAIL`` : lỗi đối số.

****

.. c:function:: esp_err_t i2s_set_dac_mode(i2s_dac_mode_t dac_mode)

Cấu hình chế độ DAC I2S. Mặc định thì chế độ DAC tích hợp I2S bị vô hiệu hóa.

**Chú ý**

Chức năng DAC tích hợp này chỉ hỗ trợ trên I2S0 cho cho chip ESP32. Nếu một trong 2 kênh DAC tích hợp được kích hoạt thì kênh còn lại không thể sử dụng chức năng RTC DAC tại thời điểm đó.

**Giá trị trả về**

    - ``ESP_OK`` :Thành công.
    - ``ESP_ERR_INVALID_ARG`` : Lỗi đối số.

**Các đối số**

    - ``Dac_mode`` : Cấu hình chế độ DAC (xem **i2s_dac_mode_t** )

****

.. c:function:: esp_err_t i2s_driver_install(i2s_port_t i2s_num, const i2s_config_t *i2s_config, int queue_size, void *i2s_queue)

Cài đặt và khởi động Bộ điều khiển I2S.

Hàm này sẽ được gọi trước khi các bộ điều khiển I2S thực hiện việc đọc hoặc ghi.

**Các đối số**

    - ``i2s_num``: **I2S_NUM_0** , **I2S_NUM_1** ``
    - ``i2s_config`` : Các cấu hình I2S, (xem cấu trúc i2s_config_t [1]_ )
    - ``queue_size`` :Độ lớn/Độ sâu của hàng chờ sự kiện I2S.
    - ``i2s_queue`` sử lý hàng chờ sự kiện I2S, Nếu đặt là **NULL** thì trình điều khiển sẽ không sử dụng hàng chờ sự kiện.

**Giá trị trả về**

    - ``ESP_OK`` : Thành công.
    - ``ESP_FAIL`` : Lỗi đối số.

****

.. c:function:: esp_err_t i2s_driver_uninstall(i2s_port_t i2s_num)

Gỡ cài đặt bộ điều khiển I2S

**Giá trị trả về**

    - ``ESP_OK`` :Thành công.
    - ``ESP_FAIL`` : Lỗi đối số.

**Các đối số**

    - ``i2s_num`` : I2S_NUM_0, I2S_NUM_1

****

.. c:function:: int i2s_write_bytes(i2s_port_t i2s_num, const char *src, size_t size, TickType_t ticks_to_wait)

Ghi dữ liệu vào bộ đệm truyền I2S DMA.

.. note::

	DMA (Direct Memory Access) là cơ chế chuyển dữ liệu trực tiếp từ I/O vào RAM mà không cần thông qua CPU. Giúp dữ liệu được truyền đi nhanh hơn đặc biệt là các dữ liệu lớn và có tính liên tục.

Định dạng dữ liệu trong bộ đệm nguồn được xác định bởi khai báo I2S (Xem i2s_config_t [1]_ )

**Các đối số**

    - ``i2s_num`` : I2S_NUM_0, I2S_NUM_1
    - ``src`` : Lựa chọn địa chỉ nguồn bắt đầu ghi.
    - ``size`` : Kích thướt dữ liệu (Byte)
    - ``ticks_to_wait`` : Bộ đệm TX sẽ đợi thời gian của các RTOS tick kết thúc.Nếu các tick này vượt quá không gian có sẵn trong bộ đệm truyền DMA thì hàm này sẽ được gọi lại. ( Lưu ý rằng nếu dữ liệu được ghi vào bộ đệm DMA thành từng phần thì thời gian thực hiện sẽ lâu hơn  việc đợi thời gian các RTOS tick kết thúc).

**Giá trị trả về**

Số Byte được ghi hoặc ESP_FAIL (-1) nếu bị lỗi đối số.
Nếu xuất hiện thời gian đợi thì số Byte được ghi sẽ ít hơn tổng kích thướt dữ liệu.

****

.. c:function:: int i2s_read_bytes(i2s_port_t i2s_num, char *dest, size_t size, TickType_t ticks_to_wait)

Đọc dữ liệu từ bộ đềm nhận I2S DMA.
Định dạng dữ liệu trong bộ đệm nguồn được xác định bởi khai báo I2S (Xem i2S_config_t [1]_ )

**Các đối số**

    - ``i2s_num`` : I2S_NUM_0, I2S_NUM_1
    - ``dest`` : Địa chỉ đích để đọc
    - ``size`` : Kích thướt dữ liệu (Byte)
    - ``ticks_to_wait`` : Bộ đệm RX sẽ đợi thời gian của các RTOS tick kết thúc.Nếu các tick này vượt quá không gian có sẵn trong bộ đệm nhận DMA thì hàm này sẽ được gọi lại. ( Lưu ý rằng nếu dữ liệu được đọc từng phần từ bộ đệm DMA thì thời gian thực hiện sẽ lâu hơn  việc đợi thời gian các RTOS tick kết thúc).

**Giá trị trả về**

Số lượng Byte đọc được, hoặc là ESP_FAIL (-1) nếu bị lỗi đối số.
Nếu xuất hiện thời gian đợi thì số Byte đọc được sẽ ít hơn tổng kích thướt dữ liệu.

****

.. c:function:: int i2s_push_sample(i2s_port_t i2s_num, const char *sample, TickType_t ticks_to_wait)

Đẩy ( viết ) một mẫu vào bộ đệm I2S DMA TX.
Kích thướt của mẫu sẽ được xác định bởi **channel_format** (mono hay stereo) và khai báo **bit_per_sample** (xem i2s_config_t [1]_)

**Giá trị trả về**

Số Byte đã đẫy thành công vào bộ đệm DMA. Hoặc ESP_FAIL(-1) khi lỗi đối số. Giá trị trả về sẽ là không hoặc kích thướt của bộ đệm mẫu được cấu hình.

**Các đối số**

	- ``i2s_num`` : I2S_NUM_0, I2S_NUM_1
	- ``sample`` : Con trỏ trỏ đến bộ đệm có mẫu để ghi. kích thướt của bộ mẫu (byte)= số kênh * (số bit trên 1 mẫu) / 8
	- ``ticks_to_wait`` : thời gian đợi đẩy trong RTOS tick. Nếu trong thời gian này mà không có khoản trống nào tồn tại trong bộ đệm DMA TX thì không có dữ liệu nào được ghi và hàm sẽ trả về 0.

****

.. c:function:: int i2s_pop_sample(i2s_port_t i2s_num, char *sample, TickType_t ticks_to_wait)

Pop (đọc) một mẫu từ bộ đệm I2S DMA RX.
Kích thướt của mẫu được xác định bởi **channel_format** (mono hay stereo) và khai báo **bit_per_sample** (xem i2s_config_t [1]_).

**Giá trị trả về**

Số Byte đọc thành công từ bộ đệm DMA. Hoặc là ESP_FAIL (-1) khi lỗi đối số. Số Byte đếm được sẽ là 0 hoặc kích thướt của bộ đệm mẫu được khai báo.

**Các đối số**

	- ``i2s_num`` : I2S_NUM_0, I2S_NUM_1
	- ``sample`` : Bộ đệm dữ liệu mẫu sẽ được đọc. Kích thướt của bộ đệm (Byte)=Số kênh * bit_per_sample / 8
	- ``ticks_to_wait`` : thời gian đợi đọc trong RTOS tick. Nếu trong thời gian này mà không có mẫu nào tồn tại trong bộ đệm DMA thì không có dữ liệu nào được đọc và hàm sẽ trả về không.

****

.. c:function:: esp_err_t i2s_set_sample_rates(i2s_port_t i2s_num, uint32_t rate)

Thiết lập tốc độ mẫu được sử dụng cho I2S RX và TX.

Xung nhịp bit (bit clock rate) được xác định bởi tốc độ lấy mẫu và các đối số trong khai báo i2s_config_t [1]_ (number of channels, bits_per_sample).

``bit_clock = rate * (number of channels) * bits_per_sample``

**Giá trị trả về**

	- ``ESP_OK`` : Thành công
	- ``ESP_FAIL`` : Lỗi đối số

**Các đối số**

	- ``i2s_num``: I2S_NUM_0, I2S_NUM_1
	- ``rate`` : Tốc độ lấy mẫu I2S (ví dụ 8000, 44100...)

****

.. c:function:: esp_err_t i2s_stop(i2s_port_t i2s_num)

Dừng bộ điều khiển I2S

Vô hiệu hóa I2S TX/RX cho tới khi i2s_star() được gọi.

**Giá trị tải về**

	- ``ESP_OK`` : Thành công
	- ``ESP_FAIL`` : Lỗi đối số

**Các đối số**

	- ``i2s_num`` : I2S_NUM_0, I2S_NUM_1

****

.. c:function:: esp_err_t i2s_start(i2s_port_t i2s_num)

Khởi động bộ điều khiển I2S.

Không nhất thiết phải gọi hàm này nếu như bạn đã gọi hàm **i2s_driver_install()** (vì nó sẽ tự khởi động bộ điều khiển I2S).

Bạn cần phải gọi hàm này để khởi động lại bộ điều khiển I2S sau khi gọi hàm **i2s_stop()** .

**Giá trị trả về**

	- ``ESP_OK`` : Thành công
	- ``ESP_FAIL`` : Lỗi đối số.

**Các đối số**

	- ``i2s_num`` : I2S_NUM_0, I2S_NUM_1

****

.. c:function:: esp_err_t i2s_zero_dma_buffer(i2s_port_t i2s_num)

Làm trống nội dung của bộ đệm TX DMA.

Đẫy các mẫu trống (zero-byte sample) vào bộ đệm TX DMA cho đến khi đầy.

**Giá trị trả về**

	- ``ESP_OK`` : Thành công
	- ``ESP_FAIL`` : Lỗi đối số.

**Các đối số**

	- ``i2s_num`` : I2S_NUM_0, I2S_NUM_1

****

.. c:function:: esp_err_t i2s_set_clk(i2s_port_t i2s_num, uint32_t rate, i2s_bits_per_sample_t bits, i2s_channel_t ch)

Thiết lập độ rộng bit và xung clock được sử dụng trong I2S TX và RX.
Ngoài việc thiết lập tốc độ lấy mẫu như i2s_set_sample_rates() , hàm này còn thiết lập thêm độ rộng bit.

**Giá trị trả về**

	- ``ESP_OK`` : Thành công
	- ``ESP_FAIL`` : Lỗi đối số.

**Các đối số**

	- ``i2s_num`` : I2S_NUM_0, I2S_NUM_1
	- ``rate`` : Tốc độ lấy mẫu I2S (ví dụ :8000, 44100...)
	- ``bits`` : Độ rộng bit I2S (I2S_BITS_PER_SAMPLE_16BIT, I2S_BITS_PER_SAMPLE_24BIT, I2S_BITS_PER_SAMPLE_32BIT)
	- ``ch`` : Kênh I2S, (I2S_CHANNEL_MONO, I2S_CHANNEL_STEREO)

**CÁC CẤU TRÚC**

.. [1]

.. c:function:: struct i2s_config_t

Các đối số cấu hình I2S được dùng trong hàm i2s_param_config

**Các thành phần**	

	- i2s_mode_t `mode`
		Chế độ làm việc của I2S
	- int `sample_rate`
		Tốc độ lấy mẫu I2S
	- i2s_bits_per_sample_t `bits_per_sample`
		Số bit trên mỗi mẫu I2S.
	- i2s_channel_fmt_t `channel_format`
		Định dạng kênh I2S
	- i2s_comm_format_t `communication_format`
		Định dạng giao tiếp I2S
	- int `intr_alloc_flags`
		Các cờ này được dùng để chỉ định ngắt. Đơn hoặc đa (ORred) ESP_INTR_FLAG_* values. (Xem esp_intr_alloc.h để biết thêm thông tin)
	- int `dma_buf_count`
		Số lượng bộ đệm I2S DMA.
	- int `dma_buf_len`
		Độ dài bộ đệm I2S DMA.

****

.. c:function:: struct i2s_event_t

Cấu trúc các sự kiện trong hàng chờ sự kiện I2S

**Các thành phần**

	- i2s_event_type_t `type`
		Kiểu sự kiện I2S
	- size_t `size`
		Kích thướt dữ liệu I2S cho sự kiên I2S_DATA

****

.. c:function:: struct i2s_pin_config_t

Số chân I2S cho hàm **i2s_set_pin**.

**Các thành phần**

	- int `bck_io_num` 
		Chân BCK
	- int `ws_io_num`
		Chân WS
	- int `data_out_num`
		Ngõ ra dữ liệu
	- int `data_in_num`
		Ngõ vào dữ liệu


**CÁC MACRO**

.. c:function:: I2S_PIN_NO_CHANGE (-1)

Sử dụng trong hàm i2s_pin_config_t. Macro này được dùng để cố định các chân không nên thay đổi.


**CÁC ĐỊNH NGHĨA KIỂU DỮ LIỆU**

.. c:function:: typedef intr_handle_t i2s_isr_handle_t



**GIÁ TRỊ CÁC ĐỐI SỐ**

.. c:function:: enum i2s_bits_per_sample_t

Độ rộng bit trên mỗi mẫu I2S

Giá trị:
	- ``I2S_BITS_PER_SAMPLE_8BIT = 8``
		Số bit trên một mẫu I2S bằng 8bit.
	- ``I2S_BITS_PER_SAMPLE_8BIT = 16``
		Số bit trên một mẫu I2S bằng 16bit.
	- ``I2S_BITS_PER_SAMPLE_8BIT = 24``
		Số bit trên một mẫu I2S bằng 24bit.
	- ``I2S_BITS_PER_SAMPLE_8BIT = 32``
		Số bit trên một mẫu I2S bằng 32bit.

****

.. c:function:: enum i2s_channel_t

Kênh I2S

Giá trị:
	- ``I2S_CHANNEL_MONO = 1``
		I2S kênh 1 (mono)
	- ``I2S_CHANNEL_STEREO = 2``
		I2S kênh 2 (stereo)

****

.. c:function:: enum i2s_comm_format_t

Định dạng chuẩn giao tiếp I2S

Giá trị:
	- ``I2S_COMM_FORMAT_I2S = 0x01``
		Định dạng theo chuẩn I2S
	- ``I2S_COMM_FORMAT_I2S_MSB = 0x02``
		ĐỊnh dạng I2S MSB
	- ``I2S_COMM_FORMAT_I2S_LSB = 0x04``
		ĐỊnh dạng I2S LSB
	- ``I2S_COMM_FORMAT_PCM = 0x08``
		Định dạng giao tiếp I2S PCM
	- ``I2S_COMM_FORMAT_PCM_SHORT = 0x10``
		PCM ngắn
	- ``I2S_COMM_FORMAT_PCM_LONG = 0x20``
		PCM dài

****

.. c:function:: enum i2s_channel_fmt_t

Kiểu định dạng kênh I2S

Giá trị:
	- ``I2S_CHANNEL_FMT_RIGHT_LEFT = 0x00``
	- ``I2S_CHANNEL_FMT_ALL_RIGHT``
	- ``I2S_CHANNEL_FMT_ALL_LEFT``
	- ``I2S_CHANNEL_FMT_ONLY_RIGHT``
	- ``I2S_CHANNEL_FMT_ONLY_LEFT``

****

.. c:function:: enum pdm_sample_rate_ratio_t

Tỉ lệ tốc độ lấy mẫu PDM (Hz)

.. note::

	PDM-Pulse Density Modulated (Điều chế mật độ xung).

	PCM-Pulse code modulation (Biến điệu mã xung).

Giá trị:
	- ``PDM_SAMPLE_RATE_RATIO_64``
	- ``PDM_SAMPLE_RATE_RATIO_128``

****

.. c:function:: enum pdm_pcm_conv_t

Cho phép/vô hiệu hóa bộ chuyển đổi PDM PCM

Giá trị:
	- ``PDM_PCM_CONV_ENABLE``
	- ``PDM_PCM_CONV_DISABLE``

****

.. c:function:: enum i2s_port_t

Ngoại vi I2S, O hoặc 1

Giá trị:
	- ``I2S_NUM_0 = 0x0``
		I2S 0 
	- ``I2S_NUM_1 = 0x1``
		I2S 1
	- ``I2S_NUM_MAX``

****

.. c:function:: enum i2s_mode_t

Chế độ I2S. Mặc định là I2S_MODE_MASTER | I2S_MODE_TX. 

Chức năng PDM và tích hợp DAC chỉ được hỗ trợ trên I2S 0 cho chip ESP32 hiện tại.

Giá trị:
	- ``I2S_MODE_MASTER = 1``
	- ``I2S_MODE_SLAVE = 2``
	- ``I2S_MODE_TX = 4``
	- ``I2S_MODE_RX = 8``
	- ``I2S_MODE_DAC_BUILT_IN = 16``
		Xuất dữ liệu I2S sang bộ tích hợp DAC. Cho dù định dạng dữ liệu là 16 hay 32 bit thì Module DAC chỉ lấy 8bit từ MSB.
	- ``I2S_MODE_PDM = 64``

****

.. c:function:: enum i2s_event_type_t

Các kiểu sự kiện I2S

Giá trị:
	- ``I2S_EVENT_DMA_ERROR``
	- ``I2S_EVENT_TX_DONE``
		I2S DMA kết thúc việc gửi một bộ đệm (buffer)
	- ``I2S_EVENT_RX_DONE``
		I2S DMA kết thúc việc nhận một bộ đệm (buffer)
	- ``I2S_EVENT_MAX``
		Chỉ số tối đa sự kiện I2S

****

.. c:function:: enum i2s_dac_mode_t

Thiết lập chế độ I2S DAC cho hàm **i2s_set_dac_mode** .

**Chú ý :** Chức năng PDM và tích hợp DAC chỉ được hỗ trợ trên I2S 0 cho chip ESP32 hiện tại.

Giá trị:
	- ``I2S_DAC_CHANNEL_DISABLE = 0``
		Vô hiệu hóa tín hiệu I2S được tích hợp trong DAC
	- ``I2S_DAC_CHANNEL_RIGHT_EN = 1``
		Cho phép DAC kênh phải tích hợp I2S, ánh xạ đến kênh DAC 1 ( chân GPIO25)
	- ``I2S_DAC_CHANNEL_LEFT_EN = 2``
		Cho phép DAC kênh trái tích hợp I2S, ánh xạ đến kênh DAC 2 ( chân GPIO 26)
	- ``I2S_DAC_CHANNEL_BOTH_EN = 0x3``
		Cho phép cả hai kênh DAC tích hợp I2S
	- ``I2S_DAC_CHANNEL_MAX = 0x4``
		Chỉ số tối đa chế độ DAC tích hợp I2S

Ví dụ
-----

Chúng ta sẽ thực hiện Demo một dự án mẫu trong thư mục ``esp-idf/examples/peripherals/i2s`` hoặc bạn có thể download `Tại đây <https://github.com/espressif/esp-idf/tree/master/examples/peripherals/i2s>`_

Trong ứng dụng này, chúng ta sẽ thiết lập giao thức i2s để gửi dữ liệu ( từ ESP32) là tín hiệu xung tam giác 100Hz với tốc độ lấy mẫu là 36kHz. Số bit trên một mẫu thay đổi lần lượt là 16 , 24 ,32 bit trên một mẫu mỗi 5 giây.

**CHUẨN BỊ**

  +--------------------+----------------------------------------------------------+
  | **Tên board mạch** | **Link**                                                 |
  +====================+==========================================================+
  | Board IoT Wifi Uno | https://github.com/esp32vn/esp32-iot-uno                 |
  +--------------------+----------------------------------------------------------+

**ĐẤU NỐI**

	- **GPIO 22** nối với chân **Data**
	- **GPIO 26** nối với chân **BCK**
	- **GPIO 25** nối với chân **WS**

**CODE**

Tham khảo `Tài Đây <https://github.com/espressif/esp-idf/blob/master/examples/peripherals/i2s/main/i2s_example_main.c>`_

**Hướng dẫn config, nạp và debug chương trình**

.. code:: cpp

  cd (đường dẫn đến thư mục chứa project)  vd:cd ~/esp/esp-idf/exambles/peripherals/i2s
  make menuconfig
  make flash
  make moniter

**DEMO**

Lưu ý
-----
* Hướng dẫn cài đặt `ESP-IDF <https://esp-idf.readthedocs.io/en/latest/index.html>`_
* Nạp và Debug chương trình `xem tại đây <https://esp-idf.readthedocs.io/en/latest/index.html>`_
* Tài nguyên hệ thống xem `tại đây <https://github.com/espressif/esp-idf>`_
