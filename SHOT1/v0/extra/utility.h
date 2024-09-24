#pragma once


struct vector4
{
  double x;
  double y;
  double z;
  double w;
};


/// @brief get a number between min and max (inclusive)
/// @param min minimum random number (inclusive)
/// @param max maximum random number (inclusive)
/// @return random number between min & max (inclusive)
double random_getd (double min, double max);

float convert_km_to_miles (float km);
