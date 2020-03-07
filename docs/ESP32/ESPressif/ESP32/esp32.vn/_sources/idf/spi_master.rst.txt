SPI Master driver
=================

Tổng quan
---------

ESP32 có 4 module giao tiếp SPI với thiết bị ngoại vi, gọi là SPI0, SPI1, HSPI và VSPI. SPI0 chỉ dành riêng để kết nối bộ nhớ flash của ESP32 với các thiết bị bộ nhớ flash khác bên ngoài. SPI1 được kết nối cũng tương tự như kiểu SPI0 nhưng nó dùng để ghi dữ liệu cho bộ nhớ flash của chíp. HSPI và VSPI được sử dụng tự do. SPI1, HSPI và VSPI đều có 3 cổng kết nối với chip, giúp chúng ta dễ dàng kết nối đồng thời với 3 slave bằng giao tiếp SPI mà ESP32 sẽ là thiết bị master.

The spi_master driver
^^^^^^^^^^^^^^^^^^^^^

Trình điều khiển SPI master của ESP32 cho phép kết nối dễ dàng với những thiết bị slave khác, thông qua cách truyền dữ liệu đa luồng (full duplex). Nó có nghĩa là truyền và nhận dữ liệu có thể xảy ra tại một thời điểm.

Terminology
^^^^^^^^^^^

Trình điều khiển của SPI master sẽ dùng các thuật ngữ sau:

* Host: thiết bị bên trong ESP32 theo giao tiếp SPI sẽ bắt đầu hoạt động truyền. Một trong 3 module giao tiếp SPI1, HSPI hay VSPI (nhưng hiện nay chỉ có 2 module HSPI và VSPI được hỗ trợ trong việc điều khiển, trong tương lai có lẽ sẽ có thể dùng thêm SPI1 trong điều khiển).

* Bus: bus SPI trong ESP32 có thể kết nối giao tiếp với tất cả thiết bị bên ngoài theo chuẩn giao tiếp SPI. Chuẩn giao tiếp SPI trong ESP32 sẽ gồm các đường là : miso, mosi, sclk và 2 chân tùy chọn tín hiệu là quadwp và quadhd. Chân slave seclect sẽ được kết nối tùy ý như các chân tín hiệu :

  - miso - còn được gọi là q, sẽ là chân input của chip ESP32 nó sẽ lấy dữ liệu từ các slave vào ESP32.

  - mosi - còn được gọi là d, sẽ là chân output của ESP32 nó sẽ xuất dữ liệu đến các slave.

  - sclk - xung giữ nhịp cho giao tiếp spi, mỗi nhịp trên chân sclk báo 1 bit dữ liệu đến hoặc đi.

  - quadwp - giữ tín hiệu được ghi. Chip sẽ dùng 4 bit cho nó.

  - quadhd - lưu giữ tín hiệu. Chip cũng dùng 4 bit cho nó.

* Device: mỗi thiết bị giao tiếp SPI với ESP32 đều có một đường kết nối riêng gọi là chip select (CS), được kích hoạt khi tín hiệu được truyền đến hoặc lấy tín hiệu từ SPI slave.

* Transaction: như đã nói ở trên, SPI là kiểu truyền dữ liệu đa luồng, cho nên khi chúng ta truyền và nhận các tín hiệu cùng một thời điểm sẽ không có gián đoạn hay ảnh hưởng gì cả.

Note:
* SPI master: thiết bị master trong giao tiếp SPI.
* SPI slave: thiết bị slave trong giao tiếp SPI.

SPI transactions
^^^^^^^^^^^^^^^^
Quá trình truyền dữ liệu của giao tiếp SPI trên ESP32 gồm các bước sau. Bất kì bước nào cũng có thể bỏ qua:

* Bước khởi động: trong bước này, một lệnh khởi động (0-16 bit) được xuất ra.
* Bước lấy địa chỉ của slave: trong bước này, một địa chỉ (0-64 bit) được xuất ra.
* Bước đọc : dữ liệu được SPI slave gửi vào cho SPI master.
* Bước ghi: SPI master sẽ gửi dữ liệu qua cho SPI slave.

