#pragma once

#include <boost/format.hpp>
#include <climits>
#include <openssl/evp.h>
#include <string>
#include <vector>

#define TO_ARRAY(x) &x[0]

namespace DCPE
{
	using ull	= unsigned long long;
	using uchar = unsigned char;
	using byte	= unsigned char;
	using uint	= unsigned int;
	using bytes = std::vector<byte>;

	template <typename VALUE_T>
	using key = std::tuple<ull, ull, VALUE_T>;

	/**
	 * @brief Primitive exception class that passes along the excpetion message
	 *
	 * Can consume std::string, C-string and boost::format
	 */
	class Exception : public std::exception
	{
		public:
		explicit Exception(const char* message) :
			msg_(message)
		{
		}

		explicit Exception(const std::string& message) :
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
		std::string msg_;
	};
}
