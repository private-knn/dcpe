#pragma once

#include <boost/format.hpp>
#include <climits>
#include <openssl/evp.h>
#include <string>
#include <vector>

// change to run all tests from different seed
#define TEST_SEED 0x13

#define TO_ARRAY(x) &x[0]

#define VALUE_T float

namespace DCPE
{
	using namespace std;

	// defines the integer type block ID
	// change (e.g. to unsigned int) if needed
	using number = unsigned long long;
	using uchar	 = unsigned char;
	using byte	 = unsigned char;
	using uint	 = unsigned int;
	using bytes	 = vector<byte>;

	using prf_key  = EVP_PKEY*;
	using prf_keys = pair<prf_key, prf_key>;

	using key = pair<prf_key, number>;

	/**
	 * @brief Primitive exception class that passes along the excpetion message
	 *
	 * Can consume std::string, C-string and boost::format
	 */
	class Exception : public exception
	{
		public:
		explicit Exception(const char* message) :
			msg_(message)
		{
		}

		explicit Exception(const string& message) :
			msg_(message)
		{
		}

		explicit Exception(const boost::format& message) :
			msg_(boost::str(message))
		{
		}

		virtual ~Exception() throw() {}

		virtual const char* what() const throw()
		{
			return msg_.c_str();
		}

		protected:
		string msg_;
	};
}
