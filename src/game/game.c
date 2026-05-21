#include <lcom/lcf.h>
#include <stdio.h>
#include <stdbool.h>

#include "game.h"

#include "../timer/timer.h"
#include "../keyboard/kbc.h"
#include "../mouse/mouse.h"
#include "../graphic/graphic.h"
#include "../images/background.xpm"
#include "../images/menu_start.xpm"
#include "../images/menu_quit.xpm"
#include "target.h"


static uint8_t timer_irq;
static uint8_t kbc_irq;
static uint8_t mouse_irq;

static game_state_t state = STATE_MENU;
static int menu_selection = 0;  /* 0 = Start Game, 1 = Quit */


static void load_menu_frame() {
	if (menu_selection == 0)
		draw_pixmap((xpm_map_t) menu_start, 0, 0);
	else
		draw_pixmap((xpm_map_t) menu_quit, 0, 0);
	vg_store_background();
}

static void enter_playing() {
	draw_pixmap((xpm_map_t) background_xpm, 0, 0);
	vg_store_background();
	mouse_x = 400;
	mouse_y = 300;
	target_init();
	state = STATE_PLAYING;
}


void game_init() {
	map_video_memory(0x115);
	set_graphics_mode(0x115);

	timer_subscribe_int(&timer_irq);
	kbc_subscribe_int(&kbc_irq);
	mouse_subscribe_int(&mouse_irq);
	mouse_enable_data();

	timer_set_frequency(0, 60);

	load_menu_frame();

	printf("init\n");
}

void game_loop() {

	int ipc_status;
	message msg;
	uint32_t r;

	uint32_t irq_set_timer = BIT(timer_irq);
	uint32_t irq_set_kbd   = BIT(kbc_irq);
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
						if (state == STATE_PLAYING) {
							target_update();
							target_draw();
							draw_crosshair(mouse_x, mouse_y, 10, 0xFF00FF);
						}
						vg_flip();
					}

					if (msg.m_notify.interrupts & irq_set_kbd) {
						kbc_ih();
						if (!scancode_error) {
							if (state == STATE_MENU) {
								switch (scancode) {
									case 0x11:  /* W make */
									case 0x48:  /* UP arrow second byte */
										if (menu_selection != 0) {
											menu_selection = 0;
											load_menu_frame();
										}
										break;
									case 0x1F:  /* S make */
									case 0x50:  /* DOWN arrow second byte */
										if (menu_selection != 1) {
											menu_selection = 1;
											load_menu_frame();
										}
										break;
									case 0x1C:  /* ENTER make */
										if (menu_selection == 0) enter_playing();
										else running = false;
										break;
									case ESC_BREAK_CODE:
										running = false;
										break;
								}
							} else {
								if (scancode == ESC_BREAK_CODE) running = false;
							}
						}
					}

					if (msg.m_notify.interrupts & irq_set_mouse) {
						mouse_ih();
						if (!mouse_error && state == STATE_PLAYING) {
							if (mouse_sync_packet(mouse_packet, &mouse_index)) {
								mouse_parse_packet(mouse_packet, &pp);
								if (!pp.x_ov && !pp.y_ov) {
									mouse_x += pp.delta_x;
									mouse_y -= pp.delta_y;
									if (mouse_x < 0)    mouse_x = 0;
									if (mouse_y < 0)    mouse_y = 0;
									if (mouse_x >= 800) mouse_x = 799;
									if (mouse_y >= 600) mouse_y = 599;
								}
								if (pp.lb)
									target_check_hit(mouse_x, mouse_y);
							}
						}
					}

					break;
				default:
					break;
			}
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
