#include "scheme.hpp"

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <faiss/IndexFlat.h>
#include <faiss/IndexIVFPQ.h>
#include <faiss/index_io.h>
#include <iomanip>
#include <iostream>
#include <random>
#include <sys/time.h>

using namespace std;

float rand_float(float max)
{
	return static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / max));
}

int main()
{
	const int dimensions	 = 4;
	const int data_size		 = 10;
	const int query_size	 = 10;
	const float max_float	 = 100.0;
	const int k				 = 4;
	const int seed			 = 0x1305;
	const DCPE::number beta	 = 100;
	const DCPE::number max_s = 10000;

	srand(seed);
	cout << setprecision(2) << fixed;

	float *data		   = new float[dimensions * data_size];
	float *queries	   = new float[dimensions * query_size];
	float *data_enc	   = new float[dimensions * data_size];
	float *queries_enc = new float[dimensions * query_size];

	auto scheme = make_unique<DCPE::Scheme>(beta, max_s);
	auto key	= scheme->keygen();

	for (auto &&plaintext : {true, false})
	{
		cout << (plaintext ? "PLAINTEXT" : "ENCRYPTED") << endl;

		for (int i = 0; i < data_size; i++)
		{
			vector<double> record(data + dimensions * i, data + dimensions * (i + 1));
			auto encrypted = scheme->encrypt(key, record);

			for (int j = 0; j < dimensions; j++)
			{
				if (plaintext)
				{
					data[dimensions * i + j] = rand_float(max_float);
				}
				else
				{
					data_enc[dimensions * i + j] = encrypted.first[j];
				}

				cout << setw(9) << (plaintext ? data : data_enc)[dimensions * i + j] << " ";
			}
			if (plaintext)
			{
				data[dimensions * i] += i / 1000.;
			}

			cout << endl;
		}

		for (int i = 0; i < query_size; i++)
		{
			vector<double> query(queries + dimensions * i, queries + dimensions * (i + 1));
			auto encrypted = scheme->encrypt(key, query);

			for (int j = 0; j < dimensions; j++)
			{
				if (plaintext)
				{
					queries[dimensions * i + j] = rand_float(max_float);
				}
				else
				{
					queries_enc[dimensions * i + j] = encrypted.first[j];
				}
			}
			if (plaintext)
			{
				queries[dimensions * i] += i / 1000.;
			}
		}

		faiss::IndexFlatL2 index(dimensions);
		index.add(data_size, plaintext ? data : data_enc);
		{
			faiss::Index::idx_t *I = new faiss::Index::idx_t[k * 5];
			float *D			   = new float[k * 5];
			index.search(5, plaintext ? data : data_enc, k, D, I);
			cout << "I =" << endl;
			for (int i = 0; i < 5; i++)
			{
				for (int j = 0; j < k; j++)
				{
					cout << setw(5) << I[i * k + j] << " ";
				}
				cout << endl;
			}
			delete[] I;
			delete[] D;
		}
		{
			faiss::Index::idx_t *I = new faiss::Index::idx_t[k * query_size];
			float *D			   = new float[k * query_size];
			index.search(query_size, plaintext ? queries : queries_enc, k, D, I);
			cout << "A =" << endl;
			for (int i = 0; i < query_size; i++)
			{
				for (int j = 0; j < k; j++)
				{
					cout << setw(5) << I[i * k + j] << " ";
				}
				cout << endl;
			}
			delete[] I;
			delete[] D;
		}
	}

	return 0;
}
