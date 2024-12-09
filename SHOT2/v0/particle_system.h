// HOW IT WORKS:
//
// Simple CPU particle simulation.
// Particle are emitted from 3 locations,
// with differing position offsets, velocities, accelerations and start & end colours.
// Particle types are split evenly between all 3 types.
// Each particle is represented by a single pixel.
//
// Each particle's position is controlled by:
//   position       |                                                     | per particle      | random within a range
//   velocity       | speed and direction change, per second              | per particle      | random within a range
//   acceleration   | velocity change, per second                         | per particle type | fixed
//
// Each particle's lifetime is controlled by:
//   life_time      | maximum amount of time the particle can live for    | per particle      | random within a range
//   life_remaining | initially set to life_time, countdown to 0          | per particle      | random within a range
//   kill_y         | the minimum position.y value before being destroyed | per particle type | fixed
//
// Each particle's colour is determined by the ratio between life_remaining and life_time.
// The start and end colours are fixed and are the same for each particle type.


#pragma once

#include "cuckoo/core/asserts.h"       // for CUCKOO_ASSERT
#include "cuckoo/core/logger.h"        // for CUCKOO_DPRINTF
#include "cuckoo/maths/maths.h"        // for cuckoo::maths::lerp, ...
#include "pigeon/gfx/driver.h"         // for pigeon::gfx::driver::render, pigeon::gfx::driver::get_screen_size
#include "pigeon/gfx/point_renderer.h" // for pigeon::gfx::point_renderer

#include "constants.h"

#include <bitset>                      // for 
#include <list>                        // for std::list
#include <random>                      // for std::random_device, std::uniform_real_distribution, std::uniform_int_distribution
#include <vector>                      // for std::vector
#include <thread>                      // for threads


// UTILITY

struct vector4
{
  double x;
  double y;
  double z;
  double w;
};

struct colourf
{
  double r;
  double g;
  double b;
  double a;
};


/// @brief returns a random number between min and max inclusive
/// @param min minimum random number (inclusive)
/// @param max maximum random number (inclusive)
/// @return random number between min & max (inclusive)
static double random_getd (double min, double max)
{
  CUCKOO_ASSERT (max >= min);


  static std::random_device rd;
  std::uniform_real_distribution <double> distribution (min, max);

  return distribution (rd);
}
/// @brief returns a random number between min and max inclusive
/// @param min minimum random number (inclusive)
/// @param max maximum random number (inclusive)
/// @return random number between min & max (inclusive)
static long long random_geti (long long min, long long max)
{
  CUCKOO_ASSERT (max >= min);


  static std::random_device rd;
  std::uniform_int_distribution <i64> distribution (min, max);

  return distribution (rd);
}


// PARTICLES

class particle
{
public:
  virtual ~particle () = default;

  /// @brief update particle's position, lifetime & colour
  /// @param elapsed_seconds elapsed time since last frame
  /// @return true, if particle has expired and needs deleting
  virtual bool process (double elapsed_seconds) = 0;

  double  life_time = {};
  double  life_remaining = {};
  double  kill_y = {};

  vector4 position = {};
  vector4 velocity = {};
  vector4 acceleration = {};

  colourf colour = {};
  colourf start_colour = {};
  colourf end_colour = {};
};

class particle_a : public particle
{
public:
  particle_a ()
  {
    // left hand side of screen

    life_time = life_remaining = random_getd (7.5, 13.0);
    kill_y = -(double)pigeon::gfx::driver::get_screen_size ().y / 2.0;

    position = { -(double)pigeon::gfx::driver::get_screen_size ().x / 2.0 + random_getd (0.0, 200.0),
      -(double)pigeon::gfx::driver::get_screen_size ().y / 2.0 + random_getd (0.0, 100.0),
      0.0, 0.0 };
    velocity = { random_getd (cuckoo::maths::cos (cuckoo::maths::radians (89.0)), cuckoo::maths::cos (cuckoo::maths::radians (75.0))) * 200.f,
      random_getd (cuckoo::maths::sin (cuckoo::maths::radians (75.0)), cuckoo::maths::sin (cuckoo::maths::radians (89.0))) * 200.f,
      0.0, 0.0 };
    acceleration = { 2.0, -26.5, 0.0, 0.0 };

    start_colour = { 1.0, 0.2, 0.2, 1.0 }; // red
    end_colour = { 0.2, 1.0, 1.0, 1.0 }; // inverse red
  }

