#include "tiles.h"

#include "cuckoo/core/asserts.h" // for CUCKOO_ASSERT
#include "cuckoo/maths/maths.h"  // for cuckoo::maths::two_pi, ...
#include "cuckoo/time/time.h"    // for cuckoo::sleep :[

#include "extra/walls.h"         // for wall_t


static void matrix_multiply (float output[4][4], float const input_a[4][4], float const input_b[4][4])
{
  // We can access the matrix (4x4 float array) with: matrix [ROW][COLUMN]
  // Remember, these matrices are stored in the ROW-MAJOR format.
  // This means groups of 4 consecutive floats in memory represent a row of that matrix.
  // (With the row-major format this would is not the case,
  // 4 consecutive floats in memory represent a column of that matrix
  // and would be accessed with: matrix [column][row].)
  // This does NOT effect how matrix multiplation is implemented! (It is the same either way.)
  // OUT00 = ROW0 of A dot COLUMN0 of B, etc
  // Just take care that you are actually getting the dot product of
  // a ROW of MATRIX A and a COLUMN of MATRIX B when addressing the memory.
  //
  // A00, A01, A02, A03      B00, B01, B02, B03      O00, O01, O02, O03
  // A10, A11, A12, A13  \/  B10, B11, B12, B13  ==  O10, O11, O12, O13
  // A20, A21, A22, A23  /\  B20, B21, B22, B23  ==  O20, O21, O22, O23
  // A30, A31, A32, A33      B30, B31, B32, B33      O30, O31, O32, O33
  //
  // e.g. OUT00 = A0n dot Bn0 = A00*B00 + A01*B10 + A02*B20 + A03*B30
  // e.g. OUT01 = A0n dot Bn1 = A00*B01 + A01*B11 + A02*B21 + A03*B31
  // e.g. OUT02 = A0n dot Bn2 = A00*B02 + A01*B12 + A02*B22 + A03*B32
  // e.g. OUT10 = A1n dot Bn0 = A10*B00 + A11*B10 + A12*B20 + A13*B30
  //
  // FYI, we can think of multidim arrays as an nD table/'graph' in memory,
  // being accessed with: arr [...][z][y][x]

  for (int row = 0; row < 4; ++row)
  {
    for (int col = 0; col < 4; ++col)
    {
      output [row][col] =
          input_a [row][0] * input_b [0][col]
        + input_a [row][1] * input_b [1][col]
        + input_a [row][2] * input_b [2][col]
        + input_a [row][3] * input_b [3][col];
    }
  }

  // Hint: to implement in SIMD, loading a row of input_a will send you down a dead end!
}


static void collision_resolve_tile_wall (pigeon::gfx::spritesheet spritesheet, tile_t* this_tile, wall_t* wall)
{
  // direction response
  if (wall->get_id () == WALL_ID_LEFT || wall->get_id () == WALL_ID_RIGHT)
  {
    // tile has hit the left or right wall on screen

    // the left and right walls are pefectly aligned with the y-axis
    // therefore, the tile's direction response is to have its x direction 'reflected' perfectly
    this_tile->direction.x = -this_tile->direction.x;
  }
  else
  {
    // tile has hit the top or bottom wall on screen

    // reflect y direction
    this_tile->direction.y = -this_tile->direction.y;
  }

  // get the tiles spritesheet rect (position, width & height)
  // this is to get the tile's size as required by the following code
  texture_rect const* tex_rect = get_tile_texture_rect (spritesheet, this_tile->get_id ());

  // position response
  if (wall->get_id () == WALL_ID_LEFT)
  {
    // tile has hit the left wall, lets move it out

    // move tile to the rightmost edge of the left wall
    this_tile->position.x = wall->position.x + wall->size / 2.0;
    // + half the width of the tile itself (remember the tile's origin is at its centre)
    this_tile->position.x += (double)tex_rect->width / 2.0;
  }
  else if (wall->get_id () == WALL_ID_RIGHT)
  {
    this_tile->position.x = wall->position.x - wall->size / 2.0;
    this_tile->position.x -= (double)tex_rect->width / 2.0;
  }
  else if (wall->get_id () == WALL_ID_TOP)
  {
    this_tile->position.y = wall->position.y - wall->size / 2.0;
    this_tile->position.y -= (double)tex_rect->height / 2.0;
  }
  else if (wall->get_id () == WALL_ID_BOTTOM)
  {
    this_tile->position.y = wall->position.y + wall->size / 2.0;
    this_tile->position.y += (double)tex_rect->height / 2.0;
  }

  // By adjusting the tile's position we have stopped the tile and wall from overlapping.
  // By reflecting the tile's direction the tile will not collide with it on the next frame.
  // Job done!


  // N.B.
  // This collision resolution is not fully physically accurate (but is more than acceptable for our purposes.)
  // e.g. on tile collision with y-oriented walls (left and right)
  // we only reflect its x direction and move it out of the wall.
  // Why could this cause a bug/be less realistic? (Think of other objects, think direction repsonse timing...)
  // 1 frame
  //                   wall
  //                    |
  //            o-o     |
  //      start | |\    |
  //            o-o \   |
  //                 \  |
  //                 o-o|
  //      ours final | |x
  //                 o-o|\
  //                  / | \
  //                 /  |  \
  //            o-o /   |   \ o-o
  // real final | |/    |    \| | tunnelling final
  //            o-o     |     o-o this would happen if the walls were too thin!
  //                    |
  //
  // But, why is ours solution (generally) acceptable? (Think temporal...)
  // What strategies could help improve this? (Think 'sub-steps', think swept volumes, think collision time...)
  //
  // This is just a thought experiment, please DO NOT attempt to fix this here!
  // More physically accurate collision detection and resolution gets complex real quick.
  // It's just interesting to discuss :)
  // If you want to learn more about how 'real'/commercial collision systems actually work, ask Andy
  // or your 'mega-module' lecturer to point you in the right direction and then do some research!
  // Here is a starter: https://tinyurl.com/mrw4d3k
  // Your final year project gives you an excellent opportunity to explore these sorts of things in depth.
}


