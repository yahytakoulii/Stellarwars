#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include <stdio.h>
#include "highscore.h"

int main(int argc, char *argv[])
{
    /* All variables declared at the top */
    SDL_Window   *window;
    SDL_Renderer *renderer;
    Highscore     hs;
    SDL_Event     event;
    int           running;
    int           fakeScore;

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0) {
        printf("SDL_Init Error: %s\n", SDL_GetError());
        return 1;
    }
    if (IMG_Init(IMG_INIT_PNG) == 0) {
        printf("IMG_Init Error: %s\n", IMG_GetError());
        return 1;
    }
    if (TTF_Init() == -1) {
        printf("TTF_Init Error: %s\n", TTF_GetError());
        return 1;
    }
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        printf("Mix_OpenAudio Error: %s\n", Mix_GetError());
        return 1;
    }
    window = SDL_CreateWindow(
        "Highscore System",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        1280, 650,
        SDL_WINDOW_SHOWN
    );
    if (!window) {
        printf("Window Error: %s\n", SDL_GetError());
        return 1;
    }
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        printf("Renderer Error: %s\n", SDL_GetError());
        return 1;
    }
    fakeScore = 123;
    init_highscore(&hs, renderer, fakeScore);
    running = 1;
    while (running) {
        /* --- Event handling --- */
        while (SDL_PollEvent(&event))
            handle_highscore_event(&hs, event, &running);
        /* --- Check back button flag -> return to main menu --- */
        if (hs.go_to_menu) {
            hs.go_to_menu = 0;
            Mix_HaltMusic();
            /* TODO: replace running = 0 with your main menu function call */
            running = 0;
        }
        /* --- Check E key flag -> go to puzzle sub-menu --- */
        if (hs.go_to_puzzle) {
            hs.go_to_puzzle = 0;
            Mix_HaltMusic();
            /* TODO: replace running = 0 with your puzzle menu function call */
            running = 0;
        }
        /* --- Render --- */
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        render_highscore(&hs, renderer);
        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }
    free_highscore(&hs);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    Mix_CloseAudio();
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
    return 0;
}
