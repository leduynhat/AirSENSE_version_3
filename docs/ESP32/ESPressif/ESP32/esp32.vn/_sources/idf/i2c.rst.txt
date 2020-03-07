Giao tiếp I2C
-------------


Tổng quan
==========
I2C là giao thức truyền thông nối tiếp đồng bộ phổ biến hiện nay, được sử dụng rộng rãi trong việc kết nối nhiều IC với nhau, hay kết nối giữa IC và các ngoại vi với tốc độ thấp. Ví dụ về kết nối oled bằng I2C có thể xem `ở đây`_.

Giao tiếp I2C sử dụng 2 dây để kết nối là SCL (Serial Clock) và SDA (Serial Data). Trong đó dây SCL có tác dụng để đồng bộ hóa giữa các thiết bị khi truyền dữ liệu, còn SDA là dây dữ liệu truyền qua.

Board ESP32-IoT-Uno có 2 bộ điều khiển I2C có thể thiết lập ở chế độ chủ (master) hoặc tớ (slave).

Chuẩn bị
========
    +---------------------------------+-----------------------------------------------------------+
    | **Tên board mạch**              | **Link**                                                  |
    +=================================+===========================================================+
    | Board ESP32 IoT Uno             | https://github.com/esp32vn/esp32-iot-uno                  |
    +---------------------------------+-----------------------------------------------------------+

Ví dụ minh họa:
===============

Ví dụ sau sẽ mô tả cách sử dụng giao tiếp I2C.

Mô tả code:
    Sử dụng 1 I2C port ở chế độ master để điều khiển 1 I2C port khác ở chế độ slave trên cùng 1 board ESP32.

Gán chân:
    * Slave:
        * Gán chân GPIO26 là chân dữ liệu SDA của i2c slave port
        * Gán chân GPIO27 là chân tín hiệu clock SCL của i2c slave port
    * Master
        * Gán chân GPIO16 là chân dữ liệu SDA của i2c master port
        * Gán chân GPIO17 là chân tín hiệu clock SCL của i2c master port

Kết nối chân:
    Tiến hành kết nối các chân SDA và SCL của slave lần lượt với SDA và SCL của master:

              * GPIO26 --> GPIO16
              * GPIO27 --> GPIO17

Các bước cơ bản:
    1. Khởi tạo các port I2C bao gồm cài driver và set các thông số cần thiết.

    2. Đọc dữ liệu từ slave về master như sau:

          - Đẩy dữ liệu từ slave ra I2C buffer
          - Master gửi lệnh để đọc dữ liệu từ I2C buffer

    3. Gửi dữ liệu từ master đến slave:

          - Đẩy dữ liệu từ master đến I2C buffer
          - Slave đọc dữ liệu từ buffer lưu vào bộ nhớ.


  .. highlight:: c

