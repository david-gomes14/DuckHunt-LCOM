#include <lcom/lcf.h>

#include "state.h"

#include "../../graphic/graphic.h"
#include "../../serial_port/uart.h"
#include "../menu/menu.h"
#include "../target/target.h"
#include "../hud/hud.h"
#include "../score/score.h"
#include "../leaderboard/leaderboard.h"

#include "../../serial_port/serial_port.h"
#include "../../images/background.xpm"
#include "../../images/game_over.xpm"
#include "../../images/game_over_win.xpm"
#include "../../images/game_over_lose.xpm"
#include "../../images/game_over_draw.xpm"
#include "../../images/leaderboard.xpm"
#include "../../images/menu_waiting.xpm"

#define CROSSHAIR_COLOR 0xFF0000
#define GAME_DURATION_TICKS (60 * 60)
#define GAMEOVER_SCORE_X 618
#define GAMEOVER_SCORE_Y 473

#define SCREEN_W 1152
#define SCREEN_H 864

#define LB_FIRST_CENTER_Y 315   // centro da placa da linha 0 (medido no xpm)
#define LB_ROW_STEP       108   // espaçamento centro-a-centro entre placas
#define LB_DIGIT_H         50   // altura de um dígito normal (para centrar na vertical)

#define LB_RANK_X         248   // marcador de posição, à esquerda da placa
#define LB_SCORE_X        305   // score, encostado à esquerda da placa
#define LB_DATETIME_X     445   // datetime, preenche o resto da placa

game_state_t state = STATE_MENU;
game_mode_t mode = MODE_SINGLEPLAYER;

extern int mouse_x;
extern int mouse_y;

static int ticks_remaining = 0;
static bool peer_ready = false;   // o adversário já clicou multiplayer e está à espera
static int opp_score = -1;        // score final do adversário no fim do tempo (-1 = ainda não recebido)

void state_render() {
    vg_restore_background();
	if (state == STATE_PLAYING) {
        hud_draw(score_get(), state_get_time());
		target_update();
		target_draw();
		draw_crosshair(mouse_x, mouse_y, 12, CROSSHAIR_COLOR);
	}
	vg_flip();
}

void state_enter_menu() {
    ticks_remaining = 0;
    menu_init();
    menu_draw();
    state = STATE_MENU;
}

void state_enter_multiplayer() {
    if (peer_ready) {
        // o adversário já estava à espera → sou o GUEST; arrancamos os dois
        peer_ready = false;
        mode = MODE_MULTIPLAYER_GUEST;
        serial_port_write_byte(GAME_START);   // diz ao host para arrancar
        state_enter_playing();
    } else {
        // sou o primeiro a clicar → HOST; fico à espera do 2º jogador
        mode = MODE_MULTIPLAYER_HOST;
        serial_port_write_byte(GAME_START);    // anuncia que estou pronto
        state_enter_mp_waiting();
    }
}

void state_enter_mp_waiting() {
    draw_pixmap((xpm_map_t) menu_waiting, 0, 0);
    vg_store_background();
    state = STATE_MP_WAITING;
}

void state_enter_playing() {
    draw_pixmap((xpm_map_t) background, 0, 0);
    vg_store_background();
    hud_init();
    hud_draw(score_get(), state_get_time());
    mouse_x = SCREEN_W / 2;
    mouse_y = SCREEN_H / 2;
    target_init();
    score_reset();
    ticks_remaining = GAME_DURATION_TICKS;
    opp_score = -1;

    state = STATE_PLAYING;
}

