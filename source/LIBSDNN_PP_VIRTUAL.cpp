//# Licensed under the BSD 3-Clause License - https://opensource.org/licenses/BSD-3-Clause

#include"LIBSDNN_PP_VIRTUAL.h"
#include <iostream>

#include "LIBSDNN_UTILITY.h"

libsdnn::base::SDNN_PP_VIRTUAL::SDNN_PP_VIRTUAL() :w_(nullptr), w_number_per_neuron_(0), output_neuron_number_(0), initial_value_min(-5), initial_value_max(5)
{
	(libsdnn::base::SDNN_PP_VIRTUAL::DetermineOutputPointer) = &SDNN_PP_VIRTUAL::DetermineOutputAstarisk;
	(libsdnn::base::SDNN_PP_VIRTUAL::TrainPointer) = &SDNN_PP_VIRTUAL::TrainAstarisk;
}

libsdnn::base::SDNN_PP_VIRTUAL::~SDNN_PP_VIRTUAL()
{
}

void libsdnn::base::SDNN_PP_VIRTUAL::InitPP(parameters::PARAMETERS &sdnn_parameter, const int input_pattern_length)
{
	libsdnn::utility::error::BugFound(0x1ff0b);
}

double libsdnn::base::SDNN_PP_VIRTUAL::DetermineOutputAstarisk(int *adress_data_list)
{
	libsdnn::utility::error::BugFound(0x1ff0b);
	return 0;
}

void libsdnn::base::SDNN_PP_VIRTUAL::TrainAstarisk(int *adress_data_list, double target)
{
	libsdnn::utility::error::BugFound(0x1ff0b);
}

void libsdnn::base::SDNN_PP_VIRTUAL::Reset(std::mt19937 &mt)
{
	if (nullptr == w_)
	{
		std::cout << "error: the SDNN were reset before initializing" << std::endl;
		libsdnn::utility::Wait();
		return;
	}
	std::uniform_int_distribution<int> kernel(initial_value_min, initial_value_max);

	int *w_buffer = w_.get();
	for (int w_counter = 0; w_counter < w_number_per_neuron_ * output_neuron_number_; w_counter++)
		*w_buffer++ = kernel(mt);
}

void libsdnn::base::SDNN_PP_VIRTUAL::LoadW(std::vector<int> w)
{
	if (w.size() != w_number_per_neuron_ * output_neuron_number_)
	{
		std::cout << "error: w.size != w_number_per_neuron_ * output_neuron_number_" << std::endl;
		std::cout << "error: please report to libsdnn.org" << std::endl;
		utility::Wait();
		return;
	}
	int *w_buffer = w_.get();
	for (int w_counter = 0; w_counter < w_number_per_neuron_ * output_neuron_number_; w_counter++)
		*w_buffer++ = w[w_counter];
}

void libsdnn::base::SDNN_PP_VIRTUAL::GetW(std::vector<int> &w)
{
	int *w_buffer = w_.get();
	for (int w_counter = 0; w_counter < w_number_per_neuron_ * output_neuron_number_; w_counter++)
		w.push_back(*w_buffer++);
}

int libsdnn::base::SDNN_PP_VIRTUAL::GetWNumber(void)
{
	return w_number_per_neuron_ * output_neuron_number_;
}

void libsdnn::base::SDNN_PP_VIRTUAL::GetPotential(std::vector<int> &out_potential, int* adress_data_list)
{
	int adress_data_list_length = *adress_data_list++;
	for (int o = 0; o < output_neuron_number_; o++)
	{
		int *w_buffer = w_.get() + o*w_number_per_neuron_;
		int *adress_data_list_buffer = adress_data_list;
		int potential = -*w_buffer;
		for (int i = 0; i < adress_data_list_length; i++)
		{
			w_buffer += *adress_data_list_buffer++;
			potential += *w_buffer**adress_data_list_buffer++;
		}
		out_potential.push_back(potential);
	}
}