::

      /* i2c - Example
         For other examples please check:
         https://github.com/espressif/esp-idf/tree/master/examples
         This example code is in the Public Domain (or CC0 licensed, at your option.)
         Unless required by applicable law or agreed to in writing, this
         software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
         CONDITIONS OF ANY KIND, either express or implied.
      */
      #include <stdio.h>
      #include "driver/i2c.h"

      /**
       * TEST CODE BRIEF
       *
       * This example will show you how to use I2C module by running a task on i2c bus:
       *
       * - Use one I2C port(master mode) to read or write the other I2C port(slave mode) on one ESP32 chip.
       *
       * Pin assignment:
       *
       * - slave :
       *    GPIO26 is assigned as the data signal of i2c slave port
       *    GPIO27 is assigned as the clock signal of i2c slave port
       * - master:
       *    GPIO16 is assigned as the data signal of i2c master port
       *    GPIO17 is assigned as the clock signal of i2c master port
       *
       * Connection:
       *
       * - connect GPIO16 with GPIO26
       * - connect GPIO17 with GPIO27
       *
       * Test items:
       *
       * - i2c master(ESP32) will write data to i2c slave(ESP32).
       * - i2c master(ESP32) will read data from i2c slave(ESP32).
       */

      #define DATA_LENGTH          512  /*!<Data buffer length for test buffer*/
      #define RW_TEST_LENGTH       129  /*!<Data length for r/w test, any value from 0-DATA_LENGTH*/
      #define DELAY_TIME_BETWEEN_ITEMS_MS   1234 /*!< delay time between different test items */

      #define I2C_EXAMPLE_SLAVE_SCL_IO     26     /*!<gpio number for i2c slave clock  */
      #define I2C_EXAMPLE_SLAVE_SDA_IO       27  /*!<gpio number for i2c slave data */
      #define I2C_EXAMPLE_SLAVE_NUM I2C_NUM_0    /*!<I2C port number for slave dev */
      #define I2C_EXAMPLE_SLAVE_TX_BUF_LEN  (10*DATA_LENGTH) /*!<I2C slave tx buffer size */
      #define I2C_EXAMPLE_SLAVE_RX_BUF_LEN  (10*DATA_LENGTH) /*!<I2C slave rx buffer size */

      #define I2C_EXAMPLE_MASTER_SCL_IO    16    /*!< gpio number for I2C master clock */
      #define I2C_EXAMPLE_MASTER_SDA_IO    17    /*!< gpio number for I2C master data  */
      #define I2C_EXAMPLE_MASTER_NUM I2C_NUM_1   /*!< I2C port number for master dev */
      #define I2C_EXAMPLE_MASTER_TX_BUF_DISABLE   0   /*!< I2C master do not need buffer */
      #define I2C_EXAMPLE_MASTER_RX_BUF_DISABLE   0   /*!< I2C master do not need buffer */
      #define I2C_EXAMPLE_MASTER_FREQ_HZ    100000     /*!< I2C master clock frequency */

      #define ESP_SLAVE_ADDR 0x28         /*!< ESP32 slave address, you can set any 7bit value */
      #define WRITE_BIT  I2C_MASTER_WRITE /*!< I2C master write */
      #define READ_BIT   I2C_MASTER_READ  /*!< I2C master read */
      #define ACK_CHECK_EN   0x1     /*!< I2C master will check ack from slave*/
      #define ACK_CHECK_DIS  0x0     /*!< I2C master will not check ack from slave */
      #define ACK_VAL    0x0         /*!< I2C ack value */
      #define NACK_VAL   0x1         /*!< I2C nack value */

      /**
       * @brief test code to read esp-i2c-slave
       *        We need to fill the buffer of esp slave device, then master can read them out.
       *
       * _______________________________________________________________________________________
       * | start | slave_addr + rd_bit +ack | read n-1 bytes + ack | read 1 byte + nack | stop |
       * --------|--------------------------|----------------------|--------------------|------|
       *
       */
      static esp_err_t i2c_example_master_read_slave(i2c_port_t i2c_num, uint8_t* data_rd, size_t size)
      {
          if (size == 0) {
              return ESP_OK;
          }
          i2c_cmd_handle_t cmd = i2c_cmd_link_create();
          i2c_master_start(cmd);
          i2c_master_write_byte(cmd, ( ESP_SLAVE_ADDR << 1 ) | READ_BIT, ACK_CHECK_EN);
          if (size > 1) {
              i2c_master_read(cmd, data_rd, size - 1, ACK_VAL);
          }
          i2c_master_read_byte(cmd, data_rd + size - 1, NACK_VAL);
          i2c_master_stop(cmd);
          esp_err_t ret = i2c_master_cmd_begin(i2c_num, cmd, 1000 / portTICK_RATE_MS);
          i2c_cmd_link_delete(cmd);
          return ret;
      }

      /**
       * @brief Test code to write esp-i2c-slave
       *        Master device write data to slave(both esp32),
       *        the data will be stored in slave buffer.
       *        We can read them out from slave buffer.
       *
       * ___________________________________________________________________
       * | start | slave_addr + wr_bit + ack | write n bytes + ack  | stop |
       * --------|---------------------------|----------------------|------|
       *
       */
      static esp_err_t i2c_example_master_write_slave(i2c_port_t i2c_num, uint8_t* data_wr, size_t size)
      {
          i2c_cmd_handle_t cmd = i2c_cmd_link_create();
          i2c_master_start(cmd);
          i2c_master_write_byte(cmd, ( ESP_SLAVE_ADDR << 1 ) | WRITE_BIT, ACK_CHECK_EN);
          i2c_master_write(cmd, data_wr, size, ACK_CHECK_EN);
          i2c_master_stop(cmd);
          esp_err_t ret = i2c_master_cmd_begin(i2c_num, cmd, 1000 / portTICK_RATE_MS);
          i2c_cmd_link_delete(cmd);
          return ret;
      }

      /**
       * @brief test code to write esp-i2c-slave
       *
       * 1. set mode
       * _________________________________________________________________
       * | start | slave_addr + wr_bit + ack | write 1 byte + ack  | stop |
       * --------|---------------------------|---------------------|------|
       * 2. wait more than 24 ms
       * 3. read data
       * ______________________________________________________________________________________
       * | start | slave_addr + rd_bit + ack | read 1 byte + ack  | read 1 byte + nack | stop |
       * --------|---------------------------|--------------------|--------------------|------|
       */

      /**
       * @brief i2c master initialization
       */
      static void i2c_example_master_init()
      {
          int i2c_master_port = I2C_EXAMPLE_MASTER_NUM;
          i2c_config_t conf;
          conf.mode = I2C_MODE_MASTER;
          conf.sda_io_num = I2C_EXAMPLE_MASTER_SDA_IO;
          conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
          conf.scl_io_num = I2C_EXAMPLE_MASTER_SCL_IO;
          conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
          conf.master.clk_speed = I2C_EXAMPLE_MASTER_FREQ_HZ;
          i2c_param_config(i2c_master_port, &conf);
          i2c_driver_install(i2c_master_port, conf.mode,
                             I2C_EXAMPLE_MASTER_RX_BUF_DISABLE,
                             I2C_EXAMPLE_MASTER_TX_BUF_DISABLE, 0);
      }

      /**
       * @brief i2c slave initialization
       */
      static void i2c_example_slave_init()
      {
          int i2c_slave_port = I2C_EXAMPLE_SLAVE_NUM;
          i2c_config_t conf_slave;
          conf_slave.sda_io_num = I2C_EXAMPLE_SLAVE_SDA_IO;
          conf_slave.sda_pullup_en = GPIO_PULLUP_ENABLE;
          conf_slave.scl_io_num = I2C_EXAMPLE_SLAVE_SCL_IO;
          conf_slave.scl_pullup_en = GPIO_PULLUP_ENABLE;
          conf_slave.mode = I2C_MODE_SLAVE;
          conf_slave.slave.addr_10bit_en = 0;
          conf_slave.slave.slave_addr = ESP_SLAVE_ADDR;
          i2c_param_config(i2c_slave_port, &conf_slave);
          i2c_driver_install(i2c_slave_port, conf_slave.mode,
                             I2C_EXAMPLE_SLAVE_RX_BUF_LEN,
                             I2C_EXAMPLE_SLAVE_TX_BUF_LEN, 0);
      }

      /**
       * @brief test function to show buffer
       */
      static void disp_buf(uint8_t* buf, int len)
      {
          int i;
          for (i = 0; i < len; i++) {
              printf("%02x ", buf[i]);
              if (( i + 1 ) % 16 == 0) {
                  printf("\n");
              }
          }
          printf("\n");
      }

      static void i2c_test_task(void* arg)
      {
          int i = 0;
          int ret;
          uint32_t task_idx = (uint32_t) arg;
          uint8_t* data = (uint8_t*) malloc(DATA_LENGTH);
          uint8_t* data_wr = (uint8_t*) malloc(DATA_LENGTH);
          uint8_t* data_rd = (uint8_t*) malloc(DATA_LENGTH);

          while (1) {
              for (i = 0; i < DATA_LENGTH; i++) {
                  data[i] = i;
              }
              size_t d_size = i2c_slave_write_buffer(I2C_EXAMPLE_SLAVE_NUM, data, RW_TEST_LENGTH, 1000 / portTICK_RATE_MS);
              if (d_size == 0) {
                  printf("i2c slave tx buffer full\n");
                  ret = i2c_example_master_read_slave(I2C_EXAMPLE_MASTER_NUM, data_rd, DATA_LENGTH);
              } else {
                  ret = i2c_example_master_read_slave(I2C_EXAMPLE_MASTER_NUM, data_rd, RW_TEST_LENGTH);
              }
              printf("*******************\n");
              printf("TASK[%d]  MASTER READ FROM SLAVE\n", task_idx);
              printf("*******************\n");
              printf("====TASK[%d] Slave buffer data ====\n", task_idx);
              disp_buf(data, d_size);
              if (ret == ESP_OK) {
                  printf("====TASK[%d] Master read ====\n", task_idx);
                  disp_buf(data_rd, d_size);
              } else {
                  printf("Master read slave error, IO not connected...\n");
              }
              vTaskDelay(( DELAY_TIME_BETWEEN_ITEMS_MS * ( task_idx + 1 ) ) / portTICK_RATE_MS);
              //---------------------------------------------------
              int size;
              for (i = 0; i < DATA_LENGTH; i++) {
                  data_wr[i] = i + 10;
              }
              //we need to fill the slave buffer so that master can read later
              ret = i2c_example_master_write_slave( I2C_EXAMPLE_MASTER_NUM, data_wr, RW_TEST_LENGTH);
              if (ret == ESP_OK) {
                  size = i2c_slave_read_buffer( I2C_EXAMPLE_SLAVE_NUM, data, RW_TEST_LENGTH, 1000 / portTICK_RATE_MS);
              }
              printf("*******************\n");
              printf("TASK[%d]  MASTER WRITE TO SLAVE\n", task_idx);
              printf("*******************\n");
              printf("----TASK[%d] Master write ----\n", task_idx);
              disp_buf(data_wr, RW_TEST_LENGTH);
              if (ret == ESP_OK) {
                  printf("----TASK[%d] Slave read: [%d] bytes ----\n", task_idx, size);
                  disp_buf(data, size);
              } else {
                  printf("TASK[%d] Master write slave error, IO not connected....\n", task_idx);
              }
              vTaskDelay(( DELAY_TIME_BETWEEN_ITEMS_MS * ( task_idx + 1 ) ) / portTICK_RATE_MS);
          }
      }

      void app_main()
      {
          i2c_example_slave_init();
          i2c_example_master_init();

          xTaskCreate(i2c_test_task, "i2c test", 1024 * 2, NULL, 10, NULL);
      }

