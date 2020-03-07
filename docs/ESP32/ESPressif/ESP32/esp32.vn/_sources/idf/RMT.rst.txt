Remote Control Module
---------------------

Tổng quan
==========
Module RMT (Remote Control) chủ yếu được thiết kế để truyền và nhận tín hiệu điều khiển từ xa bằng hồng ngoại. Tuy nhiên do được thiết kế khá linh động, module này còn có thể sử dụng để tạo ra nhiều loại tín hiệu khác. Một bộ phát RMT sẽ liên tục đọc các giá trị từ bộ nhớ và chuyển thành các mức điện áp tương ứng để truyền đi, có thể tùy chọn điều chế tín hiệu với sóng mang hoặc không. Bộ thu sẽ tiến hành kiểm tra khi có tín hiệu tại ngõ vào, có thể tùy chỉnh để lọc tín hiệu, sau đó phân tích và lưu tín hiệu vào bộ nhớ.

RMT module có 8 kênh (channel), được đánh số từ 0 đến 7. Mỗi kênh đều có các thành phần như thanh ghi, khối nhớ và các bộ truyền nhận dữ liệu.

Chuẩn bị
========
    +---------------------------------+-----------------------------------------------------------+
    | **Tên board mạch**              | **Link**                                                  |
    +=================================+===========================================================+
    | Board ESP32 IoT Uno             | https://github.com/esp32vn/esp32-iot-uno                  |
    +---------------------------------+-----------------------------------------------------------+

Ví dụ minh họa:
===============

- **Ví dụ về module điều khiển từ xa, truyền nhận tín hiệu bằng giao thức NEC, tiến hành tải code mẫu ở:** `peripherals/rmt_nec_tx_rx.`_


- Có thể test RMT module bằng 2 phương pháp:
    - Self test:
          - Tiến hành kết nối chân GPIO TX với chân GPIO RX để truyền nhận tín hiệu. (Trong ví dụ này là GPIO16 và GPIO19).
          - Tác vụ tx_task sẽ gửi NEC data mà không sử dụng sóng mang (carrier disable).
    - Test bằng LED hồng ngoại:
          - Ta cần sử dụng sóng mang (carrier enable) cho bộ phát để phát sóng hồng ngoại.
          - Khi test bằng led hồng ngoại, bộ thu sóng thường tích cực mức thấp.
- Các bước thực hiện cơ bản:
    - Tạo gói tin dữ liệu (NEC item) bao gồm các thành phần cơ bản:
          - Phần header: là 1 xung mở đầu gói tin kéo dài 9ms và theo sau là khoảng nghỉ 4.5ms, tổng thời gian truyền là 13.5ms.
          - Phần data: là 4 bytes (32 bits) được truyền theo sau phần header với bit có trọng số thấp nhất được truyền trước. Trong đó, các byte từ thấp đến cao lần lượt là: byte 1 là byte địa chỉ, byte 2 là đảo của byte 1, byte 3 là phần command chính, byte 4 là đảo của byte 3.
          - Phần kết thúc: là 1 xung kéo dài 0.56ms.
          | **Lưu ý**: Trong phần data:
          |      - Bit 1 được thể hiện bằng 1 xung kéo dài khoảng 0.56ms và theo sau là khoảng nghỉ 1.69ms, tổng thời gian truyền 2.25ms.
          |      - Bit 0 được thể hiện bằng 1 xung kéo dài khoảng 0.56ms và theo sau là khoảng nghỉ 0.56ms, tổng thời gian truyền 1.12ms.
    - **Khởi tạo module RMT TX và RX với các thông số như trong struct** :c:data:`rmt_config_t` .
    - **Tiến hành gửi các gói tin đi và xuất thông báo ra màn hình.**
    - **Nhận gói tin, phân tích dữ liệu và in lên màn hình, sau đó giải phóng bộ đệm.**

  .. highlight:: c

