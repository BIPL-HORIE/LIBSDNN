//# Licensed under the BSD 3-Clause License - https://opensource.org/licenses/BSD-3-Clause

#include "LIBSDNN_PC.h"
#include "LIBSDNN_LEXIAL.h"
#include "LIBSDNN_UTILITY.h"
#include "LIBSDNN_PARAMETERS.h"

#include <vector>
#include <string>
#include <random>
#include <iostream>
#include <omp.h>
#include <algorithm>
namespace libsdnn
{
	namespace base
	{
		namespace pc
		{
			void MakeRandomPattern(std::vector<bool> &out_pattern, std::mt19937 &mt)
			{
				int n = static_cast<int>(out_pattern.size());
				if (n % 2)
					libsdnn::utility::error::BugFound(0x1ff0f);

				int random_buffer;

				std::vector<bool> pattern_buffer(n, 0);
				out_pattern.swap(pattern_buffer);

				std::uniform_int_distribution<int> kernel(0, n - 1);
				for (int i = 0; i < n / 2; i++)
				{
					do
					{
						random_buffer = kernel(mt);
					} while (out_pattern[random_buffer] != 0);
					out_pattern[random_buffer] = 1;
				}
			}

			void MakeRandomPattern(std::vector<bool> &out_pattern, const std::vector<bool> &material, const int r, std::mt19937 &mt)
			{
				if (out_pattern.size() != material.size())
				{
					utility::error::BugFound(0x1b0ff0a);
				}
				int n = static_cast<int>(out_pattern.size());
				std::uniform_int_distribution<int> kernel(0, n - 1);

				std::vector<bool> inverse_flag(n,0);
				out_pattern = material;

				for (int r_counter = 0; r_counter < r; r_counter++)
				{
					int random_buffer;
					do
					{
						random_buffer = kernel(mt);
					} while (inverse_flag[random_buffer] == 1 || out_pattern[random_buffer] == 0);
					out_pattern[random_buffer] = 0;
					inverse_flag[random_buffer] = 1;
					do
					{
						random_buffer = kernel(mt);
					} while (inverse_flag[random_buffer] == 1 || out_pattern[random_buffer] == 1);
					out_pattern[random_buffer] = 1;
					inverse_flag[random_buffer] = 1;
				}
			}
			
			void SavePattern(unsigned int *binary, const std::vector<bool> &pattern)
			{
				auto pattern_itr = pattern.begin();
				for (unsigned int n_counter = 0; n_counter < pattern.size() - 1; n_counter++)
				{
					binary[n_counter >> 5] += *pattern_itr++;
					if (n_counter % 32 != 31)
						binary[n_counter >> 5] <<= 1;
				}
				binary[(pattern.size() - 1) >> 5] += *pattern_itr;
			}
			double CalcCorrelation(const std::vector<bool> & pattern1, const std::vector<bool> & pattern2)
			{
				if (pattern1.size() != pattern2.size())
					return 0;
				int sum = 0;
				for (unsigned int i = 0; i < pattern1.size(); i++)
					sum += (pattern1[i] == pattern2[i])?1:-1;
				return (double)sum / pattern1.size();
			}

			int CalcDiffNum(const int n, const double p, std::mt19937 &mt)
			{
				std::uniform_real_distribution<double> udouble(0, 1);
				int ret = 0;
				for (int i = 0; i < n; i++)
					if (udouble(mt) < p)
						ret++;
				return ret;
			}

			std::string CorrelationMatrixFile2Strings(const std::string &filename)
			{
				std::string result_buffer = "";
				std::ifstream matrix_file;
				utility::OpenFile(matrix_file, filename);

				std::string line_buffer;
				while (std::getline(matrix_file, line_buffer))
				{
					std::replace(line_buffer.begin(), line_buffer.end(), ',', '$');
					result_buffer += line_buffer + "\\";
				}
				result_buffer.erase(--result_buffer.end());
				return result_buffer;
			}
		}
	}
}

libsdnn::base::SDNN_PC::SDNN_PC():n_(0),n32_(0)
{}