Trong truyền dữ liệu đa luồng, giai đoạn đọc và ghi được kết hợp. Dữ liệu truyền giao tiếp SPI sẽ đọc và ghi cùng một lúc.

Ở bước khởi động và lấy địa chỉ slave, tùy thuộc vào thiết bị giao tiếp SPI bên ngoài, không phải thiết bị nào cũng cần gửi lệnh khởi động hay lấy địa chỉ. Điều này được biết khi chúng ta cấu hình thiết bị: ``command_bits`` hay ``data_bits`` sẽ không hoạt động khi chúng ta set các bit này ở mức thấp (mức 0).

Những trường hợp có thể đúng trong bước ghi và đọc dữ liệu: không phải mọi giao tiếp đều cần ghi và đọc giữ liệu, cũng có những trường hợp chúng ta chỉ cần spi_master chỉ đọc hay chỉ cần ghi dữ liệu thôi. Với những trường hợp như thế chúng ta sẽ làm như sau: khi ``rx_buffer`` được vô hiệu hóa (không set bit ``spi_use_rxdata``) thì bước đọc giữ liệu sẽ được bỏ qua, khi ``tx_buffer`` được vô hiêu hóa (không set bit ``spi_use_txdata``) thì bước ghi dữ liệu sẽ được bỏ qua.

Using the spi_master driver
^^^^^^^^^^^^^^^^^^^^^^^^^^^

- Khởi tạo bus giao tiếp SPI bằng cách gọi ``spi_bus_initialize``. Đảm bảo chọn đúng chân IO cần dùng trong cấu trúc ``bus_config``. Hãy cẩn thận set những tín hiệu không cần thiết là -1.

- Thông báo điều khiển một thiết bị SPI slave được kết nối với bus bằng cách gọi ``spi_bus_add_device``. Bất cứ khi nào bạn có yêu cầu cấu hình thiết bị của bạn có trong cấu trúc ``dev_config``. Bây giờ bạn xử lý được việc truyền dữ liệu từ thiết bị, sẽ được sử dụng khi truyền giữ liệu.

- Tương tác với thiết bị, điền vào một hoặc nhiều cấu trúc ``spi_transaction_t`` với bất kì thông số truyền nào bạn cần. Xếp hàng tất cả các giao tiếp bằng cách gọi ``spi_device_queue_trans``, sau đó truy xuất kết quả ta dùng ``spi_device_get_trans_result``, hoặc xử lý đồng bộ tất cả những yêu cầu bằng cách ``spi_device_transmit``.


- Optional: khi bạn muốn gỡ  bỏ thiết bị đang kết nối, hãy gọi ``spi_bus_remove_device``.

- Optional: khi bạn muốn ngắt điều khiển cho bus, đảm bảo không có một kết nối nào được gắn thêm vào thì ta gọi ``spi_bus_free``.

Transaction data
^^^^^^^^^^^^^^^^

Thông thường, dữ liệu được nhận vào hoặc truyền đi từ thiết bị, sẽ được đọc hoặc ghi vào 1 đoạn bộ nhớ, được chỉ ra bằng cách set các thành phần ``rx_buffer`` và ``tx_buffer`` của cấu trúc giao tiếp. Giao tiếp SPI có thể truyền dữ liệu theo cơ chế DMA, vì thường thì dữ liệu được chuyển từ thiết bị I/O tới bộ nhớ ram phải thông qua cpu theo tuần tự là: đầu tiên nhập một đơn vị thông tin từ thiết bị đến bộ nhớ của cpu, sau đó cpu ghi lại thông tin đó từ bộ nhớ cpu qua bộ nhớ ram. Như thế thì tốc độ truyền thông tin là khá chậm và mất thời gian, nên chúng ta nên truyền theo chế độ DMA cho phép chuyển dữ liệu trực tiếp từ thiết bị I/O vào trong bộ nhớ ram mà không cần phải thông qua cpu. Do đó các bộ đệm này nên được phân bổ trong bộ nhớ có khả năng sử dụng DMA ``pvPortMallocCaps(size, MALLOC_CAP_DMA)``.

