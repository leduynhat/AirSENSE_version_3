GPIO & RTC GPIO
===============

Tổng Quan
---------

Chip ESP32 có 40 pad chức năng vật lý. Một số pad không được dùng hoặc không có chân tương ứng trên chip (tham khảo datasheet). Mỗi pad có thể dùng như một chân I/O hoặc có thể kết nối với một chân tín hiệu khác của ESP32. 

- Note: GPIO-11 thường dùng cho SPI flash.
- GPIO-34-39 chỉ có thể được set là kiểu chân input và không thể dùng phần mềm để pull-up và pull-down.

Ngoài ra còn hỗ trợ riêng "RTC GPIO", có chức năng khi các chân GPIO định tuyến đến hệ thống con "RTC" low-power và analog. Những pin chức năng này có thể sử dụng khi sleep, khi Ultra Low Power co-processor đang chạy, hay khi các chức năng analog như ADC, DAC, v.v đang dùng.

Chuẩn bị
========
    +---------------------------------+-----------------------------------------------------------+
    | **Tên board mạch**              | **Link**                                                  |
    +=================================+===========================================================+
    | Board ESP32 IoT Uno             | https://github.com/esp32vn/esp32-iot-uno                  |
    +---------------------------------+-----------------------------------------------------------+

Ví dụ minh họa:
===============

Ví dụ này sẽ cho chúng ta biết cấu hình GPIO và sử dụng ngắt GPIO như thế nào.

Gán Chân:

 * GPIO18: output.
 * GPIO19: output.
 * GPIO4:  input, pulled up, ngắt từ cạnh lên và cạnh xuống.
 * GPIO5:  input, pulled up, ngắt từ cạnh lên.

Kết nối chân:

 * GPIO18 với GPIO4.
 * GPIO19 với GPIO5.
 * Tạo các xung lên trên GPIO18/19, gây ra ngắt trên GPIO04/05.

  .. highlight:: c

::

  /* GPIO Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"

/**
 * Brief:
 * This test code shows how to configure gpio and how to use gpio interrupt.
 *
 * GPIO status:
 * GPIO18: output
 * GPIO19: output
 * GPIO4:  input, pulled up, interrupt from rising edge and falling edge
 * GPIO5:  input, pulled up, interrupt from rising edge.
 *
 * Test:
 * Connect GPIO18 with GPIO4
 * Connect GPIO19 with GPIO5
 * Generate pulses on GPIO18/19, that triggers interrupt on GPIO4/5
 *
 */

#define GPIO_OUTPUT_IO_0    18
#define GPIO_OUTPUT_IO_1    19
#define GPIO_OUTPUT_PIN_SEL  ((1<<GPIO_OUTPUT_IO_0) | (1<<GPIO_OUTPUT_IO_1))
#define GPIO_INPUT_IO_0     4
#define GPIO_INPUT_IO_1     5
#define GPIO_INPUT_PIN_SEL  ((1<<GPIO_INPUT_IO_0) | (1<<GPIO_INPUT_IO_1))
#define ESP_INTR_FLAG_DEFAULT 0

static xQueueHandle gpio_evt_queue = NULL;

static void IRAM_ATTR gpio_isr_handler(void* arg)
{
    uint32_t gpio_num = (uint32_t) arg;
    xQueueSendFromISR(gpio_evt_queue, &gpio_num, NULL);
}

static void gpio_task_example(void* arg)
{
    uint32_t io_num;
    for(;;) {
        if(xQueueReceive(gpio_evt_queue, &io_num, portMAX_DELAY)) {
            printf("GPIO[%d] intr, val: %d\n", io_num, gpio_get_level(io_num));
        }
    }
}

