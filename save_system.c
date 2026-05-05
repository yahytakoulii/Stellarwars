#include "save_system.h"

#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>

#define SAVE_MAGIC "PFINAL_SAVE"
#define SAVE_VERSION 1
#define MAX_SAVE_FILES 9

typedef struct
{
    SDL_Rect posScreen;
    SDL_Rect posSprite;
    PlayerState state;
    int facing;
    int currentFrame;
    Uint32 frameDelay;
    int score;
    int vies;
    int health;
    int alive;
    int visible;
    int startX;
    int startY;
    int floorY;
    int isJumping;
    int jumpsUsed;
    int jumpHeld;
    float velY;
    float posYFloat;
} SavedPlayer;

typedef struct
{
    float x;
    float y;
    int w;
    int h;
    SDL_Rect srcRect;
    SDL_Rect dstRect;
    int useSprite;
    int direction;
    int posMin;
    int posMax;
    int groundY;
    int action;
    int health;
    int maxHealth;
    EnemyState state;
    Uint32 frameDelay;
} SavedNPC;

typedef struct
{
    char magic[16];
    int version;
    int bulletCount;
    int npcCount;
    int stableCount;
    int movingCount;
    int joueurActif;
    int puzzleChallengeUsed;
    SavedPlayer J1;
    SavedPlayer J2;
    Bullet bullets[MAX_BULLETS];
    SavedNPC npcs[MAX_NPCS];
    SDL_Rect stablePlatforms[STABLE_PLATFORM_COUNT];
    SDL_Rect movingPlatforms[MOVING_PLATFORM_COUNT];
    int movingDir[MOVING_PLATFORM_COUNT];
    int movingMin[MOVING_PLATFORM_COUNT];
    int movingMax[MOVING_PLATFORM_COUNT];
} SaveData;

typedef struct
{
    char path[SAVE_PATH_MAX];
    char name[96];
} SaveEntry;

static void copy_player_to_save(SavedPlayer *dst, const Joueur *src)
{
    dst->posScreen = src->posScreen;
    dst->posSprite = src->posSprite;
    dst->state = src->state;
    dst->facing = src->facing;
    dst->currentFrame = src->currentFrame;
    dst->frameDelay = src->frameDelay;
    dst->score = src->score;
    dst->vies = src->vies;
    dst->health = src->health;
    dst->alive = src->alive;
    dst->visible = src->visible;
    dst->startX = src->startX;
    dst->startY = src->startY;
    dst->floorY = src->floorY;
    dst->isJumping = src->isJumping;
    dst->jumpsUsed = src->jumpsUsed;
    dst->jumpHeld = src->jumpHeld;
    dst->velY = src->velY;
    dst->posYFloat = src->posYFloat;
}

static void copy_save_to_player(Joueur *dst, const SavedPlayer *src)
{
    dst->posScreen = src->posScreen;
    dst->posSprite = src->posSprite;
    dst->state = src->state;
    dst->facing = src->facing;
    dst->currentFrame = src->currentFrame;
    dst->lastFrameTime = 0;
    dst->frameDelay = src->frameDelay;
    dst->score = src->score;
    dst->vies = src->vies;
    dst->health = src->health;
    dst->alive = src->alive;
    dst->visible = src->visible;
    dst->startX = src->startX;
    dst->startY = src->startY;
    dst->floorY = src->floorY;
    dst->isJumping = src->isJumping;
    dst->jumpsUsed = src->jumpsUsed;
    dst->jumpHeld = 0;
    dst->velY = src->velY;
    dst->posYFloat = src->posYFloat;
    dst->moveLeft = 0;
    dst->moveRight = 0;
    dst->lastShotTime = 0;
    dst->invulnerableUntil = 0;
    dst->deathTime = 0;
}

static void copy_npc_to_save(SavedNPC *dst, const NPC *src)
{
    dst->x = src->x;
    dst->y = src->y;
    dst->w = src->w;
    dst->h = src->h;
    dst->srcRect = src->srcRect;
    dst->dstRect = src->dstRect;
    dst->useSprite = src->useSprite;
    dst->direction = src->direction;
    dst->posMin = src->posMin;
    dst->posMax = src->posMax;
    dst->groundY = src->groundY;
    dst->action = src->action;
    dst->health = src->health;
    dst->maxHealth = src->maxHealth;
    dst->state = src->state;
    dst->frameDelay = src->frameDelay;
}