Thỉnh thoảng, số lượng dữ liệu rất nhỏ ít hơn cả cả bộ nhớ được phân bổ riêng cho nó. Nếu dữ liệu truyền là 32 bit hay ít hơn thế, nó có thể được lưu trữ trong cấu trúc trao đổi dữ liệu của chính nó. Đối với dữ liệu truyền, ta dùng thành phần ``tx_data``  cho nó và set cờ ``spi_use_txdata`` trên sự truyền tải dữ liệu. Đối với nhận dữ liệu, ta dùng ``rx_buffer`` và set ``spi_use_txdata``. Trong cả 2 trường hợp trên các bạn thấy, tại sao chúng ta không dùng như các trường hợp trên là dùng ``rx_buffer`` hay ``tx_buffer``, mà dùng ``tx_data`` và ``rx_data`` bởi vì ``tx_data`` và ``rx_data`` sử dụng vị trí bộ nhớ cũng tương tự như chúng.


Application Example
-------------------
 
Display graphics on the ILI9341-based 320x240 LCD: https://github.com/espressif/esp-idf/tree/4ec2abb/examples/peripherals/spi_master



API Reference - SPI Common
--------------------------

Header File.

* https://github.com/espressif/esp-idf/blob/4ec2abb/components/driver/include/driver/spi_common.h

Functions
^^^^^^^^^

.. code:: cpp

    bool spicommon_periph_claim(spi_host_device_t host)

Thử kết nối với thiết bị bên ngoài bằng giao tiếp SPI.

Chúng ta sẽ gọi lệnh này nếu bạn muốn quản lí việc điều khiển thiết bị ngoại vi bằng giao tiếp SPI.

Return

Đúng nếu thiết bị được kết nối thành công, sai khi thiết bị của chúng ta đã được dùng.

Parameters

* ``host``: là thiết bị mà chúng ta muốn kết nối.

.. code:: cpp

    bool spicommon_periph_free(spi_host_device_t host)

Thoát kết nối với thiết bị, để thiết bị có thể được kết nối với một giao tiếp khác.

Return

Đúng khi thiết bị của chúng ta được trả về thành công, sai khi thiết bị chưa thực sự được kết nối.

Parameters

* ``host``: là thiết bị ngoại vi mà chúng ta muốn thoát kết nối.

.. code:: cpp

  esp_err_t spicommon_bus_initialize_io(spi_host_device_t host, const spi_bus_config_t *bus_config, int dma_chan, int flags, bool *is_native)

kết nối thiết bị ngoại vi với chân GPIO của ESP32.

Lệnh này dùng để kết nối thiết bị ngoại vi SPI với chân IO và dùng DMA trong việc giao tiếp. Tùy thuộc vào chân IO mà chúng ta dùng IO_mux hay dùng GPIO matrix.

Return

* ESP_ERR_INVALID_ARG nếu thông số không hợp lệ.

* ESP_OK thành công.

Parameters

* ``host``: thiết bị ngoại vi cần kết nối.

* ``bus_config``: trỏ chân GPIO đến cấu trúc thanh ghi spi_bus_config.

* ``dma_chan``: DMA-kênh (set 1 hoặc 2) để sử dụng DMA, nếu bạn không muốn dùng chế độ này thì có thể set nó là 0 nó sẽ không được dùng.

* ``flags``: kết hợp với các cờ của SPICOMMON_BUSFLAG_*.

* ``is_native``: giá trị 'đúng' sẽ được ghi vào địa chỉ này nếu chúng ta dùng IO_mux và 'sai' khi ta dùng GPIO matrix.

.. code:: cpp

    esp_err_t spicommon_bus_free_io(spi_host_device_t host)

Thoát kết nối cho chân IO với một thiết bị ngoại vi SPI.

Return

* ESP_ERR_INVALID_ARG nếu thông số không hợp lệ.

* ESP_OK thành công.

Parameters

* ``host``: thiết bị ngoại vi SPI chúng ta muốn thoát kết nối.

.. code:: cpp

    void spicommon_cs_initialize(spi_host_device_t host, int cs_io_num, int cs_num, int force_gpio_matrix)

Khởi tạo chân chip select CS (chân chọn slave mà chúng ta cần giao tiếp) cho thiết bị ngoại vi SPI mà mình cần giao tiếp.