void app_main()
{
    gpio_config_t io_conf;
    //disable interrupt
    io_conf.intr_type = GPIO_PIN_INTR_DISABLE;
    //set as output mode
    io_conf.mode = GPIO_MODE_OUTPUT;
    //bit mask of the pins that you want to set,e.g.GPIO18/19
    io_conf.pin_bit_mask = GPIO_OUTPUT_PIN_SEL;
    //disable pull-down mode
    io_conf.pull_down_en = 0;
    //disable pull-up mode
    io_conf.pull_up_en = 0;
    //configure GPIO with the given settings
    gpio_config(&io_conf);

    //interrupt of rising edge
    io_conf.intr_type = GPIO_PIN_INTR_POSEDGE;
    //bit mask of the pins, use GPIO4/5 here
    io_conf.pin_bit_mask = GPIO_INPUT_PIN_SEL;
    //set as input mode    
    io_conf.mode = GPIO_MODE_INPUT;
    //enable pull-up mode
    io_conf.pull_up_en = 1;
    gpio_config(&io_conf);

    //change gpio intrrupt type for one pin
    gpio_set_intr_type(GPIO_INPUT_IO_0, GPIO_INTR_ANYEDGE);

    //create a queue to handle gpio event from isr
    gpio_evt_queue = xQueueCreate(10, sizeof(uint32_t));
    //start gpio task
    xTaskCreate(gpio_task_example, "gpio_task_example", 2048, NULL, 10, NULL);

    //install gpio isr service
    gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT);
    //hook isr handler for specific gpio pin
    gpio_isr_handler_add(GPIO_INPUT_IO_0, gpio_isr_handler, (void*) GPIO_INPUT_IO_0);
    //hook isr handler for specific gpio pin
    gpio_isr_handler_add(GPIO_INPUT_IO_1, gpio_isr_handler, (void*) GPIO_INPUT_IO_1);

    //remove isr handler for gpio number.
    gpio_isr_handler_remove(GPIO_INPUT_IO_0);
    //hook isr handler for specific gpio pin again
    gpio_isr_handler_add(GPIO_INPUT_IO_0, gpio_isr_handler, (void*) GPIO_INPUT_IO_0);

    int cnt = 0;
    while(1) {
        printf("cnt: %d\n", cnt++);
        vTaskDelay(1000 / portTICK_RATE_MS);
        gpio_set_level(GPIO_OUTPUT_IO_0, cnt % 2);
        gpio_set_level(GPIO_OUTPUT_IO_1, cnt % 2);
    }
}

 


Application Example
-------------------

GPIO output and input interrupt example:`https://github.com/espressif/esp-idf/tree/dce7fcb/examples/peripherals/gpio`.

API Reference - Normal GPIO
---------------------------

Header File
^^^^^^^^^^^

- `https://github.com/espressif/esp-idf/blob/dce7fcb/components/driver/include/driver/gpio.h`


Functions
^^^^^^^^^

.. c:function:: esp_err_t gpio_config(const gpio_config_t *pGPIOConfig)

Cấu hình thông thường GPIO.

Cấu hình chế độ cho GPIO, pull-up, pull-down, IntrType.

Return

- ESP_OK thành công.

- ESP_ERR_INVALID_ARG lỗi thông số.

Parameters

- ``pGPIOConfig``: trỏ đến GPIO struct cấu hình.

.. c:function:: esp_err_t gpio_set_intr_type(gpio_num_t gpio_num, gpio_int_type_t intr_type)

Kích hoạt loại ngắt cho GPIO.

Return

- ESP_OK thành công.

- ESP_ERR_INVALID_ARG lỗi thông số.

Parameters

- ``gpio_num``: số của pin GPIO. Nếu bạn muốn set để kích hoạt nó, ví dụ: GPIO16, gpio_num phải là GPIO_NUM_16(16); 


- ``intr_type``: kiểu ngắt, chọn từ gpio_int_type_t.

.. c:function:: esp_err_t gpio_intr_enable(gpio_num_t gpio_num)

Bật chế độ tín hiệu ngắt của GPIO.

Return

- ESP_OK thành công.

- ESP_ERR_INVALID_ARG lỗi thông số.

Parameters

- ``gpio_num``:số pin của GPIO. Nếu bạn muốn bật một ngắt cho nó, ví dụ: GPIO16, gpio_num phải là GPIO_NUM_16(16); 


.. c:function:: esp_err_t gpio_intr_disable(gpio_num_t gpio_num)

Tắt chế độ tín hiệu ngắt cho GPIO.

Return

- ESP_OK thành công.

- ESP_ERR_INVALID_ARG lỗi thông số.

Parameters

- ``gpio_num``: số pin GPIO. Nếu bạn muốn tắt một ngắt cho nó, ví dụ: GPIO16, gpio_num phải là GPIO_NUM_16(16); 


.. c:function:: esp_err_t gpio_set_level(gpio_num_t gpio_num, uint32_t level)

Set mức output cho GPIO.

Return

- ESP_OK thành công.

- ESP_ERR_INVALID_ARG lỗi số pin GPIO.