  bool process (double elapsed_seconds) override
  {
    // update linear motion
    position.x += velocity.x * elapsed_seconds;
    position.y += velocity.y * elapsed_seconds;
    position.z += velocity.z * elapsed_seconds;
    position.w += velocity.w * elapsed_seconds;

    velocity.x += acceleration.x * elapsed_seconds;
    velocity.y += acceleration.y * elapsed_seconds;
    velocity.z += acceleration.z * elapsed_seconds;
    velocity.w += acceleration.w * elapsed_seconds;

    // update colour
    colour.r = cuckoo::maths::lerp (end_colour.r, start_colour.r, life_remaining / life_time);
    colour.g = cuckoo::maths::lerp (end_colour.g, start_colour.g, life_remaining / life_time);
    colour.b = cuckoo::maths::lerp (end_colour.b, start_colour.b, life_remaining / life_time);
    colour.a = cuckoo::maths::lerp (end_colour.a, start_colour.a, life_remaining / life_time);

    // update life remaining
    life_remaining -= elapsed_seconds;

    // is particle still alive?
    if (life_remaining <= 0.0)
      return true;
    else if (position.y < kill_y)
      return true;
    else
      return false;
  }
};

class particle_b : public particle
{
public:
  particle_b ()
  {
    // middle of screen

    life_time = life_remaining = random_getd (9.0, 10.0);
    kill_y = -(double)pigeon::gfx::driver::get_screen_size ().y / 2.0 + 50.0;

    position = { random_getd (0.0, (double)pigeon::gfx::driver::get_screen_size ().x / 3.0),
      (double)pigeon::gfx::driver::get_screen_size ().y / 2.0,
      0.0, 0.0 };
    velocity = { -50.0,
      random_getd (-100.0, -60.0),
      0.0, 0.0 };
    acceleration = { 0.0, 0.0, 0.0, 0.0 };

    start_colour = { 0.2, 1.0, 0.2, 1.0 }; // green
    end_colour = { 1.0, 0.2, 1.0, 1.0 }; // inverse green
  }

  bool process (double elapsed_seconds) override
  {
    // update linear motion
    position.x += velocity.x * elapsed_seconds;
    position.y += velocity.y * elapsed_seconds;
    position.z += velocity.z * elapsed_seconds;
    position.w += velocity.w * elapsed_seconds;

    velocity.x += acceleration.x * elapsed_seconds;
    velocity.y += acceleration.y * elapsed_seconds;
    velocity.z += acceleration.z * elapsed_seconds;
    velocity.w += acceleration.w * elapsed_seconds;

    // update colour
    colour.r = cuckoo::maths::lerp (end_colour.r, start_colour.r, life_remaining / life_time);
    colour.g = cuckoo::maths::lerp (end_colour.g, start_colour.g, life_remaining / life_time);
    colour.b = cuckoo::maths::lerp (end_colour.b, start_colour.b, life_remaining / life_time);
    colour.a = cuckoo::maths::lerp (end_colour.a, start_colour.a, life_remaining / life_time);

    // update life remaining
    life_remaining -= elapsed_seconds;

    // is particle still alive?
    if (position.y < kill_y)
      return true;
    else if (life_remaining <= 0.0)
      return true;
    else
      return false;
  }
};

class particle_c : public particle
{
public:
  particle_c ()
  {
    // right hand side of screen

    life_time = life_remaining = random_getd (3.5, 6.0);
    kill_y = -(double)pigeon::gfx::driver::get_screen_size ().y / 2.0 + 15.0;

    position = { (double)pigeon::gfx::driver::get_screen_size ().x / 2.0 - 300.0,
      -(double)pigeon::gfx::driver::get_screen_size ().y / 2.0 + 400.0,
      0.0, 0.0 };
    velocity = { random_getd (-50.0, 50.0),
      random_getd (-50.0, 50.0),
      0.0, 0.0 };
    acceleration = { 0.0, 0.0, 0.0, 0.0 };

    start_colour = { 0.2, 0.2, 1.0, 1.0 }; // blue
    end_colour = { 1.0, 1.0, 0.2, 1.0 }; // inverse blue
  }

