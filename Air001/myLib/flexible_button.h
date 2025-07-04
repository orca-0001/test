#ifndef __FLEXIBLE_BUTTON_H__
#define __FLEXIBLE_BUTTON_H__

#include "stdint.h"
#include "wiring.h"


typedef void (*flex_button_response_callback)(void*);

typedef uint32_t btn_type_t;


#ifndef USER_BUTTON_MAX
#define USER_BUTTON_MAX   32
#endif

#define FLEX_BTN_SCAN_FREQ_HZ 50 // 
#define FLEX_MS_TO_SCAN_CNT(ms) (ms / (1000 / FLEX_BTN_SCAN_FREQ_HZ))//事件基于扫描次数

#define MAX_MULTIPLE_CLICKS_INTERVAL (FLEX_MS_TO_SCAN_CNT(300))      //连击间隙300ms

//更换状态并调用事件回调函数
#define EVENT_SET_AND_EXEC_CB(btn, evt)                                        \
    do                                                                         \
    {                                                                          \
        btn->event = evt;                                                      \
        if(btn->cb)                                                            \
            btn->cb((flex_button_t*)btn);                                      \
    } while(0)

//宏函数 —— 获取按键状态控制
#define BTN_IS_PRESSED(i) (g_btn_status_reg & (1 << i))


enum FLEX_BTN_STAGE
{
    FLEX_BTN_STAGE_DEFAULT = 0,
    FLEX_BTN_STAGE_DOWN    = 1,
    FLEX_BTN_STAGE_MULTIPLE_CLICK = 2
};

typedef enum
{
    FLEX_BTN_PRESS_DOWN = 0,        // 按下事件
    FLEX_BTN_PRESS_CLICK,           // 单击事件
    FLEX_BTN_PRESS_DOUBLE_CLICK,    // 双击事件
    FLEX_BTN_PRESS_REPEAT_CLICK,    // 连击事件，使用 flex_button_t 中的 click_cnt 断定连击次数
    FLEX_BTN_PRESS_SHORT_START,     // 短按开始事件
    FLEX_BTN_PRESS_SHORT_UP,        // 短按抬起事件
    FLEX_BTN_PRESS_LONG_START,      // 长按开始事件
    FLEX_BTN_PRESS_LONG_UP,         // 长按抬起事件
    FLEX_BTN_PRESS_LONG_HOLD,       // 长按保持事件
    FLEX_BTN_PRESS_LONG_HOLD_UP,    // 长按保持的抬起事件
    FLEX_BTN_PRESS_MAX,
    FLEX_BTN_PRESS_NONE,
} flex_button_event_t;


typedef struct flex_button
{
    struct flex_button* next;                  //单向链表

    uint8_t  (*usr_button_read)(void *);      //用户设备的按键引脚电平读取函数
    flex_button_response_callback  cb;        //设置按键事件回调

    uint16_t scan_cnt;                        //用于记录扫描次数，按键按下是开始从零计数
    uint16_t click_cnt;                       //记录单击次数，用于判定单击、连击
    uint16_t max_multiple_clicks_interval;    //连击间隙，用于判定是否结束连击计数

    uint16_t debounce_tick;                   //消抖时间
    uint16_t short_press_start_tick;          //设置短按事件触发的起始 tick
    uint16_t long_press_start_tick;           //设置长按事件触发的起始 tick
    uint16_t long_hold_start_tick;            //设置长按保持事件触发的起始 tick

    uint8_t id;                               //当多个按键使用同一个回调函数时，用于断定属于哪个按
    uint8_t pressed_logic_level : 1;          //设置按键按下的逻辑电平。1：标识按键按下的时候为高电平
    uint8_t event               : 4;          //用于记录当前按键事件
    uint8_t status              : 3;          //用于记录当前按键的状态，用于内部状态机
} flex_button_t;


//变量列表
static uint8_t button_cnt = 0;                  //按键总数
static flex_button_t *btn_head = NULL;          
btn_type_t g_logic_level = (btn_type_t)0;       
btn_type_t g_btn_status_reg = (btn_type_t)0;



//用户自定义按键数组
static flex_button_t user_button[USER_BUTTON_MAX];
PIN_Name PIN_PAME[USER_BUTTON_MAX] ;