void state_enter_game_over(game_result_t result) {
    // o leaderboard é só do singleplayer — em multiplayer mata-se sempre 10 patos
    if (mode == MODE_SINGLEPLAYER)
        leaderboard_add(score_get());

    switch (result) {
        case RESULT_WIN:
            draw_pixmap((xpm_map_t) game_over_win, 0, 0);
            break;
        case RESULT_LOSE:
            draw_pixmap((xpm_map_t) game_over_lose, 0, 0);
            break;
        case RESULT_DRAW:
            draw_pixmap((xpm_map_t) game_over_draw, 0, 0);
            break;
        default:  // RESULT_NONE — singleplayer, mostra o score grande
            draw_pixmap((xpm_map_t) game_over, 0, 0);
            hud_draw_score_large_at(score_get(), GAMEOVER_SCORE_X, GAMEOVER_SCORE_Y);
            break;
    }

    vg_store_background();
    state = STATE_GAME_OVER;
}

void state_enter_leaderboard() {
    hud_init();
    draw_pixmap((xpm_map_t) leaderboard, 0, 0);

    leaderboard_entry_t *entries = leaderboard_get();
    int n = leaderboard_size();
    for (int i = 0; i < n; i++) {
        int cy = LB_FIRST_CENTER_Y + i * LB_ROW_STEP;
        int y  = cy - LB_DIGIT_H / 2;   // centra os dígitos de 50px na placa
        hud_draw_number_at(i + 1, LB_RANK_X, y);
        hud_draw_score_at(entries[i].score, LB_SCORE_X, y);
        hud_draw_datetime_at(entries[i].date.hour, entries[i].date.minute,
                             entries[i].date.day,  entries[i].date.month,
                             LB_DATETIME_X, y);
    }

    vg_store_background();
    state = STATE_LEADERBOARD;
}

// fim do tempo em multiplayer: compara o meu score com o do adversário e decide
static void state_decide_timeout(int me, int opp) {
    opp_score = -1;
    if (me > opp)       state_enter_game_over(RESULT_WIN);
    else if (me < opp)  state_enter_game_over(RESULT_LOSE);
    else                state_enter_game_over(RESULT_DRAW);  // empate
}

void state_tick() {
    if (state != STATE_PLAYING) return;

    if (mode == MODE_MULTIPLAYER_HOST || mode == MODE_MULTIPLAYER_GUEST) {
        // MULTIPLAYER: ganha quem fizer 10 patos primeiro; senão, quem tiver mais ao fim do tempo
        if (score_get() >= 10) {
            serial_port_write_byte(GAME_OVER);  // chegámos a 10 primeiro → vencemos
            state_enter_game_over(RESULT_WIN);
            return;
        }
        ticks_remaining--;
        if (ticks_remaining <= 0) {
            // o meu tempo acabou: envia o meu score ao adversário
            serial_port_write_byte(MP_SCORE_BASE + score_get());
            if (opp_score >= 0) {
                state_decide_timeout(score_get(), opp_score);  // já tenho o dele → decide
            } else {
                state = STATE_MP_TIMEUP;                        // espera o score dele
            }
        }
    } else {
        // SINGLEPLAYER: joga os 60s todos, sem limite de patos
        ticks_remaining--;
        if (ticks_remaining <= 0) {
            state_enter_game_over(RESULT_NONE);
        }
    }
}

int state_get_time() {
    return ticks_remaining / 60;
}

// --- reações a bytes recebidos pela série (chamadas por handle_uart) ---

void state_peer_ready() {
    if (state == STATE_MP_WAITING) {
        // sou o host à espera e o 2º jogador entrou → arrancamos
        state_enter_playing();
    } else if (state == STATE_MENU) {
        // o adversário clicou primeiro; ainda não cliquei → fica registado
        peer_ready = true;
    }
}

void state_peer_cancel() {
    peer_ready = false;
}

void state_peer_game_over() {
    // o adversário fez 10 patos → perdemos (mesmo se já estávamos à espera do score dele)
    if (state == STATE_PLAYING || state == STATE_MP_TIMEUP)
        state_enter_game_over(RESULT_LOSE);
}

void state_peer_score(int s) {
    opp_score = s;
    if (state == STATE_MP_TIMEUP) {
        // o meu tempo já tinha acabado e estava à espera → decide agora
        state_decide_timeout(score_get(), opp_score);
    }
    // se ainda estou a jogar, fica guardado; decido quando o meu tempo acabar
}

