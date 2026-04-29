

#include "game.h"
#include "assets.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

int rect_contains(const SDL_Rect *r, int x, int y)
{
    return (x >= r->x && x < r->x + r->w &&
            y >= r->y && y < r->y + r->h);
}
int rect_overlap_pct(const SDL_Rect *a, const SDL_Rect *b)
{
    int ix1 = (a->x > b->x) ? a->x : b->x;
    int iy1 = (a->y > b->y) ? a->y : b->y;
    int ix2 = (a->x+a->w < b->x+b->w) ? a->x+a->w : b->x+b->w;
    int iy2 = (a->y+a->h < b->y+b->h) ? a->y+a->h : b->y+b->h;
    if (ix2 <= ix1 || iy2 <= iy1) return 0;
    int inter = (ix2-ix1) * (iy2-iy1);
    int area_a = a->w * a->h;
    if (area_a == 0) return 0;
    return inter * 100 / area_a;
}
static SDL_Texture *create_space_background(SDL_Renderer *renderer, int w, int h)
{
    SDL_Texture *tex = SDL_CreateTexture(renderer,
                        SDL_PIXELFORMAT_RGBA8888,
                        SDL_TEXTUREACCESS_TARGET,
                        w, h);
    if (!tex) return NULL;

    SDL_SetRenderTarget(renderer, tex);

    
    for (int y = 0; y < h; y++) {
        int t = y * 255 / h;
        SDL_SetRenderDrawColor(renderer,
            4  + t/20,   /* R */
            6  + t/12,   /* G */
            18 + t/8,    /* B */
            255);
        SDL_RenderDrawLine(renderer, 0, y, w, y);
    }

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    for (int r = 180; r > 0; r -= 2) {
        int alpha = (int)(30.0f * (1.0f - (float)r / 180.0f));
        SDL_SetRenderDrawColor(renderer, 120, 30, 180, (Uint8)alpha);
        for (int angle = 0; angle < 360; angle += 2) {
            float rad = angle * 3.14159f / 180.0f;
            int px = (int)(150 + r * cosf(rad));
            int py = (int)(130 + r * 0.6f * sinf(rad));
            SDL_RenderDrawPoint(renderer, px, py);
        }
    }

    for (int r = 220; r > 0; r -= 2) {
        int alpha = (int)(20.0f * (1.0f - (float)r / 220.0f));
        SDL_SetRenderDrawColor(renderer, 0, 180, 200, (Uint8)alpha);
        for (int angle = 0; angle < 360; angle += 2) {
            float rad = angle * 3.14159f / 180.0f;
            int px = (int)(820 + r * cosf(rad));
            int py = (int)(200 + r * 0.7f * sinf(rad));
            SDL_RenderDrawPoint(renderer, px, py);
        }
    }

 
    int planet_cx = 820, planet_cy = 520, planet_r = 80;
    for (int r = planet_r; r > 0; r--) {
        float frac = (float)r / planet_r;
        Uint8 R = (Uint8)(220 - frac * 80);
        Uint8 G = (Uint8)(100 - frac * 60);
        Uint8 B = (Uint8)(20);
        SDL_SetRenderDrawColor(renderer, R, G, B, 255);
        for (int angle = 0; angle < 360; angle += 1) {
            float rad = angle * 3.14159f / 180.0f;
            int px = (int)(planet_cx + r * cosf(rad));
            int py = (int)(planet_cy + r * sinf(rad));
            SDL_RenderDrawPoint(renderer, px, py);
        }
    }
  
    SDL_SetRenderDrawColor(renderer, 210, 180, 100, 120);
    for (int rx = planet_r + 10; rx < planet_r + 55; rx += 3) {
        for (int angle = 0; angle < 360; angle += 1) {
            float rad = angle * 3.14159f / 180.0f;
            int px = (int)(planet_cx + rx * cosf(rad));
            int py = (int)(planet_cy + rx * 0.28f * sinf(rad));
            SDL_RenderDrawPoint(renderer, px, py);
        }
    }

    
    int bh_cx = MISSING_X + MISSING_W/2;
    int bh_cy = MISSING_Y + MISSING_H/2;

    
    for (int r = 100; r > 40; r -= 2) {
        float frac = 1.0f - (float)(r - 40) / 60.0f;
        Uint8 R = (Uint8)(255 * frac);
        Uint8 G = (Uint8)(160 * frac);
        Uint8 B = (Uint8)(20  * frac);
        Uint8 A = (Uint8)(180 * frac);
        SDL_SetRenderDrawColor(renderer, R, G, B, A);
        for (int angle = 0; angle < 360; angle += 1) {
            float rad = angle * 3.14159f / 180.0f;
            int px = (int)(bh_cx + r * cosf(rad));
            int py = (int)(bh_cy + r * 0.45f * sinf(rad));
            SDL_RenderDrawPoint(renderer, px, py);
        }
    }
   
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    for (int r = 38; r > 0; r--) {
        for (int angle = 0; angle < 360; angle += 1) {
            float rad = angle * 3.14159f / 180.0f;
            int px = (int)(bh_cx + r * cosf(rad));
            int py = (int)(bh_cy + r * sinf(rad));
            SDL_RenderDrawPoint(renderer, px, py);
        }
    }

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
    SDL_SetRenderTarget(renderer, NULL);
    return tex;
}