Parameters

* ``host``: thiết bị ngoại vi SPI.

* ``cs_io_num``: chọn chân GPIO mà mình cần dùng.

* ``force_gpio_matrix``: chân CS(chip select) của chúng ta sẽ luôn truyền thông qua GPIO matrix. Ngược lại, nếu chân chúng ta chọn cho phép thì nó sẽ được truyền thông qua IO_mux.

.. code:: cpp

    void spicommon_cs_free(spi_host_device_t host, int cs_num)

Thoát kết nối với một chân CS(chip select) nếu bạn không muốn dùng chân đó nữa.

Parameters

* ``host``: thiết bị ngoại vi SPI.

* ``cs_num``: chân CS mà chúng ta muốn thoát.

.. code:: cpp

    void spicommon_setup_dma_desc_links(lldesc_t *dmadesc, int len, const uint8_t *data, bool isrx)

Thiết lập một chuỗi liên kết DMA.

Hàm này sẽ thiết lập một chuỗi các bộ DMA được liên kết với nhau trong mảng được trỏ bởi ``dmadesc``. Tất cả bộ DMA sẽ được dùng sao cho phù hợp với bộ đệm của các byte ``len``, chúng sẽ được trỏ đến những vị trí tương ứng trong bộ đệm và liên kết với nhau. Kết quả cuối cùng là cho ``dmadesc[0]`` vào thanh ghi phần cứng DMA trong toàn bộ byte ``len`` của ``data`` đọc và ghi.
 
Parameters

* ``dmadesc``: trỏ tới mảng của DMA đủ lớn để có thể chuyển tải tất cả các byte ``len``.

* ``len``: độ dài của bộ đệm.

* ``data``: dữ liệu của bộ đệm dùng cho việc truyền của DMA.

* ``isrx``: đúng nếu dữ liệu được ghi vào ``data``, sai nếu dữ liệu được đọc từ ``data``.

.. code:: cpp

    spi_dev_t *spicommon_hw_for_host(spi_host_device_t host)

Lấy vị trí của thanh ghi phần cứng cho một host SPI riêng.

Return

Mô tả việc trỏ đến cấu trúc thanh ghi, trỏ vào thanh ghi của phần cứng.

Parameters

* ``host``: SPI host.

.. code:: cpp

    int spicommon_irqsource_for_host(spi_host_device_t host)

Lấy kênh ngắt IRQ (interrupt request lines) cho một SPI host.

Return

Kênh ngắt của các host.

Parameters

* ``host``: SPI host.

.. code:: cpp

    bool spicommon_dmaworkaround_req_reset(int dmachan, dmaworkaround_cb_t cb, void *arg)

Yêu cầu reset cho một kênh DMA.

Về cơ bản, khi việc reset cần thiết, trình điều khiển có thể yêu cầu dùng ``spicommon_dmaworkaround_req_reset``. Đây chắc chắn là nhiệm vụ phải được gọi do người dùng cung cấp, có chức năng như để đối chiếu. Nếu cả hai kênh DMA đều không hoạt động, lệnh gọi này nó sẽ reset hệ thống phụ của DMA và trả về đúng. Nếu kênh DMA khác vẫn còn đang bận, nó sẽ trả về sai, ngay khi kênh DMA kia đã làm xong nhiệm vụ. Tuy nhiên, nó sẽ reset hệ thống phụ của DMA và gọi callback (gọi quay về). Việc dùng callback sẽ giúp trình điều khiển SPI sẽ tiếp tục trở lại hoạt động bình thường.

Note

Trong một số trường hợp (được xác định) trong ESP32 (ít nhất là ở phiên bản v.0 và v.1), một kênh DMA trong giao tiếp SPI sẽ bị nhầm lẫn. Việc này chúng ta có thể khắc phục bằng cách reset phần cứng DMA trong giao tiếp SPI khi trường hợp việc này xảy ra. Không may là nút reset này nó dùng cho việc reset cả 2 kênh DMA, nên việc này chỉ được sử dụng khi thật sự cần thiết và an toàn nhất là khi hai kênh DMA đều đã ngưng hoạt động.

Return

