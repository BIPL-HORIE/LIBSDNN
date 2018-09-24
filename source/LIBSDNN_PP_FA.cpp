//# Licensed under the BSD 3-Clause License - https://opensource.org/licenses/BSD-3-Clause

#include"LIBSDNN_PP_FA.h"

#include "LIBPARAMETERS_JUDGEMENT.h"
#include "LIBSDNN_PARAMETERS.h"
#include "LIBSDNN_UTILITY.h"

#include <iostream>
#include<string>
#include<vector>
#include <random>
#include <algorithm>
#include <omp.h>
#include <thread>

class libsdnn::base::nn::SDNN_PP_FA::CHOOSE_LESS_POTENTIAL
{
public:
	bool operator()(const NEURON_OUTPUT& a, const NEURON_OUTPUT& b) const {
		return (a.potential != b.potential) ? (a.potential < b.potential) : (a.index < b.index);
	}
};

libsdnn::base::nn::SDNN_PP_FA::SDNN_PP_FA() :a_(0), b_(0)
{
	SDNN_PP_VIRTUAL::DetermineOutputPointer = static_cast<DetermineOutput_callback>(&SDNN_PP_FA::DetermineOutputAstarisk_MultiThread);
	SDNN_PP_VIRTUAL::TrainPointer = static_cast<Train_callback>(&SDNN_PP_FA::TrainAstarisk_MultiThread);
}

void libsdnn::base::nn::SDNN_PP_FA::InitPP(parameters::PARAMETERS &sdnn_parameter, const int input_pattern_length)
{
	std::vector<double> output_range;
	parameter_property::PARAM_NN_PP_OUTPUT_RANGE::property_.Read(output_range, sdnn_parameter);

	double step_size;
	auto judgement_buffer = std::make_unique<libsdnn::parameters::judgement::JUDGEMENT_IS_IN_RANGE<double>>(0.000001, (output_range[1] - output_range[0]));
	parameter_property::PARAM_NN_PP_OUTPUT_QUANTIZATION_STEP_SIZE::property_.judgement_ = std::move(judgement_buffer);
	parameter_property::PARAM_NN_PP_OUTPUT_QUANTIZATION_STEP_SIZE::property_.Read(step_size, sdnn_parameter);

	output_neuron_number_ = static_cast<int>((output_range[1] - output_range[0]) / step_size);
	if (((output_range[1] - output_range[0]) / step_size) != output_neuron_number_)
	{
		std::cout << "warning: please reconsider range and step size of the output" << std::endl;
	}

	w_number_per_neuron_ = input_pattern_length + 1;

	w_ = std::make_unique<int[]>(w_number_per_neuron_*output_neuron_number_);

	std::vector<int> initial_value_range;
	parameter_property::PARAM_NN_INITIAL_VALUE_RANGE::property_.Read(initial_value_range, sdnn_parameter);
	initial_value_min = initial_value_range[0];
	initial_value_max = initial_value_range[1];

	std::mt19937 mt;
	libsdnn::utility::InitMt(mt, sdnn_parameter, parameter_property::PARAM_NN_RANDOM_SEED::property_.name_, parameter_property::PARAM_NN_RANDOM_SEED::property_.default_);
	Reset(mt);

	a_ = step_size;
	b_ = output_range[0];

	std::string use_multi_thread;
	parameter_property::PARAM_OPTION_MULTI_THREAD_USE::property_.Read(use_multi_thread, sdnn_parameter);

	if (use_multi_thread == parameter_property::PARAM_OPTION_MULTI_THREAD_USE::CNT_OPTION_MULTI_THREAD_USE::y_)
	{
		SDNN_PP_VIRTUAL::DetermineOutputPointer = static_cast<DetermineOutput_callback>(&SDNN_PP_FA::DetermineOutputAstarisk_MultiThread);
		SDNN_PP_VIRTUAL::TrainPointer = static_cast<Train_callback>(&SDNN_PP_FA::TrainAstarisk_MultiThread);

	}
	else
	{
		SDNN_PP_VIRTUAL::DetermineOutputPointer = static_cast<DetermineOutput_callback>(&SDNN_PP_FA::DetermineOutputAstarisk);
		SDNN_PP_VIRTUAL::TrainPointer = static_cast<Train_callback>(&SDNN_PP_FA::TrainAstarisk);
	}
}

double libsdnn::base::nn::SDNN_PP_FA::DetermineOutputAstarisk(int *adress_data_list)
{
	int adress_data_list_length = *adress_data_list++;
	int neuron_output_sum_buffer = 0;
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
		neuron_output_sum_buffer += (potential > 0);
	}

	return a_*neuron_output_sum_buffer + b_;
}

double libsdnn::base::nn::SDNN_PP_FA::DetermineOutputAstarisk_MultiThread(int *adress_data_list)
{
	int adress_data_list_length = *adress_data_list++;
	int neuron_output_sum_buffer = 0;
#ifdef _OPENMP
#pragma omp parallel for reduction(+:neuron_output_sum_buffer)
#endif
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
		neuron_output_sum_buffer += (potential > 0);
	}

	return a_*neuron_output_sum_buffer + b_;
}

