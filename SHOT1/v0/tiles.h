#pragma once

#include "pigeon/gfx/sprite_batch.h" // for pigeon::gfx::sprite_batch
#include "pigeon/gfx/spritesheet.h"  // for pigeon::gfx::spritesheet

#include "constants.h"              // for object_type_t, object_id_t...
#include "extra/utility.h"          // for vector4, random_getf

#include <vector>                   // for std::vector


// TILE

struct tile_t
{
public:
  tile_t ();
  virtual ~tile_t () = default;

  virtual void update (double elapsed, pigeon::gfx::spritesheet spritesheet) = 0;
  virtual void render (pigeon::gfx::sprite_batch& sprite_batch,
    pigeon::gfx::spritesheet spritesheet) = 0;

  /// @brief the tile has collided with something
  /// check what type of object it is and resolve the collision appropriately
  /// @param other_type the identifier of the other object
  /// @param other_data pointer to some data, could be another tile, a wall, or anything!
  /// @param spritesheet spritesheet data, required to get size of other object
  virtual void on_collision (object_type_t other_type, void* other_data, pigeon::gfx::spritesheet spritesheet) = 0;
  virtual object_id_t get_id () const = 0;

  virtual bool needs_replacing () const = 0;


public:
  vector4 position;
  vector4 direction;
  float angle_radians;
};


// TILE NORMAL

/// @brief consumed when the player touches it
struct tile_normal_t : public tile_t
{
public:
  tile_normal_t ();

  void update (double elapsed, pigeon::gfx::spritesheet spritesheet) override;
  void render (pigeon::gfx::sprite_batch& sprite_batch,
    pigeon::gfx::spritesheet spritesheet) override;

  void on_collision (object_type_t other_type, void* other_data, pigeon::gfx::spritesheet spritesheet) override;
  object_id_t get_id () const override;

  bool needs_replacing () const override;


private:
  bool is_eaten;
};



// GENERAL

struct tiles_t
{
  std::vector <tile_normal_t> data;
};


/// @brief pre game loop tiles set up code
void initialise_tiles (tiles_t& tiles);

/// @brief remove 'expired' tiles, e.g. eaten by player, lifetime has expired
/// replace removed tiles with new ones
/// the game requires that there are always { NUM_TILES } active
tiles_t replace_expired_tiles (tiles_t tiles);

/// @brief post game loop tiles tear down code
void release_tiles (tiles_t& tiles);


/// @brief search the spritesheet for the sub-sprite associated with a particular type of tile
/// NOTE: this app uses the size of the sub-sprite as the size of the object in the game world.
/// @return a pointer to the texture_rect of the object's sub-sprite on the spritesheet
texture_rect const* get_tile_texture_rect (pigeon::gfx::spritesheet& spritesheet, object_id_t id);
