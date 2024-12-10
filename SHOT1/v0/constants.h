#pragma once

#include "cuckoo/maths/maths.h" // for cuckoo::maths::two_pi <...> ()

//#include <string>               // for std::string :[


////////////////////////////////
//// DO NOT EDIT/DELETE >>> ////
////////////////////////////////

unsigned const SCREEN_WIDTH = 1280u;
unsigned const SCREEN_HEIGHT = 720u;

////////////////////////////////
//// <<< DO NOT EDIT/DELETE ////
////////////////////////////////



// player 
double const PLAYER_SPEED = 300.0; // How long, in seconds, the player stays as a 'fast' player before reverting back to a 'normal' player.
double const PLAYER_FAST_LIFETIME = 10.0; // When player->num_points is a multiple of this number, a 'normal' player will become a 'fast' player.
unsigned const PLAYER_FAST_POINTS_SWITCH = 200u;
double const PLAYER_SPEED_MULTIPLIER_NORMAL = 1.0;// Percentage factor of 'PLAYER_SPEED' player moves at whilst a 'normal' player.
double const PLAYER_SPEED_MULTIPLIER_FAST = 1.6;// Percentage factor of 'PLAYER_SPEED' player moves at whilst a 'fast' player.



// tiles
unsigned const NUM_TILES = 1u << 10;// REMEMBER, 'NUM_TILES' MUST BE '1u << 10' WHEN YOU SUBMIT!!!
double const TILE_SPEED_MOVEMENT = 100.0;
double const TILE_SPEED_ROTATION = cuckoo::maths::two_pi<double>() * 2.0;// Rotation speed of all tile types, in radians, per second.



// With the following values,
// the value/way the following are represented may well be completely changed during your optimisation process...
// The concept of an object having a 'type' and 'id' must remain though!


// object types
using object_type_t = int;//string veiw?
object_type_t const PLAYER_TYPE (0);
object_type_t const TILE_TYPE (1);
object_type_t const WALL_TYPE (2);


using object_id_t = int;
// player
object_id_t const PLAYER_ID_NORMAL (3);
object_id_t const PLAYER_ID_FAST (4);


// tiles
object_id_t const TILE_ID_NORMAL (5);


// walls
object_id_t const WALL_ID_LEFT (6);
object_id_t const WALL_ID_RIGHT (7);
object_id_t const WALL_ID_TOP (8);
object_id_t const WALL_ID_BOTTOM (9);