  bool process (double elapsed_seconds) override
  {
    // update linear motion
    position.x += velocity.x * elapsed_seconds;
    position.y += velocity.y * elapsed_seconds;
    position.z += velocity.z * elapsed_seconds;
    position.w += velocity.w * elapsed_seconds;

    velocity.x += acceleration.x * elapsed_seconds;
    velocity.y += acceleration.y * elapsed_seconds;
    velocity.z += acceleration.z * elapsed_seconds;
    velocity.w += acceleration.w * elapsed_seconds;

    // update colour
    colour.r = cuckoo::maths::lerp (end_colour.r, start_colour.r, life_remaining / life_time);
    colour.g = cuckoo::maths::lerp (end_colour.g, start_colour.g, life_remaining / life_time);
    colour.b = cuckoo::maths::lerp (end_colour.b, start_colour.b, life_remaining / life_time);
    colour.a = cuckoo::maths::lerp (end_colour.a, start_colour.a, life_remaining / life_time);

    // update life remaining
    life_remaining -= elapsed_seconds;

    // is particle still alive?
    if (life_remaining <= 0.0)
      return true;
    else if (position.y < kill_y)
      return true;
    else
      return false;
  }
};


// PARTICLE SYSTEM

/// @brief update all active particles
/// remove expired particles
/// @param particles list of particle pointers
/// @param elapsed_seconds elapsed frame time
/// @return updated list of pointers to particles
static std::list <particle*> process (std::list <particle*> particles, double elapsed_seconds)
{
  // iterators provide a generic way to access the data at a particular element of a container
  // e.g. vectors, lists and maps // https://en.cppreference.com/w/cpp/container
  // iterators are 'special' in that they can be incremented to go to the next element in the collection
  // (even if it is not physically next to it in memory // https://en.cppreference.com/w/cpp/iterator)
  std::list <particle*>::iterator it = particles.begin ();
  while (it != particles.end ())
  {
    // get pointer to particle from iterator
    particle* p = *it;
    CUCKOO_ASSERT (p);


    // update particle, returns true of needs deleting
    if (p->process (elapsed_seconds))
    {
      // particle needs deleting

      // release data previously allocated for this particle
      // no effect if p == nullptr
      delete p;

      // remove pointer to now released particle from list
      // std::list::erase () returns next valid element :)
      it = particles.erase (it);
    }
    else
    {
      // increment iterator here if this particle was not removed
      it++;
    }
  }

  return particles;
}
/// @brief create/add new particles to the list
/// @param particles list of particle pointers
/// @param elapsed_seconds elapsed frame time
/// @return updated list of pointers to particles
static std::list <particle*> emit (std::list <particle*> particles, double elapsed_seconds)
{
  long long num_particles_spawned = 0u;
  int particle_type = 0;
  for (float i = 0.f; i < (float)PARTICLE_MAX * 2.f; i += 1.f)
  {
    // make sure we never exceed maximum particle budget per list(thread)
    if (particles.size () == PARTICLE_MAX / NUM_THREADS) 
    {
      cuckoo::printf ("num particles == PARTICLE_MAX\n");
      continue;
    }
    // make sure we never exceed frame's particle budget
    if (num_particles_spawned == PARTICLE_SPAWN_RATE / NUM_THREADS)
    {
      continue;
    }
    // keep track of how many particles have been emitted this frame
    num_particles_spawned++;

    // add particle
    // evenly spread particles between each type
    if (particle_type == 0)
    {
      particles.push_back (new particle_a);
      cuckoo::printf ("spawn particle a\n");
    }
    else if (particle_type == 1)
    {
      particles.push_back (new particle_b);
    }
    else // particle_type == 2
    {
      particles.push_back (new particle_c);
    }
    // create the next type of particle on the next iteration
    particle_type++;
    // 'wrap' particle type so its always valid, 0 <-> { NUM_PARTICLE_TYPES - 1 }
    particle_type = particle_type % NUM_PARTICLE_TYPES;
  }

  return particles;
}