Parameters

- ``gpio_num``: số pin GPIO. nếu bạn muốn set mức output, ví dụ:GPIO16, gpio_num phải là GPIO_NUM_16(16); 


- ``level``: mức output. 0:low, 1:high.

.. c:function:: int gpio_get_level(gpio_num_t gpio_num)

Đặt mức đầu vào GPIO.

Return

- 0 thì mức GPIO input là 0.

- 1 thì mức GPIO input là 1.

Parameters

- ``gpio_num``: số pin GPIO. Nếu bạn muốn đặt mức logic, ví dụ: GPIO16, gpio_num phải là GPIO_NUM_16(16); 


.. c:function:: esp_err_t gpio_set_direction(gpio_num_t gpio_num, gpio_mode_t mode)

Set hướng cho GPIO.

Định hướng GPIO, chẳng hạn như output_only, input_only, output_and_input.

Return

- ESP_OK thành công.

- ESP_ERR_INVALID_ARG lỗi GPIO.

Parameters

- ``gpio_num``: cấu hình số pin cho các pin GPIO, nó phải là số. Nếu bạn muốn set hướng, ví dụ: GPIO16, gpio_num phải là GPIO_NUM_16(16);

- ``mode``: hướng GPIO.

.. c:function:: esp_err_t gpio_set_pull_mode(gpio_num_t gpio_num, gpio_pull_mode_t pull)

Cấu hình điện trở kéo GPIO pull-up/pull-down.

Chỉ có các pin mà hỗ trợ cả hai input và output có thể tích hợp pull-up và pull-down điện trở kéo. Không dùng các GPIO 34-39 vì nó chỉ input.

return

- ESP_OK thành công.

- ESP_ERR_INVALID_ARG: lỗi thông số.

Parameters

- ``gpio_num``: số của GPIO. Nếu bạn muốn set chế độ pull-up hay down cho GPIO, ví dụ: GPIO16, gpio_num phải là GPIO_NUM_16(16);

- ``pull``: GPIO chế độ pull-up/down.

.. c:function:: esp_err_t gpio_wakeup_enable(gpio_num_t gpio_num, gpio_int_type_t intr_type)

Bật chức năng đánh thức(wake-up) cho GPIO.

Return

- ESP_OK thành công.

- ESP_ERR_INVALID_ARG lỗi thông số.

Parameters

- ``gpio_num``: số của GPIO.

- ``intr_type``: đánh thức GPIO. Chỉ có GPIO_INTR_LOW_LEVEL hoặc GPIO_INTR_HIGH_LEVEL có thể dùng.

.. c:function:: esp_err_t gpio_wakeup_disable(gpio_num_t gpio_num)

Tắt chức năng đánh thức GPIO.

Return

- ESP_OK thành công.

- ESP_ERR_INVALID_ARG lỗi thông số.

Parameters

- ``gpio_num``: số của GPIO.

.. c:function:: esp_err_t gpio_isr_register(void (*fn)(void *), void *arg, int intr_alloc_flags, gpio_isr_handle_t *handle, )

Đăng kí x ử lý ngắt cho GPIO, xử lí một ISR. Trình xử lí sẽ gắn vào cùng một lõi CPU mà chức năng này đang chạy.

Chức năng ISR này được gọi bất cứ khi nào ngắt GPIO bị gián đoạn. Xem API thay thế gpio_install_isr_service() và gpio_isr_handler_add() để hỗ trợ trình điều khiển cho mỗi GPIO ISR.

Để vô hiệu hóa hoặc loại bỏ các ISR, vượt qua các xử lí trả về chức năng phân bổ ngắt.

Parameters

- ``fn``: chức năng xử lí ngắt.

- ``intr_alloc_flags``: các cờ dùng để chỉ định ngắt. Một hoặc nhiều (ORred) ESP_INTR_FLAG_* giá trị. Xem  esp_intr_alloc.h để biết thêm thông tin.

- ``arg``: thông số cho chức năng xử lí.

- ``handle``: trỏ để xử lí trở lại. Nếu không phải NULL, một xử lí ngắt sẽ trở lại tại đây.

return

- ESP_OK thành công.

- ESP_ERR_INVALID_ARG lỗi GPIO.

.. c:function:: esp_err_t gpio_pullup_en(gpio_num_t gpio_num)

Bật pull-up trên GPIO.

Return

- ESP_OK thành công.