API Reference
==================

Header File
*************

* `driver/include/driver/i2c.h`_


Một số hàm tham khảo
***********************

.. c:function:: esp_err_t i2c_driver_install( i2c_port_t i2c_num , i2c_mode_t mode , size_t slv_rx_buf_len , size_t slv_tx_buf_len , int intr_alloc_flags )

  Mô tả hàm:
      Cài đặt I2C driver.

  Note:
      Chỉ có slave mode sử dụng giá trị này. Driver sẽ bỏ qua gía trị này trong master mode.

  Kết quả trả về:
      - ESP_OK  Thành công.
      - ESP_ERR_INVALID_ARG Lỗi đối số.
      - ESP_FAIL  Lỗi cài đặt driver.

  Các đối số:
      - ``i2c_num``: Cổng điều khiển I2C.
      - ``mode``: Chế độ làm việc của thiết bị khi kết nối I2C.
      - ``slv_rx_buf_len``: Kích thước bộ đệm thu của Slave.
      - ``slv_tx_buf_len``: Kích thước bộ đệm gửi của Slave.
      - ``intr_alloc_flags``: Các cờ cấp phát cho ngắt. Dùng để xác định mức độ ưu tiên cho ngắt. Được quy định trong file esp_intr_alloc.h

.. c:function:: esp_err_t i2c_driver_delete(i2c_port_t i2c_num)

  Mô tả hàm:
      Xóa I2C driver.

  Kết quả trả về:
      - ESP_OK Thành công.
      - ESP_ERR_INVALID_ARG Lỗi đối số.

  Các đối số:
      - ``i2c_num``: Cổng điều khiển I2C.