static void copy_save_to_npc(NPC *dst, const SavedNPC *src)
{
    dst->x = src->x;
    dst->y = src->y;
    dst->w = src->w;
    dst->h = src->h;
    dst->srcRect = src->srcRect;
    dst->dstRect = src->dstRect;
    dst->useSprite = src->useSprite;
    dst->direction = src->direction;
    dst->posMin = src->posMin;
    dst->posMax = src->posMax;
    dst->groundY = src->groundY;
    dst->action = src->action;
    dst->health = src->health;
    dst->maxHealth = src->maxHealth;
    dst->state = src->state;
    dst->lastFrameTime = 0;
    dst->frameDelay = src->frameDelay;
    dst->attackStartedAt = 0;
    dst->nextAttackAt = 0;
}

static void render_prompt(SDL_Renderer *renderer, TTF_Font *font,
                          const char *title, const char *detail)
{
    SDL_Rect panel;
    SDL_Color white = {255, 255, 255, 255};
    SDL_Color yellow = {255, 220, 80, 255};
    SDL_Surface *surface;
    SDL_Texture *texture;
    SDL_Rect dst;

    SDL_SetRenderDrawColor(renderer, 5, 8, 18, 255);
    SDL_RenderClear(renderer);

    panel.x = SCREEN_W / 2 - 360;
    panel.y = SCREEN_H / 2 - 130;
    panel.w = 720;
    panel.h = 260;
    SDL_SetRenderDrawColor(renderer, 20, 24, 38, 255);
    SDL_RenderFillRect(renderer, &panel);
    SDL_SetRenderDrawColor(renderer, 210, 210, 230, 255);
    SDL_RenderDrawRect(renderer, &panel);

    if (font != NULL)
    {
        surface = TTF_RenderText_Blended(font, title, yellow);
        if (surface != NULL)
        {
            texture = SDL_CreateTextureFromSurface(renderer, surface);
            dst.x = SCREEN_W / 2 - surface->w / 2;
            dst.y = panel.y + 55;
            dst.w = surface->w;
            dst.h = surface->h;
            SDL_FreeSurface(surface);
            if (texture != NULL)
            {
                SDL_RenderCopy(renderer, texture, NULL, &dst);
                SDL_DestroyTexture(texture);
            }
        }

        surface = TTF_RenderText_Blended(font, detail, white);
        if (surface != NULL)
        {
            texture = SDL_CreateTextureFromSurface(renderer, surface);
            dst.x = SCREEN_W / 2 - surface->w / 2;
            dst.y = panel.y + 135;
            dst.w = surface->w;
            dst.h = surface->h;
            SDL_FreeSurface(surface);
            if (texture != NULL)
            {
                SDL_RenderCopy(renderer, texture, NULL, &dst);
                SDL_DestroyTexture(texture);
            }
        }
    }

    SDL_RenderPresent(renderer);
}

static void draw_text(SDL_Renderer *renderer, TTF_Font *font,
                      const char *text, int x, int y, SDL_Color color)
{
    SDL_Surface *surface;
    SDL_Texture *texture;
    SDL_Rect dst;

    if (font == NULL)
        return;

    surface = TTF_RenderText_Blended(font, text, color);
    if (surface == NULL)
        return;

    texture = SDL_CreateTextureFromSurface(renderer, surface);
    dst.x = x;
    dst.y = y;
    dst.w = surface->w;
    dst.h = surface->h;
    SDL_FreeSurface(surface);

    if (texture != NULL)
    {
        SDL_RenderCopy(renderer, texture, NULL, &dst);
        SDL_DestroyTexture(texture);
    }
}

static int wait_for_choice(SDL_Renderer *renderer, TTF_Font *font,
                           const char *title, const char *detail,
                           SDL_Keycode yesKey, SDL_Keycode noKey,
                           SDL_Keycode cancelKey)
{
    SDL_Event event;

    render_prompt(renderer, font, title, detail);

    while (1)
    {
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
                return 0;

            if (event.type == SDL_KEYDOWN)
            {
                if (event.key.keysym.sym == yesKey)
                    return 1;
                if (event.key.keysym.sym == noKey)
                    return 0;
                if (cancelKey != 0 && event.key.keysym.sym == cancelKey)
                    return -1;
            }
        }
        SDL_Delay(16);
    }
}