- ESP_ERR_INVALID_ARG lỗi thông số.

Parameters

- ``gpio_num``: số của GPIO.

.. c:function:: esp_err_t gpio_pullup_dis(gpio_num_t gpio_num)

Tắt pull-up trên GPIO.

Return

- ESP_OK thành công.

- ESP_ERR_INVALID_ARG lỗi thông số.

Parameters

- ``gpio_num``: số của GPIO.

.. c:function:: esp_err_t gpio_pulldown_en(gpio_num_t gpio_num)

Bật pull-down trên GPIO.

Return

- ESP_OK thành công.

- ESP_ERR_INVALID_ARG lỗi thông số.

Parameters

- ``gpio_num``: số của GPIO.

.. c:function:: esp_err_t gpio_pulldown_dis(gpio_num_t gpio_num)

Tắt pull-down trên GPIO.

Return

- ESP_OK thành công.

- ESP_ERR_INVALID_ARG lỗi thông số.

Parameters

- ``gpio_num``: số của GPIO.

.. c:function:: esp_err_t gpio_install_isr_service(int intr_alloc_flags)

Cài đặt driver GPIO ISR service xử lý, cho phép xử lý ngắt trên các pin GPIO.

Hàm này không tương thích với ``gpio_isr_register()``-nếu hàm này được dùng thì một ISR chung duy nhất sẽ  đăng ký cho tất cả các ngắt GPIO. Nếu dùng hàm này ``gpio_isr_handler_add()`` thì service ISR sẽ cung cấp một GPIO ISR chung cho mỗi pin GPIO riêng lẻ.

Return

- ESP_OK thành công.

- ESP_FAIL hoạt động thất bại.

- ESP_ERR_NO_MEM không có bộ nhớ để cài đặt service này.

Parameters

-``intr_alloc_flags``: các cờ flag được dùng cho ngắt. Một hoặc nhiều giá trị (ORred) ESP_INTR_FLAG_*. Xem esp_intr_alloc.h để biết thêm chi tiết.

.. c:function:: void gpio_uninstall_isr_service()

Thoát cài đặt driver GPIO ISR service, giải phóng tài nguyên liên quan.

.. c:function:: esp_err_t gpio_isr_handler_add(gpio_num_t gpio_num, gpio_isr_t isr_handler, void *args)

Add trình xử lý ISR cho pin GPIO tương ứng.

Gọi hàm này sau khi dùng ``gpio_install_isr_service()`` để cài đặt trình xử lý GPIO service.

Các pin xử lý cần phải được khai báo với IRAM_ATTR, trừ khi bạn thông qua cờ flag ESP_INTR_FLAG_IRAM khi cấp cho ISR trong gpio_install_isr_service().

Trình xủ lý ISR này sẽ gọi từ một ISR. Vì vậy có một giới hạn kích thước stack (cấu hình "ISR stack size" trong menuconfig). Giới hạn này rất nhỏ so với trình xử lý ngắt chung GPIO do mức độ bổ sung gián tiếp.

Return

- ESP_OK thành công.

- ESP_ERR_INVALID_STATE sai trạng thái, ISR service không được khởi tạo.

- ESP_ERR_INVALID_ARG lỗi thông số.

Parameters

- ``gpio_num``: số của GPIO.

- ``isr_handler``: hàm xử lý ISR cho số GPIO tương ứng.

- ``args``: thông số cho trình xử lý ISR.

.. c:function:: esp_err_t gpio_isr_handler_remove(gpio_num_t gpio_num)

Bỏ trình xử lý ISR cho số pin GPIO tương ứng.

Return

- ESP_OK thành công.

- ESP_ERR_INVALID_STATE sai trạng thái, ISR service không được khởi tạo.

- ESP_ERR_INVALID_ARG lỗi thông số.

Parameters

- ``gpio_num``: số của GPIO.

Structures
^^^^^^^^^^

.. c:function:: struct gpio_config_t

Cấu hình các thông số của pad GPIO cho hàm ``gpio_config``.

Public Members

``uint64_t pin_bit_mask``

Pin GPIO: set với bit mask, mỗi bit maps đến một GPIO.

``gpio_mode_t mode``

Chế độ GPIO:set chế độ input/output.

``gpio_pullup_t pull_up_en``

GPIO pull-up.

``gpio_pulldown_t pull_down_en``

GPIO pull-down.

``gpio_int_type_t intr_type``

