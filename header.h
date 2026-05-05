/**
* @file header.h
* @brief Shared header file for the main SDL game.
* @author C Team
* @version 0.1
* @date May 02, 2026
*
* This file contains constants, structures, and function prototypes used
* by the player, enemy, bullet, and SDL initialization modules.
*/

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
#define PLUTO_MAX_FRAMES 8

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

/**
* @struct Animation
* @brief struct for sprite animation.
*/
typedef struct
{
    SDL_Texture *texture; /*!< Animation texture. */
    int rows; /*!< Number of sprite sheet rows. */
    int cols; /*!< Number of sprite sheet columns. */
    int frameW; /*!< Width of one frame. */
    int frameH; /*!< Height of one frame. */
    int drawW; /*!< Stable drawing width for the animation. */
    int drawH; /*!< Stable drawing height for the animation. */
    SDL_Rect frames[2][PLUTO_MAX_FRAMES]; /*!< Cropped source rectangles for animation frames. */
} Animation;

/**
* @struct Bullet
* @brief struct for a bullet.
*/
typedef struct
{
    int active; /*!< Active state of the bullet. */
    SDL_Rect rect; /*!< Bullet rectangle. */
    int direction; /*!< Bullet direction. */
    int owner; /*!< Player owner of the bullet. */
} Bullet;

/**
* @struct Joueur
* @brief struct for a player.
*/
typedef struct
{
    Animation walk; /*!< Walking animation. */
    Animation jump; /*!< Jump animation. */
    Animation fire; /*!< Fire animation. */
    Animation die; /*!< Death animation. */
    SDL_Rect posScreen; /*!< Player position on the screen. */
    SDL_Rect posSprite; /*!< Player source sprite rectangle. */
    SDL_Rect drawRect; /*!< Visual drawing rectangle for the player. */
    int drawOffsetX; /*!< Horizontal drawing offset from the hitbox. */
    int drawOffsetY; /*!< Vertical drawing offset from the hitbox bottom. */
    PlayerState state; /*!< Current player state. */
    int facing; /*!< Current facing direction. */
    int currentFrame; /*!< Current animation frame. */
    Uint32 lastFrameTime; /*!< Time of the last frame update. */
    Uint32 frameDelay; /*!< Delay between animation frames. */
    int score; /*!< Player score. */
    int vies; /*!< Player lives. */
    int health; /*!< Player health. */
    int alive; /*!< Alive state of the player. */
    int visible; /*!< Visibility state of the player. */
    int startX; /*!< Start x position. */
    int startY; /*!< Start y position. */
    int floorY; /*!< Current floor y position. */
    int isJumping; /*!< Jumping state. */
    int jumpsUsed; /*!< Number of jumps already used. */
    int jumpHeld; /*!< Jump key held state. */
    float velY; /*!< Vertical velocity. */
    float posYFloat; /*!< Precise vertical position. */
    int moveLeft; /*!< Move-left state. */
    int moveRight; /*!< Move-right state. */
    Uint32 lastShotTime; /*!< Time of the last shot. */
    Uint32 invulnerableUntil; /*!< End time of invulnerability. */
    Uint32 deathTime; /*!< Time of death. */
} Joueur;

/**
* @struct NPC
* @brief struct for an enemy character.
*/
typedef struct
{
    float x; /*!< Enemy x position. */
    float y; /*!< Enemy y position. */
    int w; /*!< Enemy width. */
    int h; /*!< Enemy height. */
    SDL_Texture *walkTexture; /*!< Walking texture. */
    SDL_Texture *slashTexture; /*!< Attack texture. */
    SDL_Texture *dieTexture; /*!< Death texture. */
    SDL_Rect srcRect; /*!< Source sprite rectangle. */
    SDL_Rect dstRect; /*!< Destination rectangle. */
    int useSprite; /*!< Sprite rendering state. */
    int direction; /*!< Enemy direction. */
    int posMin; /*!< Minimum patrol position. */
    int posMax; /*!< Maximum patrol position. */
    int groundY; /*!< Ground y position. */
    int action; /*!< Current action index. */
    int health; /*!< Enemy health. */
    int maxHealth; /*!< Maximum enemy health. */
    EnemyState state; /*!< Current enemy state. */
    Uint32 lastFrameTime; /*!< Time of the last frame update. */
    Uint32 frameDelay; /*!< Delay between animation frames. */
    Uint32 attackStartedAt; /*!< Time when attack started. */
    Uint32 nextAttackAt; /*!< Next allowed attack time. */
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
SDL_Rect getJoueurDrawRect(Joueur *J);
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
