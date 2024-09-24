#include "walls.h"

#include "cuckoo/core/asserts.h" // for CUCKOO_ASSERT
#include "cuckoo/maths/maths.h"  // for cuckoo::maths::max


// WALL

wall_t::wall_t (double size, vector4 position, object_id_t id)
  : size (size), position (position), id (id)
{
}

void wall_t::render (pigeon::gfx::sprite_batch& sprite_batch,
  pigeon::gfx::spritesheet spritesheet)
{
  texture_rect const* tex_rect = spritesheet.get_sprite_info ("wall.png");
  CUCKOO_ASSERT (tex_rect);

  for (int i = 0; i < 10; ++i)
  {
    sprite_batch.draw (*tex_rect,
      (float)position.x, (float)position.y,
      0.f,
      0.f, 0.f,
      (float)size, (float)size);
  }
}

void wall_t::on_collision (object_type_t other_type, void* other_data, pigeon::gfx::spritesheet spritesheet) {}
object_id_t wall_t::get_id () const { return id; }


// GENERAL

walls_t initialise_walls (vector4 window_size)
{
  walls_t walls;

  // origin is in centre of the screen!
  // make width of walls bigger than is visible to help prevent tunneling at low FPS

  double const wall_size = (double)cuckoo::maths::max (window_size.x, window_size.y) + 50.0;
  double const width_visible = 5.0; // how many pixels 'peek out' from off screen

  // left
  {
    vector4 const position = { -(double)window_size.x / 2.0 - wall_size / 2.0 + width_visible, 0.0, 0.0, 0.0 };
    walls.data.push_back ({ wall_size, position, WALL_ID_LEFT });
  }

  // right
  {
    vector4 const position = { (double)window_size.x / 2.0 + wall_size / 2.0 - width_visible, 0.0, 0.0, 0.0 };
    walls.data.push_back ({ wall_size, position, WALL_ID_RIGHT });
  }

  // top
  {
    vector4 const position = { 0.0, (double)window_size.y / 2.0 + wall_size / 2.0 - width_visible, 0.0, 0.0 };
    walls.data.push_back ({ wall_size, position, WALL_ID_TOP });
  }

  // bottom
  {
    vector4 const position = { 0.0, -(double)window_size.y / 2.0 - wall_size / 2.0 + width_visible, 0.0, 0.0 };
    walls.data.push_back ({ wall_size, position, WALL_ID_BOTTOM });
  }

  return walls;
}

void release_walls (walls_t& walls)
{
  walls.data.clear ();
}