::

          /* NEC remote infrared RMT example

             This example code is in the Public Domain (or CC0 licensed, at your option.)

             Unless required by applicable law or agreed to in writing, this
             software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
             CONDITIONS OF ANY KIND, either express or implied.
          */
          #include <stdio.h>
          #include <string.h>
          #include "freertos/FreeRTOS.h"
          #include "freertos/task.h"
          #include "freertos/queue.h"
          #include "freertos/semphr.h"
          #include "esp_err.h"
          #include "esp_log.h"
          #include "driver/rmt.h"
          #include "driver/periph_ctrl.h"
          #include "soc/rmt_reg.h"

          static const char* NEC_TAG = "NEC";

          //CHOOSE SELF TEST OR NORMAL TEST
          #define RMT_RX_SELF_TEST   1

          /******************************************************/
          /*****                SELF TEST:                  *****/
          /*Connect RMT_TX_GPIO_NUM with RMT_RX_GPIO_NUM        */
          /*TX task will send NEC data with carrier disabled    */
          /*RX task will print NEC data it receives.            */
          /******************************************************/
          #if RMT_RX_SELF_TEST
          #define RMT_RX_ACTIVE_LEVEL  1   /*!< Data bit is active high for self test mode */
          #define RMT_TX_CARRIER_EN    0   /*!< Disable carrier for self test mode  */
          #else
          //Test with infrared LED, we have to enable carrier for transmitter
          //When testing via IR led, the receiver waveform is usually active-low.
          #define RMT_RX_ACTIVE_LEVEL  0   /*!< If we connect with a IR receiver, the data is active low */
          #define RMT_TX_CARRIER_EN    1   /*!< Enable carrier for IR transmitter test with IR led */
          #endif

          #define RMT_TX_CHANNEL    1     /*!< RMT channel for transmitter */
          #define RMT_TX_GPIO_NUM  16     /*!< GPIO number for transmitter signal */
          #define RMT_RX_CHANNEL    0     /*!< RMT channel for receiver */
          #define RMT_RX_GPIO_NUM  19     /*!< GPIO number for receiver */
          #define RMT_CLK_DIV      100    /*!< RMT counter clock divider */
          #define RMT_TICK_10_US    (80000000/RMT_CLK_DIV/100000)   /*!< RMT counter value for 10 us.(Source clock is APB clock) */

          #define NEC_HEADER_HIGH_US    9000                         /*!< NEC protocol header: positive 9ms */
          #define NEC_HEADER_LOW_US     4500                         /*!< NEC protocol header: negative 4.5ms*/
          #define NEC_BIT_ONE_HIGH_US    560                         /*!< NEC protocol data bit 1: positive 0.56ms */
          #define NEC_BIT_ONE_LOW_US    (2250-NEC_BIT_ONE_HIGH_US)   /*!< NEC protocol data bit 1: negative 1.69ms */
          #define NEC_BIT_ZERO_HIGH_US   560                         /*!< NEC protocol data bit 0: positive 0.56ms */
          #define NEC_BIT_ZERO_LOW_US   (1120-NEC_BIT_ZERO_HIGH_US)  /*!< NEC protocol data bit 0: negative 0.56ms */
          #define NEC_BIT_END            560                         /*!< NEC protocol end: positive 0.56ms */
          #define NEC_BIT_MARGIN         20                          /*!< NEC parse margin time */

          #define NEC_ITEM_DURATION(d)  ((d & 0x7fff)*10/RMT_TICK_10_US)  /*!< Parse duration time from memory register value */
          #define NEC_DATA_ITEM_NUM   34  /*!< NEC code item number: header + 32bit data + end */
          #define RMT_TX_DATA_NUM  20    /*!< NEC tx test data number */
          #define rmt_item32_tIMEOUT_US  9500   /*!< RMT receiver timeout value(us) */

          /*
           * @brief Build register value of waveform for NEC one data bit
           */
          static inline void nec_fill_item_level(rmt_item32_t* item, int high_us, int low_us)
          {
              item->level0 = 1;
              item->duration0 = (high_us) / 10 * RMT_TICK_10_US;
              item->level1 = 0;
              item->duration1 = (low_us) / 10 * RMT_TICK_10_US;
          }

          /*
           * @brief Generate NEC header value: active 9ms + negative 4.5ms
           */
          static void nec_fill_item_header(rmt_item32_t* item)
          {
              nec_fill_item_level(item, NEC_HEADER_HIGH_US, NEC_HEADER_LOW_US);
          }

          /*
           * @brief Generate NEC data bit 1: positive 0.56ms + negative 1.69ms
           */
          static void nec_fill_item_bit_one(rmt_item32_t* item)
          {
              nec_fill_item_level(item, NEC_BIT_ONE_HIGH_US, NEC_BIT_ONE_LOW_US);
          }

          /*
           * @brief Generate NEC data bit 0: positive 0.56ms + negative 0.56ms
           */
          static void nec_fill_item_bit_zero(rmt_item32_t* item)
          {
              nec_fill_item_level(item, NEC_BIT_ZERO_HIGH_US, NEC_BIT_ZERO_LOW_US);
          }

          /*
           * @brief Generate NEC end signal: positive 0.56ms
           */
          static void nec_fill_item_end(rmt_item32_t* item)
          {
              nec_fill_item_level(item, NEC_BIT_END, 0x7fff);
          }

          /*
           * @brief Check whether duration is around target_us
           */
          inline bool nec_check_in_range(int duration_ticks, int target_us, int margin_us)
          {
              if(( NEC_ITEM_DURATION(duration_ticks) < (target_us + margin_us))
                  && ( NEC_ITEM_DURATION(duration_ticks) > (target_us - margin_us))) {
                  return true;
              } else {
                  return false;
              }
          }

          /*
           * @brief Check whether this value represents an NEC header
           */
          static bool nec_header_if(rmt_item32_t* item)
          {
              if((item->level0 == RMT_RX_ACTIVE_LEVEL && item->level1 != RMT_RX_ACTIVE_LEVEL)
                  && nec_check_in_range(item->duration0, NEC_HEADER_HIGH_US, NEC_BIT_MARGIN)
                  && nec_check_in_range(item->duration1, NEC_HEADER_LOW_US, NEC_BIT_MARGIN)) {
                  return true;
              }
              return false;
          }

          /*
           * @brief Check whether this value represents an NEC data bit 1
           */
          static bool nec_bit_one_if(rmt_item32_t* item)
          {
              if((item->level0 == RMT_RX_ACTIVE_LEVEL && item->level1 != RMT_RX_ACTIVE_LEVEL)
                  && nec_check_in_range(item->duration0, NEC_BIT_ONE_HIGH_US, NEC_BIT_MARGIN)
                  && nec_check_in_range(item->duration1, NEC_BIT_ONE_LOW_US, NEC_BIT_MARGIN)) {
                  return true;
              }
              return false;
          }

          /*
           * @brief Check whether this value represents an NEC data bit 0
           */
          static bool nec_bit_zero_if(rmt_item32_t* item)
          {
              if((item->level0 == RMT_RX_ACTIVE_LEVEL && item->level1 != RMT_RX_ACTIVE_LEVEL)
                  && nec_check_in_range(item->duration0, NEC_BIT_ZERO_HIGH_US, NEC_BIT_MARGIN)
                  && nec_check_in_range(item->duration1, NEC_BIT_ZERO_LOW_US, NEC_BIT_MARGIN)) {
                  return true;
              }
              return false;
          }


          /*
           * @brief Parse NEC 32 bit waveform to address and command.
           */
          static int nec_parse_items(rmt_item32_t* item, int item_num, uint16_t* addr, uint16_t* data)
          {
              int w_len = item_num;
              if(w_len < NEC_DATA_ITEM_NUM) {
                  return -1;
              }
              int i = 0, j = 0;
              if(!nec_header_if(item++)) {
                  return -1;
              }
              uint16_t addr_t = 0;
              for(j = 0; j < 16; j++) {
                  if(nec_bit_one_if(item)) {
                      addr_t |= (1 << j);
                  } else if(nec_bit_zero_if(item)) {
                      addr_t |= (0 << j);
                  } else {
                      return -1;
                  }
                  item++;
                  i++;
              }
              uint16_t data_t = 0;
              for(j = 0; j < 16; j++) {
                  if(nec_bit_one_if(item)) {
                      data_t |= (1 << j);
                  } else if(nec_bit_zero_if(item)) {
                      data_t |= (0 << j);
                  } else {
                      return -1;
                  }
                  item++;
                  i++;
              }
              *addr = addr_t;
              *data = data_t;
              return i;
          }

          /*
           * @brief Build NEC 32bit waveform.
           */
          static int nec_build_items(int channel, rmt_item32_t* item, int item_num, uint16_t addr, uint16_t cmd_data)
          {
              int i = 0, j = 0;
              if(item_num < NEC_DATA_ITEM_NUM) {
                  return -1;
              }
              nec_fill_item_header(item++);
              i++;
              for(j = 0; j < 16; j++) {
                  if(addr & 0x1) {
                      nec_fill_item_bit_one(item);
                  } else {
                      nec_fill_item_bit_zero(item);
                  }
                  item++;
                  i++;
                  addr >>= 1;
              }
              for(j = 0; j < 16; j++) {
                  if(cmd_data & 0x1) {
                      nec_fill_item_bit_one(item);
                  } else {
                      nec_fill_item_bit_zero(item);
                  }
                  item++;
                  i++;
                  cmd_data >>= 1;
              }
              nec_fill_item_end(item);
              i++;
              return i;
          }

          /*
           * @brief RMT transmitter initialization
           */
          static void nec_tx_init()
          {
              rmt_config_t rmt_tx;
              rmt_tx.channel = RMT_TX_CHANNEL;
              rmt_tx.gpio_num = RMT_TX_GPIO_NUM;
              rmt_tx.mem_block_num = 1;
              rmt_tx.clk_div = RMT_CLK_DIV;
              rmt_tx.tx_config.loop_en = false;
              rmt_tx.tx_config.carrier_duty_percent = 50;
              rmt_tx.tx_config.carrier_freq_hz = 38000;
              rmt_tx.tx_config.carrier_level = 1;
              rmt_tx.tx_config.carrier_en = RMT_TX_CARRIER_EN;
              rmt_tx.tx_config.idle_level = 0;
              rmt_tx.tx_config.idle_output_en = true;
              rmt_tx.rmt_mode = 0;
              rmt_config(&rmt_tx);
              rmt_driver_install(rmt_tx.channel, 0, 0);
          }

          /*
           * @brief RMT receiver initialization
           */
          static void nec_rx_init()
          {
              rmt_config_t rmt_rx;
              rmt_rx.channel = RMT_RX_CHANNEL;
              rmt_rx.gpio_num = RMT_RX_GPIO_NUM;
              rmt_rx.clk_div = RMT_CLK_DIV;
              rmt_rx.mem_block_num = 1;
              rmt_rx.rmt_mode = RMT_MODE_RX;
              rmt_rx.rx_config.filter_en = true;
              rmt_rx.rx_config.filter_ticks_thresh = 100;
              rmt_rx.rx_config.idle_threshold = rmt_item32_tIMEOUT_US / 10 * (RMT_TICK_10_US);
              rmt_config(&rmt_rx);
              rmt_driver_install(rmt_rx.channel, 1000, 0);
          }

          /**
           * @brief RMT receiver demo, this task will print each received NEC data.
           *
           */
          static void rmt_example_nec_rx_task()
          {
              int channel = RMT_RX_CHANNEL;
              nec_rx_init();
              RingbufHandle_t rb = NULL;
              //get RMT RX ringbuffer
              rmt_get_ringbuf_handler(channel, &rb);
              rmt_rx_start(channel, 1);
              while(rb) {
                  size_t rx_size = 0;
                  //try to receive data from ringbuffer.
                  //RMT driver will push all the data it receives to its ringbuffer.
                  //We just need to parse the value and return the spaces of ringbuffer.
                  rmt_item32_t* item = (rmt_item32_t*) xRingbufferReceive(rb, &rx_size, 1000);
                  if(item) {
                      uint16_t rmt_addr;
                      uint16_t rmt_cmd;
                      int offset = 0;
                      while(1) {
                          //parse data value from ringbuffer.
                          int res = nec_parse_items(item + offset, rx_size / 4 - offset, &rmt_addr, &rmt_cmd);
                          if(res > 0) {
                              offset += res + 1;
                              ESP_LOGI(NEC_TAG, "RMT RCV --- addr: 0x%04x cmd: 0x%04x", rmt_addr, rmt_cmd);
                          } else {
                              break;
                          }
                      }
                      //after parsing the data, return spaces to ringbuffer.
                      vRingbufferReturnItem(rb, (void*) item);
                  } else {
                      break;
                  }
              }
              vTaskDelete(NULL);
          }

          /**
           * @brief RMT transmitter demo, this task will periodically send NEC data. (100 * 32 bits each time.)
           *
           */
          static void rmt_example_nec_tx_task()
          {
              vTaskDelay(10);
              nec_tx_init();
              esp_log_level_set(NEC_TAG, ESP_LOG_INFO);
              int channel = RMT_TX_CHANNEL;
              uint16_t cmd = 0x0;
              uint16_t addr = 0x11;
              int nec_tx_num = RMT_TX_DATA_NUM;
              for(;;) {
                  ESP_LOGI(NEC_TAG, "RMT TX DATA");
                  size_t size = (sizeof(rmt_item32_t) * NEC_DATA_ITEM_NUM * nec_tx_num);
                  //each item represent a cycle of waveform.
                  rmt_item32_t* item = (rmt_item32_t*) malloc(size);
                  int item_num = NEC_DATA_ITEM_NUM * nec_tx_num;
                  memset((void*) item, 0, size);
                  int i, offset = 0;
                  while(1) {
                      //To build a series of waveforms.
                      i = nec_build_items(channel, item + offset, item_num - offset, ((~addr) << 8) | addr, cmd);
                      if(i < 0) {
                          break;
                      }
                      cmd++;
                      addr++;
                      offset += i;
                  }
                  //To send data according to the waveform items.
                  rmt_write_items(channel, item, item_num, true);
                  //Wait until sending is done.
                  rmt_wait_tx_done(channel);
                  //before we free the data, make sure sending is already done.
                  free(item);
                  vTaskDelay(2000 / portTICK_PERIOD_MS);
              }
              vTaskDelete(NULL);
          }

          void app_main()
          {
              xTaskCreate(rmt_example_nec_rx_task, "rmt_nec_rx_task", 2048, NULL, 10, NULL);
              xTaskCreate(rmt_example_nec_tx_task, "rmt_nec_tx_task", 2048, NULL, 10, NULL);
          }