static SDL_Texture *create_puzzle_texture(SDL_Renderer *renderer,
                                          SDL_Texture  *bg,
                                          int w, int h)
{
    SDL_Texture *tex = SDL_CreateTexture(renderer,
                        SDL_PIXELFORMAT_RGBA8888,
                        SDL_TEXTUREACCESS_TARGET,
                        w, h);
    if (!tex) return NULL;

    SDL_SetTextureBlendMode(tex, SDL_BLENDMODE_BLEND);
    SDL_SetRenderTarget(renderer, tex);

    SDL_RenderCopy(renderer, bg, NULL, NULL);

    SDL_Rect hole = {MISSING_X, MISSING_Y, MISSING_W, MISSING_H};

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 200);
    SDL_RenderFillRect(renderer, &hole);

 
    SDL_SetRenderDrawColor(renderer, 0, 220, 255, 255);
    int dash = 10, gap = 6;
    
    for (int x = hole.x; x < hole.x + hole.w; x += dash + gap) {
        int end = x + dash; if (end > hole.x + hole.w) end = hole.x + hole.w;
        SDL_RenderDrawLine(renderer, x, hole.y, end, hole.y);
        SDL_RenderDrawLine(renderer, x, hole.y + hole.h - 1, end, hole.y + hole.h - 1);
    }
  
    for (int y = hole.y; y < hole.y + hole.h; y += dash + gap) {
        int end = y + dash; if (end > hole.y + hole.h) end = hole.y + hole.h;
        SDL_RenderDrawLine(renderer, hole.x,             y, hole.x,             end);
        SDL_RenderDrawLine(renderer, hole.x + hole.w - 1, y, hole.x + hole.w - 1, end);
    }

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
    SDL_SetRenderTarget(renderer, NULL);
    return tex;
}


static SDL_Texture *create_correct_piece(SDL_Renderer *renderer,
                                         SDL_Texture  *bg)
{
   
    SDL_Texture *tex = SDL_CreateTexture(renderer,
                        SDL_PIXELFORMAT_RGBA8888,
                        SDL_TEXTUREACCESS_TARGET,
                        MISSING_W, MISSING_H);
    if (!tex) return NULL;

    SDL_SetRenderTarget(renderer, tex);

    
    SDL_Rect src = {MISSING_X, MISSING_Y, MISSING_W, MISSING_H};
    SDL_Rect dst = {0, 0, MISSING_W, MISSING_H};
    SDL_RenderCopy(renderer, bg, &src, &dst);

    
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 200);
    SDL_RenderDrawRect(renderer, &dst);

    SDL_SetRenderTarget(renderer, NULL);
    return tex;
}


