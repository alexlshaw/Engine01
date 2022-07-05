#include "Noise.h"

//fancy little deterministic pseudorandom number generator
float getNoiseHeight(int x, int y)
{
	int n = x + y * (PRIME_1 + worldSeed);
	n = (n<<13) ^ n;	//bitwise xor n with itself left shifted a bunch
	return 1.0f - (float)((n * (n * n * PRIME_2 + PRIME_3) + PRIME_4) & 0x7fffffff) / PRIME_5;
};

float smoothNoise(int x, int y)
{
	float corners = (getNoiseHeight(x - 1, y - 1) + getNoiseHeight(x + 1, y - 1) + 
					 getNoiseHeight(x - 1, y + 1) + getNoiseHeight(x + 1, y + 1)) / 16.0f;
	float sides = (getNoiseHeight(x - 1, y) + getNoiseHeight(x + 1, y) +
				   getNoiseHeight(x, y - 1) + getNoiseHeight(x, y + 1)) / 8.0f;
	float centre = getNoiseHeight(x, y) / 4.0f;
	return corners + sides + centre;
}

//weird results when passing over integer values
float getInterpolatedNoise(float x, float y)
{
	int iX = (int)x;
	int iY = (int)y;
	float fractionX = x - (float)iX;
	float fractionY = y - (float)iY;
	float v1 = smoothNoise(iX, iY);	//corner to the lower left of the point we are interested in
	float v2 = smoothNoise(iX + 1, iY);	//corner to the lower right
	float v3 = smoothNoise(iX, iY + 1);	//upper left
	float v4 = smoothNoise(iX + 1, iY + 1);	//upper right
	float i1 = interpolateCosine(v1, v2, fractionX);
	float i2 = interpolateCosine(v3, v4, fractionX);
	return interpolateCosine(i1, i2, fractionY);
}

float PerlinNoise2D(float x, float y)
{
	float frequency = 0.25f;
	float amplitude = 1.0f;
	return PerlinNoise2D(x, y, frequency, amplitude);
}

float PerlinNoise2D(float x, float y, float frequency, float amplitude)
{
	float total = getInterpolatedNoise(x * frequency, y * frequency) * amplitude;
	return total;
}

float interpolateLinear(float a, float b, float x)
{
	return (a * (1.0f - x)) + (b * x);
}

float interpolateCosine(float a, float b, float x)
{
	float f = (float)((1.0f - cos(x * PI)) * 0.5f);
	return (a * (1.0f - f)) + (b * f);
}

//Outputs a value in the range -1:1
float SimplexNoise2D(float x, float y, float frequency)
{
	//frequency /= 10.0f;
	return SimplexNoise2D(x * frequency, y * frequency);
}

//Outputs a value in the range -1:1
float SimplexNoise2D(float xin, float yin)
{
	float n0, n1, n2;

	float F2 = 0.5f * (sqrt(3.0f) - 1.0f);
	float s = (xin + yin) * F2; // Hairy factor for 2D
    int i = fastfloor(xin + s);
    int j = fastfloor(yin + s);

	float G2 = (3.0f - sqrt(3.0f)) / 6.0f;
    float t = (i + j) * G2;
    float X0 = i - t; // Unskew the cell origin back to (x,y) space
    float Y0 = j - t;
    float x0 = xin - X0; // The x,y distances from the cell origin
    float y0 = yin - Y0;

	 // For the 2D case, the simplex shape is an equilateral triangle.
    // Determine which simplex we are in.
    int i1, j1; // Offsets for second (middle) corner of simplex in (i,j) coords
    if (x0 > y0) 
	{	// lower triangle, XY order: (0,0)->(1,0)->(1,1)
		i1=1; 
		j1=0;
	} 
    else 
	{	// upper triangle, YX order: (0,0)->(0,1)->(1,1)
		i1=0; 
		j1=1;
	}

	float x1 = x0 - i1 + G2; // Offsets for middle corner in (x,y) unskewed coords
    float y1 = y0 - j1 + G2;
    float x2 = x0 - 1.0f + 2.0f * G2; // Offsets for last corner in (x,y) unskewed coords
    float y2 = y0 - 1.0f + 2.0f * G2;

	int ii = i & 255;
    int jj = j & 255;
    int gi0 = perm[ii + perm[jj]] % 12;
    int gi1 = perm[ii + i1+perm[jj+j1]] % 12;
    int gi2 = perm[ii + 1 + perm[jj+1]] % 12;

	// Calculate the contribution from the three corners
    float t0 = 0.5f - x0 * x0 - y0 * y0;
    if (t0 < 0)
	{
		n0 = 0.0f;
	}
    else 
	{
		t0 *= t0;
		n0 = t0 * t0 * dot(grad3[gi0], x0, y0);  // (x,y) of grad3 used for 2D gradient
    }

	float t1 = 0.5f - x1 * x1 - y1 * y1;
    if(t1 < 0) 
	{
		n1 = 0.0f;
	}
    else 
	{
		t1 *= t1;
		n1 = t1 * t1 * dot(grad3[gi1], x1, y1);
    }

	float t2 = 0.5f - x2 * x2 - y2 * y2;
    if(t2 < 0) 
	{
		n2 = 0.0f;
	}
    else 
	{
		t2 *= t2;
		n2 = t2 * t2 * dot(grad3[gi2], x2, y2);
    }

	 return 70.0f * (n0 + n1 + n2);
}

