#include "tile.h"

tile tiles[TILE_COUNT] = {
  {.sides={RED, YELLOW, YELLOW, BLUE, RED, BLUE}, .back=YELLOW},
  {.sides={RED, BLUE, YELLOW, YELLOW, BLUE, RED}, .back=YELLOW},
  {.sides={BLUE, BLUE, YELLOW, YELLOW, RED, RED}, .back=YELLOW},
  {.sides={YELLOW, BLUE, YELLOW, RED, BLUE, RED}, .back=RED},
  {.sides={RED, BLUE, BLUE, RED, YELLOW, YELLOW}, .back=RED},
  {.sides={YELLOW, RED, BLUE, YELLOW, BLUE, RED}, .back=BLUE},
  {.sides={RED, BLUE, BLUE, YELLOW, RED, YELLOW}, .back=BLUE},
  {.sides={YELLOW, BLUE, BLUE, RED, YELLOW, RED}, .back=BLUE},
  {.sides={RED, YELLOW, BLUE, RED, BLUE, YELLOW}, .back=YELLOW},
  {.sides={RED, BLUE, RED, BLUE, YELLOW, YELLOW}, .back=RED},
  {.sides={BLUE, YELLOW, BLUE, YELLOW, RED, RED}, .back=RED},
  {.sides={YELLOW, BLUE, YELLOW, BLUE, RED, RED}, .back=YELLOW},
  {.sides={YELLOW, RED, RED, YELLOW, BLUE, BLUE}, .back=BLUE},
  {.sides={RED, RED, YELLOW, YELLOW, BLUE, BLUE}, .back=BLUE},
  {.sides={RED, GREEN, GREEN, RED, YELLOW, YELLOW}, .back=RED},
  {.sides={YELLOW, GREEN, GREEN, YELLOW, RED, RED}, .back=RED},
  {.sides={GREEN, RED, GREEN, RED, YELLOW, YELLOW}, .back=YELLOW},
  {.sides={RED, GREEN, RED, GREEN, YELLOW, YELLOW}, .back=RED},
  {.sides={GREEN, YELLOW, GREEN, YELLOW, RED, RED}, .back=RED},
  {.sides={YELLOW, GREEN, YELLOW, GREEN, RED, RED}, .back=YELLOW},
  {.sides={YELLOW, YELLOW, GREEN, GREEN, RED, RED}, .back=YELLOW},
  {.sides={GREEN, RED, RED, GREEN, YELLOW, YELLOW}, .back=YELLOW},
  {.sides={GREEN, GREEN, YELLOW, YELLOW, RED, RED}, .back=YELLOW},
  {.sides={BLUE, RED, RED, BLUE, GREEN, GREEN}, .back=RED},
  {.sides={BLUE, BLUE, GREEN, GREEN, RED, RED}, .back=RED},
  {.sides={GREEN, BLUE, BLUE, GREEN, RED, RED}, .back=RED},
  {.sides={BLUE, GREEN, BLUE, GREEN, RED, RED}, .back=RED},
  {.sides={GREEN, GREEN, BLUE, BLUE, RED, RED}, .back=RED},
  {.sides={GREEN, BLUE, GREEN, BLUE, RED, RED}, .back=RED},
  {.sides={RED, BLUE, BLUE, RED, GREEN, GREEN}, .back=RED},
};

const pos adj[] = {
  {+0, +1},
  {+1, +0},
  {+1, -1},
  {+0, -1},
  {-1, +0},
  {-1, +1}
};