API Reference
==================

Header File
*************

* `driver/include/driver/rmt.h`_


Một số hàm tham khảo
***********************

.. c:function:: esp_err_t rmt_set_clk_div(rmt_channel_t channel, uint8_t div_cnt)

  Mô tả hàm:
      Cài đặt bộ chia clock cho RMT, clock channel được chia từ clock nguồn.

  Kết quả trả về:
      - ESP_OK  Thành công.
      - ESP_ERR_INVALID_ARG Lỗi đối số.

  Các đối số:
      - ``channel``: kênh của RMT (0 -7)
      - ``div_cnt``: Bộ chia clock cho RMT

.. c:function:: esp_err_t rmt_get_clk_div(rmt_channel_t channel, uint8_t *div_cnt)

  Mô tả hàm:
      Lấy giá trị từ bộ chia clock đã cài đặt, channel clock được chia từ clock nguồn.

  Kết quả trả về:
      - ESP_OK Thành công.
      - ESP_ERR_INVALID_ARG Lỗi đối số.

  Các đối số:
      - ``channel``: kênh của RMT (0 -7)
      - ``div_cnt``: con trỏ để tiếp nhận bộ đếm clock RMT

.. c:function:: esp_err_t rmt_set_rx_idle_thresh(rmt_channel_t channel, uint16_t thresh)

  Mô tả hàm:
      Cài đặt ngưỡng giá trị nghỉ cho RMT RX.

      Trong chế độ nhận, khi không có cạnh xung nào được phát hiện trên tín hiệu vào trong thời gian lâu hơn ngưỡng giá trị nghỉ (tính bằng chu kì xung clock), lúc đó quá trình nhận dữ liệu coi như đã hoàn tất.

  Kết quả trả về:
      - ESP_OK Thành công.
      - ESP_ERR_INVALID_ARG Lỗi đối số.

  Các đối số:
      - ``channel``: kênh của RMT (0 - 7)
      - ``thresh``: Ngưỡng nghỉ cho RMT RX

