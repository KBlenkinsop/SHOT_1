#include "collision.h"

#include "cuckoo/time/time.h" // for cuckoo::sleep

#include "tiles.h"            // for tile_t
#include "extra/player.h"     // for player_t
#include "extra/walls.h"      // for wall_t


/// @brief check whether 2 AABBs (axis-aligned bounding box) are overlapping
/// assumes both positions are at the centre of the AABB
/// @return true if overalapping, otherwise false
static bool is_overlapping (double lhs_position_x, double lhs_position_y, double lhs_width, double lhs_height,
  double rhs_position_x, double rhs_position_y, double rhs_width, double rhs_height)
{
  double const overlap = 4.f; // allow objects to overlap by this amount

  // get left and right boundaries of lhs AABB
  double const lhs_bound_left  = lhs_position_x - (lhs_width - overlap) / 2.0;
  double const lhs_bound_right = lhs_position_x + (lhs_width - overlap) / 2.0;

  // get bottom and top boundaries of lhs AABB
  double const lhs_bound_bottom = lhs_position_y - (lhs_height - overlap) / 2.0;
  double const lhs_bound_top    = lhs_position_y + (lhs_height - overlap) / 2.0;

  // get left and right boundaries of rhs AABB
  double const rhs_bound_left  = rhs_position_x - (rhs_width - overlap) / 2.0;
  double const rhs_bound_right = rhs_position_x + (rhs_width - overlap) / 2.0;

  // get bottom and top boundaries of rhs AABB
  double const rhs_bound_bottom = rhs_position_y - (rhs_height - overlap) / 2.0;
  double const rhs_bound_top    = rhs_position_y + (rhs_height - overlap) / 2.0;

  return lhs_bound_left   < rhs_bound_right
    && lhs_bound_right  > rhs_bound_left
    && lhs_bound_bottom < rhs_bound_top
    && lhs_bound_top    > rhs_bound_bottom;


  // Hint: can you spot any wasted computations above?
}

