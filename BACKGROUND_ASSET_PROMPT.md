# Mars Ship Level Background Asset

## Final Integrated Asset

- Main in-game background: `assets/mars_ship_level_full.png`
- Size: `5120x850`
- Type: single continuous horizontal 2D side-scrolling background strip
- Style: high-quality pixel art, sci-fi post-apocalyptic Mars, abandoned spaceship interior into ruined Mars civilization
- Integration: loaded by `main.c` as the base background texture

## Source Section Assets

These generated source sections were copied into the project and stitched into the final strip:

- `assets/generated_background/mars_ship_level_source_left.png`
- `assets/generated_background/mars_ship_level_source_transition.png`
- `assets/generated_background/mars_ship_level_source_right.png`

## Aligned Layer Files

The project currently renders a base background plus optional overlay textures. The playable background is single-layer, and these aligned PNGs are present for future parallax work:

- `assets/mars_ship_level_sky.png`
- `assets/mars_ship_level_far.png`
- `assets/mars_ship_level_mid.png`
- `assets/mars_ship_level_near.png`
- `assets/mars_ship_level_foreground.png`

At the moment, the optional layer files are transparent placeholders so they do not obscure gameplay.

## Generator Prompt

Create one seamless ultra-wide horizontal high-quality pixel-art 2D side-scrolling platformer background strip, exact target dimensions 5120 x 850 pixels. It must be a continuous left-to-right game level backdrop, not poster art, not top-down, not isometric, not a collage. The player path is near the lower third, with huge vertical headroom and large readable shapes. The scene must feel spacious, cinematic, playable, and horizontally scrollable.

The strip has two major sections. The left section is an enormous abandoned crashed human spaceship interior on Mars: towering dark blue-gray metal walls, very high ceiling, huge broken windows showing Mars outside, massive support beams, industrial doors, broken control panels, hanging cables, pipes, flickering lights, dust in the air, damaged metal floor, debris, and old warning sign shapes. It must feel enormous, ancient, abandoned, and playable, not a cramped hallway.

The center is a clear transition through a huge broken hangar door, destroyed cargo bay, or cracked airlock. Dark ship interior remains on the left; Mars exterior ruins open on the right. Orange Mars light floods into the ship, with dust and sand blowing inward, jagged metal edges, and a broken ramp.

The right section is a wide open post-apocalyptic Mars civilization: red-orange dusty sky, distant ruined city skyline, broken colony domes, abandoned towers, crashed satellites, communication antennas, destroyed roads and bridges, rusty sci-fi structures, large distant mountains and cliffs, far dust storms, faint glowing remains of old technology, and small broken machine lights. The exterior should feel lonely, dead, epic, and much more open than the ship.

Game readability requirements: no characters, no enemies, no UI, no text labels, no watermark. Clear separation between decorative background and playable foreground path. Keep the lower player path readable and avoid noisy clutter where the player moves. Use large silhouettes, cinematic ambient lighting, crisp pixel-art edges, strong depth, and avoid blurry or photorealistic rendering.

## Notes

The built-in generator returned smaller section images, so the integrated asset was made by generating three section images and stitching them into the project's native `5120x850` background size. This preserves the existing `WORLD_W = 5120` and avoids changing collision/camera boundaries.
