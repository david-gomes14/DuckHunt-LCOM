#include <lcom/lcf.h>
#include <stdio.h>
#include <stdbool.h>

#include "game.h"

#include "../timer/timer.h"
#include "../keyboard/kbc.h"
#include "../mouse/mouse.h"


static uint8_t timer_irq;
static uint8_t kbc_irq;
static uint8_t mouse_irq;


void game_init() {

    timer_subscribe_int(&timer_irq);
    kbc_subscribe_int(&kbc_irq);
    mouse_subscribe_int(&mouse_irq);

    mouse_enable_data();

    printf("init\n");
}

void game_loop() {
    
    int ipc_status;
    message msg;
    uint32_t r;

    uint32_t irq_set_timer = BIT(timer_irq);
    uint32_t irq_set_kbd = BIT(kbc_irq);
    uint32_t irq_set_mouse = BIT(mouse_irq);

    uint8_t mouse_packet[3];
    uint8_t mouse_index = 0;
    struct packet pp;

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

                    if (msg.m_notify.interrupts & irq_set_mouse) {
                        mouse_ih();
                        if (!mouse_error) {
                            if (mouse_sync_packet(mouse_packet, &mouse_index)) {
                                mouse_parse_packet(mouse_packet, &pp);
                                printf("dx=%d dy=%d lb=%d rb=%d\n", pp.delta_x, pp.delta_y, pp.lb, pp.rb);
                            }
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
    mouse_unsubscribe_int();

    mouse_disable_data();

    printf("cleanup\n");
}

