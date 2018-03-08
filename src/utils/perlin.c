#include "perlin.h"

// Random values to determine gradients from seed
static int hash[] = {
  208,34,231,213,32,248,233,56,161,78,24,140,71,48,140,254,245,255,247,247,40,
  185,248,251,245,28,124,204,204,76,36,1,107,28,234,163,202,224,245,128,167,204,
  9,92,217,54,239,174,173,102,193,189,190,121,100,108,167,44,43,77,180,204,8,81,
  70,223,11,38,24,254,210,210,177,32,81,195,243,125,8,169,112,32,97,53,195,13,
  203,9,47,104,125,117,114,124,165,203,181,235,193,206,70,180,174,0,167,181,41,
  164,30,116,127,198,245,146,87,224,149,206,57,4,192,210,65,210,129,240,178,105,
  228,108,245,148,140,40,35,195,38,58,65,207,215,253,65,85,208,76,62,3,237,55,89,
  232,50,217,64,244,157,199,121,252,90,17,212,203,149,152,140,187,234,177,73,174,
  193,100,192,143,97,53,145,135,19,103,13,90,135,151,199,91,239,247,33,39,145,
  101,120,99,3,186,86,99,41,237,203,111,79,220,135,158,42,30,154,120,67,87,167,
  135,176,183,191,253,115,184,21,233,58,129,233,142,39,128,211,118,137,139,255,
  114,20,218,113,154,27,127,246,250,1,8,198,250,209,92,222,173,21,88,102,219
};
int getRand(int x, int y, int seed) {
  return hash[(hash[(seed + y) % 256] + x) % 256];
}

// Compute dot product of a and b
float dotProd(float a[], float b[], int length) {
  float result = 0;
  for (int i = 0; i < length; i ++) {
    result += a[i] * b[i];
  }
  return result;
}

// Linear interploation
float linInter(float a, float b, float s) {
  return a + (b-a) * s;
}

// Interpolate using perlins polynomial
float polyInter(float a, float b, float s) {
  return linInter(a, b, 6 * s*s*s*s*s - 15 * s*s*s*s + 10 * s*s*s);
}

// Compute perlin noise at position
float perlin(int x, int y, float freq, int seed) {
  float gradient_set[4][2] = {
    { 1, 1},
    {-1, 1},
    {-1,-1},
    { 1,-1},
  };
  // Determine the values of the distance vectors
  float s_x = (float)(x - x % (int)(1/freq));
  float s_y = (float)(y - y % (int)(1/freq));
  float distances[4][2] = {
    {((float)x - s_x) * freq, ((float)y - s_y) * freq},
    {((float)x - s_x - (1/freq)) * freq, ((float)y - s_y) * freq},
    {((float)x - s_x) * freq, ((float)y - s_y - (1/freq)) * freq},
    {((float)x - s_x - (1/freq)) * freq, ((float)y - s_y - (1/freq)) * freq},
  };
  // Determine gradients (this is quite poor pseudo-randomness...)
  int gradients[4] = {
    getRand((int)s_x, (int)s_y, seed) % 4,
    getRand((int)(s_x + (1/freq)), (int)s_y, seed) % 4,
    getRand((int)s_x, (int)(s_y + (1/freq)), seed) % 4,
    getRand((int)(s_x + (1/freq)), (int)(s_y + (1/freq)), seed) % 4,
  };
  // Determine the value of the noise
  return polyInter(
    polyInter(
      dotProd(gradient_set[gradients[0]], distances[0], 2),
      dotProd(gradient_set[gradients[1]], distances[1], 2),
      ((float)x - s_x) * freq),
    polyInter(
      dotProd(gradient_set[gradients[2]], distances[2], 2),
      dotProd(gradient_set[gradients[3]], distances[3], 2),
      ((float)x - s_x) * freq),
    ((float)y - s_y) * freq
  );
}