Đúng khi việc reset được thực hiện ngay. Ngược lại thì sẽ trả về sai, trong trường hợp này callback sẽ được gọi với các đối chiếu đã được chỉ định khi logic chúng ta có thể thực hiện lại một reset, sau đó sẽ được reset.

Parameters

* ``dmachan``: kênh DMA kết hợp với một SPI host mà chúng ta cần reset.

* ``cb``: callback sẽ được gọi trong trường hợp kênh DMA không thể reset ngay được.

* ``arg``: chỉ định đối chiếu cho việc callback. 

.. code:: cpp

    bool spicommon_dmaworkaround_reset_in_progress()

Kiểm tra xem nếu việc yêu cầu reset của chúng ta chưa được chấp thuận.

Return

Đúng khi yêu cầu reset của chúng ta chưa được chấp thuận, nếu không thì sai.

.. code:: cpp
    
    void spicommon_dmaworkaround_idle(int dmachan)

Đánh dấu hoạt động của kênh DMA.

Gọi hàm này có chức năng giải quyết một cách logic cho kênh này khi nó bị ảnh hưởng bởi reset toàn bộ DMA trong giao tiếp SPI, chúng ta không nên reset toàn bộ như thế.

Structures
^^^^^^^^^^

.. code:: cpp
   
    struct spi_bus_config_t

Đây là một cấu trúc cấu hình cho một bus SPI.

Bạn có thể sử dụng cấu trúc này để xác định các chân GPIO của bus. Thông thường, trình điều khiển sẽ sử dụng GPIO matrix để định tuyến các tín hiệu. Một ngoại lệ là có thể định tuyến các tín hiệu thông qua IO_MUX hoặc là -1. Trong trường hợp IO_MUX được sử dụng sẽ có tốc độ cho phép >40MHz.

Note

Không nên dùng hai đường quadwp/quadhd để điều khiển thiết bị SPI slave và trong vùng spi_bus_config_t, đề cập những dòng này sẽ bị bỏ qua và để an toàn bạn có thể chọn những chân khác.

Public Members
^^^^^^^^^^^^^^

.. code:: cpp

    int mosi_io_num

Khai báo chân GPIO (chân MOSI) truyền tín hiệu từ master qua slave (=spi_d), set là -1 nếu bạn không muốn dùng nó.

.. code:: cpp

    int miso_io_num

Khai báo chân GPIO (chân MISO) lấy tín hiệu từ slave vào master (=spi_q),set là -1 nếu bạn không dùng nó.

.. code:: cpp

    int sclk_io_num

Khai báo chân GPIO (chân SCLK) cho tín hiệu xung clock, set là -1 nếu bạn không dùng.

.. code:: cpp

    int quadwp_io_num

Khai báo chân cho WP(write protect) (chân quadwp) tín hiệu được dùng như D2 trong chế độ truyền 4-bit, không sử dụng thì set là -1.

.. code:: cpp

    int quadhd_io_num

Khai báo chân cho HD(HolD) (chân quadhd) tín hiệu dùng như D3 trong chế độ truyền 4-bit, set -1 nếu không sử dụng.

.. code:: cpp
    int max_transfer_sz

Kích thước truyền tối đa, tính bằng byte. Mặc định là 4094 nếu có 0.

Macros
^^^^^^

.. code:: cpp

    SPI_MAX_DMA_LEN (4096-4)

.. code:: cpp

    SPICOMMON_BUSFLAG_SLAVE 0

Khởi tạo I/O ở chế độ slave.

.. code:: cpp

    SPICOMMON_BUSFLAG_MASTER (1<<0)

Khởi tạo I/O ở chế độ master.

.. code:: cpp

    SPICOMMON_BUSFLAG_QUAD (1<<1)

Khởi tạo chân WP/HD, nếu dùng.

Type Definitions
^^^^^^^^^^^^^^^^

.. code:: cpp

    typedef void (*dmaworkaround_cb_t)(void *arg)

Callback, được gọi khi chúng ta nhấn nút reset cho DMA mà không reset ngay được.

Enumerations
^^^^^^^^^^^^^

.. code:: cpp

    enum spi_host_device_t

Khai báo với 3 thiết bị ngoại vi mà phần mềm có thể truy cập vào nó.

