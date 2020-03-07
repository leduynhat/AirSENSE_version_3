SPI Slave driver
=================

Tổng quan
---------

ESP32 có 4 module giao tiếp SPI, gọi là SPI0, SPI1, HSPI và VSPI. SPI0 thì chỉ dành cho bộ nhớ flash của ESP32 dùng để map các thiết bị SPI flash kết nối đến bộ nhớ của nó. SPI1 thì kết nối tương tự như các đường của SPI0 và dùng để viết cho bộ nhớ flash của chip. HSPI và VSPI thì dùng tự do, và với trình điều khiển spi_slave, nó có thể dùng như một SPI slave, được điều khiển từ một kết nối SPI master.


The spi_slave driver
^^^^^^^^^^^^^^^^^^^^^

Trình điều khiển giao tiếp spi_slave của ESP32 cho phép HSPI và VSPI giao tiếp full-duplex (đa luồng) như một thiết bị SPI slave. Nó có thể dùng DMA cho việc truyền và nhận dữ liệu với mọi độ dài của dữ liệu.

Terminology
^^^^^^^^^^^

Trình điều khiển spi_slave dùng những thuật ngữ sau:

* Host: Thiết bị ngoại vi giao tiếp SPI bên trong ESP32 khởi tạo truyền giao tiếp SPI, dùng HSPI hoặc VSPI.
 
* Bus: là bus giao tiếp SPI, thường cho tất cả thiết bị giao tiếp SPI kết nối với một master. Nói chung bus SPI bao gồm miso, mosi, sclk và các chân tùy chọn tín hiệu quadwp và quadhd. Các SPI slave kết nối với các chân tín hiệu như nhau. Mỗi SPI slave kết nối với một chân tín hiệu CS (chip select) riêng.

  - miso - Còn được gọi là q, đây là chân output của ESP32 đến thiết bị master.

  - mosi - Còn được gọi là d, đây là chân input của ESP32 lấy tín hiệu từ thiết bị master.

  - sclk - xung giữ nhịp cho giao tiếp spi, mỗi nhịp trên chân sclk báo 1 bit dữ liệu đến hoặc đi.

  - cs - Chip Select. Một chip select hoạt động, mô tả một đường truyền từ một slave.

* Transaction: Khi chân CS hoạt động, việc truyền dữ liệu từ hoặc đến master xảy ra, và CS sẽ ngưng hoạt động. Tín hiệu truyền sẽ không bao giờ bị gián đoạn bởi các đường truyền khác.


SPI transactions
^^^^^^^^^^^^^^^^

Một trao giao tiếp SPI full-duplex (đa luồng) bắt đầu khi chân CS của master được kéo xuống mức thấp. Sau đó, master gửi ra một xung clock từ bộ xử lí trên đường CLK: mỗi nhịp xung clock của bộ xử lí gây ra là một bit dữ liệu sẽ được truyền từ master đến slave trên đường MOSI và truyền ngược lại từ slave qua master trên đường MISO. Sau khi giao tiếp kết thúc thì chân CS sẽ trở lại mức cao.

Using the spi_slave driver
^^^^^^^^^^^^^^^^^^^^^^^^^^^

- Khởi tạo một thiết bị ngoại vi giao tiếp SPI như một slave bằng cách gọi ``spi_slave_initialize``. Bạn phải set chính xác các chân IO trong cấu trúc ``bus_config``. Hãy cẩn thận set các chân tín hiệu không cần thiết để nó là -1. Bạn cần có một kênh DMA (1 hoặc 2) nếu các dữ liệu truyền lớn hơn 32 byte, nếu không thì bạn có thể set ``dma_chan`` là 0.


- Để thiết lập một giao tiếp, điền một hoặc nhiều cấu trúc ``spi_transaction_t`` với mọi số lượng giao tiếp mà bạn cần. Xếp hàng tất cả các giao tiếp bằng cách gọi ``spi_slave_queue_trans``, sau đó sắp hàng kết qủa bằng cách sử dụng ``spi_slave_get_trans_result``, hoặc xử lí tất cả các yêu cầu đồng bộ bằng cách cung cấp lệnh ``spi_slave_transmit``. Hai hàm sau sẽ bị chặn cho đến khi master bắt đầu và kết thúc một giao tiếp, làm cho hàng dữ liệu được gửi và nhận.


- Tùy chọn: gỡ bỏ trình điều khiển SPI slave, gọi ``spi_slave_free``.


Transaction data and master/slave length mismatches
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Thông thường, dữ liệu truyền đến hoặc đi từ một thiết bị sẽ đọc từ và viết đến một mảng của bộ nhớ, chỉ ra bằng hai thành phần ``rx_buffer`` và ``tx_buffer`` của cấu trúc giao tiếp. Trình điều khiển SPI có thể quyết định dùng DMA cho sự truyền tải, do đó các bộ nhớ đệm này nên được phân bổ trong bộ nhớ có khả năng sử dụng DMA ``pvPortMallocCaps(size, MALLOC_CAP_DMA)``.


