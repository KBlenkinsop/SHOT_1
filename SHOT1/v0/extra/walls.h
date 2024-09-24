#pragma once

#include "pigeon/gfx/sprite_batch.h" // for pigeon::gfx::sprite_batch
#include "pigeon/gfx/spritesheet.h"  // for pigeon::gfx::spritesheet

#include "../constants.h"            // for object_id_t, object_type_t...
#include "utility.h"                 // for vector4

#include <list>                      // for std::list
#include <vector>                    // for


struct wall_t
{
public:
  wall_t () = delete;
  wall_t (double size, vector4 position, object_id_t id);

  void render (pigeon::gfx::sprite_batch& sprite_batch,
    pigeon::gfx::spritesheet spritesheet);

  /// @brief the wall has collided with something
  /// check what type of object it is and resolve the collision appropriately
  /// @param other_type the identifier of the other object
  /// @param other_data pointer to some data, could be a tile, a player, or anything!
  /// @param spritesheet spritesheet data, required to get size of other object
  void on_collision (object_type_t other_type, void* other_data, pigeon::gfx::spritesheet spritesheet);
  object_id_t get_id () const;


public:
  double size; // x & y dimension
  vector4 position;


private:
  object_id_t id;
};


struct walls_t
{
  std::list <wall_t> data;
};


/// @brief pre game loop walls set up code
walls_t initialise_walls (vector4 screen_dim);

/// @brief post game loop walls tear down code
void release_walls (walls_t& walls);
