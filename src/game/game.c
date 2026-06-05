#include <lcom/lcf.h>
#include <stdio.h>
#include <stdbool.h>

#include "game.h"

#include "../rtc/rtc.h"
#include "../timer/timer.h"
#include "../keyboard/kbc.h"
#include "../mouse/mouse.h"
#include "../graphic/graphic.h"
#include "../serial_port/serial_port.h"

#include "menu/menu.h"
#include "input/input.h"
#include "state/state.h"
#include "target/target.h"
#include "leaderboard/leaderboard.h"

extern game_state_t state;

static uint8_t timer_irq;
static uint8_t kbc_irq;
static uint8_t mouse_irq;
static uint8_t serial_port_irq;

void game_init() {
	// map antes de set: a VRAM tem de estar mapeada antes de mudar o modo de vídeo
	map_video_memory(0x14C);
	set_graphics_mode(0x14C);

	timer_subscribe_int(&timer_irq);
	kbc_subscribe_int(&kbc_irq);
	mouse_subscribe_int(&mouse_irq);
	mouse_enable_data();  // envia 0xF4 ao rato para começar a enviar pacotes
	serial_port_connect(115200,&serial_port_irq);

	timer_set_frequency(0, 60);  // 60 Hz — batimento cardíaco do jogo

	menu_init();
	menu_draw();

	leaderboard_init();
}

void game_loop() {
	int ipc_status;
	message msg;
	uint32_t r;

	// bitmasks para identificar qual dispositivo disparou a interrupção
	uint32_t irq_set_timer = BIT(timer_irq);
	uint32_t irq_set_kbd   = BIT(kbc_irq);
	uint32_t irq_set_mouse = BIT(mouse_irq);
	uint32_t irq_set_serial_port = BIT(serial_port_irq);

	while (state != STATE_EXIT) {
		// bloqueia até chegar uma mensagem de interrupção do MINIX
		if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
			printf("driver_receive failed with: %d", r);
			continue;
		}

		if (is_ipc_notify(ipc_status)) {
			switch (_ENDPOINT_P(msg.m_source)) {
				case HARDWARE:
					// o campo interrupts é um bitmask — pode ter vários bits ativos ao mesmo tempo
					if (msg.m_notify.interrupts & irq_set_timer) {
						timer_int_handler();
						state_tick();
						state_render();  // render só aqui, a 60 Hz
					}
					if (msg.m_notify.interrupts & irq_set_kbd)
						handle_keyboard();
					if (msg.m_notify.interrupts & irq_set_mouse)
						handle_mouse();
					if(msg.m_notify.interrupts & irq_set_serial_port)
						handle_uart();
					break;
				default:
					break;
			}
		}
	}
}

void game_cleanup() {
	mouse_disable_data();
	timer_unsubscribe_int();
	kbc_unsubscribe_int();
	mouse_unsubscribe_int();
	serial_port_unsubscribe_int();
	vg_free_buffer();
	vg_exit();
}
