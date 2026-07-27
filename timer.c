#include <stdlib.h>
#include "timer.h"
#include "mul.h"

void trigger_timer_interrupt(int us_delay)
{
    uint32_t interrupt_time = get_mtime() + us_delay;
    set_mtimecmp(interrupt_time);
    enable_timer_interrupt();
}

typedef struct alarm_data {
    alarm_callback_t callback;
    uint32_t time;
    void* handle;
    struct alarm_data* next;
} alarm_data_t;

#define MAX_ALARMS 8
static alarm_data_t alarms[MAX_ALARMS];
static alarm_data_t* first_alarm;
static int used_alarms;

bool set_alarm(int ms_delay, alarm_callback_t callback, void* handle)
{
    uint32_t alarm_time = get_mtime() + mul32x16(ms_delay, 1000);
    disable_timer_interrupt();

    if (used_alarms == (1 << MAX_ALARMS) - 1) {
        enable_timer_interrupt();
        return false;
    }

    int free_alarm_idx = 0;
    while (used_alarms & (1 << free_alarm_idx)) ++free_alarm_idx;
    used_alarms |= (1 << free_alarm_idx);

    alarm_data_t* new_alarm = &alarms[free_alarm_idx];
    new_alarm->callback = callback;
    new_alarm->handle = handle;
    new_alarm->time = alarm_time;
    new_alarm->next = NULL;

    alarm_data_t* prev_alarm = NULL;
    if (first_alarm) {
        alarm_data_t* alarm = first_alarm;
        while (alarm && (int)(alarm->time - alarm_time) < 0) {
            prev_alarm = alarm;
            alarm = alarm->next;
        }
        
        if (alarm) new_alarm->next = alarm;
    }

    if (prev_alarm) {
        prev_alarm->next = new_alarm;
    } else {
        first_alarm = new_alarm;
        set_mtimecmp(alarm_time);
    }
    
    enable_timer_interrupt();
    return true;
}

void __attribute__((weak)) tqv_timer_interrupt(void)
{
    disable_timer_interrupt();
    if (first_alarm) {
        alarm_callback_t the_alarm = first_alarm->callback;
        void* handle = first_alarm->handle;

        used_alarms &= ~(1 << (first_alarm - alarms));

        first_alarm = first_alarm->next;
        if (first_alarm) {
            set_mtimecmp(first_alarm->time);
            enable_timer_interrupt();
        }
        
        the_alarm(handle);
    }
}
