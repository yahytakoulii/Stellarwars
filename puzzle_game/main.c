

#include <stdio.h>
#include "game.h"

int main(int argc, char *argv[])
{
    (void)argc; (void)argv;

    GameContext ctx;
    Uint32 result_started = 0;
    int exit_code = 2;

    /* ---------- Initialise ---------- */
    if (!game_init(&ctx)) {
        fprintf(stderr, "game_init failed – aborting.\n");
        game_destroy(&ctx);
        return 1;
    }

    /* ---------- Game loop ---------- */
    int running = 1;
    SDL_Event event;

    while (running) {
        /* --- Event processing --- */
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = 0;
                break;
            }
            /* ESC key: handled inside game_handle_event →
               sets state to STATE_FAIL; we treat that as a
               signal to check if the user pressed ESC twice. */
            if (event.type == SDL_KEYDOWN &&
                event.key.keysym.sym == SDLK_ESCAPE)
            {
                /* First ESC while playing → show fail screen;
                   ESC again (or from fail screen) → quit      */
                if (ctx.state != STATE_PLAYING) {
                    running = 0;
                    break;
                }
            }
            game_handle_event(&ctx, &event);
        }

        /* --- Update game state --- */
        game_update(&ctx);

        /* --- Render --- */
        game_render(&ctx);

        if (ctx.state != STATE_PLAYING) {
            if (result_started == 0)
                result_started = SDL_GetTicks();

            if (SDL_GetTicks() - result_started >= 1200) {
                exit_code = (ctx.state == STATE_SUCCESS) ? 0 : 2;
                running = 0;
            }
        }
    }

    /* ---------- Cleanup ---------- */
    game_destroy(&ctx);
    return exit_code;
}