Loại ngắt GPIO.

Macros
^^^^^^

.. c:function:: GPIO_SEL_0 (BIT(0))

Đã chọn pin 0.

.. c:function:: GPIO_SEL_1 (BIT(1))

Đã chọn pin 1.

.. c:function:: GPIO_SEL_2 (BIT(2))

Đã chọn pin 2.

Note

Số macros cứ như thế tới chân 39, không bao gồm chân 20,24 và 28..31. Chúng không được show ở đây để giảm những thông tin dư thừa cho các bạn.

.. c:function:: GPIO_IS_VALID_GPIO(gpio_num) ((gpio_num < GPIO_PIN_COUNT && GPIO_PIN_MUX_REG[gpio_num] != 0))

Kiểm tra xem số chân GPIO hợp lệ hay chưa.

.. c:function:: GPIO_IS_VALID_OUTPUT_GPIO(gpio_num) ((GPIO_IS_VALID_GPIO(gpio_num)) && (gpio_num < 34))

Kiểm tra xem có thể nó là các chân chỉ có chế độ output.

Type Definitions
^^^^^^^^^^^^^^^^

.. c:function:: typedef void (*gpio_isr_t)(void *)

.. c:function:: typedef intr_handle_t gpio_isr_handle_t

Enumerations
^^^^^^^^^^^^

.. c:function:: enum gpio_num_t

Values:

``GPIO_NUM_0 = 0``

GPIO0, input và output.

``GPIO_NUM_1 = 1``

GPIO1, input và output.

``GPIO_NUM_2 = 2``

GPIO2, input và output.

Note

Có nhiều hơn thế này nữa, có thể liệt kê lên đến GPIO39, không bao gồm GPIO20,GPIO24 và GPIO28..GPIO31. Chúng không được show ở đây để giảm những thông tin dư thừa cho các bạn.

Note

GPIO34..39 chỉ có chế độ input.

.. c:function:: enum gpio_int_type_t

Values:

``GPIO_INTR_DISABLE = 0``

Vô hiệu hóa GPIO ngắt.

``GPIO_INTR_POSEDGE = 1``

Kiểu ngắt GPIO: cạnh lên.

``GPIO_INTR_NEGEDGE = 2``

Kiểu ngắt GPIO: cạnh xuống.

``GPIO_INTR_ANYEDGE = 3``

Kiểu ngắt GPIO: cả cạnh lên và xuống.

``GPIO_INTR_LOW_LEVEL = 4``

Kiểu ngắt GPIO: kích hoạt input ở mức thấp.

``GPIO_INTR_HIGH_LEVEL = 5``

Kiểu ngắt GPIO: kích hoạt input ở mức cao.

``GPIO_INTR_MAX``

.. c:function:: enum gpio_mode_t

Values:

``GPIO_MODE_INPUT = GPIO_MODE_DEF_INPUT``

Chế độ GPIO: chỉ input.

``GPIO_MODE_OUTPUT = GPIO_MODE_DEF_OUTPUT``

chế độ GPIO: chỉ output.

``GPIO_MODE_OUTPUT_OD = ((GPIO_MODE_DEF_OUTPUT)|(GPIO_MODE_DEF_OD))``

Chế độ GPIO: chỉ output với chế độ open-drain.

``GPIO_MODE_INPUT_OUTPUT_OD = ((GPIO_MODE_DEF_INPUT)|(GPIO_MODE_DEF_OUTPUT)|(GPIO_MODE_DEF_OD))``

Chế độ GPIO: output và input với chế độ open-drain.

``GPIO_MODE_INPUT_OUTPUT = ((GPIO_MODE_DEF_INPUT)|(GPIO_MODE_DEF_OUTPUT))``

Chế độ GPIO: input và output.

.. c:function:: enum gpio_pullup_t

Values:

``GPIO_PULLUP_DISABLE = 0x0``

Tắt điện trở kéo lên cho GPIO.

``GPIO_PULLUP_ENABLE = 0x1``

Bật điện trở kéo lên cho GPIO.

.. c:function:: enum gpio_pulldown_t

Values:

``GPIO_PULLDOWN_DISABLE = 0x0``

Tắt điện trở kéo xuống cho GPIO.

``GPIO_PULLDOWN_ENABLE = 0x1``

Bật điện trở kéo xuống cho GPIO.

.. c:function:: enum gpio_pull_mode_t

