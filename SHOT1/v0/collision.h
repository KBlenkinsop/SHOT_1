#pragma once

#include "pigeon/gfx/spritesheet.h"  // for pigeon::gfx::spritesheet


struct player_t; // forward declare
struct tiles_t;
struct walls_t;


/// @brief 1. find overlapping game objects (player, tiles, walls)
/// 2. resolve the collisions
/// - i.e. make the 2 overlapping objects respond appropriately to hitting the other
/// - this will differ for each object, i.e. the wall doesn't do anything if a tile hits it,
///     but the tile will have its velocity reflected.
void resolve_collisions (pigeon::gfx::spritesheet spritesheet,
  player_t& p,
  tiles_t& tiles,
  walls_t& walls);