libsdnn::base::SDNN_PC::~SDNN_PC()
{
	for (auto binary_itr = binary_pattern_.begin(); binary_itr != binary_pattern_.end(); binary_itr++)
		delete[] *binary_itr;
}

void libsdnn::base::SDNN_PC::InitPC(const int n, const std::string &pattern_type, std::mt19937 &mt, const bool multi_core)
{
	//pattern_type
	//NUMERICAL(RANDOM_INVERSE(q,r))
	//NUMERICAL(CORRELATION_MATRIX(filename))
	//SYMBOL(RANDOM_INVERSE(q,r))
	//SYMBOL(CORRELATION_MATRIX(filename))

	std::vector<std::string> setting_buffer_vector;
	libsdnn::lexial::Split(setting_buffer_vector,pattern_type, '(');
	setting_buffer_vector[2].erase(--setting_buffer_vector[2].end());
	setting_buffer_vector[2].erase(--setting_buffer_vector[2].end());

	n_ = n;
	n32_ = ((n_ - 1) >> 5) + 1;

	//setting_buffer_vector = {NUMERICAL / RANDOM_INVERSE / q,r}
	//setting_buffer_vector = {NUMERICAL / CORRELATION_MATRIX / filename,batch_n,iteration,precision}

	if (setting_buffer_vector[1] == parameter_property::PARAM_SD_PC_TYPE::CNT_PC_METHOD_::random_inverse_)
	{
		std::vector<std::string> q_r;
		libsdnn::lexial::Split(q_r, setting_buffer_vector[2], ',');
		RandomInverse(stoi(q_r[0]), stoi(q_r[1]), mt);
	}
	else if (setting_buffer_vector[1] == parameter_property::PARAM_SD_PC_TYPE::CNT_PC_METHOD_::correlation_matrix_)
	{
		std::vector<std::string> f_b_i_p;
		libsdnn::lexial::Split(f_b_i_p, setting_buffer_vector[2], ',');
		CorrelationMatrix(f_b_i_p[0], stoi(f_b_i_p[1]), stoi(f_b_i_p[2]), stof(f_b_i_p[3]), mt, multi_core);
	}
	else if (setting_buffer_vector[1] == parameter_property::PARAM_SD_PC_TYPE::CNT_PC_METHOD_::saved_)
	{
		std::vector<std::string> f_b_i_p;
		libsdnn::lexial::Split(f_b_i_p, setting_buffer_vector[2], ',');

		std::vector<std::string> line_buffer;
		libsdnn::lexial::Split(line_buffer, f_b_i_p[0], '\\');
		std::vector<std::vector<double>> correlation_matrix;

		for (auto line_buffer_itr = line_buffer.begin(); line_buffer_itr != line_buffer.end(); line_buffer_itr++)
		{
			std::vector<std::string> string_buffer;
			lexial::Split(string_buffer, *line_buffer_itr, '$');
			if (string_buffer.size() <= 1)
				continue;
			std::vector<double> correlation_buffer;
			for (auto str_buf_itr = string_buffer.begin(); str_buf_itr != string_buffer.end(); str_buf_itr++)
				correlation_buffer.push_back(stof(*str_buf_itr));
			correlation_matrix.push_back(correlation_buffer);
		}
		CorrelationMatrix(correlation_matrix, stoi(f_b_i_p[1]), stoi(f_b_i_p[2]), stof(f_b_i_p[3]), mt, multi_core);
	}
	else
		utility::error::BugFound(0x1ff0a);
}

void libsdnn::base::SDNN_PC::RandomInverse(const int q, const int r, std::mt19937 &mt)
{
	utility::AllocateMemory(binary_pattern_, q, n32_);
	std::vector<bool> pattern(n_);
	std::vector<bool> previous_pattern(n_);

	auto binary_itr = binary_pattern_.begin();
	pc::MakeRandomPattern(pattern, mt);
	pc::SavePattern(*binary_itr++, pattern);
	for (;binary_itr != binary_pattern_.end(); binary_itr++)
	{
		previous_pattern = pattern;
		pc::MakeRandomPattern(pattern, previous_pattern, r, mt);
		pc::SavePattern(*binary_itr, pattern);
	}
}