/// <summary>
/// For both process and emit, there are 2 threads each. Worker assigns emit and process to these threads. 
/// </summary>
/// <param name="particles"></param>
/// <param name="elapsed_seconds"></param>
void Worker(std::list <particle*>& particles, double elapsed_seconds )
{
    particles = process(particles, elapsed_seconds);
    particles = emit(particles, elapsed_seconds); 
}

class particle_system_t
{
public:
  bool initialise (void)
  {
    pigeon::gfx::descriptor_point_renderer const desc =
    {
      .max_points = PARTICLE_MAX,
    };
    return point_renderer.initialise (desc);
  }

  /// <summary>
  /// std::vector<std::threads> threads, creates the array of to the size of NUM_THREADS (4).
  /// Then the threads are filled via the worker function using a reference to particles.
  /// Afterwards the threads are joined into the main thread and then there is a limiter for the max amount of particles spawned per thread. 
  /// </summary>
  /// <param name="elapsed_seconds"></param>
  /// <param name="num_active_particles"></param>
  void update (double elapsed_seconds, long long& num_active_particles)
  {
      std::vector <std::thread> threads;
      for (unsigned i = 0u; i < NUM_THREADS; ++i)
      {
          threads.emplace_back(Worker, std::ref(particles[i]), elapsed_seconds);
      }
      for (std::thread& t : threads)
      {
          t.join();
      }

      num_active_particles = 0;
      for (unsigned i = 0u; i < NUM_THREADS; ++i)
      {
          num_active_particles += (20 * particles[i].size()) / 20;
      }


  }
  void render (void)
  {
////////////////////////////////////////////////
//// DO NOT EDIT/DELETE/MOVE CODE BELOW >>> ////
////////////////////////////////////////////////


    point_renderer.start_batch ();


////////////////////////////////////////////////
//// <<< DO NOT EDIT/DELETE/MOVE CODE ABOVE ////
////////////////////////////////////////////////


    cuckoo::printf ("rendering particles\n");
    for (unsigned i = 0u; i < NUM_THREADS; ++i)
    {
        for (particle const* p : particles[i])
        {
          point_renderer.draw ((float)p->position.x, (float)p->position.y,
            vec4 ((float)p->colour.r, (float)p->colour.g, (float)p->colour.b, (float)p->colour.a));
        }

    }


////////////////////////////////////////////////
//// DO NOT EDIT/DELETE/MOVE CODE BELOW >>> ////
////////////////////////////////////////////////


    point_renderer.end_batch ();

    pigeon::gfx::driver::render (point_renderer);


////////////////////////////////////////////////
//// <<< DO NOT EDIT/DELETE/MOVE CODE ABOVE ////
////////////////////////////////////////////////
  }

  void release (void)
  {
////////////////////////////////////////////////
//// DO NOT EDIT/DELETE/MOVE CODE BELOW >>> ////
////////////////////////////////////////////////


    point_renderer.release ();


////////////////////////////////////////////////
//// <<< DO NOT EDIT/DELETE/MOVE CODE ABOVE ////
////////////////////////////////////////////////



    // delete all particles
    for (unsigned i = 0u; i < NUM_THREADS; ++i)
    {
        std::list <particle*>::iterator it = particles[i].begin(); 
        while (it != particles[i].end()) 
        {
          // get pointer to particle from iterator
          particle* p = *it;

          // release data previously allocated for this particle
          // no effect if p == nullptr
          delete p;

          // remove pointer to now released particle from list
          // std::list::erase () returns next valid element :)
          it = particles[i].erase(it); 
        }
    }

  }


private:
  pigeon::gfx::point_renderer point_renderer;
  std::list <particle*> particles[NUM_THREADS];

};