.. c:function:: esp_err_t rmt_get_rx_idle_thresh(rmt_channel_t channel, uint16_t *thresh)

  Mô tả hàm:
      Lấy về kết quả cài đặt cho ngưỡng giá trị nghỉ của RMT RX.

      Trong chế độ nhận, khi không có cạnh xung nào được phát hiện trên tín hiệu vào trong thời gian lâu hơn ngưỡng giá trị nghỉ (tính bằng chu kì xung clock), lúc đó quá trình nhận dữ liệu coi như đã hoàn tất.

  Kết quả trả về:
      - ESP_OK Thành công.
      - ESP_ERR_INVALID_ARG Lỗi đối số.

  Các đối số:
      - ``channel``: kênh của RMT (0 - 7)
      - ``thresh``: Con trỏ để tiếp nhận giá trị nghỉ của RMT RX.

.. c:function:: esp_err_t rmt_set_mem_block_num(rmt_channel_t channel, uint8_t rmt_mem_num)

  Mô tả hàm:
      Cài đặt số khối nhớ cho kênh RMT.

      Hàm này được sử dụng để điều chỉnh số khối nhớ đặt trong kênh n của module RMT. 8 kênh của RMT cùng sử dụng chung một RAM kích thước 512x32-bit có thể được đọc và ghi bởi bộ xử lý trung tâm thông qua APB (Advanced Peripheral Bus), cũng như được đọc bởi bộ phát và ghi bởi bộ thu.

      Khoảng địa chỉ RAM giành cho kênh n bắt đầu từ ``start_addr_CHn`` đến ``end_addr_CHn``, được xác định bởi: Địa chỉ bắt đầu khối nhớ là **RMT_CHANNEL_MEM(n)** (quy định trong file soc/rmt_reg.h), tức là, địa chỉ bắt đầu:

       ``start_addr_CHn`` = RMT base address + 0x800 + 64*4*n

      Và địa chỉ kết thúc:

       ``end_addr_CHn`` = RMT base address + 0x800 + 64*4*n + 64*4*RMT_MEM_SIZE_CHn mod 512*4.

  Note:
      Nếu số khối nhớ của 1 kênh được cài đặt là một giá trị lớn hơn 1, kênh này sẽ chiếm khối nhớ của kênh tiếp theo. Channel0 có thể sử dụng cả 8 khối nhớ của bộ nhớ, trong khi Channel7 chỉ có thể sử dụng 1 khối nhớ.

  Kết quả trả về:
      - ESP_OK Thành công.
      - ESP_ERR_INVALID_ARG Lỗi đối số.

  Các đối số:
      - ``channel``: kênh của RMT (0 - 7)
      - ``rmt_mem_num``: số khối nhớ của RMT RX, một khối nhớ có kích thước 64*32-bits.

.. c:function:: esp_err_t rmt_get_mem_block_num(rmt_channel_t channel, uint8_t *rmt_mem_num)

  Mô tả hàm:
      Trả lại số khối nhớ của module RMT.

  Kết quả trả về:
      - ESP_OK Thành công
      - ESP_ERR_INVALID_ARG Lỗi đối số

  Các đối số:
      - ``channel``: kênh của RMT (0 - 7)
      - ``rmt_mem_num``: con trỏ để tiếp nhận số khối nhớ RMT RX.

.. c:function:: esp_err_t rmt_set_tx_carrier(rmt_channel_t channel, bool carrier_en, uint16_t high_level, uint16_t low_level, rmt_carrier_level_t carrier_level)

  Mô tả hàm:
      Cài đặt sóng mang RMT cho tín hiệu TX.

      Bằng cách cài đặt các giá trị khác nhau cho carrier_high và carrier_low, ta có thể cài đặt các tần số khác nhau cho sóng mang. Đơn vị của carrier_high/low là nguồn clock tick, không phải là clock chia cho kênh RMT.

  Kết quả trả về:
      - ESP_OK Thành công.
      - ESP_ERR_INVALID_ARG Lỗi đối số.

  Các đối số:
      - ``channel``: kênh của RMT (0 - 7)
      - ``carrier_en``: Nếu cho phép ngõ ra sóng mang.
      - ``high_level``: Khoảng thời gian mức cao của sóng mang.
      - ``low_level``: Khoảng thời gian mức thấp của sóng mang.
      - ``carrier_level``: Điều chỉnh cách biến đổi sóng mang cho các kênh.
      
              - 1'b1: Truyền ngõ ra mức thấp.
              - 1'b0: Truyền ngõ ra mức cao.

.. c:function:: esp_err_t rmt_set_mem_pd(rmt_channel_t channel, bool pd_en)

  Mô tả hàm:
      Cài đặt bộ nhớ RMT ở chế độ low power (năng lượng thấp).

      Cắt giảm năng lượng tiêu tốn bởi bộ nhớ. 1: bộ nhớ đang ở trong trạng thái low power.

  Kết quả trả về:
      - ESP_OK Thành công.
      - ESP_ERR_INVALID_ARG Lỗi đối số.

  Các đối số:
      - ``channel``: kênh của RMT (0 - 7)
      - ``pd_en``: Cho phép bộ nhớ ở chế độ low power.

