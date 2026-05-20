#include <lcom/lcf.h>
#include <stdio.h>
#include <stdbool.h>

#include "game.h"

#include "../timer/timer.h"
#include "../keyboard/kbc.h"
#include "../mouse/mouse.h"
#include "../graphic/graphic.h"
#include "../images/background.xpm"


static uint8_t timer_irq;
static uint8_t kbc_irq;
static uint8_t mouse_irq;


void game_init() {

	map_video_memory(0x115);
	set_graphics_mode(0x115);

	timer_subscribe_int(&timer_irq);
	kbc_subscribe_int(&kbc_irq);
	mouse_subscribe_int(&mouse_irq);

	mouse_enable_data();
	
	timer_set_frequency(0, 60);
	
	draw_pixmap((xpm_map_t) background_xpm, 0, 0);
	vg_store_background();

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
						vg_restore_background();
						draw_crosshair(mouse_x, mouse_y, 10, 0xFF0000);
						vg_flip();
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
								if (!pp.x_ov && !pp.y_ov) {
									mouse_x += pp.delta_x;
									mouse_y -= pp.delta_y;
									if (mouse_x < 0) mouse_x = 0;
									if (mouse_y < 0) mouse_y = 0;
									if (mouse_x >= 800) mouse_x = 799;
									if (mouse_y >= 600) mouse_y = 599;
								}
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

	mouse_disable_data();

	timer_unsubscribe_int();
	kbc_unsubscribe_int();
	mouse_unsubscribe_int();

	vg_free_buffer();

	vg_exit();

	printf("cleanup\n");
}