void libsdnn::base::nn::SDNN_PP_FA::TrainAstarisk(int *adress_data_list, double target)
{
	std::vector<NEURON_OUTPUT> result0(output_neuron_number_);
	std::vector<NEURON_OUTPUT> result1(output_neuron_number_);

	int neuron_output_sum_buffer = 0;
	int adress_data_list_length = *adress_data_list++;
	int w_number_per_neuron_access_buffer = w_number_per_neuron_;
	int *w_access_buffer = w_.get();

	for (int o = 0; o < output_neuron_number_; o++)
	{
		int *w_buffer = w_access_buffer + o*w_number_per_neuron_;
		int *adress_data_list_buffer = adress_data_list;
		int potential = -*w_buffer;
		for (int i = 0; i < adress_data_list_length; i++)
		{
			w_buffer += *adress_data_list_buffer++;
			potential += *w_buffer**adress_data_list_buffer++;
		}
		neuron_output_sum_buffer += (potential > 0);

		result0[o].potential = (potential > 0) ? potential : 0x8fffff;
		result0[o].index = o;
		result1[o].potential = (potential <= 0) ? -potential : 0x8fffff;
		result1[o].index = o;
	}

	neuron_output_sum_buffer -= static_cast<int>(0.5 + (target - b_) / a_);

	if (neuron_output_sum_buffer == 0)
		return;

	int target_direction = 1;
	NEURON_OUTPUT *looking_result_buffer;

	if (neuron_output_sum_buffer < 0)
	{
		looking_result_buffer = result1.data();
		neuron_output_sum_buffer *= -1;
	}
	else
	{
		looking_result_buffer = result0.data();
		target_direction = -1;
	}

	std::partial_sort(looking_result_buffer, looking_result_buffer + neuron_output_sum_buffer,
		looking_result_buffer + output_neuron_number_, CHOOSE_LESS_POTENTIAL());

	for (int t = 0; t < neuron_output_sum_buffer; t++)
	{
		int *w_buffer = w_.get() + w_number_per_neuron_ * looking_result_buffer[t].index;
		int *adress_data_list_buffer = adress_data_list;
		*w_buffer -= target_direction;
		for (int i = 0; i < adress_data_list_length; i++)
		{
			w_buffer += *adress_data_list_buffer++;
			*w_buffer += target_direction**adress_data_list_buffer++;
		}
	}
}

void libsdnn::base::nn::SDNN_PP_FA::TrainAstarisk_MultiThread(int *adress_data_list, double target)
{
	std::vector<NEURON_OUTPUT> result0(output_neuron_number_);
	std::vector<NEURON_OUTPUT> result1(output_neuron_number_);

	int neuron_output_sum_buffer = 0;
	int adress_data_list_length = *adress_data_list++;
	int w_number_per_neuron_access_buffer = w_number_per_neuron_;
	int *w_access_buffer = w_.get();

#ifdef _OPENMP
#pragma omp parallel for reduction(+:neuron_output_sum_buffer)
#endif
	for (int o = 0; o < output_neuron_number_; o++)
	{
		int *w_buffer = w_access_buffer + o*w_number_per_neuron_;
		int *adress_data_list_buffer = adress_data_list;
		int potential = -*w_buffer;
		for (int i = 0; i < adress_data_list_length; i++)
		{
			w_buffer += *adress_data_list_buffer++;
			potential += *w_buffer**adress_data_list_buffer++;
		}
		neuron_output_sum_buffer += (potential > 0);

		result0[o].potential = (potential > 0) ? potential : 0x8fffff;
		result0[o].index = o;
		result1[o].potential = (potential <= 0) ? -potential : 0x8fffff;
		result1[o].index = o;
	}

	neuron_output_sum_buffer -= static_cast<int>(0.5 + (target - b_) / a_);

	if (neuron_output_sum_buffer == 0)
		return;

	int target_direction = 1;
	NEURON_OUTPUT *looking_result_buffer;

	if (neuron_output_sum_buffer < 0)
	{
		looking_result_buffer = result1.data();
		neuron_output_sum_buffer *= -1;
	}
	else
	{
		looking_result_buffer = result0.data();
		target_direction = -1;
	}

	std::partial_sort(looking_result_buffer, looking_result_buffer + neuron_output_sum_buffer,
		looking_result_buffer + output_neuron_number_, CHOOSE_LESS_POTENTIAL());

#ifdef _OPENMP
#pragma omp parallel for
#endif
	for (int t = 0; t < neuron_output_sum_buffer; t++)
	{
		int *w_buffer = w_.get() + w_number_per_neuron_ * looking_result_buffer[t].index;
		int *adress_data_list_buffer = adress_data_list;
		*w_buffer -= target_direction;
		for (int i = 0; i < adress_data_list_length; i++)
		{
			w_buffer += *adress_data_list_buffer++;
			*w_buffer += target_direction**adress_data_list_buffer++;
		}
	}
}
