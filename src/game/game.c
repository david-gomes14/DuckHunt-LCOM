#include <lcom/lcf.h>
#include <stdio.h>
#include <stdbool.h>

#include "game.h"

#include "../timer/timer.h"
#include "../keyboard/kbc.h"


static uint8_t timer_irq;
static uint8_t kbc_irq;


void game_init() {

    timer_subscribe_int(&timer_irq);
    kbc_subscribe_int(&kbc_irq);

    printf("init\n");
}

void game_loop() {
    
    int ipc_status;
    message msg;
    uint32_t r;

    uint32_t irq_set_timer = BIT(timer_irq);
    uint32_t irq_set_kbd = BIT(kbc_irq);

    bool running = true;

    while (running) {

        if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
            printf("driver_receive failed with: %d", r);
            continue;
        }
        if (is_ipc_notify(ipc_status)) {
            switch (_ENDPOINT_P(msg.m_source)) {
                case HARDWARE:
                    if (msg.m_notify.interrupts & irq_set_timer) {
                        timer_int_handler();
                        if (timer_counter % 60 == 0) {
                            printf("1 second\n");
                        }    
                    }

                    if (msg.m_notify.interrupts & irq_set_kbd) {
                        kbc_ih();
                        if (scancode == ESC_BREAK_CODE) {
                            running = false;
                        }    
                    }
                    break;
                default:
                    break;
            }
        } else {

        }
    }

    printf("loop\n");
}

void game_cleanup() {

    timer_unsubscribe_int();
    kbc_unsubscribe_int();

    printf("cleanup\n");
}

