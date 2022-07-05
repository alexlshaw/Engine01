#ifndef ENGINE01_NOISE_H
#define ENGINE01_NOISE_H
#include <windows.h>
#include <math.h>
#include <vector>
#include "glm\glm.hpp"

#include "Math.h"
#include "Settings.h"	//just included for the world seed

//Mersenne Twister stuff
#define LOWER_MASK            0x7fffffff 
#define M_                     397
#define MATRIX_A              0x9908b0df 
#define N_                     624
#define TEMPERING_MASK_B      0x9d2c5680
#define TEMPERING_MASK_C      0xefc60000
#define TEMPERING_SHIFT_L(y)  (y >> 18)
#define TEMPERING_SHIFT_S(y)  (y << 7)
#define TEMPERING_SHIFT_T(y)  (y << 15)
#define TEMPERING_SHIFT_U(y)  (y >> 11)
#define UPPER_MASK            0x80000000 

//"Perlin" stuff (it's not actually perlin)
#define PRIME_1 57
#define PRIME_2 15731
#define PRIME_3 789221
#define PRIME_4 1376312589
#define PRIME_5 1073741824.0f

//simplex stuff
static std::vector<glm::vec3> grad3;

const static int p[] = {151,160,137,91,90,15,
  131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,8,99,37,240,21,10,23,
  190, 6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,35,11,32,57,177,33,
  88,237,149,56,87,174,20,125,136,171,168, 68,175,74,165,71,134,139,48,27,166,
  77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,55,46,245,40,244,
  102,143,54, 65,25,63,161, 1,216,80,73,209,76,132,187,208, 89,18,169,200,196,
  135,130,116,188,159,86,164,100,109,198,173,186, 3,64,52,217,226,250,124,123,
  5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,189,28,42,
  223,183,170,213,119,248,152, 2,44,154,163, 70,221,153,101,155,167, 43,172,9,
  129,22,39,253, 19,98,108,110,79,113,224,232,178,185, 112,104,218,246,97,228,
  251,34,242,193,238,210,144,12,191,179,162,241, 81,51,145,235,249,14,239,107,
  49,192,214, 31,181,199,106,157,184, 84,204,176,115,121,50,45,127, 4,150,254,
  138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180};

static int perm[512];

void initSimplexNoise();

int fastfloor(float x);

float dot(glm::vec3 g, float x, float y);

float getNoiseHeight(float x, float y);
float smoothNoise(int x, int y);
float getInterpolatedNoise(float x, float y);
float PerlinNoise2D(float x, float y);
float PerlinNoise2D(float x, float y, float frequency, float amplitude);
float interpolateLinear(float a, float b, float x);
float interpolateCosine(float a, float b, float x);

float SimplexNoise2D(float x, float y, float frequency);
float SimplexNoise2D(float xin, float yin);


static int              k = 1;
static unsigned long    mag01[2] = {0x0, MATRIX_A};
static unsigned long    ptgfsr[N_];

unsigned long RandomVal();
unsigned long RandomVal(int range);
float RandomFloat();
void RandomInit(unsigned long seed);

void GenerateNoiseMap(float* values, int width, int height);

#endif