.. c:function:: esp_err_t i2c_param_config(i2c_port_t i2c_num, const i2c_config_t *i2c_conf)

  Mô tả hàm:
      Khởi tạo các thông số điều khiển I2C.

  Kết quả trả về:
      - ESP_OK Thành công.
      - ESP_ERR_INVALID_ARG Lỗi đối số.

  Các đối số:
      - ``i2c_num``: Cổng điều khiển I2C.
      - ``i2c_conf``: Con trỏ trỏ đến biến chứa thông số cài đặt I2C.

.. c:function:: esp_err_t i2c_reset_rx_fifo(i2c_port_t i2c_num)

  Mô tả hàm:
      Reset bộ đệm thu của cổng I2C..

  Kết quả trả về:
      - ESP_OK Thành công.
      - ESP_ERR_INVALID_ARG Lỗi đối số.

  Các đối số:
      - ``i2c_num``: Cổng điều khiển I2C.

.. c:function:: esp_err_t i2c_reset_tx_fifo(i2c_port_t i2c_num)

  Mô tả hàm:
      Reset bộ đệm phát của cổng I2C.

  Kết quả trả về:
      - ESP_OK Thành công.
      - ESP_ERR_INVALID_ARG Lỗi đối số.

  Các đối số:
      - ``i2c_num``: Cổng điều khiển I2C.

.. c:function:: esp_err_t i2c_isr_register(i2c_port_t i2c_num, void (*fn)(void *), void *arg, int intr_alloc_flags, intr_handle_t *handle, )

  Mô tả hàm:
      Quản lý trình phục vụ ngắt trong giao tiếp I2C.

  Kết quả trả về:
      - ESP_OK Thành công
      - ESP_ERR_INVALID_ARG Lỗi đối số

  Các đối số:
      - ``i2c_num``: Cổng điều khiển I2C.
      - ``fn``: Hàm gọi chương trình phục vụ ngắt.
      - ``arg``: Đối số cho hàm phục vụ ngắt.
      - ``intr_alloc_flags``: Các cờ cấp phát cho ngắt. Dùng để xác định mức độ ưu tiên cho ngắt. Được quy định trong file esp_intr_alloc.h
      - ``hanlde``: Con trỏ trỏ đến trình phục vụ ngắt, quản lý trở về từ esp_intr_alloc.

.. c:function:: esp_err_t i2c_isr_free(intr_handle_t handle)

  Mô tả hàm:
      Xóa trình phục vụ ngắt, giải phóng bộ nhớ.

  Kết quả trả về:
      - ESP_OK Thành công.
      - ESP_ERR_INVALID_ARG Lỗi đối số.

  Các đối số:
      - ``handle``: Chương trình quản lý phục vụ ngắt.

.. c:function:: esp_err_t i2c_set_pin(i2c_port_t i2c_num, gpio_num_t sda_io_num, gpio_num_t scl_io_num, gpio_pullup_t sda_pullup_en, gpio_pullup_t scl_pullup_en, i2c_mode_t mode)

  Mô tả hàm:
      Cấu hình chân GPIO thành SCL và SDA để kết nối I2C.

  Kết quả trả về:
      - ESP_OK Thành công.
      - ESP_ERR_INVALID_ARG Lỗi đối số.

  Các đối số:
      - ``i2c_num``: Cổng điều khiển I2C.
      - ``sda_io_num``: Chân GPIO được cấu hình thành SDA
      - ``scl_io_num``: Chân GPIO được cấu hình thành SCL
      - ``sda_pullup_en``: Cho phép chức năng pullup trên chân SDA.
      - ``scl_pullup_en``: Cho phép chức năng pullup trên chân SCL.
      - ``mode``: Chế độ hoạt động của thiết bị kết nối I2C.