value:

``SPI_HOST =0``

SPI1, SPI.

``HSPI_HOST =1``

SPI2, HSPI.

``VSPI_HOST =2``

SPI3, VSPI.


API Reference - SPI Master
--------------------------

Header File
^^^^^^^^^^^

* https://github.com/espressif/esp-idf/blob/4ec2abb/components/driver/include/driver/spi_master.h

Functions
^^^^^^^^^

.. code:: cpp

    esp_err_t spi_bus_initialize(spi_host_device_t host, const spi_bus_config_t *bus_config, int dma_chan)

Khởi tạo một SPI bus.

Warning:

* Hiện tại chỉ hỗ trợ cho HSPI và VSPI.

* Nếu một kênh DMA được chọn, bất kì bộ truyền với bộ đệm nào được sử dụng phải được phân bổ trong bộ nhớ có chế độ DMA.

Return

* ESP_ERR_INVALID_ARG nếu cấu hình không hợp lệ.

* ESP_ERR_INVALID_STATE nếu host đã được dùng.

* ESP_ERR_NO_MEM nếu tràn bộ nhớ.

* ESP_OK thành công.

Parameters

* ``host``: thiết bị ngoại vi được điều khiển bằng bus này.

* ``bus_config``: trỏ tới cấu trúc spi_bus_config_t xác định host cần được khởi tạo như thế nào.

* ``dma_chan``: set là 1 hoặc 2, hoặc là 0 trong trường hợp không muốn dùng DMA. Chọn kênh DMA cho một bus SPI thì kích thước cho phép dữ liệu truyền, được giới hạn bởi bộ nhớ trong. Không chọn kênh DMA thì giới hạn dữ liệu truyền là 32 byte.

.. code:: cpp

    esp_err_t spi_bus_free(spi_host_device_t host)

Thoát giao tiếp SPI cho một bus.

Warning:

Để thành công, đầu tiên tất cả thiết bị phải được gỡ bỏ.

Return

* ESP_ERR_INVALID_ARG nếu thông số không hợp lệ.

* ESP_ERR_INVALID_STATE nếu tất cả thiết bị chưa được gõ bỏ.

* ESP_OK thành công.

Parameters

* ``host``:thiết bị ngoại vi SPI cần được thoát.

.. code:: cpp

   esp_err_t spi_bus_add_device(spi_host_device_t host, spi_device_interface_config_t *dev_config, spi_device_handle_t *handle)


Cấp một thiết bị trên một bus SPI.

Điều này khởi tạo cấu trúc bên trong cho thiết bị, cấp một chân CS (chip select) trên thiết bị ngoại vi và định tuyến đến chân GPIO mà chúng ta đã chọn, tất cả các thiết bị SPI master đều có 3 chân CS do đó có thể kết nối điều khiển 3 thiết bị ngoại vi.

Note

Nói chung, các chân SPI chuyên dụng được hỗ trợ tốc độ lên đến 80MHz và 40MHz trên các chân GPIO matrix được định tuyến, giao tiếp kiểu full-duplex(đa luồng) được định tuyến qua GPIO matrix chỉ hỗ trợ tốc độ lên tới 26MHz.

Return

* ESP_ERR_INVALID_ARG nếu thông số không hợp lệ.

* ESP_ERR_NOT_FOUND nếu thiết bị không còn chân CS nào trống.

* ESP_ERR_NO_MEM nếu bộ nhớ đầy.

* ESP_OK thành công.

Parameters

* ``host``: thiết bị cần cấp.

* ``dev_config``: giao thức cấu hình giao diện cho thiết bị SPI.

* ``handle``: trỏ đến biến xử lí của thiết bị.

.. code:: cpp

    esp_err_t spi_bus_remove_device(spi_device_handle_t handle)

Loại bỏ một thiết bị từ bus SPI.

Return

* ESP_ERR_INVALID_ARG nếu thông số không hợp lệ.

* ESP_ERR_INVALID_STATE nếu thiết bị đã được loại bỏ.

* ESP_OK thành công.

Parameters

* ``handle``: xử lí thiết bị muốn loại bỏ.

