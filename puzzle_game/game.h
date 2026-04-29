#ifndef GAME_H
#define GAME_H

/* ============================================================
 *  game.h  –  Public interface for the puzzle game module
 * ============================================================ */

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

/* Optional mixer – compile with -DUSE_MIXER to enable */
#ifdef USE_MIXER
#  include <SDL2/SDL_mixer.h>
#endif

#include "assets.h"

/* ------------------------------------------------------------ */
/*  Enumerations                                                 */
/* ------------------------------------------------------------ */

typedef enum {
    STATE_PLAYING = 0,
    STATE_SUCCESS,
    STATE_FAIL,
    STATE_TIMEOUT
} GameState;

/* ------------------------------------------------------------ */
/*  Data structures                                              */
/* ------------------------------------------------------------ */

/* One candidate puzzle piece */
typedef struct {
    SDL_Texture *texture;   /* rendered texture                  */
    SDL_Rect     tray_rect; /* rest position in the bottom tray  */
    SDL_Rect     drag_rect; /* current position while dragging   */
    int          correct;   /* 1 = this is the matching piece    */
    int          placed;    /* 1 = successfully placed           */
} Piece;

/* All assets and state bundled together */
typedef struct {
    /* SDL core */
    SDL_Window   *window;
    SDL_Renderer *renderer;

    /* Textures */
    SDL_Texture  *tex_puzzle;      /* main puzzle image (with hole)  */
    SDL_Texture  *tex_bg;          /* space background                */
    SDL_Texture  *tex_overlay;     /* dark semi-transparent overlay   */

    /* Font */
    TTF_Font     *font_large;
    TTF_Font     *font_small;

    /* Pieces (3 candidates) */
    Piece         pieces[3];
    int           correct_index;   /* which of the 3 is correct       */

    /* Drag state */
    int           dragging;        /* index of piece being dragged, -1 */
    int           drag_offset_x;   /* cursor offset inside piece       */
    int           drag_offset_y;

    /* Target zone (where the piece must land) */
    SDL_Rect      target_rect;

    /* Timer */
    Uint32        start_ticks;     /* SDL_GetTicks() at game start    */
    float         timer_fraction;  /* 1.0 → full, 0.0 → expired       */

    /* Game status */
    GameState     state;

    /* Feedback text texture (cached) */
    SDL_Texture  *tex_feedback;
    SDL_Rect      feedback_rect;

    /* Stars (simple particle background) */
    SDL_Point     stars[200];
    float         star_speeds[200];

#ifdef USE_MIXER
    Mix_Chunk    *sfx_success;
    Mix_Chunk    *sfx_fail;
    Mix_Chunk    *sfx_pick;
#endif

} GameContext;

/* ------------------------------------------------------------ */
/*  Function declarations                                        */
/* ------------------------------------------------------------ */

/* Lifecycle */
int  game_init   (GameContext *ctx);
void game_destroy(GameContext *ctx);

/* Per-frame */
void game_handle_event (GameContext *ctx, const SDL_Event *e);
void game_update       (GameContext *ctx);
void game_render       (GameContext *ctx);

/* Utilities (internal but exposed for testing) */
int  rect_contains(const SDL_Rect *r, int x, int y);
int  rect_overlap_pct(const SDL_Rect *a, const SDL_Rect *b);

/* Restart */
void game_restart(GameContext *ctx);

#endif /* GAME_H */