// TILE

tile_t::tile_t ()
  : position { 0.0, 0.0, 0.0, 0.0 }
  , direction { 0.0, 0.0, 0.0, 0.0 }
{
  angle_radians = (float)random_getd (0.0, cuckoo::maths::two_pi <double> ());
}


// TILE NORMAL

tile_normal_t::tile_normal_t ()
  : tile_t ()
  , is_eaten { false }
{
  position.x = random_getd (SCREEN_WIDTH / -2.0, SCREEN_WIDTH / 2.0);
  position.y = random_getd (SCREEN_HEIGHT / -2.0, SCREEN_HEIGHT / 2.0);

  direction.x = random_getd (-1.0, 1.0);
  direction.y = random_getd (-1.0, 1.0);
  double const magnitude = cuckoo::maths::sqrt (direction.x * direction.x + direction.y * direction.y);
  direction.x /= magnitude; // normalise direction
  direction.y /= magnitude;
}

void tile_normal_t::update (double elapsed, pigeon::gfx::spritesheet spritesheet)
{
  // update position
  position.x += direction.x * TILE_SPEED_MOVEMENT * elapsed;
  position.y += direction.y * TILE_SPEED_MOVEMENT * elapsed;

  // update angle
  angle_radians += (float)TILE_SPEED_ROTATION * elapsed;

  // limit angle
  float const angle_limit = cuckoo::maths::two_pi <float> ();
  angle_radians = cuckoo::maths::mod (angle_radians, angle_limit);
  // Don't understand what is going on here?
  // Ask Andy to talk through it in your tutorial session.
  // ...
  // Now you know what is going on, how would you implement it in SIMD?
  // Hint: there is no 'mod' function in SIMD.
}
void tile_normal_t::render (pigeon::gfx::sprite_batch& sprite_batch,
  pigeon::gfx::spritesheet spritesheet)
{
  texture_rect const* tex_rect = get_tile_texture_rect (spritesheet, get_id ());
  CUCKOO_ASSERT (tex_rect);

  float const position_x = position.x;
  float const position_y = position.y;
  float const angle = angle_radians; // must be in radians!
  float const scale_x = (float)tex_rect->width;
  float const scale_y = (float)tex_rect->height;


/////////////////////////////////////////////////////////////////////////////////////
//// DO NOT EDIT CODE BELOW - THIS CODE MUST BE IN YOUR TILE RENDER FUNCTION >>> ////
/////////////////////////////////////////////////////////////////////////////////////


  // Row-/Column-major matrices refers to the order in which matrix elements are stored in memory.
  // "In row-major order, the consecutive elements of a row reside next to each other,
  // whereas the same holds true for consecutive elements of a column in column-major order."
  // https://en.wikipedia.org/wiki/Row-_and_column-major_order
  // Magpie uses the COLUMN-MAJOR matrices.
  //
  // However, to simplify the matrix_multiply function (and eventual optimisation using SIMD),
  // we are going to calculate our model (world) matrix with the input matrices in row-major format
  // and then transpose them before using to render the tile (in effect converting it from row-major to column-major

  alignas (16) float matrix_position [4][4];
  matrix_position [0][0] = 1.f; matrix_position [0][1] = 0.f; matrix_position [0][2] = 0.f; matrix_position [0][3] = position_x;
  matrix_position [1][0] = 0.f; matrix_position [1][1] = 1.f; matrix_position [1][2] = 0.f; matrix_position [1][3] = position_y;
  matrix_position [2][0] = 0.f; matrix_position [2][1] = 0.f; matrix_position [2][2] = 1.f; matrix_position [2][3] = 0.f;
  matrix_position [3][0] = 0.f; matrix_position [3][1] = 0.f; matrix_position [3][2] = 0.f; matrix_position [3][3] = 1.f;

  float const c = cuckoo::maths::cos (angle);
  float const s = cuckoo::maths::sin (angle);
  alignas (16) float matrix_rotation [4][4];
  matrix_rotation [0][0] = c;   matrix_rotation [0][1] = -s;  matrix_rotation [0][2] = 0.f; matrix_rotation [0][3] = 0.f;
  matrix_rotation [1][0] = s;   matrix_rotation [1][1] = c;   matrix_rotation [1][2] = 0.f; matrix_rotation [1][3] = 0.f;
  matrix_rotation [2][0] = 0.f; matrix_rotation [2][1] = 0.f; matrix_rotation [2][2] = 1.f; matrix_rotation [2][3] = 0.f;
  matrix_rotation [3][0] = 0.f; matrix_rotation [3][1] = 0.f; matrix_rotation [3][2] = 0.f; matrix_rotation [3][3] = 1.f;

  alignas (16) float matrix_scale [4][4];
  matrix_scale [0][0] = scale_x; matrix_scale [0][1] = 0.f;     matrix_scale [0][2] = 0.f; matrix_scale [0][3] = 0.f;
  matrix_scale [1][0] = 0.f;     matrix_scale [1][1] = scale_y; matrix_scale [1][2] = 0.f; matrix_scale [1][3] = 0.f;
  matrix_scale [2][0] = 0.f;     matrix_scale [2][1] = 0.f;     matrix_scale [2][2] = 1.f; matrix_scale [2][3] = 0.f;
  matrix_scale [3][0] = 0.f;     matrix_scale [3][1] = 0.f;     matrix_scale [3][2] = 0.f; matrix_scale [3][3] = 1.f;

  alignas (16) float matrix_position_rotation [4][4];
  alignas (16) float matrix_model [4][4];


  // matrix_model = matrix_position * matrix_rotation * matrix_scale
  // Remember, matrix maths dictates that we multiply the matrices in the reverse of order of the desired transformation!
  // i.e. here we are performing the scaling FIRST, THEN the rotation and FINALLY the translation.
  matrix_multiply (matrix_position_rotation, matrix_position, matrix_rotation);
  matrix_multiply (matrix_model, matrix_position_rotation, matrix_scale);
  *(mat4*)matrix_model = cuckoo::maths::transpose (*(mat4*)matrix_model);


  sprite_batch.draw (*tex_rect, (float*)matrix_model);


/////////////////////////////////////////////////////////////////////////////////////
//// <<< DO NOT EDIT CODE ABOVE - THIS CODE MUST BE IN YOUR TILE RENDER FUNCTION ////
/////////////////////////////////////////////////////////////////////////////////////

}

