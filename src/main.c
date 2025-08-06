#include <raymath.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <math.h>

#include "tile.h"
#include "raylib.h"

Color color_conv[4] = {
  {.r=230, .g=41 , .b=55 , .a=255},
  {.r=0  , .g=121, .b=241, .a=255},
  {.r=0  , .g=228, .b=48 , .a=255},
  {.r=253, .g=249, .b=0  , .a=255}
};

// typedef tile tile_grid[ (2*TILE_COUNT + 1) * (2*TILE_COUNT + 1)];

#define COS_2PI_OVER_12 0.86602540378444

const double radius = 60;
const double pad = 2;
const double grid_step = 2 * radius * COS_2PI_OVER_12;
const double arc_thickness = 4.0;
const double arc_pad = 4;

int64_t tile_at_pos(tile *tiles, int64_t x, int64_t y);
uint8_t tile_fits(tile *tiles, size_t tile_idx);
uint8_t find_solution(tile tiles[TILE_COUNT], size_t tile_count);
void DrawTile(tile t);
void DrawTiles(tile tiles[TILE_COUNT], size_t tile_count);
uint8_t verify_solution(tile tiles[TILE_COUNT], size_t tile_count);
Vector2 TileGetWorldPos(tile t);

#define CAMERA_SPEED 4.0

int main(int argc, char** argv)
{
  (void) argc, (void) argv;

  size_t difficulty = 12;
  printf("How many pieces do you want to solve for ? ");
  scanf("%zu", &difficulty);

  printf("%u\n", find_solution(tiles, difficulty));

  printf("is %s a solution\n", verify_solution(tiles, difficulty) ? "actually" : "not");

  const int screenWidth = 800, screenHeight = 400;

  InitWindow(screenWidth, screenHeight, "tantrix");

  SetTargetFPS(60);

  Camera2D camera = {0};
  camera.target = (Vector2){0, 0};
  camera.offset = (Vector2){screenWidth * 1.0 / 2, screenHeight * 1.0 / 2};
  camera.rotation = 0.0;
  camera.zoom = 1.0;

  while(!WindowShouldClose())
  {
    if (IsKeyDown(KEY_A)) camera.target.x -= CAMERA_SPEED;
    if (IsKeyDown(KEY_D)) camera.target.x += CAMERA_SPEED;
    if (IsKeyDown(KEY_W)) camera.target.y -= CAMERA_SPEED;
    if (IsKeyDown(KEY_S)) camera.target.y += CAMERA_SPEED;
    if (IsKeyDown(KEY_Q)) camera.rotation += CAMERA_SPEED;
    if (IsKeyDown(KEY_E)) camera.rotation -= CAMERA_SPEED;
    
    camera.zoom = expf(logf(camera.zoom) + (float)GetMouseWheelMove() * 0.1f);

    if (IsKeyPressed(KEY_R))
    {
      camera.rotation = 0;
      camera.zoom = 1.0;
    }

    BeginDrawing();
    ClearBackground(RAYWHITE);
    BeginMode2D(camera);

    DrawTiles(tiles, difficulty);
    
    EndMode2D();

    DrawRectangle(0, 0, 40, 20, color_conv[tiles[difficulty - 1].back]);
    EndDrawing();
  }

  CloseWindow();

  return 0;
}

/*
 * returns the index of the tile in tiles which pos is (x, y)
 * returns negative if there was no tile at pos
 */
int64_t tile_at_pos(tile *tiles, int64_t x, int64_t y)
{
  for (size_t i = 0; i < TILE_COUNT; ++i)
    if (tiles[i].x == x && tiles[i].y == y && tiles[i].used)
      return i;

  return -1;
}

uint8_t tile_fits(tile *tiles, size_t tile_idx)
{
  for (uint8_t side = 0; side < SIDE_COUNT; ++side)
  {
    const uint8_t symetric_side = (side + 3) % SIDE_COUNT;

    const int64_t x = tiles[tile_idx].x + adj[side].x;
    const int64_t y = tiles[tile_idx].y + adj[side].y;
    int64_t neighboor = tile_at_pos(tiles, x, y);
    if (neighboor < 0) continue; // there is no neighboor there : will always fit
    
    if (TILE_GET_COLOR(tiles[tile_idx], side) != TILE_GET_COLOR(tiles[neighboor], symetric_side))
      return 0;
  }
 
  return 1;
}

