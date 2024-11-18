// APPLICATION NOTES:
//
// Simple CPU particle simulation.
// This is a small graphical application in which 3 types of particle are emitted from separate locations,
// with differing position offsets, velocities, accelerations and start & end colours.
// Each particle is represented by a single pixel.
// Go to 'particle_system.h' for more details.
//
//
// ASSIGNMENT NOTES RECAP:
//
// Please use git to version your code throughout development.
// Commit little & often and be sure to fully detail the code changes made in your commit messages.
// It is also suggested that you make use of tags to mark commits when optimisations have been completed.
// (This will be very helpful for your trophy tracker!)
// You do not need to version the pigeon libs or assets.
// Put this code in git - GO DO IT NOW!!!
// A clean copy of this starter code, the framework code and the assets will always be available on Blackboard,
// so please just version this project code and clone it into pigeon's project folder on your machine.
//
// NO MARKS will be awarded for extending/'fixing' the pigeon library in anyway.
// Andy will use a clean copy of pigeon and the assets to test your submission.
// (Andy has tested pigeon extensively, but if you genuinely think you have found a bug, please let him know:)
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
// Remember, not all optimisations result in a noticeable/noteworthy reduction in computation time! (Remember Amdahl's Law?)
// This does NOT mean that they are necessarily invalid or incorrect.
//
// If there is ANY section of this application starter code that you feel is poorly documented
// or just do not understand, please do not hesitate to ask Andy for more details :)
//
// Please see the assignment specification document on Blackboard for full assignment details.
//
// Finally, this code base is littered with instances of what may appear to be 'uncommon'
// parts of the C++ language. If you don't know what it is... RESEARCH!
//
//
// PIGEON NOTES:
//
// Pigeon is the name of Andy's rendering framework :)
// Andy will show you how to build pigeon and how to get started in your sessions.
// Pigeon consists of 3 layers:
// Cuckoo: platform level code e.g. memory
// Magpie: rendering code
// Pigeon: 2D rendering layer on top of Magpie
//
// In pigeon, the screen's origin is at the centre of the screen, up = +ve y, right = +ve x.
//
// In this application, window resizing/maximising has been disabled.
//
// Do not add .cpp/.h files directly here in VS!
// When/if you want to add/delete files:
// - place/remove them from the 'pigeon_projects' folder
// - rerun the batch script :)
//
//
// Concept by: A.MacDougall - 2021
// Last revised: A.Hamilton - 2024


#include "constants.h"       // for PARTICLE_MAX
#include "particle_system.h" // for particle_system_t

#include "pigeon/pigeon.h"   // for pigeon window/rendering components

#include <optional>          // for
#include <string>            // for


ENTRY_POINT
{
////////////////////////////////////////////////
//// DO NOT EDIT/DELETE/MOVE CODE BELOW >>> ////
////////////////////////////////////////////////
  // DRIVER
  {
    pigeon::gfx::driver::descriptor const desc =
    {
      .title             = STRINGIFY (PROJECT_NAME),
      //.initial_width     = cuckoo::DEFAULT_SCREEN_WIDTH,
      //.initial_height    = cuckoo::DEFAULT_SCREEN_HEIGHT,
      //.on_focus_callback = {},
      //.on_size_callback  = {},
      .is_resizable      = false,
      //.show_cursor       = true,

      .clear_colour      = { 0.f, 0.f, 0.f },

      //.camera_type       = pigeon::gfx::driver::camera_type_t::MAGPIE,
    };
    if (!pigeon::gfx::driver::initialise (desc))
    {
      CUCKOO_ASSERT (!"pigeon::gfx::driver::initialise failed");
    }
  }
////////////////////////////////////////////////
//// <<< DO NOT EDIT/DELETE/MOVE CODE ABOVE ////
////////////////////////////////////////////////


  // SETUP

  particle_system_t particle_system;
  if (!particle_system.initialise ())
  {
    CUCKOO_ASSERT (!"particle_system.initialise failed");
  }

  long long num_active_particles = 0;


  unsigned long long const clock_frequency = cuckoo::get_cpu_frequency ();
  // frame timer
  unsigned long long ticks_frame_start;
  ticks_frame_start = cuckoo::get_cpu_time (); // start frame timer
  // have really small first frame elapsed seconds, rather than an unknown time


  // GAME LOOP
  while (pigeon::gfx::driver::process_os_messages ())
  {
    unsigned long long const ticks_frame_end = cuckoo::get_cpu_time (); // end frame timer
    double const elapsed_seconds = (double)(ticks_frame_end - ticks_frame_start) / (double)clock_frequency;
    ticks_frame_start = cuckoo::get_cpu_time (); // start frame timer
    cuckoo::printf ("frame : %.5f seconds\n", elapsed_seconds);


    unsigned long long const ticks_update_start = cuckoo::get_cpu_time (); // start update timer


    // UPDATE
    {
      particle_system.update (elapsed_seconds, num_active_particles);
    }


    unsigned long long const ticks_update_end = cuckoo::get_cpu_time (); // end update timer
    double const elapsed_seconds_update = (double)(ticks_update_end - ticks_update_start) / (double)clock_frequency;
    cuckoo::printf ("update: %.5f seconds\n", elapsed_seconds_update);


////////////////////////////////////////////////
//// DO NOT EDIT/DELETE/MOVE CODE BELOW >>> ////
////////////////////////////////////////////////
    // RENDER
    if (pigeon::gfx::driver::can_render_frame ())
    {
      if (!pigeon::gfx::driver::begin_frame ())
      {
        CUCKOO_ASSERT (!"pigeon::gfx::driver::begin_frame failed");
      }
      {
////////////////////////////////////////////////
//// <<< DO NOT EDIT/DELETE/MOVE CODE ABOVE ////
////////////////////////////////////////////////


        particle_system.render ();


////////////////////////////////////////////////
//// DO NOT EDIT/DELETE/MOVE CODE BELOW >>> ////
////////////////////////////////////////////////
      }
      if (!pigeon::gfx::driver::end_frame ()) // render to window
      {
        CUCKOO_ASSERT (!"pigeon::gfx::driver::end_frame failed");
      }
    }
////////////////////////////////////////////////
//// <<< DO NOT EDIT/DELETE/MOVE CODE ABOVE ////
////////////////////////////////////////////////


    cuckoo::printf ("\nnumber of active particles = %d, All paricles are active: %s, ns/P = %.2f\n",
      num_active_particles,                                             // number of active particles
      num_active_particles == PARTICLE_MAX ? "YES" : "NO",              // all particles are active?
      elapsed_seconds * 1'000'000'000.f / (float)num_active_particles); // time (ns) per particle
  }


  // RELEASE RESOURCES
  {
    particle_system.release ();


////////////////////////////////////////////////
//// DO NOT EDIT/DELETE/MOVE CODE BELOW >>> ////
////////////////////////////////////////////////


    pigeon::gfx::driver::release ();


////////////////////////////////////////////////
//// <<< DO NOT EDIT/DELETE/MOVE CODE ABOVE ////
////////////////////////////////////////////////
  }

  return 0;
}
