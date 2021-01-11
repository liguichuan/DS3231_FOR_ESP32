/*
 * @Author: your name
 * @Date: 2021-01-11 11:50:58
 * @LastEditTime: 2021-01-11 18:34:26
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: \ds3231\main\ds3231_main.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "sdkconfig.h"
#include "ds3231.h" //引用DS3231库文件，库文件包括ds3231.h、ds3231.c两个文件，和main.c放在同一个目录下即可

#define BLINK_GPIO 2 //开发板上的LED灯，接第2引脚
#define SDA 18       //DS3231时钟模块的SDA引脚接开发板的18引脚
#define SCL 19       //DS3231时钟模块的SCL引脚接开发板的119引脚
//说明：DS3231模块只需接4根线：GND(地或负极)、VCC(正极3.3V)、SDA、SCL

//开发板开机后将从app_main()函数处开始运行
void app_main(void)
{

    gpio_pad_select_gpio(BLINK_GPIO); //选中LED灯引脚
    gpio_pad_select_gpio(SDA);        //选中SDA引脚
    gpio_pad_select_gpio(SCL);

    gpio_set_direction(BLINK_GPIO, GPIO_MODE_OUTPUT); //设置引脚为输出模式
    gpio_set_direction(SDA, GPIO_MODE_INPUT_OUTPUT);  //设置引脚为输入输出模式
    gpio_set_direction(SCL, GPIO_MODE_INPUT_OUTPUT);  //设置引脚为输入输出模式
    DS3231_Info ds3231;                               //创建一个设备。DS3231_Info详见ds3231.h中的定义
    struct tm set_time;                               //定义标准C/C++中的时间结构体,用于设置时间

    //struct tm 和 time_t 时间和日期的使用方法详见这里:https://blog.csdn.net/gerryzhu/article/details/50371255

    //下面对结构体中的各项参数进行配置
    set_time.tm_year = 2021 - 1900; /* 年份，其值等于实际年份减去1900 */
    set_time.tm_mon = 12 - 1;       /* 月份（从一月开始，0代表一月） - 取值区间为[0,11] */
    set_time.tm_mday = 31;          /* 一个月中的日期 - 取值区间为[1,31] */
    set_time.tm_hour = 23;          /* 时 - 取值区间为[0,23] */
    set_time.tm_min = 59;           /* 分 - 取值区间为[0,59] */
    set_time.tm_sec = 0;            /* 秒 – 取值区间为[0,59] */
    set_time.tm_wday = 5 - 1;       /* 星期 – 取值区间为[0,6]，其中0代表星期天，1代表星期一，以此类推 */
    set_time.tm_yday = 365;         /* 从每年的1月1日开始的天数 – 取值区间为[0,365]，其中0代表1月1日，1代表1月2日，以此类推 */
    set_time.tm_isdst = -1;         /* 夏令时标识符，实行夏令时的时候，tm_isdst为正。不实行夏令时的进候，tm_isdst为0；不了解情况时，tm_isdst()为负。*/

    ds3231_init_info(&ds3231, I2C_NUM_1, SDA, SCL, 1000); //初始化ds3231实例
    ds3231_set_time(&ds3231, &set_time);                  //用已经定义并配置过的struct tm set_time来设置时间

    struct tm get_time;              //定义标准C/C++中的时间结构体,用于获取时间
    char format_time_str[50] = "\0"; //创建一个接收时间的变量
    while (1)
    {
        ds3231_get_time(&ds3231, &get_time);
        vTaskDelay(50 / portTICK_PERIOD_MS);
        //printf(text, "TIME:%d-%d-%d %d:%d:%d  WeekDay:%d", get_time.tm_year + 1900, get_time.tm_mon + 1, get_time.tm_mday, get_time.tm_hour, get_time.tm_min, get_time.tm_sec, get_time.tm_wday + 1);

        //C 库函数 - strftime()
        //描述:size_t strftime(char *str, size_t maxsize, const char *format, const struct tm *timeptr) 根据 format 中定义的格式化规则，格式化结构 timeptr 表示的时间，并把它存储在 str 中。
        //具体使用详见这里：https://weibo01.blog.csdn.net/article/details/82913032?utm_medium=distribute.pc_relevant_t0.none-task-blog-BlogCommendFromBaidu-1.control&depth_1-utm_source=distribute.pc_relevant_t0.none-task-blog-BlogCommendFromBaidu-1.control
        //声明原形：size_t strftime(char *str, size_t maxsize, const char *format, const struct tm *timeptr)
        //参数说明：
        //      str -- 这是指向目标数组的指针，用来复制产生的 C 字符串。
        //      maxsize-- 这是被复制到 str 的最大字符数。
        //      format-- 这是 C 字符串，包含了普通字符和特殊格式说明符的任何组合。这些格式说明符由函数替换为表示 tm 中所指定时间的相对应值。格式说明符是

        strftime(format_time_str, sizeof(format_time_str), "%Y-%m-%d %H:%M:%S", &get_time); //XXX.c_str() 兼容c字符串，c中没有string类型

        printf("当前时间为 : |****%s****|\n", format_time_str);

        //下面语句仅为让开发板上的LED灯进行闪烁，没其它作用，可删去
        /* Blink off (output low) */
        //printf("Turning off the LED\n");
        gpio_set_level(BLINK_GPIO, 0);        //熄灭
        vTaskDelay(490 / portTICK_PERIOD_MS); //延时490MS

        /* Blink on (output high) */
        //printf("Turning on the LED\n");
        gpio_set_level(BLINK_GPIO, 1);        //亮起
        vTaskDelay(490 / portTICK_PERIOD_MS); //延时490MS
    }
}