/*
 * returns non-zero uppon success
 */
uint8_t find_solution_inside_old(tile tiles[TILE_COUNT], size_t tile_count, size_t placed_tiles[TILE_COUNT], size_t placed_count)
{
  if (placed_count == tile_count)
    return verify_solution(tiles, tile_count);

  /*
   * first two for loop iterate over all the open side around the already placed tiles
   * last two iterate over all the possible tiles and their rotations which can fit
   */
  for (size_t i = 0; i < tile_count; ++i)
  {
    if (!tiles[i].used) continue;

    for (uint8_t side = 0; side < SIDE_COUNT; ++side)
    {
      const int64_t x = tiles[i].x + adj[side].x;
      const int64_t y = tiles[i].y + adj[side].y;
      int64_t neighboor = tile_at_pos(tiles, x, y);
      if (neighboor >= 0) continue; // there is already a tile there

      for (size_t tile = 0; tile < tile_count; ++tile)
      {
        if (tiles[tile].used) continue;

        tiles[tile].x = x;
        tiles[tile].y = y;
        tiles[tile].used = 1;
        placed_tiles[placed_count] = tile;

        for (uint8_t rot = 0; rot < SIDE_COUNT; ++rot)
        {
          tiles[tile].rotation = rot;

          if (!tile_fits(tiles, tile)) continue;

          if (find_solution_inside_old(tiles, tile_count, placed_tiles, placed_count + 1))
            return 1; // we found a solution
        }

        // reset all the tiles placed after
        for (size_t j = placed_count; j < tile_count; ++j)
        {
          tiles[j].used = 0;
          placed_tiles[j] = 0;
        }
      }
    }
  }

  return 0;
}

    /*
     * find the side(s) with the back color
     * return negative in case of failure : if no open side had color c
     * The only case where a tile might have two of its sides back colored and open is the first tile
     * we only need to find the first instance as the other will be checked later when the first one gets used by other piece
     */
int8_t find_colored_side(tile tiles[TILE_COUNT], size_t tile_idx, color c)
{
    for (uint8_t side = 0; side < SIDE_COUNT; ++side)
    {
      if (TILE_GET_COLOR(tiles[tile_idx], side) != c)
        continue;
      const int64_t x = tiles[tile_idx].x + adj[side].x;
      const int64_t y = tiles[tile_idx].y + adj[side].y;
      int64_t neighboor = tile_at_pos(tiles, x, y);
      if (neighboor < 0) // space is open
        return side;
    }

    return -1;
}


uint8_t find_solution_inside(tile tiles[TILE_COUNT], size_t tile_count, int64_t placed_tiles[TILE_COUNT], size_t placed_count)
{
  const color back = tiles[tile_count - 1].back;

  if (placed_count == tile_count)
    return verify_solution(tiles, tile_count);

  /*
   * first for loop iterate over all the open side around the already placed tiles
   * last two iterate over all the possible tiles and their rotations which can fit
   */
  for (size_t i = 0; i < tile_count; ++i)
  {
    if (!tiles[i].used) continue;

    int8_t side = find_colored_side(tiles, i, back);

    if (side < 0)
      continue; // this piece already has both its back colored side filled
    const int64_t x = tiles[i].x + adj[side].x;
    const int64_t y = tiles[i].y + adj[side].y;

    for (size_t tile = 0; tile < tile_count; ++tile)
    {
      if (tiles[tile].used) continue;

      if (placed_count == 1)
        printf("%zu\n", tile);

      tiles[tile].x = x;
      tiles[tile].y = y;
      tiles[tile].used = 1;
      placed_tiles[placed_count] = tile;

      // rot is where the back color is on tiles[tile]
      for (uint8_t rot = 0; rot < SIDE_COUNT; ++rot)
      {
        tiles[tile].rotation = 0;
        if (TILE_GET_COLOR(tiles[tile], rot) != back)
          continue;

        /*
         * we want to aligne the color at rot with (side + 3) % SIDE_COUNT
         * We hence need to rotate by (side + 3) % SIDE_COUNT - rot all mode SIDE_COUNT
         */
        tiles[tile].rotation = (side + 3 - rot + SIDE_COUNT) % SIDE_COUNT;
        if (!tile_fits(tiles, tile)) continue;

        if (find_solution_inside(tiles, tile_count, placed_tiles, placed_count + 1))
          return 1; // we found a solution
      }

      // reset all the tiles placed after
      for (size_t j = placed_count; j < tile_count; ++j)
      {
        if (placed_tiles[j] < 0) continue;

        tiles[placed_tiles[j]].used = 0;
        placed_tiles[j] = -1;
      }
    }
  }

  return 0;
}