static void sanitize_save_name(const char *input, char *output, int outputSize)
{
    int i;
    int j = 0;
    char c;

    for (i = 0; input[i] != '\0' && j < outputSize - 1; i++)
    {
        c = input[i];
        if ((c >= 'a' && c <= 'z') ||
            (c >= 'A' && c <= 'Z') ||
            (c >= '0' && c <= '9') ||
            c == '_' || c == '-')
            output[j++] = c;
        else if (c == ' ')
            output[j++] = '_';
    }

    if (j == 0)
    {
        snprintf(output, outputSize, "%s", "save");
        return;
    }

    output[j] = '\0';
}

static void render_save_name_prompt(SDL_Renderer *renderer, TTF_Font *font,
                                    const char *name, const char *error)
{
    SDL_Rect panel;
    SDL_Rect inputBox;
    SDL_Color white = {255, 255, 255, 255};
    SDL_Color yellow = {255, 220, 80, 255};
    SDL_Color muted = {170, 180, 200, 255};
    SDL_Color red = {230, 80, 80, 255};
    char line[128];

    SDL_SetRenderDrawColor(renderer, 5, 8, 18, 255);
    SDL_RenderClear(renderer);

    panel.x = SCREEN_W / 2 - 420;
    panel.y = SCREEN_H / 2 - 170;
    panel.w = 840;
    panel.h = 340;
    SDL_SetRenderDrawColor(renderer, 20, 24, 38, 255);
    SDL_RenderFillRect(renderer, &panel);
    SDL_SetRenderDrawColor(renderer, 210, 210, 230, 255);
    SDL_RenderDrawRect(renderer, &panel);

    draw_text(renderer, font, "Name your save file", panel.x + 35, panel.y + 35, yellow);
    draw_text(renderer, font, "Type a save name, then press Enter to validate.", panel.x + 35, panel.y + 90, muted);

    inputBox.x = panel.x + 35;
    inputBox.y = panel.y + 145;
    inputBox.w = panel.w - 70;
    inputBox.h = 58;
    SDL_SetRenderDrawColor(renderer, 8, 12, 24, 255);
    SDL_RenderFillRect(renderer, &inputBox);
    SDL_SetRenderDrawColor(renderer, 220, 220, 235, 255);
    SDL_RenderDrawRect(renderer, &inputBox);

    snprintf(line, sizeof(line), "%s_", name);
    draw_text(renderer, font, line, inputBox.x + 18, inputBox.y + 13, white);

    draw_text(renderer, font, "Backspace edits. Escape cancels saving.", panel.x + 35, panel.y + 230, muted);
    if (error != NULL && error[0] != '\0')
        draw_text(renderer, font, error, panel.x + 35, panel.y + 275, red);

    SDL_RenderPresent(renderer);
}

int save_exists(const char *path)
{
    FILE *file = fopen(path, "rb");

    if (file == NULL)
        return 0;

    fclose(file);
    return 1;
}

int make_save_path(char *path, int pathSize, const char *saveName)
{
    time_t rawTime;
    struct tm *timeInfo;
    char stamp[64];
    char cleanName[64];
    struct stat info;

    if (stat(SAVE_DIR, &info) != 0 && mkdir(SAVE_DIR, 0775) != 0)
        return 0;

    rawTime = time(NULL);
    timeInfo = localtime(&rawTime);
    if (timeInfo == NULL)
        return 0;

    sanitize_save_name(saveName, cleanName, sizeof(cleanName));
    strftime(stamp, sizeof(stamp), "%Y%m%d_%H%M%S", timeInfo);
    snprintf(path, pathSize, "%s/%s_%s_%u.dat", SAVE_DIR, cleanName, stamp, SDL_GetTicks());
    return 1;
}

