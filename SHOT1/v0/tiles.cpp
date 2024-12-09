#include "tiles.h"

#include "cuckoo/core/asserts.h" // for CUCKOO_ASSERT
#include "cuckoo/maths/maths.h"  // for cuckoo::maths::two_pi, ...

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


static void collision_resolve_tile_wall (pigeon::gfx::spritesheet spritesheet, tiles_t* tiles, wall_t* wall, int index)
{
  // direction response
  if (wall->get_id () == WALL_ID_LEFT || wall->get_id () == WALL_ID_RIGHT)
  {
    // tile has hit the left or right wall on screen
    // the left and right walls are pefectly aligned with the y-axis
    // therefore, the tile's direction response is to have its x direction 'reflected' perfectly
    tiles->direction[index].x = -tiles->direction[index].x;
  }
  else
  {
    // tile has hit the top or bottom wall on screen
    // reflect y direction
    tiles->direction[index].y = -tiles->direction[index].y;
  }

  // get the tiles spritesheet rect (position, width & height)
  // this is to get the tile's size as required by the following code
  texture_rect const* tex_rect = get_tile_texture_rect (spritesheet, tiles->get_id ());

  // position response
  if (wall->get_id () == WALL_ID_LEFT)
  {
    // tile has hit the left wall, lets move it out

    // move tile to the rightmost edge of the left wall
    tiles->position[index].x = wall->position.x + wall->size / 2.0;
    // + half the width of the tile itself (remember the tile's origin is at its centre)
    tiles->position[index].x += (double)tex_rect->width / 2.0;
  }
  else if (wall->get_id () == WALL_ID_RIGHT)
  {
    tiles->position[index].x = wall->position.x - wall->size / 2.0;
    tiles->position[index].x -= (double)tex_rect->width / 2.0;
  }
  else if (wall->get_id () == WALL_ID_TOP)
  {
    tiles->position[index].y = wall->position.y - wall->size / 2.0;
    tiles->position[index].y -= (double)tex_rect->height / 2.0;
  }
  else if (wall->get_id () == WALL_ID_BOTTOM)
  {
    tiles->position[index].y = wall->position.y + wall->size / 2.0;
    tiles->position[index].y += (double)tex_rect->height / 2.0;
  }

  // By adjusting the tile's position we have stopped the tile and wall from overlapping.
  // By reflecting the tile's direction the tile will not collide with it on the next frame.


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


void tiles_t::render(pigeon::gfx::sprite_batch& spritebatch, pigeon::gfx::spritesheet& spritesheet)
{
    texture_rect const* tex_rect = get_tile_texture_rect(spritesheet, TILE_ID_NORMAL);
    CUCKOO_ASSERT(tex_rect);
    for (int i = 0; i < NUM_TILES; ++i)
    {


        float const position_x = position[i].x;
        float const position_y = position[i].y;
        float const angle = angle_radians[i]; // must be in radians!
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

        alignas (16) float matrix_position[4][4];
        matrix_position[0][0] = 1.f; matrix_position[0][1] = 0.f; matrix_position[0][2] = 0.f; matrix_position[0][3] = position_x;
        matrix_position[1][0] = 0.f; matrix_position[1][1] = 1.f; matrix_position[1][2] = 0.f; matrix_position[1][3] = position_y;
        matrix_position[2][0] = 0.f; matrix_position[2][1] = 0.f; matrix_position[2][2] = 1.f; matrix_position[2][3] = 0.f;
        matrix_position[3][0] = 0.f; matrix_position[3][1] = 0.f; matrix_position[3][2] = 0.f; matrix_position[3][3] = 1.f;

        float const c = cuckoo::maths::cos(angle);
        float const s = cuckoo::maths::sin(angle);
        alignas (16) float matrix_rotation[4][4];
        matrix_rotation[0][0] = c;   matrix_rotation[0][1] = -s;  matrix_rotation[0][2] = 0.f; matrix_rotation[0][3] = 0.f;
        matrix_rotation[1][0] = s;   matrix_rotation[1][1] = c;   matrix_rotation[1][2] = 0.f; matrix_rotation[1][3] = 0.f;
        matrix_rotation[2][0] = 0.f; matrix_rotation[2][1] = 0.f; matrix_rotation[2][2] = 1.f; matrix_rotation[2][3] = 0.f;
        matrix_rotation[3][0] = 0.f; matrix_rotation[3][1] = 0.f; matrix_rotation[3][2] = 0.f; matrix_rotation[3][3] = 1.f;

        alignas (16) float matrix_scale[4][4];
        matrix_scale[0][0] = scale_x; matrix_scale[0][1] = 0.f;     matrix_scale[0][2] = 0.f; matrix_scale[0][3] = 0.f;
        matrix_scale[1][0] = 0.f;     matrix_scale[1][1] = scale_y; matrix_scale[1][2] = 0.f; matrix_scale[1][3] = 0.f;
        matrix_scale[2][0] = 0.f;     matrix_scale[2][1] = 0.f;     matrix_scale[2][2] = 1.f; matrix_scale[2][3] = 0.f;
        matrix_scale[3][0] = 0.f;     matrix_scale[3][1] = 0.f;     matrix_scale[3][2] = 0.f; matrix_scale[3][3] = 1.f;

        alignas (16) float matrix_position_rotation[4][4];
        alignas (16) float matrix_model[4][4];


        // matrix_model = matrix_position * matrix_rotation * matrix_scale
        // Remember, matrix maths dictates that we multiply the matrices in the reverse of order of the desired transformation!
        // i.e. here we are performing the scaling FIRST, THEN the rotation and FINALLY the translation.
        matrix_multiply(matrix_position_rotation, matrix_position, matrix_rotation);
        matrix_multiply(matrix_model, matrix_position_rotation, matrix_scale);
        *(mat4*)matrix_model = cuckoo::maths::transpose(*(mat4*)matrix_model);


        spritebatch.draw(*tex_rect, (float*)matrix_model);


        /////////////////////////////////////////////////////////////////////////////////////
        //// <<< DO NOT EDIT CODE ABOVE - THIS CODE MUST BE IN YOUR TILE RENDER FUNCTION ////
        /////////////////////////////////////////////////////////////////////////////////////
    }


}

void tiles_t::on_collision(object_type_t other_type, void* other_data, pigeon::gfx::spritesheet spritesheet, int index)
{
    if (other_type == WALL_TYPE)
    {
        // 'other_data' is a wall of some kind
        // this tile has hit a wall, make the appropriate changes to this tile as a result of it
        collision_resolve_tile_wall(spritesheet, this, (wall_t*)other_data, index);
        
    }
    else if (other_type == PLAYER_TYPE)
    {
        // 'other_data' is a player

        is_eaten[index] = true; // mark this tile as 'eaten' and therefore requires replacing 
    }
}

object_id_t tiles_t::get_id() const
{
    return TILE_ID_NORMAL;
}

bool tiles_t::needs_replacing(int index)
{
    if (is_eaten[index])
    {
        return true;
    }
    else
    {
        return false;
    }
}


// GENERAL

void tiles_t::initialise_tile(int index) //initialises the replacement of any tiles that have been eaten. ONLY FOR ONE TILE. using the index to find out which tile needs replacing.
{
    is_eaten[index] = false;
    {
        position[index].x = random_getd(SCREEN_WIDTH / -2.0, SCREEN_WIDTH / 2.0);
        position[index].y = random_getd(SCREEN_HEIGHT / -2.0, SCREEN_HEIGHT / 2.0);

        direction[index].x = random_getd(-1.0, 1.0);
        direction[index].y = random_getd(-1.0, 1.0);
        double const magnitude = cuckoo::maths::sqrt(direction[index].x * direction[index].x + direction[index].y * direction[index].y);
        direction[index].x /= magnitude; // normalise direction
        direction[index].y /= magnitude;
        angle_radians[index] = (float)random_getd(0.0, cuckoo::maths::two_pi <double>());
    }
}

void initialise_tiles (tiles_t& tiles)
{
  tiles = replace_expired_tiles (tiles);
}

tiles_t replace_expired_tiles (tiles_t tiles)
{
  // The game requires that there are always active { NUM_TILES } on screen.
  // 1. iterate over tiles, remove tiles that need replacing
  // 2. replace removed tiles with new ones  

  // REMOVE OLD TILES
  // iterators provide a generic way to access the data at a particular element of a container
  // e.g. vectors, lists and maps - https://en.cppreference.com/w/cpp/container
  // iterators are 'special' in that they can be incremented to go to the next element in the collection
  // (even if it is not physically next to it in memory - https://en.cppreference.com/w/cpp/iterator)
    for (int i = 0; i < NUM_TILES; ++i)
    {
        if (tiles.needs_replacing(i))
            tiles.initialise_tile(i);
    }
    return tiles;
}

texture_rect const* get_tile_texture_rect(pigeon::gfx::spritesheet& spritesheet, object_id_t id)
{
    texture_rect const* rect = nullptr;

    if (id == TILE_ID_NORMAL)
    {
        rect = spritesheet.get_sprite_info("tile_0.png");
    }

    return rect;
}