.. code:: cpp
 
    esp_err_t spi_device_queue_trans(spi_device_handle_t handle, spi_transaction_t *trans_desc, TickType_t ticks_to_wait)

Xếp hàng một giao tiếp SPI muốn thực hiện.

Return

* ESP_ERR_INVALID_ARG nếu thông số không hợp lệ.

* ESP_OK thành công.

Parameters

* ``handle``: xử lí thiết bị bằng cách sử dụng spi_host_add_dev.

* ``trans_desc``: mô tả thực hiện trao đổi tín hiệu.

* ``ticks_to_wait``: đánh dấu để đợi cho đến khi có chỗ trong hàng, dùng portMAX_DELAY để không hết thời gian chờ.

.. code:: cpp

  esp_err_t spi_device_get_trans_result(spi_device_handle_t handle, spi_transaction_t **trans_desc, TickType_t ticks_to_wait)

Lấy kết quả của một giao tiếp SPI đã được hoàn thành.

Thủ tục này sẽ đợi đến khi một giao tiếp với thiết bị đã cho (đã được xếp trước với ``spi_device_queue_trans``) hoàn thành. Sau đó, nó sẽ trả lại những mô tả của giao tiếp đã hoàn tất để phần mềm có thể kiểm tra lại kết quả, ví dụ: giải phóng bộ nhớ hoặc tái sử dụng bộ đệm.

Return

* ESP_ERR_INVALID_ARG nếu thông số không hợp lệ.

* ESP_OK thành công.

Parameters

* ``handle``: xử lí thiết bị thu được bằng cách sử dụng spi_host_add_dev.

* ``trans_desc``: trỏ đến biến chứa con trỏ mô tả giao tiếp đã thực hiện.

* ``ticks_to_wait``: đánh dấu để đợi cho đến khi trả lại một mục, sử dụng portMAX_DELAY để không hết thời gian chờ.

.. code:: cpp
 
   esp_err_t spi_device_transmit(spi_device_handle_t handle, spi_transaction_t *trans_desc)

Thực hiện giao tiếp SPI.

Không dùng khi một giao tiếp chưa kết thúc bằng cách dùng spi_device_get_trans_result.

Return

* ESP_ERR_INVALID_ARG nếu thông số không hợp lệ.

* ESP_OK thành công.

Parameters

* ``handle``: xử lí thiết bị thu được bằng cách sử dụng spi_host_add_dev.

* ``trans_desc``: trỏ đến biến chứa con trỏ mô tả giao tiếp đã thực hiện.


Structures
^^^^^^^^^^

.. code:: cpp

    struct spi_device_interface_config_t

Đây là cấu hình cho một thiết bị SPI slave được kết nối với một trong các bus.

Public Members
^^^^^^^^^^^^^^

``uint8_t command_bits``

Số bit dùng để điều khiển (0-16).

``uint8_t address_bits``

Số bit cho việc lấy địa chỉ (0-64).

``uint8_t dummy_bits``

Số bit được dùng để chèn giữa địa chỉ và dữ liệu.

``uint8_t mode``

Chế độ trong giao tiếp SPI (0-3)

``uint8_t duty_cycle_pos``

Chu kì xung clock, 1/256 một nhịp (128=50%/50% hiệu suất). Set là 0 (=không set nó) thì nó tương đương với cách set trên là 128.

``uint8_t cs_ena_pretrans``

Số bit chu kì giao tiếp SPI của CS được kích hoạt trước khi truyền (0-16). Điều này chỉ hoạt động khi làm việc trên giao tiếp half_duplex (bán đa luồng).

``uint8_t cs_ena_posttrans``

Số bit chu kì giao tiếp SPI của CS ở lại hoạt động sau khi truyền (0-16).

``int clock_speed_hz``

Tốc độ xung clock, tính bằng Hz.

``int spics_io_num``

Khởi tạo chân GPIO cho chân CS của thiết bị, không dùng thì set là -1.

``uint32_t flags``

Bitwise OR of SPI_DEVICE_* flags.

``int queue_size``

Kích thước hàng đợi giao tiếp. Có nghĩa chúng ta có thể đặt được bao nhiêu giao tiếp (xếp hàng bằng cách sử dụng spi_device_queue_trans nhưng nếu giao tiếp trước chưa hoàn thành thì dùng spi_device_get_trans_result) cùng một lúc.

