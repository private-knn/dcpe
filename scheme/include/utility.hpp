#pragma once

#include "definitions.h"

namespace DCPE
{
	using namespace std;

	/**
	 * @brief generate an array of bytes pseudorandomly
	 *
	 * \note
	 * It uses OpenSSL PRG unless TESTING macro is defined.
	 * If it is, C++ standard rand() is used (easy for testing and debugging).
	 *
	 * @param size the number of bytes to generate
	 * @return bytes the resulting bytes
	 */
	bytes get_random_bytes(const number size);

	/**
	 * @brief returns a pseudorandom number
	 *
	 * @param max the non-inclusive max of the range (min is inclusive 0).
	 * @return number the resulting number
	 */
	number get_ramdom_number(const number max);
}