int save_game_state(const char *path,
                    Joueur *J1, Joueur *J2,
                    Bullet bullets[], int bulletCount,
                    NPC npcs[], int npcCount,
                    SDL_Rect stablePlatforms[], int stableCount,
                    SDL_Rect movingPlatforms[], int movingCount,
                    int movingDir[], int movingMin[], int movingMax[],
                    int joueurActif, int puzzleChallengeUsed)
{
    FILE *file;
    SaveData data;
    int i;

    memset(&data, 0, sizeof(data));
    snprintf(data.magic, sizeof(data.magic), "%s", SAVE_MAGIC);
    data.version = SAVE_VERSION;
    data.bulletCount = bulletCount;
    data.npcCount = npcCount;
    data.stableCount = stableCount;
    data.movingCount = movingCount;
    data.joueurActif = joueurActif;
    data.puzzleChallengeUsed = puzzleChallengeUsed;
    copy_player_to_save(&data.J1, J1);
    copy_player_to_save(&data.J2, J2);

    for (i = 0; i < bulletCount && i < MAX_BULLETS; i++)
        data.bullets[i] = bullets[i];

    for (i = 0; i < npcCount && i < MAX_NPCS; i++)
        copy_npc_to_save(&data.npcs[i], &npcs[i]);

    for (i = 0; i < stableCount && i < STABLE_PLATFORM_COUNT; i++)
        data.stablePlatforms[i] = stablePlatforms[i];

    for (i = 0; i < movingCount && i < MOVING_PLATFORM_COUNT; i++)
    {
        data.movingPlatforms[i] = movingPlatforms[i];
        data.movingDir[i] = movingDir[i];
        data.movingMin[i] = movingMin[i];
        data.movingMax[i] = movingMax[i];
    }

    file = fopen(path, "wb");
    if (file == NULL)
        return 0;

    if (fwrite(&data, sizeof(data), 1, file) != 1)
    {
        fclose(file);
        return 0;
    }

    fclose(file);
    return 1;
}

int load_game_state(const char *path,
                    Joueur *J1, Joueur *J2,
                    Bullet bullets[], int bulletCount,
                    NPC npcs[], int npcCount,
                    SDL_Rect stablePlatforms[], int stableCount,
                    SDL_Rect movingPlatforms[], int movingCount,
                    int movingDir[], int movingMin[], int movingMax[],
                    int *joueurActif, int *puzzleChallengeUsed)
{
    FILE *file;
    SaveData data;
    int i;

    file = fopen(path, "rb");
    if (file == NULL)
        return 0;

    if (fread(&data, sizeof(data), 1, file) != 1)
    {
        fclose(file);
        return 0;
    }
    fclose(file);

    if (strcmp(data.magic, SAVE_MAGIC) != 0 || data.version != SAVE_VERSION)
        return 0;

    copy_save_to_player(J1, &data.J1);
    copy_save_to_player(J2, &data.J2);

    for (i = 0; i < bulletCount && i < data.bulletCount && i < MAX_BULLETS; i++)
        bullets[i] = data.bullets[i];
    for (; i < bulletCount; i++)
        bullets[i].active = 0;

    for (i = 0; i < npcCount && i < data.npcCount && i < MAX_NPCS; i++)
        copy_save_to_npc(&npcs[i], &data.npcs[i]);

    for (i = 0; i < stableCount && i < data.stableCount && i < STABLE_PLATFORM_COUNT; i++)
        stablePlatforms[i] = data.stablePlatforms[i];

    for (i = 0; i < movingCount && i < data.movingCount && i < MOVING_PLATFORM_COUNT; i++)
    {
        movingPlatforms[i] = data.movingPlatforms[i];
        movingDir[i] = data.movingDir[i];
        movingMin[i] = data.movingMin[i];
        movingMax[i] = data.movingMax[i];
    }

    *joueurActif = data.joueurActif;
    *puzzleChallengeUsed = data.puzzleChallengeUsed;
    J1->visible = (*joueurActif == 1);
    J2->visible = (*joueurActif == 2);

    return 1;
}

static int load_save_entries(SaveEntry entries[], int maxEntries)
{
    DIR *dir;
    struct dirent *entry;
    int count = 0;
    int len;

    if (save_exists("savegame.dat") && count < maxEntries)
    {
        snprintf(entries[count].name, sizeof(entries[count].name), "%s", "savegame.dat");
        snprintf(entries[count].path, sizeof(entries[count].path), "%s", "savegame.dat");
        count++;
    }

    dir = opendir(SAVE_DIR);
    if (dir == NULL)
        return count;

    while ((entry = readdir(dir)) != NULL && count < maxEntries)
    {
        len = (int)strlen(entry->d_name);
        if (len > 4 && len < (int)sizeof(entries[count].name) &&
            strcmp(entry->d_name + len - 4, ".dat") == 0)
        {
            memcpy(entries[count].name, entry->d_name, (size_t)len + 1);
            if (snprintf(entries[count].path, sizeof(entries[count].path), "%s/%s",
                         SAVE_DIR, entry->d_name) < (int)sizeof(entries[count].path))
                count++;
        }
    }

    closedir(dir);
    return count;
}

