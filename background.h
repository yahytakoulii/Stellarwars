/**
* @file background.h
* @brief Header file for background, platforms, timer, guide, and buttons.
* @author C Team
* @version 0.1
* @date May 02, 2026
*
* This file contains the structures and function prototypes used for
* rendering and controlling the scrolling background module.
*/

#ifndef BACKGROUND_H
#define BACKGROUND_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#define W_SCREEN        1280
#define H_SCREEN         650
#define W_HALF          (W_SCREEN / 2)
#define SCROLL_SPEED       4
#define MAX_PLATFORMS     20

#define PLATFORM_FIXED        0
#define PLATFORM_MOBILE       1
#define PLATFORM_DESTRUCTIBLE 2

/* Screen modes */
#define MODE_SPLIT  0
#define MODE_SINGLE 1

/**
* @struct Platform
* @brief struct for a platform.
*/
typedef struct {
    SDL_Rect rect; /*!< Rectangle of the platform. */
    int type; /*!< Platform type. */
    int dx; /*!< Horizontal movement speed. */
    int alive; /*!< Active state of the platform. */
} Platform;

/**
* @struct Background
* @brief struct for background.
*/
typedef struct {
    SDL_Texture *image; /*!< Background texture. */
    int imgW; /*!< Background image width. */
    int imgH; /*!< Background image height. */
    SDL_Rect posBack1; /*!< Source rectangle for the first background camera. */
    SDL_Rect posBack2; /*!< Source rectangle for the second background camera. */
    SDL_Rect posScreen1; /*!< Destination rectangle for the first screen viewport. */
    SDL_Rect posScreen2; /*!< Destination rectangle for the second screen viewport. */
    Platform platforms[MAX_PLATFORMS]; /*!< Array of platforms. */
    int platformCount; /*!< Number of platforms. */
    int level; /*!< Current level number. */
    int screenMode; /*!< MODE_SPLIT or MODE_SINGLE. */
} Background;

/**
* @struct GameTimer
* @brief struct for a game timer.
*/
typedef struct {
    Uint32 startTime; /*!< Timer start time. */
    Uint32 elapsed; /*!< Elapsed time. */
} GameTimer;

#define GUIDE_MAX_LINES 20

/**
* @struct Guide
* @brief struct for the help guide.
*/
typedef struct {
    int visible; /*!< Guide visibility state. */
    char lines[GUIDE_MAX_LINES][120]; /*!< Guide text lines. */
    int lineCount; /*!< Number of guide lines. */
} Guide;

/**
* @struct Button
* @brief struct for a graphical button.
*/
typedef struct {
    SDL_Rect      pos; /*!< Button position. */
    SDL_Texture  *texture; /*!< Button texture. */
    int           hovered; /*!< Hover state of the button. */
} Button;

/* Background */
void init_background(Background *bg, SDL_Renderer *renderer, int level);
void init_platforms(Background *bg, int level);
void scroll_background(Background *bg, int numBack, int dx, int dy);
void update_platforms(Background *bg);
void render_background(Background *bg, SDL_Renderer *renderer);
void render_platforms(Background *bg, SDL_Renderer *renderer);
void free_background(Background *bg);

/* Timer  – x,y let caller position it per viewport */
void init_timer(GameTimer *t);
void update_timer(GameTimer *t);
void render_timer(GameTimer *t, SDL_Renderer *renderer, TTF_Font *font,
                  int centerX, int y);

/* Guide – offsetX = left edge of the viewport, maxW = viewport width */
void init_guide(Guide *g, int player);
void toggle_guide(Guide *g);
void render_guide(Guide *g, SDL_Renderer *renderer, TTF_Font *font,
                  int offsetX, int maxW);

/* Alpha helper */
void fill_rect_alpha(SDL_Renderer *r, SDL_Rect rect,
                     Uint8 red, Uint8 green, Uint8 blue, Uint8 alpha);

/* Button */
void init_button(Button *btn, SDL_Renderer *renderer, const char *path,
                 int x, int y, int w, int h);
void check_button_hover(Button *btn, int mx, int my);
int  button_clicked(Button *btn, int mx, int my);
void render_button(Button *btn, SDL_Renderer *renderer, TTF_Font *font,
                   const char *label);
void free_button(Button *btn);

#endif