int fastfloor(float x)
{
	return x > 0 ? (int)x : (int)x - 1;
}

float dot(glm::vec3 g, float x, float y)
{
	return g.x * x + g.y * y;
}

void initSimplexNoise()
{
	grad3.push_back(glm::vec3(1,1,0));
	grad3.push_back(glm::vec3(-1, 1, 0));
	grad3.push_back(glm::vec3(1, -1, 0));
	grad3.push_back(glm::vec3(-1, -1, 0));
	grad3.push_back(glm::vec3(1, 0, 1));
	grad3.push_back(glm::vec3(-1, 0, 1));
	grad3.push_back(glm::vec3(1, 0, -1));
	grad3.push_back(glm::vec3(-1, 0, -1));
	grad3.push_back(glm::vec3(0, 1, 1));
	grad3.push_back(glm::vec3(0, -1, 1));
	grad3.push_back(glm::vec3(0, 1, -1));
	grad3.push_back(glm::vec3(0, -1, -1));

	for (int i = 0; i < 512; i++) perm[i] = p[i & 255];
}

//mersenne methods

unsigned long RandomVal ()
{
	int	kk;
	unsigned long y; 
  
	if (k == N_) {
		for (kk = 0; kk < N_ - M_; kk++) 
		{
			y = (ptgfsr[kk] & UPPER_MASK) | (ptgfsr[kk + 1] & LOWER_MASK);
			ptgfsr[kk] = ptgfsr[kk + M_] ^ (y >> 1) ^ mag01[y & 0x1];
		}
		for (; kk < N_ - 1; kk++) 
		{
			y = (ptgfsr[kk] & UPPER_MASK) | (ptgfsr[kk + 1] & LOWER_MASK);
			ptgfsr[kk] = ptgfsr[kk + (M_ - N_)] ^ (y >> 1) ^ mag01[y & 0x1];
		}
		y = (ptgfsr[N_ - 1] & UPPER_MASK) | (ptgfsr[0] & LOWER_MASK);
		ptgfsr[N_ - 1] = ptgfsr[M_ - 1] ^ (y >> 1) ^ mag01[y & 0x1];
		k = 0;
	}
	y = ptgfsr[k++];
	y ^= TEMPERING_SHIFT_U (y);
	y ^= TEMPERING_SHIFT_S (y) & TEMPERING_MASK_B;
	y ^= TEMPERING_SHIFT_T (y) & TEMPERING_MASK_C;
	return y ^= TEMPERING_SHIFT_L (y);
}

unsigned long RandomVal (int range)
{
	return range ? (RandomVal () % range) : 0;
}

//returns a float between 0 and 1
float RandomFloat ()
{
	return (float)(RandomVal(10000)) / 10000.0f;
}

void RandomInit(unsigned long seed)
{
	mag01[0] = 0;
	mag01[1] = MATRIX_A;
	ptgfsr[0] = seed;
	for (k = 1; k < N_; k++)
	{
		ptgfsr[k] = 69069 * ptgfsr[k - 1];
	}
	k = 1;
}

void GenerateNoiseMap(float* values, int width, int height)
{
	for (int x = 0; x < width; x++)
	{
		for (int y = 0; y < height; y++)
		{
			float v = RandomFloat();
			values[x + (width * y)] = v;
		}
	}
}