.. c:function:: i2c_cmd_handle_t i2c_cmd_link_create()

  Mô tả hàm:
      Tạo và khởi động command link

  Note:
      Khi muốn gửi lệnh thông qua I2C, ta phải gọi hàm này để tạo command link. Sau khi kết thúc việc gửi các lệnh, ta cần xóa command link để giải phóng tài nguyên cho CPU bằng cách gọi hàm i2c_cmd_link_delete().

  Kết quả trả về:
      - i2c command link handle.

.. c:function:: void i2c_cmd_link_delete(i2c_cmd_handle_t cmd_handle)

  Mô tả hàm:
      Xóa command link.

  Các đối số:
      - ``cmd_handle``: i2c command link handle.

.. c:function:: esp_err_t i2c_master_start(i2c_cmd_handle_t cmd_handle)

  Mô tả hàm:
      Gửi 1 lệnh đến hàng chờ của I2C master để tạo tín hiệu start.

  Note:
      Chỉ gọi hàm này khi ở chế độ master, gọi hàm i2c_master_cmd_begin() để gửi tất cả các lệnh ở hàng chờ.

  Kết quả trả về:
      - ESP_OK Thành công.
      - ESP_ERR_INVALID_ARG Lỗi đối số.

  Các đối số:
      - ``cmd_handle``: i2c command link handle.

.. c:function:: esp_err_t i2c_master_write_byte(i2c_cmd_handle_t cmd_handle, uint8_t data, bool ack_en)

  Mô tả hàm:
      Gửi 1 lệnh đến hàng chờ của I2C master để ghi 1 byte dữ liệu lên I2C bus.

  Note:
      Chỉ gọi hàm này khi ở chế độ master, gọi hàm i2c_master_cmd_begin() để gửi tất cả các lệnh ở hàng chờ.

  Kết quả trả về:
      - ESP_OK Thành công.
      - ESP_ERR_INVALID_ARG Lỗi đối số.

  Các đối số:
      - ``cmd_handle``: i2c command link handle.
      - ``data``: 1 byte dữ liệu ghi lên bus của I2C.
      - ``ack_en``: Cho phép chức năng kiểm tra ACK.

.. c:function:: esp_err_t i2c_master_write(i2c_cmd_handle_t cmd_handle, uint8_t *data, size_t data_len, bool ack_en)

  Mô tả hàm:
      Gửi 1 lệnh đến hàng chờ của I2C master để ghi dữ liệu vào bộ đệm I2C bus.

  Note:
      Chỉ gọi hàm này khi ở chế độ master, gọi hàm i2c_master_cmd_begin() để gửi tất cả các lệnh ở hàng chờ.

  Kết quả trả về:
      - ESP_OK Thành công.
      - ESP_ERR_INVALID_ARG Lỗi đối số.

  Các đối số:
      - ``cmd_handle``: i2c command link handle.
      - ``data``: Dữ liệu cần gửi.
      - ``data_len``: Độ dài dữ liệu.
      - ``ack_en``: Cho phép chức năng kiểm tra ACK.

.. c:function:: esp_err_t i2c_master_read_byte(i2c_cmd_handle_t cmd_handle, uint8_t *data, int ack)

  Mô tả hàm:
      Gửi 1 lệnh đến hàng chờ của I2C master để đọc 1byte dữ liệu từ I2C bus.

  Note:
      Chỉ gọi hàm này khi ở chế độ master, gọi hàm i2c_master_cmd_begin() để gửi tất cả các lệnh ở hàng chờ.

  Kết quả trả về:
      - ESP_OK Thành công.
      - ESP_ERR_INVALID_ARG Lỗi đối số.

  Các đối số:
      - ``cmd_handle``: i2c command link handle.
      - ``data``: Con trỏ trỏ đến bộ nhớ tiếp nhận dữ liệu để đọc.
      - ``ack``: Giá trị ACK cho lệnh đọc dữ liệu.

.. c:function:: esp_err_t i2c_master_read(i2c_cmd_handle_t cmd_handle, uint8_t *data, size_t data_len, int ack)

  Mô tả hàm:
      Gửi 1 lệnh đến hàng chờ của I2C master để đọc dữ liệu từ I2C bus.

  Note:
      Chỉ gọi hàm này khi ở chế độ master, gọi hàm i2c_master_cmd_begin() để gửi tất cả các lệnh ở hàng chờ.

  Kết quả trả về:
      - ESP_OK Thành công.
      - ESP_ERR_INVALID_ARG Lỗi đối số.

  Các đối số:
      - ``cmd_handle``: i2c command link handle.
      - ``data``: Con trỏ trỏ đến bộ nhớ tiếp nhận dữ liệu để đọc.
      - ``data_len``: Độ dài dữ liệu cần đọc.
      - ``ack``: Giá trị ACK cho lệnh đọc dữ liệu.

