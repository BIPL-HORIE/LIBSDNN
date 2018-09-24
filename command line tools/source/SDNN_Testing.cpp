//# Licensed under the BSD 3-Clause License - https://opensource.org/licenses/BSD-3-Clause

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <LIBSDNN.h>
#include <LIBSDNN_PARAMETERS.h>
#include <LIBSDNN_UTILITY.h>
#pragma comment(lib,"LIBSDNN.lib")

void file2sample(const std::string &filename, std::vector<std::vector<double>> &input, std::vector<double> &target)
{
	std::ifstream train_sample;

	libsdnn::utility::OpenFile(train_sample, filename);

	std::string buffer;
	while (std::getline(train_sample, buffer, '\n'))
	{
		std::string buffer2nd;
		std::stringstream ss(buffer);
		std::getline(ss, buffer2nd, ',');
		target.push_back(std::stod(buffer2nd));
		std::vector<double> input_buffer;
		while (std::getline(ss, buffer2nd, ','))
		{
			input_buffer.push_back(std::stod(buffer2nd));
		}
		input.push_back(input_buffer);
	}
}

int main(int argc, char* argv[])
{
	if (argc != 4)
	{
		std::cout << "command error" << std::endl;
		return 0;
	}
	libsdnn::SDNN sdnn;

	sdnn.Load(argv[1]);

	std::vector<double> target;
	std::vector<std::vector<double>> input;
	file2sample(argv[2], input, target);

	std::ofstream output_file;
	libsdnn::utility::OpenFile(output_file, argv[3]);

	std::string nn_type;
	sdnn.GetParameter(nn_type, libsdnn::parameter_property::PARAM_SDNN_TYPE::property_.name_);

	std::cout << "progress" << std::endl;

	if (nn_type == libsdnn::parameter_property::PARAM_SDNN_TYPE::CNT_SDNN_TYPE_::function_approximation_)
	{
		double rmse = 0;
		double result;
		int last = -1;
		for (int i = 0; i < input.size(); i++)
		{
			result = sdnn.Estimate(input[i]);
			rmse += pow(result - target[i], 2);
			output_file << target[i] << "," << result << std::endl;
			if (last != i / (static_cast<int>(input.size()) / 10))
			{
				last = i / (static_cast<int>(input.size()) / 10);
				int j;
				std::cout << "\t\t\r[";
				for (j = 0; j < last; j++)
					std::cout << "*";
				for (; j < 10; j++)
					std::cout << " ";
				std::cout << "]\r";
			}
		}
		rmse /= input.size();
		rmse = sqrt(rmse);
		std::cout << "rmse = " << rmse << std::endl;
	}
	else if (nn_type == libsdnn::parameter_property::PARAM_SDNN_TYPE::CNT_SDNN_TYPE_::pattern_recognition_)
	{
		int correct = 0;
		double result;
		int last = -1;
		for (int i = 0; i < input.size(); i++)
		{
			result = sdnn.Estimate(input[i]);
			correct += (result == target[i]);
			output_file << target[i] << "," << result << std::endl;
			if (last != i / (static_cast<int>(input.size()) / 10))
			{
				last = i / (static_cast<int>(input.size()) / 10);
				int j;
				std::cout << "\t\t\r[";
				for (j = 0; j < last; j++)
					std::cout << "=";
				for (; j < 10; j++)
					std::cout << " ";
				std::cout << "]\r";
			}
		}
		std::cout << "error = " << 1.0 - static_cast<double>(correct) / input.size() << std::endl;
	}

	output_file.close();
	return 0;
}