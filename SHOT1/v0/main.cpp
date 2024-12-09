// APPLICATION NOTES:
//
// Simple 2D physics game.
// This is a small graphical application in which the user controls a coloured dinosaur
// with the aim of 'eating' tiles that are moving around the game area.
//
// The are 3 'game components' in this application: player, tile and wall.
// There are 2 types of player:
// normal = blue dinosaur
// wide   = green dinosaur
//     bigger than normal player
//     and moves faster
//     automatically revertd to 'normal' player after { PLAYER_FAST_LIFETIME } seconds
// There is 1 type of tile:
// normal = blue tile
//     player gains 1 point when eaten
//
// Once the player reaches a multiple of { PLAYER_FAST_POINTS_SWITCH } points, the player turns into a 'fast' player
//
// Tiles start from a random position and move in a random direction.
// There is no external force (e.g. gravity) acting on the tiles and do not lose energy from collisions.
// All tiles move at a constant predetermined speed.
//
// The dinosaur is moved around the game area using the arrows keys on a keyboard or the d-pad on a controller.
//
//
// ASSIGNMENT NOTES RECAP:
//
// This starter code base has been DELIBERATELY written badly to reduce code performance!
// This application's starter code is now YOURS! Alter/move/delete ANY part of it as YOU see fit!
// (There are only a couple of very small exceptions for setup/rendering code. Keep an eye out, they are clearly marked.)
// You are allowed to change ANY part of the application starter code for this assignment,
// just remember that the application must have the same behaviour/visual output when you are finished.
// Please see Blackboard for an exemplar executable of what the application should like when you are finished.
//
// Suggested approach to optimisation:
// 1.	Inspect Code/Trophies/Research/Plan/Think
// 2.	Implement optimisation
//   o  Add appropriate code comments
//   o  Ensure relevant code standards are followed, see “Code Standards” section below
// 3.	Test and gather times
//   o  It is highly recommended that you keep track of average frame times for both projects over the course of development, before and after each trophy is implemented
// 4.	Update your ‘Trophy Tracker’ spreadsheet
//   o  See “Filling in the ‘Trophy Tracker’” section below
// 5.	Show your code and spreadsheet to your tutor
// 6.	Repeat
//
// Finally, this code base is littered with instances of what may appear to be 'uncommon'
// parts of the C++ language. If you don't know what it is... RESEARCH!
//
//
// PIGEON NOTES:
//
// Pigeon is the name of Andy's rendering framework :)
// Pigeon consists of 3 layers:
// Cuckoo: platform level code e.g. memory
// Magpie: rendering code
// Pigeon: 2D rendering layer on top of Magpie
//
// In pigeon, the screen's origin is at the centre of the screen, up = +ve y, right = +ve x.
//
// In pigeon, textures are scaled in pixels.
// So, if we want a texture to be rendered @ 300x300 pixels on screen, we scale it by 300 in the x & y direction.
// i.e. the original texture's size is ignored, e.g. scaling by 1x1 will render a single pixel!
// In this application, the player and tile's scale is derived from their pixel size on the spritesheet.
// i.e. the tile/player, when rendered, is scaled by its size from the spritesheet.
// This size data is then also used in the collision detection code related to the tile/player.
// As a result, the tile/player's size in the game world is the same as their sub-texture's width/height from the spritesheet.
// (Hint: all tile textures have identical x & y sizes...)
//
// In this application, window resizing/maximising has been disabled.
//
// Do not add .cpp/.h files directly here in VS!
// When/if you want to add/delete files:
// - place/remove them from the 'pigeon_projects' folder
// - rerun the batch script :)
//
//
// Original Author: A.Hamilton - 2023
// Updated: September 2024


#include "pigeon/pigeon.h"           // for pigeon window/rendering components

#include "constants.h"               // for SCREEN_WIDTH, SCREEN_HEIGHT
#include "collision.h"               // for resolve_collisions
#include "tiles.h"                   // for tiles_t
#include "extra/player.h"            // for player_t
#include "extra/walls.h"             // for walls_t
#include "Timer.h"                   // for timer class
#include <cstdlib>                   // for srand
#include <optional>                  // for


