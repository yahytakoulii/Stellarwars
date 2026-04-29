#ifndef HEADER_H
#define HEADER_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SCREEN_W 1280
#define SCREEN_H 720
#define WORLD_W 5120
#define WORLD_H 850
#define GROUND_Y 500
#define MARS_GROUND_Y 470
#define MAX_NPCS 7
#define STABLE_PLATFORM_COUNT 3
#define MOVING_PLATFORM_COUNT 2
#define SHIP_END_X 3050

#define PLAYER_W 120
#define PLAYER_H 120
#define PLAYER_SPEED 4
#define PLAYER_LIVES 3
#define PLAYER_HEALTH_MAX 3
#define PLAYER_DAMAGE_COOLDOWN 900
#define PLAYER_RESPAWN_DELAY 1200

#define PLUTO_WALK_ROWS 2
#define PLUTO_WALK_COLS 5
#define PLUTO_FIRE_ROWS 2
#define PLUTO_FIRE_COLS 5
#define PLUTO_JUMP_ROWS 2
#define PLUTO_JUMP_COLS 7
#define PLUTO_DIE_ROWS 2
#define PLUTO_DIE_COLS 6

#define NPC_WALK_COLS 9
#define NPC_WALK_ROWS 2
#define NPC_WALK_RIGHT_ROW 1
#define NPC_SLASH_COLS 6
#define NPC_SLASH_ROWS 2
#define NPC_DIE_COLS 6
#define NPC_DIE_ROWS 1

#define FACE_RIGHT 0
#define FACE_LEFT 1

#define MAX_BULLETS 50
#define BULLET_W 24
#define BULLET_H 8
#define BULLET_SPEED 14
#define FIRE_COOLDOWN 250

#define ENEMY_HEALTH_MAX 3
#define ENEMY_ATTACK_RANGE 45
#define ENEMY_ATTACK_DURATION 420
#define ENEMY_ATTACK_COOLDOWN 900
#define ENEMY_DAMAGE_SCORE 10
#define ENEMY_HIT_SCORE 10
#define ENEMY_KILL_SCORE 100
#define PUZZLE_CHALLENGE_ROUNDS 5
#define PUZZLE_CHALLENGE_REQUIRED 3
#define PUZZLE_COMMAND "cd puzzle_game && ./puzzle"

typedef enum
{
    STATE_IDLE,
    STATE_WALK,
    STATE_JUMP,
    STATE_FIRE,
    STATE_DIE
} PlayerState;

typedef enum
{
    ENEMY_ALIVE,
    ENEMY_INJURED,
    ENEMY_NEUTRALIZED,
    ENEMY_ATTACKING,
    ENEMY_REMOVED
} EnemyState;

typedef struct
{
    SDL_Texture *texture;
    int rows;
    int cols;
    int frameW;
    int frameH;
} Animation;

typedef struct
{
    int active;
    SDL_Rect rect;
    int direction;
    int owner;
} Bullet;

typedef struct
{
    Animation walk;
    Animation jump;
    Animation fire;
    Animation die;
    SDL_Rect posScreen;
    SDL_Rect posSprite;
    PlayerState state;
    int facing;
    int currentFrame;
    Uint32 lastFrameTime;
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
    int moveLeft;
    int moveRight;
    Uint32 lastShotTime;
    Uint32 invulnerableUntil;
    Uint32 deathTime;
} Joueur;

typedef struct
{
    float x;
    float y;
    int w;
    int h;
    SDL_Texture *walkTexture;
    SDL_Texture *slashTexture;
    SDL_Texture *dieTexture;
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
    Uint32 lastFrameTime;
    Uint32 frameDelay;
    Uint32 attackStartedAt;
    Uint32 nextAttackAt;
} NPC;

int initSDL(SDL_Window **window, SDL_Renderer **renderer);
void shutdownSDL(SDL_Window *window, SDL_Renderer *renderer);

int initialiserJoueur(Joueur *J, SDL_Renderer *renderer, int x, int y);
void libererJoueur(Joueur *J);
void gererEntreeJoueurClavier(Joueur *J, const Uint8 *keys,
                              SDL_Scancode left, SDL_Scancode right,
                              SDL_Scancode jumpKey);
void lancerSaut(Joueur *J);
void updateJoueur(Joueur *J, Uint32 now);
void renderJoueur(SDL_Renderer *renderer, Joueur *J);
void reinitialiserPositionJoueur(Joueur *J);
void appliquerDegatsJoueur(Joueur *J, int damage, Uint32 now);
void respawnJoueur(Joueur *J);
void ajouterScoreJoueur(Joueur *J, int delta);

void initBullets(Bullet bullets[], int size);
void effacerBullets(Bullet bullets[], int size);
void tirerBullet(Bullet bullets[], int size, Joueur *shooter, int owner, Uint32 now);
void updateBullets(Bullet bullets[], int size);
void renderBullets(SDL_Renderer *renderer, Bullet bullets[], int size);

void initNPC(NPC *npc, SDL_Renderer *renderer);
void destroyNPC(NPC *npc);
void updateNPC(NPC *npc);
void renderNPC(SDL_Renderer *renderer, NPC *npc);
int isNPCActive(NPC *npc);

int collisionAABB(SDL_Rect a, SDL_Rect b);

#endif