.. c:function:: esp_err_t rmt_get_mem_pd(rmt_channel_t channel, bool *pd_en)

  Mô tả hàm:
      Lấy kết quả cài đặt chế độ low power cho bộ nhớ.

  Kết quả trả về:
      - ESP_OK Thành công.
      - ESP_ERR_INVALID_ARG Lỗi đối số.

  Các đối số:
      - ``channel``: kênh của RMT (0 - 7)
      - ``pd_en``: Con trỏ để tiếp nhận bộ nhớ chế độ low power của RMT.

.. c:function:: esp_err_t rmt_tx_start(rmt_channel_t channel, bool tx_idx_rst)

  Mô tả hàm:
      Cài đặt RMT bắt đầu gửi dữ liệu từ bộ nhớ.

  Kết quả trả về:
      - ESP_OK Thành công.
      - ESP_ERR_INVALID_ARG Lỗi đối số.

  Các đối số:
      - ``channel``: kênh của RMT (0 - 7)
      - ``tx_idx_rst``: Đặt là "true" để reset chỉ số nhớ cho bộ truyền TX. Các trường hợp còn lại, bộ truyền sẽ tiếp tục gửi dữ liệu từ số chỉ cuối cùng trong bộ nhớ.

.. c:function:: esp_err_t rmt_tx_stop(rmt_channel_t channel)

  Mô tả hàm:
      Cài đặt RMT kết thúc gửi dữ liệu.

  Kết quả trả về:
      - ESP_OK Thành công.
      - ESP_ERR_INVALID_ARG Lỗi đối số.

  Các đối số:
      - ``channel``: kênh của RMT (0 - 7)

.. c:function:: esp_err_t rmt_rx_start(rmt_channel_t channel, bool rx_idx_rst)

  Mô tả hàm:
      Cài đặt RMT bắt đầu nhận dữ liệu.

  Kết quả trả về:
      - ESP_OK Thành công.
      - ESP_ERR_INVALID_ARG Lỗi đối số.

  Các đối số:
      - ``channel``: kênh của RMT (0 - 7)
      - ``rx_idx_rst``: Đặt là "true" để reset chỉ số nhớ cho bộ thu RX. Các trường hợp còn lại, bộ thu sẽ tiếp tục nhận dữ liệu từ số chỉ cuối cùng trong bộ nhớ.

.. c:function:: esp_err_t rmt_rx_stop(rmt_channel_t channel)

  Mô tả hàm:
      Cài đặt RMT kết thúc nhận dữ liệu.

  Kết quả trả về:
      - ESP_OK Thành công.
      - ESP_ERR_INVALID_ARG Lỗi đối số.

  Các đối số:
      - ``channel``: kênh của RMT (0 - 7)

.. c:function:: esp_err_t rmt_memory_rw_rst(rmt_channel_t channel)

  Mô tả hàm:
      Reset chỉ số nhớ bộ truyền và nhận của RMT.

  Kết quả trả về:
      - ESP_OK Thành công.
      - ESP_ERR_INVALID_ARG Lỗi đối số.

  Các đối số:
      - ``channel``: kênh của RMT (0 - 7)

.. c:function:: esp_err_t rmt_set_memory_owner(rmt_channel_t channel, rmt_mem_owner_t owner)

  Mô tả hàm:
      Cài đặt quyền sở hữu bộ nhớ của RMT.

  Kết quả trả về:
      - ESP_OK Thành công.
      - ESP_ERR_INVALID_ARG Lỗi đối số.

  Các đối số:
      - ``channel``: kênh của RMT (0 - 7)
      - ``owner``: Cài đặt khi nào thì bộ truyền hay bộ nhận có thể truy cập để xử lý dữ liệu trong bộ nhớ của channel.

.. c:function:: esp_err_t rmt_get_memory_owner(rmt_channel_t channel, rmt_mem_owner_t *owner)

  Mô tả hàm:
      Lấy kết quả cài đặt quyền sở hữu bộ nhớ của RMT.

  Kết quả trả về:
      - ESP_OK Thành công.
      - ESP_ERR_INVALID_ARG Lỗi đối số.

  Các đối số:
      - ``channel``: kênh của RMT (0 - 7)
      - ``owner``: Con trỏ để lấy quyền sử dụng bộ nhớ.

.. c:function:: esp_err_t rmt_set_tx_loop_mode(rmt_channel_t channel, bool loop_en)

  Mô tả hàm:
      Cài đặt chế độ lặp cho bộ truyền RMT.

  Kết quả trả về:
      - ESP_OK Thành công.
      - ESP_ERR_INVALID_ARG Lỗi đối số.

  Các đối số:
      - ``channel``: kênh của RMT (0 - 7)
      - ``loop_en``: cho phép bộ truyền của RMT ở chế độ truyền lặp vòng.
                    Nếu đặt là "true" thì bộ truyền sẽ tiếp tục gửi từ dữ liệu đầu tiên đến dữ liệu cuối trong channel0-7 lặp đi lặp lại.

.. c:function:: esp_err_t rmt_get_tx_loop_mode(rmt_channel_t channel, bool *loop_en)

  Mô tả hàm:
      Lấy chế độ truyền lặp vòng của RMT

  Kết quả trả về:
      - ESP_OK Thành công.
      - ESP_ERR_INVALID_ARG Lỗi đối số.

  Các đối số:
      - ``channel``: kênh của RMT (0 - 7)
      - ``loop_en``: con trỏ để tiếp nhận chế độ truyền lặp vòng của RMT.

.. c:function:: esp_err_t rmt_set_rx_filter(rmt_channel_t channel, bool rx_filter_en, uint8_t thresh)

  Mô tả hàm:
      Cài đặt bộ lọc cho RMT RX.

      Trong chế độ nhận, channel0-7 sẽ bỏ qua xung ngõ vào khi độ rộng xung này nhỏ hơn mức ngưỡng quy định. Độ rộng này tính bằng clock nguồn, không phải clock đã chia cho channel.

  Kết quả trả về:
      - ESP_FAIL(-1) Parameter error
      - Others(>=0) Số byte dữ liệu đọc được từ slave buffer.

  Các đối số:
      - ``channel``: kênh của RMT (0 - 7)
      - ``rx_filter_en``: Cho phép sử dụng lọc trong bộ thu RMT.
      - ``thresh``: Ngưỡng độ rộng xung cho bộ thu.

