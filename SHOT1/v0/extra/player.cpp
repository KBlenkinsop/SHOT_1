#include "player.h"

#include "pigeon/systems/input/input.h"

#include "../tiles.h" // for tile_t
#include "walls.h"    // for wall_t

#include <algorithm>  // for


static void collision_resolve_player_wall (pigeon::gfx::spritesheet spritesheet, player_t* player, wall_t* wall)
{
  // get the tiles spritesheet rect (position, width & height)
  // this is to get the tile's size as required by the following code
  texture_rect const* tex_rect = get_player_texture_rect (spritesheet, player->get_id ());

  // position response
  if (wall->get_id () == WALL_ID_LEFT)
  {
    player->position.x = wall->position.x + wall->size / 2.0;
    player->position.x += (double)tex_rect->width / 2.0;
  }
  else if (wall->get_id () == WALL_ID_RIGHT)
  {
    player->position.x = wall->position.x - wall->size / 2.0;
    player->position.x -= (double)tex_rect->width / 2.0;
  }
  else if (wall->get_id () == WALL_ID_TOP)
  {
    player->position.y = wall->position.y - wall->size / 2.0;
    player->position.y -= (double)tex_rect->height / 2.0;
  }
  else if (wall->get_id () == WALL_ID_BOTTOM)
  {
    player->position.y = wall->position.y + wall->size / 2.0;
    player->position.y += (double)tex_rect->height / 2.0;
  }
}


// PLAYER

player_t::player_t (double position_x, double position_y, unsigned in_num_points)
  : position { position_x, position_y, 0.0, 0.0 }
  , new_player_id {}
  , num_points { in_num_points }
{
}


// PLAYER NORMAL

player_normal_t::player_normal_t (double position_x, double position_y, unsigned in_num_points)
  : player_t (position_x, position_y, in_num_points)
{
}

void player_normal_t::update (double elapsed, pigeon::gfx::spritesheet spritesheet)
{
  // update position
  if (pigeon::input::is_key_down (cuckoo::input::keyboard_key_flag::LEFT) || pigeon::input::is_down (0u, cuckoo::input::controller_button_flag::LEFT))
  {
    position.x -= PLAYER_SPEED * PLAYER_SPEED_MULTIPLIER_FAST * elapsed;
  }
  if (pigeon::input::is_key_down (cuckoo::input::keyboard_key_flag::RIGHT) || pigeon::input::is_down (0u, cuckoo::input::controller_button_flag::RIGHT))
  {
    position.x += PLAYER_SPEED * PLAYER_SPEED_MULTIPLIER_FAST * elapsed;
  }
  if (pigeon::input::is_key_down (cuckoo::input::keyboard_key_flag::UP) || pigeon::input::is_down (0u, cuckoo::input::controller_button_flag::UP))
  {
    position.y += PLAYER_SPEED * PLAYER_SPEED_MULTIPLIER_FAST * elapsed;
  }
  if (pigeon::input::is_key_down (cuckoo::input::keyboard_key_flag::DOWN) || pigeon::input::is_down (0u, cuckoo::input::controller_button_flag::DOWN))
  {
    position.y -= PLAYER_SPEED * PLAYER_SPEED_MULTIPLIER_FAST * elapsed;
  }
}
void player_normal_t::render (pigeon::gfx::sprite_batch& sprite_batch,
  pigeon::gfx::spritesheet spritesheet)
{
  texture_rect const* tex_rect = get_player_texture_rect (spritesheet, get_id ());
  CUCKOO_ASSERT (tex_rect);

  sprite_batch.draw (*tex_rect,
    (float)position.x, (float)position.y,
    0.f,
    0.f, 0.f,
    (float)tex_rect->width, (float)tex_rect->height);
}

void player_normal_t::on_collision (object_type_t other_type, void* other_data, pigeon::gfx::spritesheet spritesheet)
{
  if (other_type == WALL_TYPE)
  {
    // 'other_data' is a wall of some kind

    // player has hit a wall, make the appropriate changes to player as a result of it
    collision_resolve_player_wall (spritesheet, this, (wall_t*)other_data);
  }
  else if (other_type == TILE_TYPE)
  {
    // 'other_data' is a tile of some kind

    // this tile has hit a tile, make the appropriate changes to this tile as a result of it
    tile_t* tile = (tile_t*)other_data;
    if (tile->get_id () == TILE_ID_NORMAL)
    {
      ++num_points;
      if (num_points % PLAYER_FAST_POINTS_SWITCH == 0u)
      {
        new_player_id = PLAYER_ID_FAST;
      }
    }
  }
  else if (other_type == PLAYER_TYPE)
  {
  }
}
object_id_t player_normal_t::get_id () const { return PLAYER_ID_NORMAL; }


