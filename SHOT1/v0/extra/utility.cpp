#include "utility.h"

#include "cuckoo/core/asserts.h" // for CUCKOO_ASSERT

#include <cstdlib>               // for rand


double random_getd (double min, double max)
{
  CUCKOO_ASSERT (max > min);


  double const random = (double)rand () / (double)RAND_MAX;
  double const range = max - min;
  return (random * range) + min;
}

float convert_km_to_miles (float km) { return km * 0.621371f; }