//函数声明
static void user_button_init(uint8_t Press_state,uint16_t Short_press_time,uint16_t Long_press_time,uint16_t Long_keep_time);
int32_t flex_button_register(flex_button_t *button);

static void flex_button_read(void);
static uint8_t flex_button_process(void);
uint8_t flex_button_scan(void);
flex_button_event_t flex_button_event_read(flex_button_t* button);
static uint8_t common_btn_read(void *arg);
extern void common_btn_evt_cb(void *arg);


//按键GPIO初始化
void button_gpio_Init(uint8_t button_id, PIN_Name pin, GPIOMode_TypeDef mode)
{
    pinMode(pin, mode);
    PIN_PAME[button_id] = pin;
}


//按键初始化函数  Press_state (1:按下高电平)
static void user_button_init(uint8_t Press_state,uint16_t Short_press_time,uint16_t Long_press_time,uint16_t Long_keep_time)
{
    int i;
    
    memset(&user_button[0], 0x0, sizeof(user_button));

    for (i = 0; i < USER_BUTTON_MAX; i ++)
    {
        user_button[i].id = i;
        user_button[i].usr_button_read = common_btn_read;
        user_button[i].cb = common_btn_evt_cb;
        user_button[i].pressed_logic_level = Press_state;
        user_button[i].short_press_start_tick = FLEX_MS_TO_SCAN_CNT(Short_press_time);
        user_button[i].long_press_start_tick = FLEX_MS_TO_SCAN_CNT(Long_press_time);
        user_button[i].long_hold_start_tick = FLEX_MS_TO_SCAN_CNT(Long_keep_time);

        flex_button_register(&user_button[i]);//注册按键
    }
}

//使用该接口注册一个用户按键，入参为一个 flex_button_t 结构体实例的地址。
int32_t flex_button_register(flex_button_t *button)
{
    flex_button_t *curr = btn_head;
    
    if (!button || (button_cnt > sizeof(btn_type_t) * 8))
    {
        return -1;
    }

    while (curr)
    {
        if(curr == button)
        {
            return -1;  /* 已存在 */
        }
        curr = curr->next;
    }

    button->next = btn_head;
    button->status = FLEX_BTN_STAGE_DEFAULT;
    button->event = FLEX_BTN_PRESS_NONE;
    button->scan_cnt = 0;
    button->click_cnt = 0;
    button->max_multiple_clicks_interval = MAX_MULTIPLE_CLICKS_INTERVAL;
    btn_head = button;

    g_logic_level |= (button->pressed_logic_level << button_cnt);
    button_cnt ++;

    return button_cnt;
}



//按键读取
static uint8_t common_btn_read(void *arg)
{
    uint8_t value = 0;

    flex_button_t *btn = (flex_button_t *)arg;

    value = digitalRead(PIN_PAME[btn->id]);

    // switch (btn->id)
    // {
    // case 0/* constantf-expression */:
    //     /* code */value = digitalRead(PIN_PAME[0]);
    //     break;
    // case 1/* constantf-expression */:
    //     /* code */value = digitalRead(PIN_PAME[1]);
    //     break;
    // case 2/* constantf-expression */:
    // /* code */
    // break;  
    // default:
    //     break;
    // }

    return value;
}

//读取所有按键值在一个周期内
static void flex_button_read(void)
{
    uint8_t i;
    flex_button_t* target;

    
    btn_type_t raw_data = 0;

    for(target = btn_head, i = button_cnt - 1;
        (target != NULL) && (target->usr_button_read != NULL);
        target = target->next, i--)
    {
        raw_data = raw_data | ((target->usr_button_read)(target) << i);
    }
    g_btn_status_reg = (~raw_data) ^ g_logic_level;
}