Một số dữ liệu viết vào bộ nhớ đệm bị giới hạn bởi thành phần ``length`` của cấu trúc truyền tải: trình điều khiển sẽ không bao giờ đọc/ghi nhiều hơn dữ liệu được chỉ ra. ``length`` không thể mặc định được độ dài dữ liệu truyền giao tiếp SPI trong thực tế, Nó được xác định bởi master khi truyền tín hiệu qua đường xung clock và CS. Trong trường hợp độ dài của việc truyền tải lớn hơn độ dài của bộ nhớ đệm, bắt đầu của việc truyền tải sẽ được gửi và nhận. Trong trường hợp độ dài truyền tải ngắn hơn độ dài của bộ nhớ đệm, chỉ dữ liệu có độ dài của bộ nhớ đệm sẽ được trao đổi.
 

Warning: Do một đặc điểm trong thiết kế của ESP32, nếu số byte được gửi bởi một master hoặc độ dài của hàng truyền tải dữ liệu trong trình điều khiển slave, tính bằng byte, thì không được lớn hơn 8 và chia cho 4, phần cứng của giao tiếp SPI có thể không viết tới được byte số 7 cho việc nhận dữ liệu của bộ nhớ đệm.


Application Example
-------------------

Slave/master communication: https://github.com/espressif/esp-idf/tree/4ec2abb/examples/peripherals/spi_slave

API Reference
-------------

* https://github.com/espressif/esp-idf/blob/4ec2abb/components/driver/include/driver/spi_slave.h

Functions
^^^^^^^^^

.. code:: cpp

   esp_err_t spi_slave_initialize(spi_host_device_t host, const spi_bus_config_t *bus_config, const 
   spi_slave_interface_config_t *slave_config, int dma_chan)

Khởi tạo một bus SPI như một giao diện slave.

Warning :
 
* Hiện tại chỉ hỗ trợ HSPI và VSPI.
* Nếu bạn chọn một kênh DMA, việc truyền và nhận của bộ nhớ đệm nên được phân bổ trong bộ nhớ có khả năng dùng DMA.

Return

* ESP_ERR_INVALID_ARG nếu cấu hình không hợp lệ.

* ESP_ERR_INVALID_STATE nếu host đã được sử dụng.

* ESP_OK thành công.

Parameters

* ``host``: thiết bị ngoại vi SPI dùng như một giao diện slave.

* ``bus_config``: trỏ đến một struct spi_bus_config_t xác định host cần được khởi tạo như thế nào.

* ``slave_config``:trỏ đến một struct spi_slave_interface_config_t xác định chi tiết cho giao diện slave.

* ``dma_chan``: 1 hoặc 2. Một bus SPI được sử dụng bởi driver này phải có một kênh DMA kết hợp với nó. Phần cứng SPI có 2 kênh DMA để chia sẻ. Tham số này cho biết kênh nào được sử dụng.

.. code:: cpp

    esp_err_t spi_slave_free(spi_host_device_t host)

Bỏ kết nối một bus của SPI slave.

Return

* ESP_ERR_INVALID_ARG nếu thông số không hợp lệ.
* ESP_ERR_INVALID_STATE nếu tất cả thiết bị trên bus chưa được thoát.
* ESP_OK thành công.

Parameters

* ``host``: thiết bị ngoại vi SPI cần thoát.

.. code:: cpp

    esp_err_t spi_slave_queue_trans(spi_host_device_t host, const spi_slave_transaction_t *trans_desc, TickType_t ticks_to_wait)

Xếp một giao tiếp SPI để thực hiện.

Xếp một giao tiếp SPI để thực hiện bởi thiết bị SPI slave này, (kích thước hàng chờ của giao tiếp được xác định khi thiết bị slave được khởi tạo thông qua spi_slave_lize). Chức năng này có thể bị chặn nếu hàng chờ đã đầy (tùy theo thông số trên ticks_to_wait). Không có hoạt động SPI nào khởi động trực tiếp bằng chức năng này, một giao tiếp được xếp tiếp theo sẽ hoạt động khi master khởi động một giao tiếp SPI bằng cách kéo CS xuống và gửi một tín hiệu xung clock ra.

Chức năng này chuyển giao quyền sở hữu của bộ nhớ đệm trong ``trans_desc`` để điều khiển SPI slave, ứng dụng không truy cập vào bộ nhớ này cho đến khi ``spi_slave_queue_trans`` gọi chuyển quyền sở hữu trở lại cho ứng dụng.

Return

* ``host``: thiết bị ngoại vi đang hoạt động như một slave.

