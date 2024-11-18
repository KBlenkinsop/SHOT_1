#pragma once

#include "pigeon/gfx/sprite_batch.h" // for pigeon::gfx::sprite_batch
#include "pigeon/gfx/spritesheet.h"  // for pigeon::gfx::spritesheet
#include "cuckoo/core/asserts.h"    // for cuckoo assert
#include "constants.h"              // for object_type_t, object_id_t...
#include "extra/utility.h"          // for vector4, random_getf

#include <vector>                   // for std::vector



// TILE NORMAL

/// @brief consumed when the player touches it
//struct tile_normal_t
//{
//public:
//  tile_normal_t ();
//
//  void update(double elapsed, pigeon::gfx::spritesheet spritesheet) override;
//  void render(pigeon::gfx::sprite_batch& sprite_batch,
//    pigeon::gfx::spritesheet spritesheet) override;
//
//  void on_collision(object_type_t other_type, void* other_data, pigeon::gfx::spritesheet spritesheet) override;
//  object_id_t get_id() const override;
//
//  bool needs_replacing() const override;
//
//public:
//  vector4 position;
//  vector4 direction;
//  float angle_radians;
//private:
//  bool is_eaten;
//};
// GENERAL

struct tiles_t
{
  //std::vector <tile_normal_t> data;

    void initialise_tile(int index);

    void update(double elapsed)
    {
        for (int i = 0; i < NUM_TILES; ++i)
        {
            position[i].x += direction[i].x * TILE_SPEED_MOVEMENT * elapsed;
            position[i].y += direction[i].y * TILE_SPEED_MOVEMENT * elapsed;

            angle_radians[i] += (float)TILE_SPEED_ROTATION * elapsed;

            float const angle_limit = cuckoo::maths::two_pi <float>();
            angle_radians[i] = cuckoo::maths::mod(angle_radians[i], angle_limit);
        }
    }

    void render(pigeon::gfx::sprite_batch& spritebatch, pigeon::gfx::spritesheet& spritesheet); //spritesheet 
  

    void on_collision(object_type_t other_type, void* other_data, pigeon::gfx::spritesheet spritesheet, int index);

    object_id_t get_id() const ;

    bool needs_replacing(int index);


    vector4 position[NUM_TILES];
    vector4 direction[NUM_TILES];
    float angle_radians[NUM_TILES];
private:
    bool is_eaten[NUM_TILES];

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