static SDL_Texture *create_wrong_piece(SDL_Renderer *renderer,
                                       SDL_Texture  *bg,
                                       int shift_x, int shift_y)
{
    
    int ox = MISSING_X + shift_x;
    int oy = MISSING_Y + shift_y;
    if (ox < 0) ox = 0;
    if (oy < 0) oy = 0;
    if (ox + MISSING_W > WINDOW_W) ox = WINDOW_W - MISSING_W;
    if (oy + MISSING_H > WINDOW_H) oy = WINDOW_H - MISSING_H;

    SDL_Texture *tex = SDL_CreateTexture(renderer,
                        SDL_PIXELFORMAT_RGBA8888,
                        SDL_TEXTUREACCESS_TARGET,
                        MISSING_W, MISSING_H);
    if (!tex) return NULL;

    SDL_SetRenderTarget(renderer, tex);

   
    SDL_Rect src = {ox, oy, MISSING_W, MISSING_H};
    SDL_Rect dst = {0,  0,  MISSING_W, MISSING_H};
    SDL_RenderCopy(renderer, bg, &src, &dst);

  
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 200);
    SDL_RenderDrawRect(renderer, &dst);

    SDL_SetRenderTarget(renderer, NULL);
    return tex;
}


static void shuffle3(int arr[3])
{
    for (int i = 2; i > 0; i--) {
        int j = rand() % (i + 1);
        int tmp = arr[i]; arr[i] = arr[j]; arr[j] = tmp;
    }
}

static void init_stars(GameContext *ctx)
{
    for (int i = 0; i < 200; i++) {
        ctx->stars[i].x = rand() % WINDOW_W;
        ctx->stars[i].y = rand() % (TRAY_Y - 20); /* stay above tray */
        ctx->star_speeds[i] = 0.1f + (rand() % 10) * 0.03f;
    }
}

int game_init(GameContext *ctx)
{
    memset(ctx, 0, sizeof(*ctx));
    srand((unsigned)time(NULL));

    
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        fprintf(stderr, "SDL_Init error: %s\n", SDL_GetError());
        return 0;
    }

    ctx->window = SDL_CreateWindow(
        WINDOW_TITLE,
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        WINDOW_W, WINDOW_H,
        SDL_WINDOW_SHOWN
    );
    if (!ctx->window) {
        fprintf(stderr, "SDL_CreateWindow error: %s\n", SDL_GetError());
        return 0;
    }

    ctx->renderer = SDL_CreateRenderer(ctx->window, -1,
                        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!ctx->renderer)
        ctx->renderer = SDL_CreateRenderer(ctx->window, -1, SDL_RENDERER_SOFTWARE);

    if (!ctx->renderer) {
        fprintf(stderr, "SDL_CreateRenderer error: %s\n", SDL_GetError());
        return 0;
    }

    
    if (!(IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG) & (IMG_INIT_PNG | IMG_INIT_JPG))) {
        fprintf(stderr, "IMG_Init error: %s\n", IMG_GetError());
        return 0;
    }

    
    if (TTF_Init() < 0) {
        fprintf(stderr, "TTF_Init error: %s\n", TTF_GetError());
        return 0;
    }

#ifdef USE_MIXER
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        fprintf(stderr, "Mix_OpenAudio error: %s\n", Mix_GetError());
        
    } else {
        ctx->sfx_success = Mix_LoadWAV(SFX_SUCCESS_PATH);
        ctx->sfx_fail    = Mix_LoadWAV(SFX_FAIL_PATH);
        ctx->sfx_pick    = Mix_LoadWAV(SFX_PICK_PATH);
    }