// PLAYER FAST

player_fast_t::player_fast_t (double position_x, double position_y, unsigned in_num_points)
  : player_t (position_x, position_y, in_num_points)
  , lifetime (PLAYER_FAST_LIFETIME)
{
}

void player_fast_t::update(double elapsed, pigeon::gfx::spritesheet spritesheet)
{
  // update position
  if (pigeon::input::is_key_down (cuckoo::input::keyboard_key_flag::LEFT) || pigeon::input::is_down (0u, cuckoo::input::controller_button_flag::LEFT))
  {
    position.x -= PLAYER_SPEED * PLAYER_SPEED_MULTIPLIER_FAST * elapsed;
  }
  if (pigeon::input::is_key_down (cuckoo::input::keyboard_key_flag::RIGHT) || pigeon::input::is_down (0u, cuckoo::input::controller_button_flag::RIGHT))
  {
    position.x += PLAYER_SPEED * PLAYER_SPEED_MULTIPLIER_FAST * elapsed;
  }
  if (pigeon::input::is_key_down (cuckoo::input::keyboard_key_flag::UP) || pigeon::input::is_down (0u, cuckoo::input::controller_button_flag::UP))
  {
    position.y += PLAYER_SPEED * PLAYER_SPEED_MULTIPLIER_FAST * elapsed;
  }
  if (pigeon::input::is_key_down (cuckoo::input::keyboard_key_flag::DOWN) || pigeon::input::is_down (0u, cuckoo::input::controller_button_flag::DOWN))
  {
    position.y -= PLAYER_SPEED * PLAYER_SPEED_MULTIPLIER_FAST * elapsed;
  }

  // update lifetime: player_fast_t only
  lifetime -= elapsed;
  if (lifetime < 0.0)
  {
    new_player_id = PLAYER_ID_NORMAL;
  }
}
void player_fast_t::render (pigeon::gfx::sprite_batch& sprite_batch,
  pigeon::gfx::spritesheet spritesheet)
{
  texture_rect const* tex_rect = get_player_texture_rect (spritesheet, get_id ());
  CUCKOO_ASSERT (tex_rect);

  sprite_batch.draw (*tex_rect,
    (float)position.x, (float)position.y,
    0.f,
    0.f, 0.f,
    (float)tex_rect->width, (float)tex_rect->height);
}

void player_fast_t::on_collision (object_type_t other_type, void* other_data, pigeon::gfx::spritesheet spritesheet)
{
  if (other_type == WALL_TYPE)
  {
    // 'other_data' is a wall of some kind

    // player has hit a wall, make the appropriate changes to player as a result of it
    collision_resolve_player_wall (spritesheet, this, (wall_t*)other_data);
  }
  else if (other_type == TILE_TYPE)
  {
    // 'other_data' is a tile of some kind

    // this tile has hit a tile, make the appropriate changes to this tile as a result of it
    tile_t* tile = (tile_t*)other_data;
    if (tile->get_id () == TILE_ID_NORMAL)
    {
      ++num_points;
    }
  }
  else if (other_type == PLAYER_TYPE)
  {
  }
}
object_id_t player_fast_t::get_id () const { return PLAYER_ID_FAST; }


// GENERAL

void initialise_player (player_t*& player)
{
  // hhhmmm, what else could go here?

  player = new player_normal_t (0.0, 0.0, 0u);
}

void check_player_needs_replacing (player_t*& player)
{
  if (player->new_player_id == PLAYER_ID_FAST)
  {
    vector4 const old_position = player->position;
    unsigned const old_num_pints = player->num_points;
    delete player;
    player = new player_fast_t (old_position.x, old_position.y, old_num_pints);
  }
  else if (player->new_player_id == PLAYER_ID_NORMAL)
  {
    vector4 const old_position = player->position;
    unsigned const old_num_pints = player->num_points;
    delete player;
    player = new player_normal_t (old_position.x, old_position.y, old_num_pints);
  }
}

void release_player (player_t*& player)
{
  delete player;
  player = nullptr;
}


texture_rect const* get_player_texture_rect (pigeon::gfx::spritesheet const& spritesheet, object_id_t id)
{
  texture_rect const* rect = nullptr;

  if (id == PLAYER_ID_FAST)
  {
    rect = spritesheet.get_sprite_info ("player_1.png");
  }
  else if (id == PLAYER_ID_NORMAL)
  {
    rect = spritesheet.get_sprite_info ("player_0.png");
  }

  return rect;
}
