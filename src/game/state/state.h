#ifndef STATE_H
#define STATE_H

typedef enum {
    STATE_MENU = 0,
    STATE_MP_WAITING,   // host à espera que o 2º jogador entre em multiplayer
    STATE_PLAYING,
    STATE_MP_TIMEUP,    // multiplayer: o meu tempo acabou, à espera do score do adversário
    STATE_GAME_OVER,
    STATE_LEADERBOARD,
    STATE_EXIT
} game_state_t;

typedef enum {
    MODE_SINGLEPLAYER,
    MODE_MULTIPLAYER_HOST,
    MODE_MULTIPLAYER_GUEST
} game_mode_t;

typedef enum {
    RESULT_NONE,   // singleplayer / neutro: mostra game_over.xpm com o score
    RESULT_WIN,    // multiplayer: ganhou (10 patos primeiro, ou mais patos no fim do tempo)
    RESULT_LOSE,   // multiplayer: perdeu
    RESULT_DRAW    // multiplayer: empate no fim do tempo (mesmo nº de patos)
} game_result_t;

extern game_state_t state;
extern game_mode_t mode;

void state_render();
void state_enter_menu();
void state_enter_multiplayer();   // chamado ao clicar "Multiplayer": decide host/guest
void state_enter_mp_waiting();    // ecrã "waiting for player 2" (host à espera)
void state_enter_playing();
void state_enter_game_over(game_result_t result);
void state_enter_leaderboard();
void state_tick();
int state_get_time();

// reações a bytes recebidos pela série (chamadas por handle_uart)
void state_peer_ready();         // recebeu GAME_START do adversário
void state_peer_cancel();        // recebeu MP_CANCEL (host desistiu da espera)
void state_peer_game_over();     // recebeu GAME_OVER (adversário fez 10 → perdemos)
void state_peer_score(int s);    // recebeu o score final do adversário (fim do tempo)

#endif