void tile_normal_t::on_collision (object_type_t other_type, void* other_data, pigeon::gfx::spritesheet spritesheet)
{
  if (other_type == WALL_TYPE)
  {
    // 'other_data' is a wall of some kind

    // this tile has hit a wall, make the appropriate changes to this tile as a result of it
    collision_resolve_tile_wall (spritesheet, this, (wall_t*)other_data);
  }
  else if (other_type == PLAYER_TYPE)
  {
    // 'other_data' is a player of some kind

    is_eaten = true; // mark this tile as 'eaten' and therefore requires replacing
  }
}
object_id_t tile_normal_t::get_id () const { return TILE_ID_NORMAL; }

bool tile_normal_t::needs_replacing () const
{
  if (is_eaten) // :[
  {
    return true;
  }
  else
  {
    return false;
  }
}


// GENERAL

void initialise_tiles (tiles_t& tiles)
{
  // hhhmmm, what else could go here?

  tiles = replace_expired_tiles (tiles);
}

tiles_t replace_expired_tiles (tiles_t tiles)
{
  // The game requires that there are always active { NUM_TILES } on screen.
  // 1. iterate over tiles, remove tiles that need replacing
  // 2. replace removed tiles with new ones


  tiles_t tiles_copy = tiles;


  // REMOVE OLD TILES
  // iterators provide a generic way to access the data at a particular element of a container
  // e.g. vectors, lists and maps - https://en.cppreference.com/w/cpp/container
  // iterators are 'special' in that they can be incremented to go to the next element in the collection
  // (even if it is not physically next to it in memory - https://en.cppreference.com/w/cpp/iterator)
  auto it = tiles_copy.data.begin ();
  while (it != tiles_copy.data.end ())
  {
    if (it->needs_replacing ())
    {
      // std::vector::erase () returns next valid element :)
      it = tiles_copy.data.erase (it);
    }
    else
    {
      // increment iterator here in case tile was not removed
      ++it;
    }
  }


  // CREATE NEW TILES
  while (tiles_copy.data.size () < NUM_TILES)
  {
    // insert new tile into our container
    tiles_copy.data.emplace_back ();
  }

  tiles = tiles_copy;

  return tiles;
}

void release_tiles (tiles_t& tiles)
{
  tiles.data.clear ();
}


texture_rect const* get_tile_texture_rect (pigeon::gfx::spritesheet& spritesheet, object_id_t id)
{
  texture_rect const* rect = nullptr;

  if (id == TILE_ID_NORMAL)
  {
    rect = spritesheet.get_sprite_info ("tile_0.png");
  }

  return rect;
}