static void render_save_list(SDL_Renderer *renderer, TTF_Font *font,
                             SaveEntry entries[], int count)
{
    SDL_Rect panel;
    SDL_Color white = {255, 255, 255, 255};
    SDL_Color yellow = {255, 220, 80, 255};
    SDL_Color muted = {170, 180, 200, 255};
    char line[160];
    int i;

    SDL_SetRenderDrawColor(renderer, 5, 8, 18, 255);
    SDL_RenderClear(renderer);

    panel.x = SCREEN_W / 2 - 430;
    panel.y = 70;
    panel.w = 860;
    panel.h = SCREEN_H - 140;
    SDL_SetRenderDrawColor(renderer, 20, 24, 38, 255);
    SDL_RenderFillRect(renderer, &panel);
    SDL_SetRenderDrawColor(renderer, 210, 210, 230, 255);
    SDL_RenderDrawRect(renderer, &panel);

    draw_text(renderer, font, "Saved games", panel.x + 35, panel.y + 30, yellow);

    if (count == 0)
        draw_text(renderer, font, "No save files found. Press N to start a new game.", panel.x + 35, panel.y + 105, white);
    else
    {
        draw_text(renderer, font, "Press 1-9 to load a save, or N to start a new game.", panel.x + 35, panel.y + 85, muted);
        for (i = 0; i < count; i++)
        {
            snprintf(line, sizeof(line), "%d. %s", i + 1, entries[i].name);
            draw_text(renderer, font, line, panel.x + 55, panel.y + 140 + i * 42, white);
        }
    }

    SDL_RenderPresent(renderer);
}

int prompt_select_save(SDL_Renderer *renderer, TTF_Font *font, char *path, int pathSize)
{
    SaveEntry entries[MAX_SAVE_FILES];
    SDL_Event event;
    int count;
    int index;

    count = load_save_entries(entries, MAX_SAVE_FILES);
    render_save_list(renderer, font, entries, count);

    while (1)
    {
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
                return 0;

            if (event.type == SDL_KEYDOWN)
            {
                if (event.key.keysym.sym == SDLK_n)
                    return 0;

                if (event.key.keysym.sym >= SDLK_1 && event.key.keysym.sym <= SDLK_9)
                {
                    index = event.key.keysym.sym - SDLK_1;
                    if (index >= 0 && index < count)
                    {
                        snprintf(path, pathSize, "%s", entries[index].path);
                        return 1;
                    }
                }
            }
        }
        SDL_Delay(16);
    }
}

int prompt_save_game(SDL_Renderer *renderer, TTF_Font *font)
{
    return wait_for_choice(renderer, font,
                           "Save your progress?",
                           "Press Y to save, N to quit, or ESC to cancel",
                           SDLK_y, SDLK_n, SDLK_ESCAPE);
}

int prompt_save_name(SDL_Renderer *renderer, TTF_Font *font, char *name, int nameSize)
{
    SDL_Event event;
    int len = 0;
    const char *error = "";

    name[0] = '\0';
    SDL_StartTextInput();
    render_save_name_prompt(renderer, font, name, error);

    while (1)
    {
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                SDL_StopTextInput();
                return 0;
            }

            if (event.type == SDL_TEXTINPUT)
            {
                int i;
                for (i = 0; event.text.text[i] != '\0' && len < nameSize - 1; i++)
                {
                    if (event.text.text[i] >= 32 && event.text.text[i] <= 126)
                        name[len++] = event.text.text[i];
                }
                name[len] = '\0';
                error = "";
                render_save_name_prompt(renderer, font, name, error);
            }

            if (event.type == SDL_KEYDOWN)
            {
                if (event.key.keysym.sym == SDLK_ESCAPE)
                {
                    SDL_StopTextInput();
                    return 0;
                }

                if (event.key.keysym.sym == SDLK_BACKSPACE && len > 0)
                {
                    name[--len] = '\0';
                    error = "";
                    render_save_name_prompt(renderer, font, name, error);
                }

                if (event.key.keysym.sym == SDLK_RETURN ||
                    event.key.keysym.sym == SDLK_KP_ENTER)
                {
                    if (len == 0)
                    {
                        error = "Please enter a save name before validating.";
                        render_save_name_prompt(renderer, font, name, error);
                    }
                    else
                    {
                        SDL_StopTextInput();
                        return 1;
                    }
                }
            }
        }
        SDL_Delay(16);
    }
}

void show_save_message(SDL_Renderer *renderer, TTF_Font *font, const char *message)
{
    render_prompt(renderer, font, message, "Press any key to continue");

    while (1)
    {
        SDL_Event event;

        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_KEYDOWN || event.type == SDL_QUIT)
                return;
        }
        SDL_Delay(16);
    }
}
