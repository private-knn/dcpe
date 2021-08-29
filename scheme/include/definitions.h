#pragma once

#include <boost/format.hpp>
#include <climits>
#include <string>
#include <vector>

// use 256-bit security
#define KEYSIZE 32

// change to run all tests from different seed
#define TEST_SEED 0x13

#define TO_ARRAY(x) &x[0]

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

	enum EncryptionMode
	{
		ENCRYPT,
		DECRYPT
	};

	enum BlockCipherMode
	{
		CBC,
		CTR,
		NONE
	};

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

	/**
	 * @brief global setting, block cipher mode which will be used for encryption
	 *
	 */
	inline BlockCipherMode __blockCipherMode = CBC;
}