#endif

  
    ctx->font_large = TTF_OpenFont(FONT_PATH, 42);
    ctx->font_small = TTF_OpenFont(FONT_PATH, 22);
    if (!ctx->font_large || !ctx->font_small) {
     
        ctx->font_large = TTF_OpenFont("/usr/share/fonts/truetype/dejavu/DejaVuSansMono-Bold.ttf", 42);
        ctx->font_small = TTF_OpenFont("/usr/share/fonts/truetype/dejavu/DejaVuSansMono.ttf", 22);
    }
    if (!ctx->font_large || !ctx->font_small) {
        fprintf(stderr, "TTF_OpenFont error: %s\n", TTF_GetError());
        return 0;
    }

   
    ctx->tex_bg = create_space_background(ctx->renderer, WINDOW_W, WINDOW_H);
    if (!ctx->tex_bg) {
        fprintf(stderr, "Failed to create background texture\n");
        return 0;
    }

    ctx->tex_puzzle = create_puzzle_texture(ctx->renderer, ctx->tex_bg, WINDOW_W, WINDOW_H);
    if (!ctx->tex_puzzle) {
        fprintf(stderr, "Failed to create puzzle texture\n");
        return 0;
    }

    SDL_Texture *pc  = create_correct_piece(ctx->renderer, ctx->tex_bg);
   
    SDL_Texture *pw1 = create_wrong_piece(ctx->renderer, ctx->tex_bg, +200,  +60);
    
    SDL_Texture *pw2 = create_wrong_piece(ctx->renderer, ctx->tex_bg, -200,  -80);

    if (!pc || !pw1 || !pw2) {
        fprintf(stderr, "Failed to create piece textures\n");
        return 0;
    }
    SDL_Texture *textures[3] = {pc, pw1, pw2};
    int correct_flags[3]     = {1,  0,   0  };
    int order[3]             = {0,  1,   2  };
    shuffle3(order);

    int total_w = 3 * TRAY_PIECE_W + 2 * TRAY_GAP;
    int tray_x_start = (WINDOW_W - total_w) / 2;

    for (int i = 0; i < 3; i++) {
        int slot = order[i];
        ctx->pieces[i].texture   = textures[slot];
        ctx->pieces[i].correct   = correct_flags[slot];
        ctx->pieces[i].placed    = 0;

        ctx->pieces[i].tray_rect.x = tray_x_start + i * (TRAY_PIECE_W + TRAY_GAP);
        ctx->pieces[i].tray_rect.y = TRAY_Y;
        ctx->pieces[i].tray_rect.w = TRAY_PIECE_W;
        ctx->pieces[i].tray_rect.h = TRAY_PIECE_H;

        ctx->pieces[i].drag_rect = ctx->pieces[i].tray_rect;

        if (correct_flags[slot]) ctx->correct_index = i;
    }

    /* Target slot */
    ctx->target_rect.x = MISSING_X;
    ctx->target_rect.y = MISSING_Y;
    ctx->target_rect.w = MISSING_W;
    ctx->target_rect.h = MISSING_H;

    /* Stars */
    init_stars(ctx);

    /* Drag state */
    ctx->dragging = -1;

    /* Timer */
    ctx->start_ticks  = SDL_GetTicks();
    ctx->timer_fraction = 1.0f;

    ctx->state = STATE_PLAYING;

    return 1;
}

/* ------------------------------------------------------------------ */
/*  game_destroy                                                        */
/* ------------------------------------------------------------------ */
void game_destroy(GameContext *ctx)
{
    if (!ctx) return;

    for (int i = 0; i < 3; i++) {
        if (ctx->pieces[i].texture) SDL_DestroyTexture(ctx->pieces[i].texture);
    }
    if (ctx->tex_puzzle)   SDL_DestroyTexture(ctx->tex_puzzle);
    if (ctx->tex_bg)       SDL_DestroyTexture(ctx->tex_bg);
    if (ctx->tex_feedback) SDL_DestroyTexture(ctx->tex_feedback);
    if (ctx->font_large)   TTF_CloseFont(ctx->font_large);
    if (ctx->font_small)   TTF_CloseFont(ctx->font_small);

#ifdef USE_MIXER
    if (ctx->sfx_success) Mix_FreeChunk(ctx->sfx_success);
    if (ctx->sfx_fail)    Mix_FreeChunk(ctx->sfx_fail);
    if (ctx->sfx_pick)    Mix_FreeChunk(ctx->sfx_pick);
    Mix_CloseAudio();
#endif

    if (ctx->renderer) SDL_DestroyRenderer(ctx->renderer);
    if (ctx->window)   SDL_DestroyWindow(ctx->window);

    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
}

/* ------------------------------------------------------------------ */
/*  Feedback text helper                                                */
/* ------------------------------------------------------------------ */
static void set_feedback(GameContext *ctx, const char *msg,
                         Uint8 r, Uint8 g, Uint8 b)
{
    if (ctx->tex_feedback) {
        SDL_DestroyTexture(ctx->tex_feedback);
        ctx->tex_feedback = NULL;
    }
    if (!ctx->font_large) return;

    SDL_Color col = {r, g, b, 255};
    SDL_Surface *surf = TTF_RenderText_Blended(ctx->font_large, msg, col);
    if (!surf) return;

    ctx->tex_feedback = SDL_CreateTextureFromSurface(ctx->renderer, surf);
    ctx->feedback_rect.w = surf->w;
    ctx->feedback_rect.h = surf->h;
    ctx->feedback_rect.x = (WINDOW_W - surf->w) / 2;
    ctx->feedback_rect.y = (WINDOW_H / 2) - surf->h / 2 - 30;
    SDL_FreeSurface(surf);
}