void libsdnn::base::SDNN_PC::CorrelationMatrix(const std::vector<std::vector<double>> &correlation_matrix, const int batch_n, const int max_iteration, const double precision, std::mt19937 &mt, const bool multi_core)
{
	const int itr = n_ / batch_n;
	const int q = static_cast<int>(correlation_matrix.size());

	std::uniform_int_distribution<int> uint_q(0, q - 1);
	std::uniform_int_distribution<int> uint_n(0, batch_n - 1);

	std::vector<std::vector<bool>> final_pattern(q);
	for (int i = 0; i < q; i++)
	{
		final_pattern[i].resize(0);
	}

	std::vector<std::mt19937> mt_list(itr);
	std::uniform_int_distribution<int> seed_dist(0, 4294967295);

	for (int r = 0; r < itr; r++)
	{
		std::vector<unsigned int> random_seed;
		for (int i = 0; i < 10; i++)
			random_seed.insert(random_seed.begin(), seed_dist(mt)); //‡”Ô‚ð‚Ð‚Á‚­‚è•Ô‚·D
		std::seed_seq random_seed_seq(random_seed.begin(), random_seed.end());
		mt_list[r].seed(random_seed_seq);
	}

	if (multi_core)
	{
		std::vector<std::vector<std::vector<bool>>> pattern_strage(itr);

#ifndef _OPENMP
		std::cout << "not compiled with openmp" << std::endl;
#else
#pragma omp parallel for
#endif
		for (int r = 0; r < itr; r++)
		{
			int margin = 1;
			int margin_count = 0;
			std::vector<std::vector<bool>> pattern(q);
			for (int i = 0; i < q; i++)
			{
				pattern[i].resize(batch_n);
			}

			std::vector<int> list_of_init(q);
			std::vector<int> list_of_init_buffer(q);

		label1:

			for (int i = 0; i < q; i++)
				list_of_init_buffer[i] = 0;

			for (int i = 0; i < q; i++)
			{
				libsdnn::base::pc::MakeRandomPattern(pattern[i], mt_list[r]);
			}
			
			for (int i = 0; i < q; i++)
			{
				int c = 0;
				do
				{
					c = uint_q(mt_list[r]);
				} while (list_of_init_buffer[c] != 0);
				list_of_init[i] = c;
				list_of_init_buffer[c] = 1;
			}

			for (int i = 0; i < q; i++)
			{
				double now_correlation;
				double correlation_delta_sum;
				std::uniform_int_distribution<int>check(0, i);

				int iteration = 0;

				double correlation_sum = 0;
				for (int j = i; j >= 0; j--)
				{
					correlation_sum += abs(correlation_matrix[list_of_init[i]][list_of_init[j]]);
				}

				int k;
				int k2;
				int c_elem_num;
				do {
					int j = check(mt_list[r]);
					c_elem_num = base::pc::CalcDiffNum(batch_n, correlation_matrix[list_of_init[i]][list_of_init[j]], mt_list[r]);

					now_correlation = libsdnn::base::pc::CalcCorrelation(pattern[list_of_init[i]], pattern[list_of_init[j]]) * batch_n;
					now_correlation -= c_elem_num;

					do
					{
						if (now_correlation >= 2)
						{
							do
							{
								k = uint_n(mt_list[r]);
							} while (pattern[list_of_init[i]][k] != pattern[list_of_init[j]][k] || pattern[list_of_init[i]][k] == 0);
							do
							{
								k2 = uint_n(mt_list[r]);
							} while (pattern[list_of_init[i]][k2] != pattern[list_of_init[j]][k2] || pattern[list_of_init[i]][k2] == 1);
							pattern[list_of_init[i]][k] = 0;
							pattern[list_of_init[i]][k2] = 1;
						}
						else if (now_correlation <= -2)
						{
							do
							{
								k = uint_n(mt_list[r]);
							} while (pattern[list_of_init[i]][k] == pattern[list_of_init[j]][k] || pattern[list_of_init[i]][k] == 0);
							do
							{
								k2 = uint_n(mt_list[r]);
							} while (pattern[list_of_init[i]][k2] == pattern[list_of_init[j]][k2] || pattern[list_of_init[i]][k2] == 1);
							pattern[list_of_init[i]][k] = 0;
							pattern[list_of_init[i]][k2] = 1;
						}
						now_correlation = libsdnn::base::pc::CalcCorrelation(pattern[list_of_init[i]], pattern[list_of_init[j]]) * batch_n;
						now_correlation -= c_elem_num;
					} while (abs(now_correlation) > 2);

					correlation_delta_sum = 0;
					for (int j = i; j >= 0; j--)
					{
						correlation_delta_sum += abs(correlation_matrix[list_of_init[i]][list_of_init[j]] * ((libsdnn::base::pc::CalcCorrelation(pattern[list_of_init[i]], pattern[list_of_init[j]]) - correlation_matrix[list_of_init[i]][list_of_init[j]])));
					}
					correlation_delta_sum /= correlation_sum;
					iteration++;
				} while (correlation_delta_sum > precision * margin && iteration < max_iteration);

				if (iteration >= max_iteration)
				{
					margin_count++;
					if (margin_count >= 3)
					{
						margin_count = 0;
						margin++;
					}
					goto label1;
				}
				
			}
			pattern_strage[r] = pattern;
		}
		for (int r = 0; r < itr; r++)
		{
			for (int i = 0; i < q; i++)
				final_pattern[i].insert(final_pattern[i].end(), pattern_strage[r][i].begin(), pattern_strage[r][i].end());
		}
	}
	else
	{
		for (int r = 0; r < itr; r++)
		{
			int margin = 1;
			int margin_count = 0;
			std::vector<std::vector<bool>> pattern(q);
			for (int i = 0; i < q; i++)
			{
				pattern[i].resize(batch_n);
			}

			std::vector<int> list_of_init(q);
			std::vector<int> list_of_init_buffer(q);

		label2:

			for (int i = 0; i < q; i++)
				list_of_init_buffer[i] = 0;

			for (int i = 0; i < q; i++)
			{
				libsdnn::base::pc::MakeRandomPattern(pattern[i], mt_list[r]);
			}

			for (int i = 0; i < q; i++)
			{
				int c = 0;
				do
				{
					c = uint_q(mt_list[r]);
				} while (list_of_init_buffer[c] != 0);
				list_of_init[i] = c;
				list_of_init_buffer[c] = 1;
			}

			for (int i = 0; i < q; i++)
			{
				double now_correlation;
				double correlation_delta_sum;
				std::uniform_int_distribution<int>check(0, i);

				int iteration = 0;

				double correlation_sum = 0;
				for (int j = i; j >= 0; j--)
				{
					correlation_sum += abs(correlation_matrix[list_of_init[i]][list_of_init[j]]);
				}

				int k;
				int k2;
				int c_elem_num;
				do {
					int j = check(mt_list[r]);
					c_elem_num = base::pc::CalcDiffNum(batch_n, correlation_matrix[list_of_init[i]][list_of_init[j]], mt_list[r]);

					now_correlation = libsdnn::base::pc::CalcCorrelation(pattern[list_of_init[i]], pattern[list_of_init[j]]) * batch_n;
					now_correlation -= c_elem_num;

					do
					{
						if (now_correlation >= 2)
						{
							do
							{
								k = uint_n(mt_list[r]);
							} while (pattern[list_of_init[i]][k] != pattern[list_of_init[j]][k] || pattern[list_of_init[i]][k] == 0);
							do
							{
								k2 = uint_n(mt_list[r]);
							} while (pattern[list_of_init[i]][k2] != pattern[list_of_init[j]][k2] || pattern[list_of_init[i]][k2] == 1);
							pattern[list_of_init[i]][k] = 0;
							pattern[list_of_init[i]][k2] = 1;
						}
						else if (now_correlation <= -2)
						{
							do
							{
								k = uint_n(mt_list[r]);
							} while (pattern[list_of_init[i]][k] == pattern[list_of_init[j]][k] || pattern[list_of_init[i]][k] == 0);
							do
							{
								k2 = uint_n(mt_list[r]);
							} while (pattern[list_of_init[i]][k2] == pattern[list_of_init[j]][k2] || pattern[list_of_init[i]][k2] == 1);
							pattern[list_of_init[i]][k] = 0;
							pattern[list_of_init[i]][k2] = 1;
						}
						now_correlation = libsdnn::base::pc::CalcCorrelation(pattern[list_of_init[i]], pattern[list_of_init[j]]) * batch_n;
						now_correlation -= c_elem_num;
					} while (abs(now_correlation) > 2);

					correlation_delta_sum = 0;
					for (int j = i; j >= 0; j--)
					{
						correlation_delta_sum += abs(correlation_matrix[list_of_init[i]][list_of_init[j]] * ((libsdnn::base::pc::CalcCorrelation(pattern[list_of_init[i]], pattern[list_of_init[j]]) - correlation_matrix[list_of_init[i]][list_of_init[j]])));
					}
					correlation_delta_sum /= correlation_sum;
					iteration++;
				} while (correlation_delta_sum > precision * margin && iteration < max_iteration);

				if (iteration >= max_iteration)
				{
					margin_count++;
					if (margin_count >= 3)
					{
						margin_count = 0;
						margin++;
					}
					goto label2;
				}
			}
			for (int i = 0; i < q; i++)
				final_pattern[i].insert(final_pattern[i].end(), pattern[i].begin(), pattern[i].end());
		}
	}

	utility::AllocateMemory(binary_pattern_, q, n32_);

	auto pattern_list_itr = final_pattern.begin();
	for (auto binary_itr = binary_pattern_.begin(); binary_itr != binary_pattern_.end(); binary_itr++)
	{
		pc::SavePattern(*binary_itr, *pattern_list_itr++);
	}

	double average_prec = 0;
	for (unsigned int i = 0; i < final_pattern.size(); i++)
	{
		for (unsigned int j = 0; j < final_pattern.size(); j++)
		{
			average_prec += pow(correlation_matrix[i][j] - pc::CalcCorrelation(final_pattern[i], final_pattern[j]), 2);
		}
	}
	average_prec /= pow(final_pattern.size(), 2);
	std::cout << "\taverage_precision = " << sqrt(average_prec) << std::endl;
}

