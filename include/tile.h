#ifndef __TILE__
#define __TILE__

#include <stdint.h>

typedef enum color_e
{
  RED,
  BLUE,
  GREEN,
  YELLOW,
  COLOR_COUNT
} color;

typedef struct {
  int64_t x, y;
} pos;

extern const pos adj[];

#define SIDE_COUNT 6

typedef struct tile_s
{
  color sides[SIDE_COUNT];
  color back;
  int64_t x, y;
  uint8_t rotation;
  /*
   * rotation used as an offset in sides:
   *   |> 0 is the base
   *   |> 1 is rotate clockwise by 60°
   *   |> 2 is rotate clockwise by 120°
   *   ...
   */
  uint8_t used;
} tile;

#define TILE_GET_COLOR(tile, side) ((tile).sides[((side) - tile.rotation + SIDE_COUNT) % SIDE_COUNT])

#define TILE_COUNT 30

extern tile tiles[TILE_COUNT];

#endif // __TILE__