* ``trans_desc``: mô tả việc thực hiện truyền tải. Không cố định, vì chúng ta có thể muốn ghi lại trạng thái mô tả giao tiếp.

* ``ticks_to_wait``: đánh dấu chờ cho tới khi có chỗ trong hàng chờ, dùng portMAX_DELAY để không bao giờ hết thời gian chờ.

.. code:: cpp

    esp_err_t spi_slave_get_trans_result(spi_host_device_t host, spi_slave_transaction_t **trans_desc, TickType_t ticks_to_wait)

Lấy kết quả của một giao tiếp SPI đứng trước trong hàng.

Thủ tục này sẽ chờ cho đến khi một giao tiếp với thiết bị đã cho (xếp hàng trước với spi_slave_queue_trans) đã hoàn thành. Nó sẽ trả lại mô tả của việc hoàn thành giao tiếp, do đó phần mềm có thể xem xét kết quả, ví dụ : giải phóng và tái sử dụng bộ nhớ đệm.

Bắt buộc nó sử dụng chức năng này cuối cùng cho việc xếp hàng giao tiếp bằng cách ``spi_slave_queue_trans``.

Return

* ESP_ERR_INVALID_ARG nếu thông số không hợp lệ.

* ESP_OK thành công.

Parameters

* ``host``:thiết bị ngoại vi SPI giao tiếp như một slave.

* ``trans_desc``: trỏ đến biến có thể chứa một con trỏ để mô tả giao tiếp đã thực hiện.

* ``ticks_to_wait``: đánh dấu chờ cho đến khi có trả lại một mục, dùng portMAX_DELAY để không phải hết hàng chờ.

.. code:: cpp

    esp_err_t spi_slave_transmit(spi_host_device_t host, spi_slave_transaction_t *trans_desc, TickType_t ticks_to_wait)

Thực hiện một giao tiếp SPI.

Về cơ bản không giống như ``spi_slave_queue_trans`` tiếp theo là ``spi_slave_get_trans_result``. Không nên làm như thế khi vẫn còn một giao tiếp trong hàng chưa hoàn tất, dùng ``spi_slave_get_trans_result``.

Return

* ESP_ERR_INVALID_ARG nếu thông số không hợp lệ.

* ESP_OK thành công.

Parameters

* ``host``: thiết bị ngoại vi SPI hoạt động như một slave.

* ``trans_desc``: trỏ đến biến có thể chứa một con trỏ để mô tả giao tiếp đã thực hiện. Không cố định, vì chúng ta có thể muốn ghi lại trạng thái mô tả giao tiếp.

* ``ticks_to_wait``: đánh dấu chờ cho tới khi trả lại về một mục, dùng portMAX_DELAY để không hết thời gian chờ.


Structures
^^^^^^^^^^

``struct spi_slave_interface_config_t``

Đây là cấu hình cho một SPI host hoạt động như một SPI slave.

``int spics_io_num``

Chân GPIO CS cho thiết bị này.

``uint32_t flags``

Bitwise OR of SPI_SLAVE_* flags.

``int queue_size``

Kích thước hàng chờ. Hàm này có thể đặt bao nhiêu giao tiếp 'trong không khí' (xếp hàng dùng spi_slave_queue_trans nhưng giao tiếp trước chưa hoàn tất thì dùng spi_slave_get_trans_result) cùng một lúc.

``uint8_t mode``

SPI mode (0-3)

``slave_transaction_cb_t post_setup_cb``

Gọi callback sau khi thanh ghi SPI nhận dữ liệu mới.

``slave_transaction_cb_t post_trans_cb``

Cấu trúc này mô tả một giao tiếp SPI.

Public Members

``size_t length``

Tổng độ dài dữ liệu, theo bit.

``const void *tx_buffer``

Pointer to transmit buffer, or NULL for no MOSI phase.

``void *rx_buffer``

Biến do chúng ta xác định. Có thể được dùng để lưu trữ, ví dụ: truyền ID.

Macros
^^^^^^

``SPI_SLAVE_TXBIT_LSBFIRST (1<<0)``

Truyền lệnh/địa chỉ/dữ liệu LSB đầu tiên thay vì MSB được mặc định đầu tiên.

``SPI_SLAVE_RXBIT_LSBFIRST (1<<1)``

Nhận dữ liệu LSB đầu tiên thay vì MSB đầu tiên.

``SPI_SLAVE_BIT_LSBFIRST (SPI_TXBIT_LSBFIRST|SPI_RXBIT_LSBFIRST);``

Truyền và nhận dữ liệu LSB đầu tiên.

Type Definitions
^^^^^^^^^^^^^^^^

``typedef struct spi_slave_transaction_t spi_slave_transaction_t``


``typedef void (*slave_transaction_cb_t)(spi_slave_transaction_t *trans)

Next  Previous.






