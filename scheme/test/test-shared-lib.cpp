#include "scheme.hpp"

#include <cmath>
#include <iostream>

int main()
{
	const auto BETA = 5.0;

	std::cout << "Running the scheme using compiled shared library..." << std::endl;

	std::cout << "BETA: " << BETA << std::endl;

	std::vector<float> a = {0.0, 40.0};
	std::vector<float> b = {30.0, 0.0};
	std::vector<float> c = {-30.0, 0.0};

	auto distance = [](std::vector<float> x, std::vector<float> y)
	{
		return sqrt((x[0] - y[0]) * (x[0] - y[0]) + (x[1] - y[1]) * (x[1] - y[1]));
	};

	std::cout << "Distance a<->b: " << distance(a, b) << std::endl;
	std::cout << "Distance a<->c: " << distance(a, c) << std::endl;
	std::cout << "Distance b<->c: " << distance(b, c) << std::endl;

	auto scheme = std::make_unique<DCPE::Scheme<float>>(BETA);
	auto key	= scheme->keygen();
	std::cout << "key: " << std::get<0>(key) << " " << std::get<1>(key) << " " << std::get<2>(key) << std::endl;

	std::vector<float> a_enc, b_enc, c_enc;
	a_enc.resize(2);
	b_enc.resize(2);
	c_enc.resize(2);

	scheme->encrypt(key, TO_ARRAY(a), 2, TO_ARRAY(a_enc));
	scheme->encrypt(key, TO_ARRAY(b), 2, TO_ARRAY(b_enc));
	scheme->encrypt(key, TO_ARRAY(c), 2, TO_ARRAY(c_enc));

	std::cout << "a_enc: {" << a_enc[0] << ", " << a_enc[1] << "}" << std::endl;
	std::cout << "b_enc: {" << b_enc[0] << ", " << b_enc[1] << "}" << std::endl;
	std::cout << "c_enc: {" << c_enc[0] << ", " << c_enc[1] << "}" << std::endl;

	std::cout << "Distance a_enc<->b_enc: " << distance(a_enc, b_enc) << std::endl;
	std::cout << "Distance a_enc<->c_enc: " << distance(a_enc, c_enc) << std::endl;
	std::cout << "Distance b_enc<->c_enc: " << distance(b_enc, c_enc) << std::endl;

	std::cout << "Done!" << std::endl;

	return 0;
}