void resolve_collisions (pigeon::gfx::spritesheet spritesheet,
  player_t& p,
  tiles_t& tiles,
  walls_t& walls)
{
  // lhs = left hand side
  // rhs = right hand side


  /// Collison detection/resolution strategy:
  /// e.g. for PLAYER v TILE
  ///
  /// lhs = player
  /// rhs = individual tile
  ///
  /// 1. Iterate over all tiles.
  /// 2. Check whether this player-tile pair overlap with eachother.
  /// 3. Once we find an overlapping player-tile pair:
  ///   Call on_collision for each object
  ///   a. Tell player that it has collided with a TILE_TYPE object.
  ///      Provide the player a way to access that tile's data.
  ///      Currently, this is via a pointer to the individual tile,
  ///      but after DOD, there will be such thing as a pointer to a single tile...
  ///      Finally, pass the spritesheet so the player can get the tile's size. (All tile's have the same size, this seems a bit inefficient...)
  ///   b. Tell tile that it has collided with a PLAYER_TYPE object...
  ///
  /// The 'on_collision' function 'resolves' the actual collision.
  /// The 'on_collision' function only resolves the collison for the object it is called on,
  /// so must be called twice, once on 'lhs' and again on 'rhs'.
  ///
  /// This same strategy in used in subsequent 'OBJECT A v OBJECT B' collision detection/resolution strategies.


  // PLAYER v TILE
  // lhs = player
  // rhs = tile
  {
    player_t& lhs = p;
    for (auto rhs_it = tiles.data.begin (); rhs_it != tiles.data.end (); rhs_it++) // for each tile
    {
      tile_t& rhs = *rhs_it;

      // get size of player and tile via their spritesheet size
      texture_rect const* lhs_rect = get_player_texture_rect (spritesheet, lhs.get_id ());
      texture_rect const* rhs_rect = get_tile_texture_rect (spritesheet, rhs.get_id ());

      if (is_overlapping (lhs.position.x, lhs.position.y, lhs_rect->width, lhs_rect->height,
        rhs.position.x, rhs.position.y, rhs_rect->width, rhs_rect->height))
      {
        lhs.on_collision (TILE_TYPE,   (void*)&rhs, spritesheet); // tell player it hit a tile
        rhs.on_collision (PLAYER_TYPE, (void*)&lhs, spritesheet); // tell tile it hit the player
      }
    }
  }


  // PLAYER v WALL
  // lhs = player
  // rhs = wall
  // after 'DOD'ing the tiles this code will need altering to reflect the new way
  // in which we get tile data, i.e. via an index (0 --> NUM_TILES - 1)
  // for convenience, feel free to comment out this code whilst testing
  {
    player_t* lhs = &p;
    for (auto rhs_it = walls.data.begin (); rhs_it != walls.data.end (); rhs_it++) // for each wall
    {
      wall_t* rhs = &(*rhs_it);

      // get size of player via their spritesheet size
      texture_rect const* lhs_rect = get_player_texture_rect (spritesheet, lhs->get_id ());

      if (is_overlapping (lhs->position.x, lhs->position.y, lhs_rect->width, lhs_rect->height,
        rhs->position.x, rhs->position.y, rhs->size, rhs->size))
      {
        lhs->on_collision (WALL_TYPE,   (void*)rhs, spritesheet); // tell player it hit a wall
        rhs->on_collision (PLAYER_TYPE, (void*)lhs, spritesheet); // tell wall the player hit it
      }
    }
  }


  /// TILE v TILE
  ///
  /// 'tile v tile' collisions is a big, time consuming CPU task!
  /// Remember Big O?
  /// Big O helps us describe how the amount of 'work' increases as we increase the number of inputs.
  /// 'vanilla' collision detection is a O((n*(n-1))/2) algorithm
  /// (but only if you are clever with looping, otherwise it deteoriates to O(n^2)!!!)
  /// At small values this can be acceptable (remember human time vs. reward & Ahmdal's Law),
  /// but we have 1,024 tiles...
  /// That means 523,776 collision checks (and then we still need to resolve any actual collisions!)
  /// and this increases exponentially when we increase n (the number of tiles.)
  ///
  /// If you would like the 'vanilla' code to check for
  /// and resolve 'tile vs tile' collisions, talk to Andy.
  /// (Remember, Andy will only supply you with the poorly performing
  /// code you have come to expect in the starter code.)
  ///
  /// Want to attempt to optimise that algorithm?
  /// Andy is going to introduce you to algorithms that help
  /// reduce the amount of work needed to detect collisions.


  // TILE v WALL
  // lhs = tile
  // rhs = wall
  //
  // after 'DOD'ing the tiles this code will need altering to reflect the new way
  // in which we get tile data, i.e. via an index (0 --> NUM_TILES - 1)
  // for convenience, feel free to comment out this code whilst testing
  // however, PLEASE COME BACK TO UPDATE THIS CODE!!!
  // if the tiles don't collide with the walls they will just fly away
  // and that would be much of a demo then...
  for (auto lhs_it = tiles.data.begin (); lhs_it != tiles.data.end (); lhs_it++) // for each tile
  {
    tile_t& lhs = *lhs_it;

    // get size of tile via their spritesheet size
    texture_rect const* lhs_rect = get_tile_texture_rect (spritesheet, lhs.get_id ());

    for (auto rhs_it = walls.data.begin (); rhs_it != walls.data.end (); rhs_it++) // for each wall
    {
      wall_t& rhs = *rhs_it;

      if (is_overlapping (lhs.position.x, lhs.position.y, lhs_rect->width, lhs_rect->height,
        rhs.position.x, rhs.position.y, rhs.size, rhs.size))
      {
        lhs.on_collision (WALL_TYPE, (void*)&rhs, spritesheet); // tell tile it hit a wall
        rhs.on_collision (TILE_TYPE, (void*)&lhs, spritesheet); // tell wall a tile hit it
      }
    }
  }


  // Hint: that is a lot of getting object texture_rects...
  // Do we really need all that info repeatedly every frame?
  // Plus, how do we get that texture_rect? Is the underlying function quick?
}