/*
 * tries to place all of the tiles in `tiles` 
 * return non-zero uppon success
 */
uint8_t find_solution(tile tiles[TILE_COUNT], size_t tile_count)
{
  int64_t placed_tiles[TILE_COUNT] = {0};
  
  for (size_t i = 0; i < TILE_COUNT; ++i)
    placed_tiles[i] = -1;

  // preplace tile 0
  tiles[0].used = 1;
  tiles[0].x = 0;
  tiles[0].y = 0;
  tiles[0].rotation = 0;
  placed_tiles[0] = 0;

  return find_solution_inside(tiles, tile_count, placed_tiles, 1);
}

#define VERIFY_LOOP_START_TILE 0

uint8_t verify_solution(tile tiles[TILE_COUNT], size_t tile_count)
{
  // sanity check
  for (size_t i = 0; i < tile_count; ++i)
    if (!tile_fits(tiles, i)) return 0;

  // continuous check
  color back = tiles[tile_count - 1].back;
  uint8_t seen[TILE_COUNT] = {0};
  memset(seen, 0, sizeof(seen) / sizeof(*seen));

  int64_t curr = VERIFY_LOOP_START_TILE;
  seen[curr] = 1;

  uint8_t side, arrived_from;
  for (side = 0; side < SIDE_COUNT &&
      TILE_GET_COLOR(tiles[curr], side) != back; ++side) ;

  curr = tile_at_pos(tiles,
      tiles[curr].x + adj[side].x,
      tiles[curr].y + adj[side].y
      );
  if (curr < 0) return 0;
  arrived_from = (side + 3) % SIDE_COUNT;
  seen[curr] = 1;

  while (curr != VERIFY_LOOP_START_TILE)
  {
    for (side = 0; side < SIDE_COUNT; ++side)
      if (side != arrived_from && TILE_GET_COLOR(tiles[curr], side) == back)
        break;
    curr = tile_at_pos(tiles,
        tiles[curr].x + adj[side].x,
        tiles[curr].y + adj[side].y
        );
    if (curr < 0) return 0;
    arrived_from = (side + 3) % SIDE_COUNT;
    seen[curr] = 1;
  }

  for (size_t i = 0; i < tile_count; ++i)
    if (!seen[i]) return 0;

  return 1;
}


#define POS_PLUS_POLAR(posx, posy, r, theta) posx + (r) * cos((theta)), posy - (r) * sin((theta))
#define POS_PLUS_POLAR_X(pos, r, theta) POS_PLUS_POLAR(pos, r, theta)
#define POS_PLUS_EGAL_POLAR(posx, posy, r, theta) POS_PLUS_POLAR(posx = posx, posy = posy, r, theta)
#define VEC2_POS_PLUS_POLAR(pos, r, theta) \
  (Vector2) { POS_PLUS_POLAR(((pos).x), ((pos).y), r, theta) }
#define VEC2_POS_PLUS_POLAR_X(pos, r, theta) VEC2_POS_PLUS_POLAR((pos), r, theta)