ENTRY_POINT
{
    ////////////////////////////////////////////////
    //// DO NOT EDIT/DELETE/MOVE CODE BELOW >>> ////
    ////////////////////////////////////////////////
    srand(0); // initialise rand ()


// DRIVER
{
    pigeon::gfx::driver::descriptor const desc =
    {
       .title = STRINGIFY(PROJECT_NAME),
    //.initial_width     = cuckoo::DEFAULT_SCREEN_WIDTH,
    //.initial_height    = cuckoo::DEFAULT_SCREEN_HEIGHT,
    //.on_focus_callback = {},
    //.on_size_callback  = {},
        .is_resizable = false,
    //.show_cursor       = true,

    //.clear_colour      = { .39f, .8f, .92f }, // cornflower blue

    //.camera_type       = pigeon::gfx::driver::camera_type_t::MAGPIE,
  };
    if (!pigeon::gfx::driver::initialise(desc))
    {
        CUCKOO_ASSERT(!"pigeon::gfx::driver::initialise failed");
    }
}
////////////////////////////////////////////////
//// <<< DO NOT EDIT/DELETE/MOVE CODE ABOVE ////
////////////////////////////////////////////////


  // SETUP

  player_t* player;
  initialise_player(player);

  tiles_t tiles;
  initialise_tiles(tiles);

  pigeon::gfx::spritesheet spritesheet = {};
  if (!spritesheet.initialise("data/textures/SHOT1/sprites.xml"))
  {
      CUCKOO_ASSERT(!"spritesheet.initialise failed");
  }

  pigeon::gfx::sprite_batch sprite_batch{};
  {
      // We need enough capacity for this sprite batch to render 1 player sprite, 4 wall sprites and { NUM_TILES } tile sprites
      // Each sprite requires memory for 4 vertices in RAM.
      pigeon::gfx::descriptor_sprite_batch const desc =
      {
        .source_image = spritesheet.get_image(),
        .max_sprites = NUM_TILES * 10u,
      };
      if (!sprite_batch.initialise(desc))
      {
          CUCKOO_ASSERT(!"sprite_batch.initialise failed");
      }
  }

  timer FrameTimer;



  FrameTimer.start_timer();  // start frame timer, have really small first frame elapsed seconds, rather than an unknown time
  // GAME LOOP
  while (pigeon::gfx::driver::process_os_messages())
  {
      FrameTimer.end_timer();
      float elapsed_seconds = FrameTimer.get_elapsed_time_secs();//end timer

      FrameTimer.start_timer(); // start frame timer
      cuckoo::printf("frame : %.5f seconds\n", elapsed_seconds);

      float average_time = 0;//working out average time
      int frames_passed = 0;
   
      frames_passed++;

      if (frames_passed < 100)
      {
          average_time = elapsed_seconds / frames_passed;
      }
        cuckoo::printf("AverageTime : %.5f seconds\n", average_time);






    // UPDATE
    {
        // PLAYER
        {
          player->update(elapsed_seconds, spritesheet);
        }

        // TILES
        tiles.update(elapsed_seconds);

        // COLLISIONS
        {
            vector4 window_size = { (double)pigeon::gfx::driver::get_screen_size().x, (double)pigeon::gfx::driver::get_screen_size().y, 0.0, 0.0 };
            walls_t walls = initialise_walls(window_size);
            resolve_collisions(spritesheet, *player, tiles, walls);
            release_walls(walls);
        }
        check_player_needs_replacing(player);
        tiles = replace_expired_tiles(tiles);
      }


    ////////////////////////////////////////////////
    //// DO NOT EDIT/DELETE/MOVE CODE BELOW >>> ////
    ////////////////////////////////////////////////
        // RENDER
    if (pigeon::gfx::driver::can_render_frame())
    {
        if (!pigeon::gfx::driver::begin_frame())
        {
            CUCKOO_ASSERT(!"pigeon::gfx::driver::begin_frame failed");
        }
        
        {
            if (!sprite_batch.start_batch())
            {
              CUCKOO_ASSERT(!"sprite_batch.start_batch failed");
            }
            ////////////////////////////////////////////////
            //// <<< DO NOT EDIT/DELETE/MOVE CODE ABOVE ////
            ////////////////////////////////////////////////


            // PLAYER
            {
                player->render(sprite_batch, spritesheet);
            }

            // TILES
            {
                // iterators provide a generic way to access the data at a particular element of a container
                // e.g. vectors, lists and maps // https://en.cppreference.com/w/cpp/container
                // iterators are 'special' in that they can be incremented to go to the next element in the collection
                // (even if it is not physically next to it in memory // https://en.cppreference.com/w/cpp/iterator)
                tiles.render(sprite_batch, spritesheet);

                // WALLS
                {
                    vector4 window_size = { (double)pigeon::gfx::driver::get_screen_size().x, (double)pigeon::gfx::driver::get_screen_size().y, 0.0, 0.0 };
                    walls_t walls = initialise_walls(window_size);
                    for (auto& wall : walls.data)
                    {
                        wall.render(sprite_batch, spritesheet);
                    }
                    release_walls(walls);
                }


                ////////////////////////////////////////////////
                //// DO NOT EDIT/DELETE/MOVE CODE BELOW >>> ////
                ////////////////////////////////////////////////
                sprite_batch.end_batch();
                pigeon::gfx::driver::render(sprite_batch);
            }
            if (!pigeon::gfx::driver::end_frame()) // render to window
            {
                CUCKOO_ASSERT(!"pigeon::gfx::driver::end_frame failed");
            }
          }
          ////////////////////////////////////////////////
          //// <<< DO NOT EDIT/DELETE/MOVE CODE ABOVE ////
          ////////////////////////////////////////////////

    }

            } // GAME LOOP: END


            // RELEASE RESOURCES
            {
                sprite_batch.release();
                spritesheet.release();
                release_player(player);


              pigeon::gfx::driver::release();
            }

            return 0;
}