.. c:function:: esp_err_t i2c_master_stop(i2c_cmd_handle_t cmd_handle)

  Mô tả hàm:
      Gửi 1 lệnh đến hàng chờ của I2C master để tạo tín hiệu stop.

  Note:
      Chỉ gọi hàm này khi ở chế độ master, gọi hàm i2c_master_cmd_begin() để gửi tất cả các lệnh ở hàng chờ.

  Kết quả trả về:
      - ESP_OK Thành công.
      - ESP_ERR_INVALID_ARG Lỗi đối số.

  Các đối số:
      - ``cmd_handle``: i2c command link handle.

.. c:function:: esp_err_t i2c_master_cmd_begin(i2c_port_t i2c_num, i2c_cmd_handle_t cmd_handle, portBASE_TYPE ticks_to_wait)

  Mô tả hàm:
      I2C master gửi đi tất cả các lệnh trong hàng chờ. Các tác vụ khác sẽ bị block cho đến khi tất cả các lệnh được gửi đi. Vì vậy, khi sử dụng I2C trong việc điều khiển nhiều tác vụ cần chú ý đến vấn đề multi-thread.

  Note:
      Chỉ gọi hàm này khi ở chế độ master.

  Kết quả trả về:
      - ESP_OK Thành công.
      - ESP_ERR_INVALID_ARG Lỗi đối số.
      - ESP_FAIL Lỗi khi gửi lệnh, slave không nhận được đường truyền.
      - ESP_ERR_INVALID_STATE Chưa cài đặt I2C driver hoặc đang không ở trong master mode.
      - ESP_ERR_TIMEOUT Quá thời gian chờ, đường truyền đang bận.

  Các đối số:
      - ``i2c_num``: Cổng điều khiển I2C.
      - ``cmd_handle``: i2c command link handle.
      - ``ticks_to_wait``: thời gian chờ tối đa.

.. c:function:: int i2c_slave_write_buffer(i2c_port_t i2c_num, uint8_t *data, int size, portBASE_TYPE ticks_to_wait)

  Mô tả hàm:
      I2C slave ghi dữ liệu vào bộ đệm trong (internal ringbuffer), khi bộ đệm truyền (tx fifo) rỗng, dữ liệu sẽ được đẩy vào fifo từ bộ đệm internal rungbuffer này.

  Note:
      Chỉ gọi hàm này khi ở chế độ slave.

  Kết quả trả về:
      - ESP_FAIL(-1) Parameter error
      - Others(>=0) Số byte dữ liệu được đẩy ra slave buffer.

  Các đối số:
      - ``i2c_num``: Cổng điều khiển I2C.
      - ``data``: Con trỏ dữ liệu trỏ đến bộ nhớ đệm internal ringbuffer.
      - ``size``: Kích thước dữ liệu.
      - ``ticks_to_wait``: thời gian chờ tối đa.

.. c:function:: int i2c_slave_read_buffer(i2c_port_t i2c_num, uint8_t *data, size_t max_size, portBASE_TYPE ticks_to_wait)

  Mô tả hàm:
      I2C slave đọc dữ liệu từ bộ đệm internal buffer. Khi I2C slave nhận dữ liệu, dữ liệu này sẽ được copy từ bộ đệm thu (rx fifo) đến bộ đệm internal ringbuffer.

  Kết quả trả về:
      - ESP_FAIL(-1) Parameter error
      - Others(>=0) Số byte dữ liệu đọc được từ slave buffer.

  Các đối số:
      - ``i2c_num``: Cổng điều khiển I2C.
      - ``data``: Con trỏ dữ liệu trỏ đến bộ nhớ đệm internal ringbuffer.
      - ``max_size``: Kích thước dữ liệu tối đa có thể đọc.
      - ``ticks_to_wait``: thời gian chờ tối đa.

.. c:function:: esp_err_t i2c_set_period(i2c_port_t i2c_num, int high_period, int low_period)

  Mô tả hàm:
      Cài đặt chu kì clock cho I2C master

  Kết quả trả về:
      - ESP_OK Thành công.
      - ESP_ERR_INVALID_ARG Lỗi đối số.

  Các đối số:
      - ``i2c_num``: Cổng điều khiển I2C.
      - ``high_period``: Số chu kỳ clock ở mức cao, high_period là 1 giá trị 14 bit.
      - ``low_period``: Số chu kỳ clock ở mức thấp, low_period là 1 giá trị 14 bit.