Values:

``GPIO_PULLUP_ONLY``

Pad pull-up.

``GPIO_PULLDOWN_ONLY``

Pad pull-down.

``GPIO_PULLUP_PULLDOWN``

Pad pull-up và pull-down.

``GPIO_FLOATING``

Pad floating


API Reference - RTC GPIO
------------------------

Header File
^^^^^^^^^^^

- `https://github.com/espressif/esp-idf/blob/9314bf0/components/driver/include/driver/rtc_io.h`

Functions
^^^^^^^^^

.. c:function:: static bool rtc_gpio_is_valid_gpio(gpio_num_t gpio_num)

Xác định xem GPIO đã chỉ định có phải là RTC GPIO không.

Return

True nếu GPIO hợp lệ cho việc dùng RTC GPIO. Ngược lại là False.

Parameters

- ``gpio_num``: số của GPIO.

.. c:function:: esp_err_t rtc_gpio_init(gpio_num_t gpio_num)

Khởi động một GPIO thành RTC GPIO.

Hàm này phải được gọi khi khởi tạo một pad cho một chức năng tương tự.

Return

- ESP_OK thành công.

- ESP_ERR_INVALID_ARG GPIO không phải là một RTC IO.

Parameters

- ``gpio_num``: số của GPIO (ví dụ: GPIO_NUM_12).

.. c:function:: esp_err_t rtc_gpio_deinit(gpio_num_t gpio_num)

Khởi tạo một GPIO như một digital GPIO.

Return

- ESP_OK thành công.

- ESP_ERR_INVALID_ARG GPIO không phải là một RTC IO.

Parameters

- ``gpio_num``: số của GPIO (ví dụ: GPIO_NUM_12).

.. c:function:: uint32_t rtc_gpio_get_level(gpio_num_t gpio_num)

Nhận mức đầu vào RTC IO.

Return

- 1 mức cao.

- 0 mức thấp.

- ESP_ERR_INVALID_ARG GPIO không phải là một RTC IO.

Parameters

- ``gpio_num``: số của GPIO (ví dụ: GPIO_NUM_12).

.. c:function:: esp_err_t rtc_gpio_set_level(gpio_num_t gpio_num, uint32_t level)

Set mức RTC IO output.

Return

- ESP_OK thành công.

- ESP_ERR_INVALID_ARG GPIO không phải là một RTC IO.

Parameters

- ``gpio_num``: số của GPIO (ví dụ: GPIO_NUM_12).

- ``level``: mức output.

.. c:function:: esp_err_t rtc_gpio_set_direction(gpio_num_t gpio_num, rtc_gpio_mode_t mode)

Set hướng cho GPIO.

Cấu hình phương hướng cho GPIO, chẳng hạn như chỉ input, chỉ output, input và output.

Return

- ESP_OK thành công.

- ESP_ERR_INVALID_ARG GPIO không phải là RTC IO.

Parameters

- ``gpio_num``: số của GPIO (ví dụ: GPIO_NUM_12).

- ``mode``: hướng làm việc của GPIO.

.. c:function:: esp_err_t rtc_gpio_pullup_en(gpio_num_t gpio_num)

Bật RTC GPIO pull-up.

Hàm này chỉ làm việc cho các RTC IO. Nói chung, gọi ``gpio_pulldown_en``, sẽ làm việc cho cả hai các GPIO thông thường và  RTC IO.

Return

- ESP_OK thành công.

- ESP_ERR_INVALID_ARG GPIO không phải là một RTC IO.

Parameters

-``gpio_num``: số của GPIO (ví dụ: GPIO_NUM_12).

.. c:function:: esp_err_t rtc_gpio_pullup_dis(gpio_num_t gpio_num)

Tắt RTC GPIO pull-up.

Hàm này chỉ làm việc cho các RTC IO. Nói chung, gọi ``gpio_pullup_dis``, sẽ làm việc cho cả hai các GPIO thông thường và RTC IO.

Return

- ESP_OK thành công.

- ESP_ERR_INVALID_ARG GPIO không phải là một RTC IO.

Parameters

- ``gpio_num``: số của GPIO (ví dụ: GPIO_NUM_12).

.. c:function:: esp_err_t rtc_gpio_pulldown_disIO.(gpio_num_t gpio_num)

Tắt RTC GPIO pull-down.