void DrawPaddedRing(Vector2 center, double innerRadius, double outerRadius, double startAngle, double endAngle, double pad, int segments, Color ringColor, Color padColor)
{
  DrawRing(center, innerRadius - pad, outerRadius + pad, startAngle, endAngle, segments, padColor);
  DrawRing(center, innerRadius, outerRadius, startAngle, endAngle, segments, ringColor); // main ring

  return;
}

void DrawPaddedLineEx(Vector2 startPos, Vector2 endPos, double thick, double pad, Color color, Color padColor)
{
  DrawLineEx(startPos, endPos, thick + 2 * pad, padColor);
  DrawLineEx(startPos, endPos, thick, color); // main line
        
  return;
}

void DrawTile(tile t)
{
  const Vector2 center = TileGetWorldPos(t);
  DrawPoly(center, SIDE_COUNT, radius, 30, BLACK);

  // DrawCircleV(center, 5, PURPLE);

  for (uint8_t col = 0; col < COLOR_COUNT; ++col)
  {
    int8_t pos1 = -1, pos2 = -1;
    for (uint8_t side = 0; side < SIDE_COUNT; ++side)
    {
      if (TILE_GET_COLOR(t, side) == col)
      {
        if (pos1 < 0)
          pos1 = side;
        else
          // pos1 is already set
          pos2 = side;
      }
    }

    if ((pos1 < 0) ^ (pos2 < 0))
    {
      fprintf(stderr, "[ERROR] color %u found only once in tile!!!\n", col);
      return;
    }

    if (pos1 < 0 && pos2 < 0) continue; // color not found
    
    const uint8_t delta = pos2 - pos1; // pos2 > pos1 in the loop
    double angle = 90;
    switch (delta) {
      case 5:
        angle += 5 * 60;
        // fallthrough
      case 1:
        // smol one
        angle -= pos2 * 60;

        const Vector2 smol_center = VEC2_POS_PLUS_POLAR(center, radius, angle * DEG2RAD);

        // DrawCircleV(arc_center, 5, MAGENTA);

        DrawPaddedRing(smol_center,
            radius / 2 - arc_thickness,
            radius / 2 + arc_thickness,
            -angle + 120,
            -angle + 240,
            arc_pad,
            100,
            color_conv[col],
            BLACK);
       break; 

      case 4:
       angle += 30 - pos1 * 60;
       angle += -30 + pos2 * 60; // compensate for the fallthrough
       // fallthrough
      case 2:
       angle -= -30 + pos2 * 60;
       // big one

       const Vector2 big_center = VEC2_POS_PLUS_POLAR(center, grid_step, angle * DEG2RAD);

       DrawPaddedRing(big_center,
           3.0 / 2.0 * radius - arc_thickness,
           3.0 / 2.0 * radius + arc_thickness,
           -angle - 210, -angle - 150,
           arc_pad,
           100, color_conv[col], BLACK);
       break;
     
      case 3:
       // line
       angle = 60 - pos1 * 60;

       DrawPaddedLineEx(
           VEC2_POS_PLUS_POLAR(center, grid_step / 2, angle * DEG2RAD),
           VEC2_POS_PLUS_POLAR(center, grid_step / 2, (angle + 180) * DEG2RAD),
           2 * arc_thickness, arc_pad,
           color_conv[col], BLACK);
       break;

     default:
       fprintf(stderr, "[UNREACHABLE] delta did not match any know value, got: %u!!\n", delta);
       return;
      }
  }

  return;
}

void DrawTiles(tile tiles[TILE_COUNT], size_t tile_count)
{
  for (size_t i = 0; i < tile_count; ++i)
    if (tiles[i].used)
      DrawTile(tiles[i]);
  
  return;
}

Vector2 TileGetWorldPos(tile t)
{
  const double radius = 60;
  const double grid_step = 2 * radius * cos (2 * PI / 12);

  return (Vector2){
    .x=t.x * (grid_step + pad) 
      + t.y * (grid_step + pad) * cos(2 * PI / 6),
    .y=-t.y * (grid_step + pad) * sin(2 * PI / 6)
  };
}