.. c:function:: esp_err_t i2c_get_period(i2c_port_t i2c_num, int *high_period, int *low_period)

  Mô tả hàm:
      Lấy chu kì clock của I2C master

  Kết quả trả về:
      - ESP_OK Thành công.
      - ESP_ERR_INVALID_ARG Lỗi đối số.

  Các đối số:
      - ``i2c_num``: Cổng điều khiển I2C.
      - ``high_period``: con trỏ đến số chu kỳ clock mức cao, sẽ lấy về 1 giá trị 14 bit.
      - ``low_period``: con trỏ đến số chu kỳ clock mức thấp, sẽ lấy về 1 giá trị 14 bit.

.. c:function:: esp_err_t i2c_set_start_timing(i2c_port_t i2c_num, int setup_time, int hold_time)

  Mô tả hàm:
      Cài đặt timing cho tín hiệu start

  Kết quả trả về:
      - ESP_OK Thành công.
      - ESP_ERR_INVALID_ARG Lỗi đối số.

  Các đối số:
      - ``i2c_num``: Cổng điều khiển I2C.
      - ``setup_time``: số clock tính từ cạnh xuống của SDA đến cạnh lên của SCL. Là giá trị 10 bit
      - ``hold_time``: số clock tính từ cạnh xuống của SDA đến cạnh xuống của SCL. Là gía trị 10 bit

.. c:function:: esp_err_t i2c_get_start_timing(i2c_port_t i2c_num, int *setup_time, int *hold_time)

  Mô tả hàm:
      Lấy kết quả cài đặt timing của start signal.

  Kết quả trả về:
      - ESP_OK Thành công.
      - ESP_ERR_INVALID_ARG Lỗi đối số.

  Các đối số:
      - ``i2c_num``: Cổng điều khiển I2C.
      - ``setup_time``: con trỏ đến setup time start.
      - ``hold_time``: con trỏ đến hold time start.

.. c:function:: esp_err_t i2c_set_stop_timing(i2c_port_t i2c_num, int setup_time, int hold_time)

  Mô tả hàm:
      Cài đặt timing cho tín hiệu stop

  Kết quả trả về:
      - ESP_OK Thành công.
      - ESP_ERR_INVALID_ARG Lỗi đối số.

  Các đối số:
      - ``i2c_num``: Cổng điều khiển I2C.
      - ``setup_time``: số clock tính từ cạnh lên của SCL đến cạnh lên của SDA. Đây là 1 giá trị 10 bit.
      - ``hold_time``: số clock tính từ sau cạnh lên của STOP bit, là gía trị 14 bit.

.. c:function:: esp_err_t i2c_get_stop_timing(i2c_port_t i2c_num, int *setup_time, int *hold_time)

  Mô tả hàm:
      Lấy kết quả cài đặt timing của stop signal.

  Kết quả trả về:
      - ESP_OK Thành công.
      - ESP_ERR_INVALID_ARG Lỗi đối số.

  Các đối số:
      - ``i2c_num``: Cổng điều khiển I2C.
      - ``setup_time``: con trỏ đến setup time stop.
      - ``hold_time``: con trỏ đến hold time stop.

.. c:function:: esp_err_t i2c_set_data_timing(i2c_port_t i2c_num, int sample_time, int hold_time)

  Mô tả hàm:
      Cài đặt timing cho tín hiệu data.

  Kết quả trả về:
      - ESP_OK Thành công.
      - ESP_ERR_INVALID_ARG Lỗi đối số.

  Các đối số:
      - ``i2c_num``: Cổng điều khiển I2C.
      - ``sample_time``: số clock sử dụng để lấy mẫu trên SDA sau khi phát hiện cạnh lên của SCL. Là 1 giá trị 10 bit.
      - ``hold_time``: số clock sử dụng để giữ dữ liệu sau khi có cạnh xuống của SCL. Là 1 giá trị 10 bit.

.. c:function:: esp_err_t i2c_get_data_timing(i2c_port_t i2c_num, int *sample_time, int *hold_time)

  Mô tả hàm:
      Lấy kết quả cài đặt timing của tín hiệu data.

  Kết quả trả về:
      - ESP_OK Thành công.
      - ESP_ERR_INVALID_ARG Lỗi đối số.

  Các đối số:
      - ``i2c_num``: Cổng điều khiển I2C.
      - ``sample_time``: Con trỏ đến sample time data.
      - ``hold_time``: Con trỏ đến hold time data.

.. c:function:: esp_err_t i2c_set_data_mode(i2c_port_t i2c_num, i2c_trans_mode_t tx_trans_mode, i2c_trans_mode_t rx_trans_mode)

  Mô tả hàm:
    Cài đặt chế độ truyền dữ liệu.

  Kết quả trả về:
      - ESP_OK Thành công.
      - ESP_ERR_INVALID_ARG Lỗi đối số.

  Các đối số:
      - "i2c_num": Cổng điều khiển I2C.
      - ``tx_trans_mode``: Chế độ gửi dữ liệu.
      - ``rx_trans_mode``: Chế độ thu dữ liệu.

