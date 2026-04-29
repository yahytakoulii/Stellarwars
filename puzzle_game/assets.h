#ifndef ASSETS_H
#define ASSETS_H

/* ============================================================
 *  assets.h  –  Centralised asset path constants
 *  All image / font / sound filenames live here so nothing is
 *  hard-coded inside game logic.
 * ============================================================ */

/* -- Fonts -------------------------------------------------- */
#define FONT_PATH        "assets/fonts/SpaceMono-Regular.ttf"
#define FONT_BOLD_PATH   "assets/fonts/SpaceMono-Bold.ttf"

/* -- Background / puzzle image ------------------------------ */
#define BG_IMAGE_PATH    "assets/images/space_bg.png"
#define PUZZLE_IMG_PATH  "assets/images/puzzle_main.png"

/* -- Puzzle candidate pieces -------------------------------- */
#define PIECE_CORRECT_PATH  "assets/images/piece_correct.png"
#define PIECE_WRONG1_PATH   "assets/images/piece_wrong1.png"
#define PIECE_WRONG2_PATH   "assets/images/piece_wrong2.png"

/* -- UI elements -------------------------------------------- */
#define FRAME_PATH       "assets/images/frame.png"

/* -- Sounds (optional – only used when SDL2_mixer present) -- */
#define SFX_SUCCESS_PATH "assets/sounds/success.wav"
#define SFX_FAIL_PATH    "assets/sounds/fail.wav"
#define SFX_PICK_PATH    "assets/sounds/pick.wav"

/* -- Window -------------------------------------------------- */
#define WINDOW_TITLE  "Interstellar Puzzle"
#define WINDOW_W      1024
#define WINDOW_H      680

/* -- Puzzle geometry ---------------------------------------- */
/* The missing piece is cut from this region of the main image */
#define MISSING_X     380
#define MISSING_Y     180
#define MISSING_W     160
#define MISSING_H     160

/* Y position of the three candidate pieces tray at the bottom */
#define TRAY_Y        490
/* CRITICAL: piece dimensions MUST equal MISSING_W/H so that a placed
   piece fills the hole pixel-perfectly with zero scaling artifact.     */
#define TRAY_PIECE_W  MISSING_W   /* 160 */
#define TRAY_PIECE_H  MISSING_H   /* 160 */
#define TRAY_GAP      40   /* horizontal gap between pieces */

/* Timer bar geometry */
#define TIMER_BAR_X   40
#define TIMER_BAR_Y   640
#define TIMER_BAR_W   (WINDOW_W - 80)
#define TIMER_BAR_H   14
#define TIMER_SECONDS 60   /* seconds before time-out */

#endif /* ASSETS_H */