void libsdnn::base::SDNN_PC::CorrelationMatrix(const std::string &correlation_matrix_filename, const int batch_n, const int max_iteration, const double precision, std::mt19937 &mt, const bool multi_core)
{
	std::ifstream ifs;
	std::string buffer;
	std::vector<std::string> string_buffer;
	std::vector<std::vector<double>> correlation_matrix;

	std::string matrix_filename = correlation_matrix_filename;

	bool is_correct_matrix = true;
	do {
		utility::OpenFile(ifs, matrix_filename);
		while (std::getline(ifs, buffer))
		{
			lexial::DeleteCharactor(buffer, " \t");
			lexial::Split(string_buffer, buffer, ',');
			if (string_buffer.size() <= 1)
				continue;
			std::vector<double> correlation_buffer;
			for (auto str_buf_itr = string_buffer.begin(); str_buf_itr != string_buffer.end(); str_buf_itr++)
				correlation_buffer.push_back(stof(*str_buf_itr));
			correlation_matrix.push_back(correlation_buffer);
		}

		for (int i = 0; i < correlation_matrix.size(); i++)
		{
			for (int j = i; j >= 0; j--)
			{
				if (i == j)
					if (correlation_matrix[i][i] != 1)
						is_correct_matrix = false;
				else
					if (correlation_matrix[i][j] != correlation_matrix[j][i])
						is_correct_matrix = false;
				if (correlation_matrix[i][j] < -1 || correlation_matrix[i][j] > 1)
					is_correct_matrix = false;
			}
		}

		if (!is_correct_matrix)
		{
			ifs.close();
			std::cout << "This Correlation Matrix can not be reproducted." << std::endl;
			std::cout << "Type a correct file_name: ";
			std::cin >> matrix_filename;
			std::cin.ignore();
			is_correct_matrix = true;
			continue;
		}
	} while (!is_correct_matrix);
	ifs.close();

	CorrelationMatrix(correlation_matrix, batch_n, max_iteration, precision, mt, multi_core);
}