.. c:function:: esp_err_t i2c_get_data_mode(i2c_port_t i2c_num, i2c_trans_mode_t *tx_trans_mode, i2c_trans_mode_t *rx_trans_mode)

  Mô tả hàm:
    Lấy kết quả cài đặt chế độ truyền dữ liệu.

  Kết quả trả về:
      - ESP_OK Thành công.
      - ESP_ERR_INVALID_ARG Lỗi đối số.

  Các đối số:
      - "i2c_num": Cổng điều khiển I2C.
      - ``tx_trans_mode``: Con trỏ để lấy chế độ truyền dữ liệu
      - ``rx_trans_mode``: Con trỏ để thu chế độ truyền dữ liệu

Structures
**********

.. c:member:: struct i2c_config_t

    Khởi tạo các thông số cho I2C.

      **Public Members**

      .. c:member:: i2c_mode_t mode

          I2C mode

      .. c:member:: gpio_num_t sda_io_num

          Chân GPIO cho tín hiệu SDA

      .. c:member:: gpio_pullup_t sda_pullup_en

          Chế độ pull-up cho tín hiệu SDA

      .. c:member:: gpio_num_t scl_io_num

          Chân GPIO cho tín hiệu SCL

      .. c:member:: gpio_pullup_t scl_pullup_en

          Chế độ pull-up cho tín hiệu SCL

      .. c:member:: uint32_t clk_speed

          Tần số clock cho I2C master mode, (không lớn hơn 1MHz)

      .. c:member:: uint8_t addr_10bit_en

          Chế độ 10 bit địa chỉ cho slave

      .. c:member::  uint16_t slave_addr

          Địa chỉ của Slave

Macro
*****

.. c:macro:: I2C_APB_CLK_FREQ APB_CLK_FREQ

    Nguồn clock cấp cho I2C là APB clock (Advanced Peripheral Bus) 80MHz

.. c:macro:: I2C_FIFO_LEN (32)

    Độ dài của FIFO hardware

Type Definitions
****************

.. c:type:: typedef void *i2c_cmd_handle_t

    I2C command handle

Enumerations
************

.. cpp:enum:: i2c_mode_t

    `Các giá trị`:

    .. c:macro:: I2C_MODE_SLAVE = 0

        I2C slave mode

    .. c:macro:: I2C_MODE_MASTER

        I2C master mode

    .. c:macro:: I2C_MODE_MAX

.. cpp:enum:: i2c_rw_t

    `Các giá trị`:

    .. c:macro:: I2C_MASTER_WRITE = 0

        I2C ghi dữ liệu từ master đến slave

    .. c:macro:: I2C_MASTER_READ

        I2C đọc dữ liệu từ slave về master

.. cpp:enum:: i2c_trans_mode_t

    `Các gía trị`:

    .. c:macro:: I2C_DATA_MODE_MSB_FIRST = 0

        Bit có trọng số cao của dữ liệu được truyền trước

    .. c:macro:: I2C_DATA_MODE_LSB_FIRST = 1

        Bit có trọng số thấp của dữ liệu được truyền trước

    .. c:macro:: I2C_DATA_MODE_MAX

.. cpp:enum:: i2c_opmode_t

    `Các giá trị`:

    .. c:macro:: I2C_CMD_RESTART = 0

        Lệnh restart I2C

    .. c:macro:: I2C_CMD_WRITE

        I2C write command

    .. c:macro:: I2C_CMD_READ

        I2C read command

    .. c:macro:: I2C_CMD_STOP

        I2C stop command

    .. c:macro:: I2C_CMD_END

        Lệnh kết thúc I2C

.. cpp:enum:: i2c_port_t

   `Các giá trị`:

    .. c:macro:: I2C_NUM_0 = 0

        I2C port 0

    .. c:macro:: I2C_NUM_1

        I2C port 1

    .. c:macro:: I2C_NUM_MAX

.. cpp:enum:: i2c_addr_mode_t

   `Các giá trị`:

    .. c:macro:: I2C_ADDR_BIT_7 = 0

        7 bit địa chỉ cho I2C slave

    .. c:macro:: I2C_ADDR_BIT_10

        10 bit địa chỉ cho I2C slave

    .. c:macro:: I2C_ADDR_BIT_MAX

.. _driver/include/driver/i2c.h: https://github.com/espressif/esp-idf/blob/6fbd6a0/components/driver/include/driver/i2c.h

.. _ở đây: https://esp32.vn/arduino/i2c_oled.html

Lưu ý
=====
* Hướng dẫn cài đặt `ESP-IDF <https://esp-idf.readthedocs.io/en/latest/index.html>`_
* Nạp và Debug chương trình `xem tại đây <https://esp-idf.readthedocs.io/en/latest/index.html>`_
* Tài nguyên hệ thống xem `tại đây <https://github.com/espressif/esp-idf>`_