.. c:function:: esp_err_t rmt_set_source_clk(rmt_channel_t channel, rmt_source_clk_t base_clk)

  Mô tả hàm:
      Cài đặt nguồn clock cho RMT.

      Module RMT có 2 nguồn clock là:
          1. APB clock tương ứng 80MHz
          2. REF tick clock tương ứng 1MHz (không được hỗ trợ trong phiên bản này.)

  Kết quả trả về:
      - ESP_OK Thành công.
      - ESP_ERR_INVALID_ARG Lỗi đối số.

  Các đối số:
      - ``channel``: kênh của RMT (0 - 7)
      - ``base_clk``: Chọn nguồn clock cho module RMT.

.. c:function:: esp_err_t rmt_get_source_clk(rmt_channel_t channel, rmt_source_clk_t *src_clk)

  Mô tả hàm:
      Lấy nguồn clock cho module RMT.

  Kết quả trả về:
      - ESP_OK Thành công.
      - ESP_ERR_INVALID_ARG Lỗi đối số.

  Các đối số:
      - ``channel``: kênh của RMT (0 - 7)
      - ``src_clk``: Con trỏ tiếp nhận nguồn clock cho module RMT.

.. c:function:: esp_err_t rmt_set_idle_level(rmt_channel_t channel, bool idle_out_en, rmt_idle_level_t level)

  Mô tả hàm:
      Cài đặt mức chế độ nghỉ ngõ ra cho bộ truyền RMT

  Kết quả trả về:
      - ESP_OK Thành công.
      - ESP_ERR_INVALID_ARG Lỗi đối số.

  Các đối số:
      - ``channel``: kênh của RMT (0 - 7)
      - ``idle_out_en``: Cho phép mức chế độ nghỉ ngõ ra.
      - ``level``: Cài đặt mức tín hiệu ngõ ra khi ở trạng thái nghỉ cho channel0-7.

.. c:function:: esp_err_t rmt_get_status(rmt_channel_t channel, uint32_t *status)

  Mô tả hàm:
      Lấy trạng thái của module RMT.

  Kết quả trả về:
      - ESP_OK Thành công.
      - ESP_ERR_INVALID_ARG Lỗi đối số.

  Các đối số:
      - ``channel``: kênh của RMT (0 - 7)
      - ``status``: con trỏ để tiếp nhận trạng thái kênh.

.. c:function:: void rmt_set_intr_enable_mask(uint32_t mask)

  Mô tả hàm:
      Đặt giá trị mặt nạ cho thanh ghi cho phép ngắt của RMT.

  Các đối số:
      - ``mask``: bit mặt nạ để cài đặt thanh ghi.

.. c:function:: void rmt_clr_intr_enable_mask(uint32_t mask)

  Mô tả hàm:
      Giá trị mặt nạ để xóa cài đặt thanh ghi cho phép ngắt.

  Các đối số:
      - ``mask``: bit mặt nạ để xóa cài đặt thanh ghi.

.. c:function:: esp_err_t rmt_set_rx_intr_en(rmt_channel_t channel, bool en)

  Mô tả hàm:
      Cài đặt cho phép ngắt thu RMT.

  Kết quả trả về:
      - ESP_OK Thành công.
      - ESP_ERR_INVALID_ARG Lỗi đối số.

  Các đối số:
      - ``channel``: kênh của RMT (0 - 7)
      - ``en``: cho phép hoặc không cho phép ngắt thu RMT.

.. c:function:: esp_err_t rmt_set_err_intr_en(rmt_channel_t channel, bool en)

  Mô tả hàm:
      Cài đặt cho phép lỗi ngắt thu RMT.

  Kết quả trả về:
      - ESP_OK Thành công.
      - ESP_ERR_INVALID_ARG Lỗi đối số.

  Các đối số:
      - ``channel``: kênh của RMT (0 - 7)
      - ``en``: cho phép hoặc không cho phép lỗi ngắt thu RMT.

.. c:function:: esp_err_t rmt_set_tx_intr_en(rmt_channel_t channel, bool en)

  Mô tả hàm:
      Cài đặt cho phép ngắt phát RMT.

  Kết quả trả về:
      - ESP_OK Thành công.
      - ESP_ERR_INVALID_ARG Lỗi đối số.

  Các đối số:
      - ``channel``: kênh của RMT (0 - 7)
      - ``en``: cho phép hoặc không cho phép ngắt phát RMT.

.. c:function:: esp_err_t rmt_set_tx_thr_intr_en(rmt_channel_t channel, bool en, uint16_t evt_thresh)

  Mô tả hàm:
    Cài đặt cho phép ngắt ngưỡng sự kiện phát RMT.

    Gây ra 1 ngắt khi có một số lượng dữ liệu được truyền xong. Số lượng này có thể cài đặt được gọi là ngưỡng.

  Kết quả trả về:
      - ESP_OK Thành công.
      - ESP_ERR_INVALID_ARG Lỗi đối số.

  Các đối số:
      - ``channel``: kênh của RMT (0 - 7)
      - ``en``: cho phép hoặc không cho phép ngắt phát theo sự kiện.
      - ``evt_thresh``: Giá trị ngưỡng để ngắt sự kiện cho RMT.

.. c:function:: esp_err_t rmt_set_pin(rmt_channel_t channel, rmt_mode_t mode, gpio_num_t gpio_num)

  Mô tả hàm:
      Cài đặt chân cho module RMT.

  Kết quả trả về:
      - ESP_OK Thành công.
      - ESP_ERR_INVALID_ARG Lỗi đối số.

  Các đối số:
      - ``channel``: kênh của RMT (0 - 7)
      - ``mode``: Chế độ truyền (TX) hay nhận (RX) cho RMT.
      - ``gpio_num``: Tên chân GPIO để truyền/nhận tín hiệu.

.. c:function:: esp_err_t rmt_config(const rmt_config_t *rmt_param)

  Mô tả hàm:
      Cấu hình các thông số cho RMT.

  Kết quả trả về:
      - ESP_OK Thành công.
      - ESP_ERR_INVALID_ARG Lỗi đối số.

  Các đối số:
      - ``rmt_param``: Con trỏ đến trường chứa thông số cho RMT.