/* ------------------------------------------------------------------ */
/*  game_restart                                                        */
/* ------------------------------------------------------------------ */
void game_restart(GameContext *ctx)
{
    /* Reset piece positions */
    for (int i = 0; i < 3; i++) {
        ctx->pieces[i].drag_rect = ctx->pieces[i].tray_rect;
        ctx->pieces[i].placed    = 0;
    }

    /* Reshuffle which piece is correct (re-shuffle tray order) */
    int order[3] = {0, 1, 2};
    shuffle3(order);

    /* Rebuild textures with shuffled order */
    SDL_Texture *textures[3];
    int          corrects[3];
    for (int i = 0; i < 3; i++) {
        textures[i] = ctx->pieces[i].texture;
        corrects[i] = ctx->pieces[i].correct;
    }
    SDL_Texture *tmp_tex[3];
    int          tmp_cor[3];
    for (int i = 0; i < 3; i++) {
        tmp_tex[i] = textures[order[i]];
        tmp_cor[i] = corrects[order[i]];
    }
    for (int i = 0; i < 3; i++) {
        ctx->pieces[i].texture = tmp_tex[i];
        ctx->pieces[i].correct = tmp_cor[i];
        ctx->pieces[i].drag_rect = ctx->pieces[i].tray_rect;
        ctx->pieces[i].placed    = 0;
        if (tmp_cor[i]) ctx->correct_index = i;
    }

    if (ctx->tex_feedback) {
        SDL_DestroyTexture(ctx->tex_feedback);
        ctx->tex_feedback = NULL;
    }

    ctx->dragging       = -1;
    ctx->start_ticks    = SDL_GetTicks();
    ctx->timer_fraction = 1.0f;
    ctx->state          = STATE_PLAYING;
}

/* ------------------------------------------------------------------ */
/*  game_handle_event                                                   */
/* ------------------------------------------------------------------ */
void game_handle_event(GameContext *ctx, const SDL_Event *e)
{
    if (e->type == SDL_KEYDOWN) {
        if (e->key.keysym.sym == SDLK_ESCAPE)
            ctx->state = STATE_FAIL; /* signals quit to main.c */
        if (e->key.keysym.sym == SDLK_r)
            game_restart(ctx);
        return;
    }

    if (ctx->state != STATE_PLAYING) return;

    if (e->type == SDL_MOUSEBUTTONDOWN && e->button.button == SDL_BUTTON_LEFT) {
        int mx = e->button.x, my = e->button.y;
        /* Find which piece was clicked */
        for (int i = 0; i < 3; i++) {
            if (!ctx->pieces[i].placed &&
                rect_contains(&ctx->pieces[i].drag_rect, mx, my))
            {
                ctx->dragging       = i;
                ctx->drag_offset_x  = mx - ctx->pieces[i].drag_rect.x;
                ctx->drag_offset_y  = my - ctx->pieces[i].drag_rect.y;
#ifdef USE_MIXER
                if (ctx->sfx_pick) Mix_PlayChannel(-1, ctx->sfx_pick, 0);
#endif
                break;
            }
        }
    }

    if (e->type == SDL_MOUSEMOTION && ctx->dragging >= 0) {
        int mx = e->motion.x, my = e->motion.y;
        ctx->pieces[ctx->dragging].drag_rect.x = mx - ctx->drag_offset_x;
        ctx->pieces[ctx->dragging].drag_rect.y = my - ctx->drag_offset_y;
    }

    if (e->type == SDL_MOUSEBUTTONUP && e->button.button == SDL_BUTTON_LEFT
        && ctx->dragging >= 0)
    {
        int idx = ctx->dragging;
        ctx->dragging = -1;

        /* Check overlap with target zone.
           Use the TARGET area as denominator so the player must cover
           at least 70% of the hole – not just overlap 70% of the piece. */
        int pct = rect_overlap_pct(&ctx->target_rect, &ctx->pieces[idx].drag_rect);

        if (pct >= 70) {
            /* Snap: centre the piece exactly over the hole */
            ctx->pieces[idx].drag_rect.x = ctx->target_rect.x
                + (ctx->target_rect.w - ctx->pieces[idx].drag_rect.w) / 2;
            ctx->pieces[idx].drag_rect.y = ctx->target_rect.y
                + (ctx->target_rect.h - ctx->pieces[idx].drag_rect.h) / 2;

            if (ctx->pieces[idx].correct) {
                ctx->pieces[idx].placed = 1;
                ctx->state = STATE_SUCCESS;
                set_feedback(ctx, "PUZZLE SOLVED!", 0, 255, 160);
#ifdef USE_MIXER
                if (ctx->sfx_success) Mix_PlayChannel(-1, ctx->sfx_success, 0);
#endif
            } else {
                ctx->state = STATE_FAIL;
                set_feedback(ctx, "WRONG PIECE — PRESS R", 255, 80, 80);
#ifdef USE_MIXER
                if (ctx->sfx_fail) Mix_PlayChannel(-1, ctx->sfx_fail, 0);
#endif
            }
        } else {
            /* Return to tray */
            ctx->pieces[idx].drag_rect = ctx->pieces[idx].tray_rect;
        }
    }
}