//所有按键状态更新（以状态机）
static uint8_t flex_button_process(void)
{
    uint8_t i;
    uint8_t active_btn_cnt = 0;
    flex_button_t* target;
    
    for (target = btn_head, i = button_cnt - 1; target != NULL; target = target->next, i--)
    {
        if (target->status > FLEX_BTN_STAGE_DEFAULT)
        {
            target->scan_cnt ++;
            if (target->scan_cnt >= ((1 << (sizeof(target->scan_cnt) * 8)) - 1))
            {
                target->scan_cnt = target->long_hold_start_tick;
            }
        }

        switch (target->status)
        {
        case FLEX_BTN_STAGE_DEFAULT: 
            if (BTN_IS_PRESSED(i)) 
            {
                target->scan_cnt = 0;
                target->click_cnt = 0;

                EVENT_SET_AND_EXEC_CB(target, FLEX_BTN_PRESS_DOWN);

                target->status = FLEX_BTN_STAGE_DOWN;
            }
            else
            {
                target->event = FLEX_BTN_PRESS_NONE;
            }
            break;

        case FLEX_BTN_STAGE_DOWN: 
            if (BTN_IS_PRESSED(i)) 
            {
                if (target->click_cnt > 0) 
                {
                    if (target->scan_cnt > target->max_multiple_clicks_interval)
                    {
                        EVENT_SET_AND_EXEC_CB(target, 
                            target->click_cnt < FLEX_BTN_PRESS_REPEAT_CLICK ? 
                                target->click_cnt :
                                FLEX_BTN_PRESS_REPEAT_CLICK);

                        target->status = FLEX_BTN_STAGE_DOWN;
                        target->scan_cnt = 0;
                        target->click_cnt = 0;
                    }
                }
                else if (target->scan_cnt >= target->long_hold_start_tick)
                {
                    if (target->event != FLEX_BTN_PRESS_LONG_HOLD)
                    {
                        EVENT_SET_AND_EXEC_CB(target, FLEX_BTN_PRESS_LONG_HOLD);
                    }
                }
                else if (target->scan_cnt >= target->long_press_start_tick)
                {
                    if (target->event != FLEX_BTN_PRESS_LONG_START)
                    {
                        EVENT_SET_AND_EXEC_CB(target, FLEX_BTN_PRESS_LONG_START);
                    }
                }
                else if (target->scan_cnt >= target->short_press_start_tick)
                {
                    if (target->event != FLEX_BTN_PRESS_SHORT_START)
                    {
                        EVENT_SET_AND_EXEC_CB(target, FLEX_BTN_PRESS_SHORT_START);
                    }
                }
            }
            else 
            {
                if (target->scan_cnt >= target->long_hold_start_tick)
                {
                    EVENT_SET_AND_EXEC_CB(target, FLEX_BTN_PRESS_LONG_HOLD_UP);
                    target->status = FLEX_BTN_STAGE_DEFAULT;
                }
                else if (target->scan_cnt >= target->long_press_start_tick)
                {
                    EVENT_SET_AND_EXEC_CB(target, FLEX_BTN_PRESS_LONG_UP);
                    target->status = FLEX_BTN_STAGE_DEFAULT;
                }
                else if (target->scan_cnt >= target->short_press_start_tick)
                {
                    EVENT_SET_AND_EXEC_CB(target, FLEX_BTN_PRESS_SHORT_UP);
                    target->status = FLEX_BTN_STAGE_DEFAULT;
                }
                else
                {
                    target->status = FLEX_BTN_STAGE_MULTIPLE_CLICK;
                    target->click_cnt ++;
                }
            }
            break;

        case FLEX_BTN_STAGE_MULTIPLE_CLICK: 
            if (BTN_IS_PRESSED(i)) 
            {
                target->status = FLEX_BTN_STAGE_DOWN;
                target->scan_cnt = 0;
            }
            else
            {
                if (target->scan_cnt > target->max_multiple_clicks_interval)
                {
                    EVENT_SET_AND_EXEC_CB(target, 
                        target->click_cnt < FLEX_BTN_PRESS_REPEAT_CLICK ? 
                            target->click_cnt :
                            FLEX_BTN_PRESS_REPEAT_CLICK);

                    target->status = FLEX_BTN_STAGE_DEFAULT;
                }
            }
            break;
        }
        
        if (target->status > FLEX_BTN_STAGE_DEFAULT)
        {
            active_btn_cnt ++;
        }
    }
    
    return active_btn_cnt;
}




//按键扫描函数
uint8_t flex_button_scan(void)
{
    flex_button_read();
    return flex_button_process();
}

//获取按键事件
flex_button_event_t flex_button_event_read(flex_button_t* button)
{
    return (flex_button_event_t)(button->event);
}


#endif /* __FLEXIBLE_BUTTON_H__ */