Hàm này chỉ làm việc cho các RTC IO. Nói chung, gọi ``gpio_pulldown_dis``, sẽ làm việc cho cả hai các GPIO thông thường và RTC IO.

Return

- ESP_OK thành công.

- ESP_ERR_INVALID_ARG GPIO không phải là một RTC IO.

Parameters

- ``gpio_num``: số của GPIO (ví dụ: GPIO_NUM_12).

.. c:function:: esp_err_t rtc_gpio_hold_en(gpio_num_t gpio_num)

Kích hoạt chức năng giữ cho một pad RTC GPIO.

Kích hoạt chức năng giữ sẽ làm cho pad chốt lại những giá trị đang hiện hành của việc kích hoạt input, kích hoạt output, giá trị output, chức năng, các giá trị nhanh chậm của drive. Hàm này rất hữu ích khi đem vào chế độ light hay deep sleep để ngăn chặn thay đổi cấu hình chân.

Return

- ESP_OK thành công.

- ESP_ERR_INVALID_ARG GPIO không phải là một RTC IO.

Parameters

- ``gpio_num``: số của GPIO (ví dụ: GPIO_NUM_12).

.. c:function:: esp_err_t rtc_gpio_hold_dis(gpio_num_t gpio_num)

Tắt chức năng force hold cho pad RTC IO.

Tắt chức năng giữ sẽ cho phép pad nhận các giá trị của cho phép input, cho phép output, function, drive strength từ RTC_IO ngoại vi.

Return

- ESP_OK thành công.

- ESP_ERR_INVALID_ARG GPIO không phải là một RTC IO.

Parameters

- ``gpio_num``: số của GPIO.(ví dụ: GPIO_NUM_12).

.. c:function:: void rtc_gpio_force_hold_dis_all()

Tắt chức năng giữ tín hiệu cho tất cả các RTC IO.

Mỗi pad RTC có tín hiệu input từ bộ điều khiển RTC. Nếu tín hiệu này được set, pad sẽ chốt các giá trị hiện tại của cho phép input, function, cho phép output, và các tín hiệu khác đến từ MUX RTC. Tín hiệu force hold được kích hoạt trước khi deep sleep cho các pin được sử dụng cho việc đánh thức EXT1.

Structures
^^^^^^^^^^

.. c:function:: struct rtc_gpio_desc_t

Thông tin pin chức năng cho các chức năng RTC của mỗi GPIO riêng.

Đây là một chức năng bên trong của trình điều khiển, và thường không hữu ích cho việc dùng bên ngoài.

Public Members
^^^^^^^^^^^^^^

``uint32_t reg``

Bit mask để chọn pad digital hay pad RTC.

``uint32_t func``

Shift của vùng chức năng (FUN_SEL).

``uint32_t ie``

Mask của input cho phép.

``uint32_t pullup``

Mask của pull-up cho phép.

``uint32_t pulldown``

Mask của pull-down cho phép.

``uint32_t slpsel``

Nếu bit slpsel được set, slpie sẽ dùng như pad tín hiệu cho phép input trong chế độ ngủ.

``uint32_t slpie``

Mask của input cho phép trong chế độ ngủ.

``uint32_t hold``

Mask của cho phép hold.

``uint32_t hold_force``

Mask của bit hold_force cho RTC IO trong RTC_CNTL_HOLD_FORCE_REG.

``int rtc_num``

Số RTC IO, hay -1 nếu không phải một RTC GPIO.

Macros
^^^^^^

.. c:function:: RTC_GPIO_IS_VALID_GPIO(gpio_num) rtc_gpio_is_valid_gpio(gpio_num)

Enumerations
^^^^^^^^^^^^

.. c:function:: enum rtc_gpio_mode_t

Values:

``RTC_GPIO_MODE_INPUT_ONLY``

Pad output.

``RTC_GPIO_MODE_OUTPUT_ONLY``

Pad input.

``RTC_GPIO_MODE_INPUT_OUTUT``

Pad pull output + input.

``RTC_GPIO_MODE_DISABLED``

Tắt Pad (output + input).

Lưu ý
=====
* Hướng dẫn cài đặt `ESP-IDF <https://esp-idf.readthedocs.io/en/latest/index.html>`_
* Nạp và Debug chương trình `xem tại đây <https://esp-idf.readthedocs.io/en/latest/index.html>`_
* Tài nguyên hệ thống xem `tại đây <https://github.com/espressif/esp-idf>`_






 

















