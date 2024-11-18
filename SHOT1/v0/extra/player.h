#pragma once

#include "pigeon/gfx/sprite_batch.h" // for pigeon::gfx::sprite_batch
#include "pigeon/gfx/spritesheet.h"  // for pigeon::gfx::spritesheet

#include "../constants.h"            // for object_type_t, object_id_t...
#include "utility.h"                 // for vector4


// PLAYER

struct player_t
{
public:
  player_t () = delete;
  player_t (double position_x, double position_y, unsigned in_num_points);
  virtual ~player_t () = default;

  virtual void update (double elapsed, pigeon::gfx::spritesheet spritesheet) = 0;
  virtual void render (pigeon::gfx::sprite_batch& sprite_batch,
    pigeon::gfx::spritesheet spritesheet) = 0;

  /// @brief the player has collided with something
  /// check what type of object it is and resolve the collision appropriately
  /// @param other_type the identifier of the other object
  /// @param other_data pointer to some data, could be a tile or wall, or anything!
  /// @param spritesheet spritesheet data, required to get size of other object
  virtual void on_collision (object_type_t other_type, void* other_data, pigeon::gfx::spritesheet spritesheet, int index) = 0;
  virtual object_id_t get_id () const = 0;


public:
  vector4 position;
  object_id_t new_player_id;
  unsigned num_points;
};


// PLAYER NORMAL

/// @brief standard user controlled player
struct player_normal_t : public player_t
{
public:
  player_normal_t () = delete;
  player_normal_t (double position_x, double position_y, unsigned in_num_points);

  void update (double elapsed, pigeon::gfx::spritesheet spritesheet) override;
  void render (pigeon::gfx::sprite_batch& sprite_batch,
    pigeon::gfx::spritesheet spritesheet) override;

  void on_collision (object_type_t other_type, void* other_data, pigeon::gfx::spritesheet spritesheet, int index) override;
  object_id_t get_id () const override;
};


// PLAYER FAST

/// @brief has a larger area for consuming tiles than player_normal
/// reverts back to player_normal after { PLAYER_FAST_LIFETIME } seconds
/// faster movement speed than player_normal
struct player_fast_t : public player_t
{
public:
  player_fast_t () = delete;
  player_fast_t (double position_x, double position_y, unsigned in_num_points);

  void update (double elapsed, pigeon::gfx::spritesheet spritesheet) override;
  void render (pigeon::gfx::sprite_batch& sprite_batch,
    pigeon::gfx::spritesheet spritesheet) override;

  void on_collision (object_type_t other_type, void* other_data, pigeon::gfx::spritesheet spritesheet, int index) override;
  object_id_t get_id () const override;


private:
  double lifetime;
};


// GENERAL

/// @brief pre game loop player set up code
void initialise_player (player_t*& player);

/// @brief check if player needs replacing
/// @param player reference to a pointer - player to check for replacement
void check_player_needs_replacing (player_t*& player);

/// @brief post game loop player tear down code
void release_player (player_t*& player);


/// @brief search the spritesheet for the sub-sprite associated with a particular type of plater
/// NOTE: this app uses the size of the sub-sprite as the size of the object in the game world.
/// @return a pointer to the texture_rect of the object's sub-sprite on the spritesheet
texture_rect const* get_player_texture_rect (pigeon::gfx::spritesheet const& spritesheet, object_id_t id);