.. c:function:: esp_err_t rmt_isr_register(void (*fn)(void *), void *arg, int intr_alloc_flags, rmt_isr_handle_t *handle, )

  Mô tả hàm:
      Đăng kí trình xử lý ngắt cho RMT. Trình xử lý này là một trình phục vụ ngắt.

      Trình xử lý này sẽ được đính kèm vào chung bộ xử lý trung tâm đã gọi hàm này.

  Note:
      Nếu đã gọi hàm rmt_driver_install để sử dụng RMT driver thì không được đăng kí trình phục vụ ngắt nữa.

  Kết quả trả về:
      - ESP_OK Thành công.
      - ESP_ERR_INVALID_ARG Lỗi đối số.
      - ESP_FAIL Driver hệ thống đã được cài đặt, không thể đăng kí trình phục vụ ngắt cho RMT.

  Các đối số:
      - ``fn``: tên hàm xử lý ngắt.
      - ``arg``: đối số cho hàm xử lý ngắt.
      - ``intr_alloc_flags``: Các cờ sử dụng để cấp phát ngắt. Một hay nhiều giá trị ESP_INTR_FLAG_* dùng để xác định mức độ ưu tiên cho ngắt. Được quy định trong file esp_intr_alloc.h
      - ``handle``: Nếu khác 0, một trình xử lý khác sẽ được tạo ra để xóa trình phục vụ ngắt lưu ở đây.

.. c:function:: esp_err_t rmt_isr_deregister(rmt_isr_handle_t handle)

  Mô tả hàm:
      Hủy đăng kí trình xử lý ngắt RMT đã có.

  Kết quả trả về:
      - ESP_OK Thành công.
      - ESP_ERR_INVALID_ARG Lỗi đối số.

  Các đối số:
      - ``handle``: trình xử lý có được khi gọi hàm rmt_isr_deregister.

.. c:function:: esp_err_t rmt_fill_tx_items(rmt_channel_t channel, const rmt_item32_t *item, uint16_t item_num, uint16_t mem_offset)

  Mô tả hàm:
      Lấp đầy bộ nhớ dữ liệu của channel bằng các gói tin của RMT.

  Kết quả trả về:
      - ESP_OK Thành công.
      - ESP_ERR_INVALID_ARG Lỗi đối số.

  Các đối số:
      - ``channel``: kênh của RMT (0 - 7)
      - ``item``: Con trỏ chỉ tới các gói tin.
      - ``item_num``: số gói tin gửi đi.
      - ``mem_offset``: Số chỉ của bộ nhớ.

.. c:function:: esp_err_t rmt_driver_install(rmt_channel_t channel, size_t rx_buf_size, int intr_alloc_flags)

  Mô tả hàm:
      Khởi tạo RMT driver.

  Kết quả trả về:
      - ESP_OK Thành công.
      - ESP_ERR_INVALID_ARG Lỗi đối số.
      - ESP_ERR_NO_MEM Cấp phát bộ nhớ thất bại.
      - ESP_ERR_INVALID_STATE Driver đã được cài đặt rồi, gọi hàm rmt_driver_uninstall trước.

  Các đối số:
      - ``channel``: kênh của RMT (0 - 7)
      - ``rx_buf_size``: Kích thước bộ đệm thu. Có thể cài là 0 nếu không sử dụng bộ đệm thu.
      - ``intr_alloc_flags``: Các cờ sử dụng cho ngắt. Đặt là 0 để sử dụng cờ mặc định. Xem file esp_intr_alloc.h để biết thông tin chi tiết.

.. c:function:: esp_err_t rmt_driver_uninstall(rmt_channel_t channel)

  Mô tả hàm:
      Gỡ cài đặt driver

  Kết quả trả về:
      - ESP_OK Thành công.
      - ESP_ERR_INVALID_ARG Lỗi đối số.

  Các đối số:
      - ``channel``: kênh của RMT (0 - 7)

.. c:function:: esp_err_t rmt_write_items(rmt_channel_t channel, const rmt_item32_t *rmt_item, int item_num, bool wait_tx_done)

  Mô tả hàm:
      RMT gửi đi dạng sóng từ mảng rmt_item.

      API này cho phép người dùng gửi đi sóng có chiều dài bất kì.

  Kết quả trả về:
      - ESP_OK Thành công.
      - ESP_ERR_INVALID_ARG Lỗi đối số.

  Các đối số:
      - ``channel``: kênh của RMT (0 - 7)
      - ``rmt_item``: Con trỏ tới địa chỉ đầu tiên của mảng gói tin.
      - ``item_num``: Số lượng gói tin dữ liệu.
      - ``wait_tx_done``: Nếu cài đặt là 1, nó sẽ chặn các tác vụ khác và chờ cho đến khi việc gửi đi hoàn tất.

      .. highlight::

      ::

        Nếu cài đặt là 0, nó sẽ không chờ và return ngay lập tức.
        @note
        Hàm này sẽ không sao chép dữ liệu, thay vào đó, nó sẽ trỏ tới dữ liệu gốc,
        và gửi đi dữ liệu theo dạng sóng.
        Nếu wait_tx_done được cài là "true", hàm này sẽ chặn và không return cho đến khi
        tất cả dữ liệu được gửi đi.
        Nếu wait_tx_done được cài là "false", hàm này sẽ return ngay lập tức, và một trình điểu khiển ngắt
        (driver interrupt) sẽ tiếp tục gửi dữ liệu đi.
        Ta phải đảm bảo dữ liệu sẽ không bị mất khi driver gửi dữ liệu đi trong driver interrupt.

.. c:function:: esp_err_t rmt_wait_tx_done(rmt_channel_t channel)

  Mô tả hàm:
      Chờ RMT gửi hoàn tất.

  Kết quả trả về:
      - ESP_OK Thành công.
      - ESP_ERR_INVALID_ARG Lỗi đối số.

  Các đối số:
      - ``channel``: kênh của RMT (0 - 7)

.. c:function:: esp_err_t rmt_get_ringbuf_handler(rmt_channel_t channel, RingbufHandle_t *buf_handler)

  Mô tả hàm:
      Lấy bộ đệm (ringbuffer) từ UART

      Người dùng có thể lấy trình xử lý bộ đệm thu, và xử lý dữ liệu thu được.

  Kết quả trả về:
      - ESP_OK Thành công.
      - ESP_ERR_INVALID_ARG Lỗi đối số.

  Các đối số:
      - ``channel``: kênh của RMT (0 - 7)
      - ``buf_handler``: Con trỏ đến buffer handler để tiếp nhận trình xử lý bộ đệm thu.

