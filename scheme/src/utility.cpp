#include "utility.hpp"

#include <iomanip>
#include <openssl/rand.h>
#include <random>
#include <vector>

namespace DCPE
{
	using namespace std;

	bytes get_random_bytes(const number size)
	{
		uchar material[size];
#if defined(TESTING) || defined(DEBUG)
		for (number i = 0; i < size; i++)
		{
			material[i] = (uchar)rand();
		}
#else
		RAND_bytes(material, size);
#endif
		return bytes(material, material + size);
	}

	number get_ramdom_number(const number max)
	{
		number material[1];
#if defined(TESTING) || defined(DEBUG)
		auto intMaterial = (int *)material;
		intMaterial[0]	 = rand();
		intMaterial[1]	 = rand();
#else
		RAND_bytes((uchar *)material, sizeof(number));
#endif
		return material[0] % max;
	}
}