``transaction_cb_t pre_cb``

callback được gọi trước khi bắt đầu truyền. Callback được gọi trong bối cảnh ngắt.

``struct spi_transaction_t``

Cấu trúc này mô tả giao tiếp SPI.

Public Members
^^^^^^^^^^^^^^

``uint32_t flags``

Bitwise OR of SPI_TRANS_* flags.

``uint16_t command``

Dữ liệu điều khiển. Độ dài cụ thể sẽ được biết khi thiết bị được thêm vào bus.

``uint64_t address``

Địa chỉ. Độ dài cụ thể sẽ được biết khi thiết bị được thêm vào bus.

``size_t length``

Tổng độ dài dữ liệu, tính bằng bit.

``size_t rxlength``

Tổng độ dài dữ liệu nhận được. Nếu khác độ dài ban đầu. (0 giá trị này là mặc định của ``lenght``).

``void *user``

Biến do người sử dụng mặc định. Có thể dùng để lưu trữ  ví dụ như truyền ID.

``const void *tx_buffer``

Trỏ để truyền dữ liệu bộ nhớ đệm, hoặc không có hiệu lực khi ta không dùng MOSI.

``uint8_t tx_data[4]``

Nếu SPI_USE_TXDATA được set, dữ liệu sẽ được gửi trực tiếp từ biến này.

``void *rx_buffer``

Trỏ để nhận dữ liệu bộ nhớ đệm, hoặc không có hiệu lực khi ta không dùng MISO.

``uint8_t rx_data[4]``

Nếu SPI_USE_RXDATA được set, dữ liệu sẽ được nhận trực tiếp đến biến này.

Macros
^^^^^^

``SPI_DEVICE_TXBIT_LSBFIRST (1<<0)``

Truyền lệnh, địa chỉ, dữ liệu LSB đầu tiên thay vì MSB được mặc định truyền đầu tiên.

``SPI_DEVICE_RXBIT_LSBFIRST (1<<1)``

Nhận dữ liệu LSB thay vì MSB được mặc định nhận đầu tiên.

``SPI_DEVICE_BIT_LSBFIRST (SPI_TXBIT_LSBFIRST|SPI_RXBIT_LSBFIRST);``

Truyền và nhận dữ liệu LSB đầu tiên.

``SPI_DEVICE_3WIRE (1<<2)``

Sử dụng MOSI để gửi và nhận dữ liệu.

``SPI_DEVICE_POSITIVE_CS (1<<3)``

Làm cho chân CS tích cực.

``SPI_DEVICE_HALFDUPLEX (1<<4)``

Truyền dữ liệu trước khi nhận nó, thay vì làm cùng lúc.

``SPI_DEVICE_CLK_AS_CS (1<<5)``

output xung clock cho đường CS, nếu CS đang hoạt động.

``SPI_TRANS_MODE_DIO (1<<0)``

Truyền/nhận dữ liệu ở chế độ 2-bit.

``SPI_TRANS_MODE_QIO (1<<1)``

Truyền/nhận dữ liệu ở chế độ 4-bit.

``SPI_TRANS_MODE_DIOQIO_ADDR (1<<2)``

Truyền địa chỉ ở chế độ được chọn bằng cách dùng SPI_MODE_DIO/SPI_MODE_QIO.

``SPI_TRANS_USE_RXDATA (1<<2)``

Dùng rx_data của spi_transaction_t để nhận dữ liệu thay vì dùng rx_buffer của bộ nhớ.

``SPI_TRANS_USE_TXDATA (1<<3)``

Dùng dữ liệu rx_data của spi_transaction_t để truyền dữ liệu thay vì dùng dữ liệu tại rx_buffer. Không set rx_buffer khi đang sử dụng rx_data.

Type Definitions
^^^^^^^^^^^^^^^^

``typedef struct spi_transaction_t spi_transaction_t``

``typedef void (*transaction_cb_t)(spi_transaction_t *trans)``

``typedef struct spi_device_t *spi_device_handle_t``

Xử lý một thiết bị trên bus SPI.




 















 

 