Structures
**********

.. c:member:: struct rmt_tx_config_t

    Trường dữ liệu của RMT TX dùng để cấu hình các thông số.

      **Public Members**

      .. c:member:: bool loop_en

          Chế độ truyền lặp vòng cho RMT.

      .. c:member:: uint32_t carrier_freq_hz

          Tần số sóng mang cho RMT.

      .. c:member:: uint8_t carrier_duty_percent

          Hiệu suất sóng mang của RMT. (%)

      .. c:member:: rmt_carrier_level_t carrier_level

          Mức sóng mang của RMT

      .. c:member:: bool carrier_en

          Cho phép RMT sử dụng sóng mang.

      .. c:member:: rmt_idle_level_t idle_level

          Mức chế độ nghỉ của RMT.

      .. c:member:: bool idle_output_en

          Cho phép mức chế độ nghỉ ở ngõ ra.

.. c:member:: struct rmt_rx_config_t

      Trường dữ liệu của RMT RX dùng để cấu hình các thông số.

        **Public Members**

      .. c:member:: bool filter_en

          Cho phép chức năng bộ lọc thu.

      .. c:member:: uint8_t filter_ticks_thresh

          Số tick hệ thống để lọc dữ liệu khi nhận vào.

      .. c:member:: uint16_t idle_threshold

          Ngưỡng nghỉ cho RMT RX.

.. c:member:: struct rmt_config_t

      Trường dữ liệu của RMT dùng để cấu hình các thông số.

        **Public Members**

      .. c:member:: rmt_mode_t rmt_mode

          Chế độ RMT: phát hay thu.

      .. c:member:: rmt_channel_t channel

          RMT channel.

      .. c:member:: uint8_t clk_div

          Bộ chia clock cho channel.

      .. c:member:: gpio_num_t gpio_num

          Chân GPIO cho RMT.

      .. c:member:: uint8_t mem_block_num

          Số khối nhớ của RMT.

      .. c:member:: rmt_tx_config_t tx_config

          Thông số cho RMT TX

      .. c:member:: rmt_rx_config_t rx_config

          Thông số cho RMT RX

Macro
*****

.. c:macro:: RMT_MEM_BLOCK_BYTE_NUM (256)

.. c:macro:: RMT_MEM_ITEM_NUM (RMT_MEM_BLOCK_BYTE_NUM/4)

Type Definitions
****************

.. c:type:: typedef intr_handle_t rmt_isr_handle_t

Enumerations
************

.. cpp:enum:: rmt_channel_t

    `Các giá trị`:

    .. c:macro:: RMT_CHANNEL_0 = 0

        RMT channel0

    .. c:macro:: RMT_CHANNEL_1

        RMT channel1

    .. c:macro:: RMT_CHANNEL_2

        RMT channel2

    .. c:macro:: RMT_CHANNEL_3

        RMT channel3

    .. c:macro:: RMT_CHANNEL_4

        RMT channel4

    .. c:macro:: RMT_CHANNEL_5

        RMT channel5

    .. c:macro:: RMT_CHANNEL_6

        RMT channel6

    .. c:macro:: RMT_CHANNEL_7

        RMT channel7

    .. c:macro:: RMT_CHANNEL_MAX

.. cpp:enum:: rmt_mem_owner_t

    `Các giá trị`:

    .. c:macro:: RMT_MEM_OWNER_TX = 0

        RMT TX mode, bộ phát sử dụng khối nhớ.

    .. c:macro:: RMT_MEM_OWNER_RX = 1

        RMT RX mode, bộ thu sử dụng khối nhớ.

    .. c:macro:: RMT_MEM_OWNER_MAX

.. cpp:enum:: rmt_source_clk_t

    `Các gía trị`:

    .. c:macro:: RMT_BASECLK_REF = 0

        Nguồn clock của RMT là REF clock, mặc định là 1MHz (Không hỗ trợ ở phiên bản này)

    .. c:macro:: RMT_BASECLK_APB

        Nguồn clock của RMT là APB (Advanced Peripheral Bus) clock, mặc định là 80MHz.

    .. c:macro:: RMT_BASECLK_MAX

.. cpp:enum:: rmt_data_mode_t

    `Các giá trị`:

    .. c:macro:: RMT_DATA_MODE_FIFO = 0

    .. c:macro:: RMT_DATA_MODE_MEM = 1

    .. c:macro:: RMT_DATA_MODE_MAX

.. cpp:enum:: rmt_mode_t

   `Các giá trị`:

    .. c:macro:: RMT_MODE_TX = 0

        RMT TX mode.

    .. c:macro:: RMT_MODE_RX

        RMT RX mode

    .. c:macro:: RMT_MODE_MAX

.. cpp:enum:: rmt_idle_level_t

   `Các giá trị`:

    .. c:macro:: RMT_IDLE_LEVEL_LOW = 0

        Mức nghỉ cho RMT TX: mức thấp

    .. c:macro:: RMT_IDLE_LEVEL_HIGH

        Mức nghỉ cho RMT TX: mức cao

    .. c:macro:: RMT_IDLE_LEVEL_MAX

.. cpp:enum:: rmt_carrier_level_t

   `Các giá trị`:

    .. c:macro:: RMT_CARRIER_LEVEL_LOW = 0

        Sóng mang RMT được biến đổi để ngõ ra mức thấp.

    .. c:macro:: RMT_CARRIER_LEVEL_HIGH

        Sóng mang RMT được biến đổi để ngõ ra mức cao.

    .. c:macro:: RMT_CARRIER_LEVEL_MAX

.. _driver/include/driver/rmt.h: https://github.com/espressif/esp-idf/blob/dce7fcb/components/driver/include/driver/rmt.h
.. _peripherals/rmt_nec_tx_rx.: https://github.com/espressif/esp-idf/tree/e165336/examples/peripherals/rmt_nec_tx_rx
Lưu ý
=====
* Hướng dẫn cài đặt `ESP-IDF <https://esp-idf.readthedocs.io/en/latest/index.html>`_
* Nạp và Debug chương trình `xem tại đây <https://esp-idf.readthedocs.io/en/latest/index.html>`_
* Tài nguyên hệ thống xem `tại đây <https://github.com/espressif/esp-idf>`_