/* ------------------------------------------------------------------ */
/*  game_update                                                         */
/* ------------------------------------------------------------------ */
void game_update(GameContext *ctx)
{
    if (ctx->state != STATE_PLAYING) return;

    /* Timer */
    Uint32 elapsed = SDL_GetTicks() - ctx->start_ticks;
    float total_ms = TIMER_SECONDS * 1000.0f;
    ctx->timer_fraction = 1.0f - ((float)elapsed / total_ms);
    if (ctx->timer_fraction < 0.0f) {
        ctx->timer_fraction = 0.0f;
        ctx->state = STATE_TIMEOUT;
        set_feedback(ctx, "TIME OUT — PRESS R", 255, 200, 0);
    }

    /* Slowly drift stars downward for parallax feel */
    for (int i = 0; i < 200; i++) {
        ctx->stars[i].y += (int)(ctx->star_speeds[i] * 0.5f + 0.1f);
        if (ctx->stars[i].y > TRAY_Y - 20)
            ctx->stars[i].y = 0;
    }
}

void game_render(GameContext *ctx)
{
    SDL_Renderer *r = ctx->renderer;

    SDL_SetRenderDrawColor(r, 4, 6, 18, 255);
    SDL_RenderClear(r);

    SDL_Rect puzzle_dst = {0, 0, WINDOW_W, WINDOW_H};
    SDL_RenderCopy(r, ctx->tex_puzzle, NULL, &puzzle_dst);

    /* ---- Stars (bright dots, parallax) ---- */
    SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_BLEND);
    for (int i = 0; i < 200; i++) {
        float b = 0.4f + ctx->star_speeds[i] * 2.0f;
        Uint8 bright = (b > 1.0f) ? 255 : (Uint8)(b * 255);
        SDL_SetRenderDrawColor(r, bright, bright, bright, bright);
        SDL_RenderDrawPoint(r, ctx->stars[i].x, ctx->stars[i].y);
    }
    SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_NONE);

    /* ---- Tray background ---- */
    SDL_Rect tray_bg = {0, TRAY_Y - 16, WINDOW_W, WINDOW_H - TRAY_Y + 16};
    SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(r, 10, 14, 35, 230);
    SDL_RenderFillRect(r, &tray_bg);
    /* Tray top border */
    SDL_SetRenderDrawColor(r, 0, 180, 255, 200);
    SDL_RenderDrawLine(r, 0, TRAY_Y - 16, WINDOW_W, TRAY_Y - 16);
    SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_NONE);

    /* ---- Label "SELECT PIECE" ---- */
    if (ctx->font_small && ctx->state == STATE_PLAYING) {
        SDL_Color col = {0, 180, 255, 255};
        SDL_Surface *s = TTF_RenderText_Blended(ctx->font_small, "SELECT & DRAG THE MISSING PIECE", col);
        if (s) {
            SDL_Texture *t = SDL_CreateTextureFromSurface(r, s);
            SDL_Rect dst = {(WINDOW_W - s->w)/2, TRAY_Y - 14, s->w, s->h};
            SDL_FreeSurface(s);
            SDL_RenderCopy(r, t, NULL, &dst);
            SDL_DestroyTexture(t);
        }
    }

    /* ---- Pieces (non-dragged first, then dragged on top) ---- */
    for (int pass = 0; pass < 2; pass++) {
        for (int i = 0; i < 3; i++) {
            if (ctx->pieces[i].placed) continue;
            int is_dragged = (ctx->dragging == i);
            if ((pass == 0 && is_dragged) || (pass == 1 && !is_dragged)) continue;

            /* Shadow when dragging */
            if (is_dragged) {
                SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_BLEND);
                SDL_SetRenderDrawColor(r, 0, 0, 0, 100);
                SDL_Rect shadow = ctx->pieces[i].drag_rect;
                shadow.x += 6; shadow.y += 6;
                SDL_RenderFillRect(r, &shadow);
                SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_NONE);
            }

            SDL_RenderCopy(r, ctx->pieces[i].texture, NULL, &ctx->pieces[i].drag_rect);

            /* Glow border when hovering over target — use target as denominator
               to match the snap threshold logic exactly                       */
            if (is_dragged) {
                int pct = rect_overlap_pct(&ctx->target_rect, &ctx->pieces[i].drag_rect);
                if (pct > 30) {
                    SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_BLEND);
                    SDL_SetRenderDrawColor(r, 0, 255, 180, 200);
                    SDL_Rect border = ctx->pieces[i].drag_rect;
                    for (int t = 0; t < 3; t++) {
                        border.x--; border.y--; border.w += 2; border.h += 2;
                        SDL_RenderDrawRect(r, &border);
                    }
                    SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_NONE);
                }
            }
        }
    }

    /* ---- Placed piece fills the hole ---- */
    for (int i = 0; i < 3; i++) {
        if (ctx->pieces[i].placed) {
            SDL_Rect fill = {MISSING_X, MISSING_Y, MISSING_W, MISSING_H};
            SDL_RenderCopy(r, ctx->pieces[i].texture, NULL, &fill);
        }
    }

    /* ---- Timer bar ---- */
    {
        SDL_Rect bar_bg = {TIMER_BAR_X, TIMER_BAR_Y, TIMER_BAR_W, TIMER_BAR_H};
        SDL_SetRenderDrawColor(r, 30, 30, 60, 255);
        SDL_RenderFillRect(r, &bar_bg);

        /* Colour transitions: green → yellow → red */
        float f = ctx->timer_fraction;
        Uint8 br = (f > 0.5f) ? (Uint8)((1.0f - f) * 2 * 255) : 255;
        Uint8 bg2= (f > 0.5f) ? 255 : (Uint8)(f * 2 * 255);
        int bar_w = (int)(TIMER_BAR_W * f);
        if (bar_w < 0) bar_w = 0;
        SDL_Rect bar_fill = {TIMER_BAR_X, TIMER_BAR_Y, bar_w, TIMER_BAR_H};
        SDL_SetRenderDrawColor(r, br, bg2, 30, 255);
        SDL_RenderFillRect(r, &bar_fill);

        /* Border */
        SDL_SetRenderDrawColor(r, 80, 80, 120, 255);
        SDL_RenderDrawRect(r, &bar_bg);
    }

    /* ---- Feedback overlay (success / fail) ---- */
    if (ctx->tex_feedback && ctx->state != STATE_PLAYING) {
        /* Dark overlay */
        SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(r, 0, 0, 0, 160);
        SDL_Rect overlay = {0, 0, WINDOW_W, WINDOW_H};
        SDL_RenderFillRect(r, &overlay);
        SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_NONE);

        /* Main message */
        SDL_RenderCopy(r, ctx->tex_feedback, NULL, &ctx->feedback_rect);

        /* Sub-message */
        if (ctx->font_small) {
            SDL_Color sub_col = {180, 180, 220, 255};
            const char *sub = "Press R to restart   |   ESC to quit";
            SDL_Surface *ss = TTF_RenderText_Blended(ctx->font_small, sub, sub_col);
            if (ss) {
                SDL_Texture *st = SDL_CreateTextureFromSurface(r, ss);
                SDL_Rect sdst = {
                    (WINDOW_W - ss->w) / 2,
                    ctx->feedback_rect.y + ctx->feedback_rect.h + 20,
                    ss->w, ss->h
                };
                SDL_FreeSurface(ss);
                SDL_RenderCopy(r, st, NULL, &sdst);
                SDL_DestroyTexture(st);
            }
        }
    }

    SDL_RenderPresent(r);
}
