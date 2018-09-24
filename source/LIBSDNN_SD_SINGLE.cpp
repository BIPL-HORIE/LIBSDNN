//# Licensed under the BSD 3-Clause License - https://opensource.org/licenses/BSD-3-Clause

#include "LIBSDNN_SD_SINGLE.h"
#include "LIBSDNN_PARAMETERS.h"
#include "LIBPARAMETERS_JUDGEMENT.h"
#include "LIBSDNN_UTILITY.h"
#include "LIBSDNN_LEXIAL.h"

#include "LIBSDNN_PC.h"

#include <iostream>
#include <fstream>

libsdnn::base::sd::SDNN_SD_SINGLE::SDNN_SD_SINGLE():n_(0),n32_(0),adress_data_list_length_per2(0)
{
	SDNN_SD_VIRTUAL::SelectiveDesensitizationPointer = static_cast<SelectiveDesensitization_callback>(&SDNN_SD_SINGLE::SelectiveDesensitizationAstarisk);
}

libsdnn::base::sd::SDNN_SD_SINGLE::~SDNN_SD_SINGLE()
{
}

void libsdnn::base::sd::SDNN_SD_SINGLE::InitSD(parameters::PARAMETERS &sdnn_parameter)
{
	parameter_property::PARAM_INPUT_NUMBER::property_.Read(input_number_, sdnn_parameter);
	parameter_property::PARAM_SD_PC_N::property_.Read(n_, sdnn_parameter);

	n32_ = ((n_ - 1) >> 5) + 1;

	quantized_input_.resize(input_number_);

	std::string method;
	parameter_property::PARAM_SD_METHOD::property_.Read(method, sdnn_parameter);
	if (method == parameter_property::PARAM_SD_METHOD::CNT_SD_METHOD::mutual_)
	{
		MakePatternAndListWithMutual();
	}
	else if (method == parameter_property::PARAM_SD_METHOD::CNT_SD_METHOD::file_)
	{
		std::ifstream sd_file;
		std::string sd_filename;
		parameter_property::PARAM_SD_METHOD::PARAM_FILE::property_.Read(sd_filename, sdnn_parameter);
		sd_file.open(sd_filename);
		MakePatternAndListWithFile(sd_file);
	}
	else if (method == parameter_property::PARAM_SD_METHOD::CNT_SD_METHOD::saved_)
	{
		std::vector<std::string> line;
		parameter_property::PARAM_SD_METHOD::PARAM_LINE::property_.Read(line, sdnn_parameter);
		MakePatternAndListWithLine(line);
	}
	else
	{
		utility::error::BugFound(0x1ff03);
	}

	std::mt19937 mt;
	libsdnn::utility::InitMt(mt, sdnn_parameter, parameter_property::PARAM_SD_PC_RANDOM_SEED::property_.name_, parameter_property::PARAM_SD_PC_RANDOM_SEED::property_.default_);

	std::vector<std::string> init_type;
	auto judgement_buffer = std::make_unique<libsdnn::parameter_property::PARAM_SD_PC_TYPE::JUDGEMENT_PC_TYPE_>(input_number_, n_);
	parameter_property::PARAM_SD_PC_TYPE::property_.judgement_ = std::move(judgement_buffer);
	parameter_property::PARAM_SD_PC_TYPE::property_.Read(init_type, sdnn_parameter);
	input_type_.resize(input_number_);
	q_.resize(input_number_);
	for (unsigned int i = 0; i < input_number_; i++)
	{
		std::vector<std::string> init_type_buffer;
		libsdnn::lexial::Split(init_type_buffer, init_type[i], '(');
		if (init_type_buffer[0] == parameter_property::PARAM_SD_PC_TYPE::CNT_INPUT_TYPE_::numerical_)
		{
			input_type_[i] = NUMERICAL;
		}
		else if (init_type_buffer[0] == parameter_property::PARAM_SD_PC_TYPE::CNT_INPUT_TYPE_::symbol_)
		{
			input_type_[i] = SYMBOL;
		}
		else
		{
			utility::error::BugFound(0x1ff09);
		}
	}

	auto c_list_itr = c_list_.cbegin();
	auto s_list_itr = s_list_.cbegin();
	auto pattern_c_itr = pattern_c_.begin();
	auto pattern_s_itr = pattern_s_.begin();

	std::string multi_thread;
	parameter_property::PARAM_OPTION_MULTI_THREAD_USE::property_.Read(multi_thread, sdnn_parameter);

	for (unsigned int d_counter = 0; d_counter < desensitize_number_; d_counter++)
	{
		std::cout << "Init code pattern (SD[" << *c_list_itr << "," << *s_list_itr << "]>Input " << *c_list_itr << "):" << std::endl;
		pattern_c_itr->InitPC(n_, init_type[*c_list_itr], mt, multi_thread == libsdnn::parameters::judgement::JUDGEMENT_YN::CNT_YN_::y_);
		std::cout << "Init code pattern (SD[" << *c_list_itr << "," << *s_list_itr << "]>Input " << *s_list_itr << "):" << std::endl;
		pattern_s_itr->InitPC(n_, init_type[*s_list_itr], mt, multi_thread == libsdnn::parameters::judgement::JUDGEMENT_YN::CNT_YN_::y_);
		q_[*c_list_itr++] = pattern_c_itr++->GetPatternNumber();
		q_[*s_list_itr++] = pattern_s_itr++->GetPatternNumber();
	}

	pattern_c_itr = pattern_c_.begin();
	desensitized_pattern_neuron_number_ = 0;
	for (unsigned int d_counter = 0; d_counter < desensitize_number_; d_counter++)
	{
		desensitized_pattern_neuron_number_ += pattern_c_itr++->GetN();
	}
	adress_data_list_length_per2 = desensitized_pattern_neuron_number_ >> 1;

	std::string use_multi_thread;
	parameter_property::PARAM_OPTION_MULTI_THREAD_USE::property_.Read(use_multi_thread, sdnn_parameter);

	if (desensitize_number_ > 44 && use_multi_thread == parameter_property::PARAM_OPTION_MULTI_THREAD_USE::CNT_OPTION_MULTI_THREAD_USE::y_)
		(SDNN_SD_VIRTUAL::SelectiveDesensitizationPointer) = static_cast<SelectiveDesensitization_callback>(&SDNN_SD_SINGLE::SelectiveDesensitizationAstarisk_MultiThread);
}


void libsdnn::base::sd::SDNN_SD_SINGLE::MakePatternAndListWithMutual(void)
{
	desensitize_number_ = input_number_*(input_number_ - 1);

	pattern_s_.resize(desensitize_number_);
	pattern_c_.resize(desensitize_number_);
	s_list_.resize(desensitize_number_);
	c_list_.resize(desensitize_number_);

	auto s_list_itr = s_list_.begin();
	auto c_list_itr = c_list_.begin();

	for (unsigned int input_counter1 = 0; input_counter1 < input_number_; input_counter1++)
	{
		for (unsigned int input_counter2 = 0; input_counter2 < input_number_; input_counter2++)
		{
			if (input_counter1 == input_counter2)
				continue;
			*s_list_itr++ = input_counter1;
			*c_list_itr++ = input_counter2;
		}
	}
}

void libsdnn::base::sd::SDNN_SD_SINGLE::MakePatternAndListWithFile(std::ifstream &sd_file)
{
	std::string line_buffer;

	desensitize_number_ = 0;

	while (getline(sd_file, line_buffer))
	{
		if (line_buffer != "")
			desensitize_number_++;
	}
	if (desensitize_number_ == 0)
	{
		sd_file.close();
		std::cout << "this file is emply." << std::endl;
		std::cout << "type the desensitization filename correctly: ";
		std::cin >> line_buffer;
		std::cin.ignore();
		std::ifstream desensitize_file(line_buffer);
		MakePatternAndListWithFile(desensitize_file);
		return;
	}

	pattern_s_.resize(desensitize_number_);
	pattern_c_.resize(desensitize_number_);
	s_list_.resize(desensitize_number_);
	c_list_.resize(desensitize_number_);

	sd_file.clear();
	sd_file.seekg(0);
	std::vector<std::string> sd_buffer;
	int counter = 0;
	while (getline(sd_file, line_buffer))
	{
		if (line_buffer == "")
			continue;
		sd_buffer.clear();
		lexial::Split(sd_buffer, line_buffer, ',');

		do
		{
			if (sd_buffer.size() == 2)
			{
				s_list_[counter] = std::stoi(sd_buffer[0]);
				if (s_list_[counter] >= 0 && s_list_[counter] < input_number_)
				{
					c_list_[counter] = std::stoi(sd_buffer[1]);
					if (c_list_[counter] >= 0 && c_list_[counter] < input_number_)
					{
						break;
					}
				}
			}
			std::cout << "Syntactic error :";
			std::cout << line_buffer << std::endl;
			std::cout << "type the desentisization pair correctly: ";
			std::cin >> line_buffer;
			std::cin.ignore();
			lexial::Split(sd_buffer, line_buffer, ',');
		} while (1);
		counter++;
	}
}

void libsdnn::base::sd::SDNN_SD_SINGLE::MakePatternAndListWithLine(std::vector<std::string> &line)
{
	desensitize_number_ = static_cast<unsigned int>(line.size());

	pattern_s_.resize(desensitize_number_);
	pattern_c_.resize(desensitize_number_);
	s_list_.resize(desensitize_number_);
	c_list_.resize(desensitize_number_);

	auto s_list_itr = s_list_.begin();
	auto c_list_itr = c_list_.begin();
	for (auto line_itr = line.begin(); line_itr != line.end(); line_itr++)
	{
		lexial::DeleteCharactor(*line_itr, "()");
		std::vector<std::string> line_buffer;
		lexial::Split(line_buffer, *line_itr, ',');
		if (line_buffer.size() != 2)
			utility::error::BugFound(0x1ff1f);
		*s_list_itr++ = stoi(line_buffer[0]);
		*c_list_itr++ = stoi(line_buffer[1]);
	}
}

void libsdnn::base::sd::SDNN_SD_SINGLE::SelectiveDesensitizationAstarisk(int* out_adress_data_list, const double* input)
{
	{
		const double *input_buffer = input;
		auto quantized_input_itr = quantized_input_.begin();
		auto input_type_itr = input_type_.cbegin();
		for (auto q_itr = q_.cbegin(); q_itr != q_.cend(); q_itr++)
		{
			*quantized_input_itr = (*input_type_itr++ == NUMERICAL) ? static_cast<int>(*q_itr**input_buffer++) : static_cast<int>(*input_buffer++);
			*quantized_input_itr++ = (*quantized_input_itr >= *q_itr) ? *q_itr - 1 : (*quantized_input_itr < 0) ? 0 : *quantized_input_itr;
		}
	}

	auto s_list_itr = s_list_.cbegin();
	auto c_list_itr = c_list_.cbegin();

	int *out_adress_data_list_buffer = out_adress_data_list + 1;

	auto pattern_s_itr = pattern_s_.begin();
	auto pattern_c_itr = pattern_c_.begin();

	unsigned int *pattern_s_uint_buffer;
	unsigned int *pattern_c_uint_buffer;
	unsigned int pattern_s_content;
	unsigned int pattern_c_content;

	*out_adress_data_list_buffer = 1;
	for (unsigned int d_counter = 0; d_counter < desensitize_number_; d_counter++)
	{
		pattern_s_uint_buffer = pattern_s_itr++->GetPatternBinary(quantized_input_[*s_list_itr++]);
		pattern_c_uint_buffer = pattern_c_itr++->GetPatternBinary(quantized_input_[*c_list_itr++]);

		for (int n32_counter = 0; n32_counter < n32_ - 1; n32_counter++)
		{
			pattern_s_content = *pattern_s_uint_buffer++;
			pattern_c_content = *pattern_c_uint_buffer++;
			for (int counter32 = 0; counter32 < 32; counter32++)
			{
				if (pattern_c_content & 0x1)
				{
					*++out_adress_data_list_buffer = 2 * (pattern_s_content & 0x1) - 1;
					*++out_adress_data_list_buffer = 1;
				}
				else
				{
					(*out_adress_data_list_buffer)++;
				}
				pattern_s_content >>= 1;
				pattern_c_content >>= 1;
			}
		}
		pattern_s_content = *pattern_s_uint_buffer++;
		pattern_c_content = *pattern_c_uint_buffer++;
		for (int remain_counter = 0; remain_counter < n_ - (n32_ - 1) * 32; remain_counter++)
		{
			if (pattern_c_content & 0x1)
			{
				*++out_adress_data_list_buffer = 2 * (pattern_s_content & 0x1) - 1;
				*++out_adress_data_list_buffer = 1;
			}
			else
			{
				(*out_adress_data_list_buffer)++;
			}
			pattern_s_content >>= 1;
			pattern_c_content >>= 1;
		}
	}
	*out_adress_data_list = adress_data_list_length_per2;
}

void libsdnn::base::sd::SDNN_SD_SINGLE::SelectiveDesensitizationAstarisk_MultiThread(int* out_adress_data_list, const double* input)
{
	{
		const double *input_buffer = input;
		auto quantized_input_itr = quantized_input_.begin();
		auto input_type_itr = input_type_.cbegin();
		for (auto q_itr = q_.cbegin(); q_itr != q_.cend(); q_itr++)
		{
			*quantized_input_itr = (*input_type_itr++ == NUMERICAL) ? static_cast<int>(*q_itr**input_buffer++) : static_cast<int>(*input_buffer++);
			*quantized_input_itr++ = (*quantized_input_itr >= *q_itr) ? *q_itr - 1 : (*quantized_input_itr < 0) ? 0 : *quantized_input_itr;
		}
	}

	for (int list_counter = 0; list_counter < adress_data_list_length_per2 + 1; list_counter++)
	{
		out_adress_data_list[2 * list_counter + 1] = 0;
	}

	out_adress_data_list[1] = 1;

#ifdef _OPENMP
#pragma omp parallel for
#endif
	for (int d_counter = 0; d_counter < static_cast<int>(desensitize_number_); d_counter++)
	{
		unsigned int *pattern_s_uint_buffer = pattern_s_[d_counter].GetPatternBinary(quantized_input_[s_list_[d_counter]]);
		unsigned int *pattern_c_uint_buffer = pattern_c_[d_counter].GetPatternBinary(quantized_input_[c_list_[d_counter]]);
		unsigned int pattern_s_content;
		unsigned int pattern_c_content;

		int *out_adress_data_list_buffer = out_adress_data_list + 1 + d_counter*n_;

		for (int n32_counter = 0; n32_counter < n32_ - 1; n32_counter++)
		{
			pattern_s_content = *pattern_s_uint_buffer++;
			pattern_c_content = *pattern_c_uint_buffer++;
			for (int counter32 = 0; counter32 < 32; counter32++)
			{
				if (pattern_c_content & 0x1)
				{
					*++out_adress_data_list_buffer = 2 * (pattern_s_content & 0x1) - 1;
					(*++out_adress_data_list_buffer)++;
				}
				else
				{
					(*out_adress_data_list_buffer)++;
				}
				pattern_s_content >>= 1;
				pattern_c_content >>= 1;
			}
		}
		pattern_s_content = *pattern_s_uint_buffer++;
		pattern_c_content = *pattern_c_uint_buffer++;
		for (int remain_counter = 0; remain_counter < n_ - (n32_ - 1) * 32; remain_counter++)
		{
			if (pattern_c_content & 0x1)
			{
				*++out_adress_data_list_buffer = 2 * (pattern_s_content & 0x1) - 1;
				(*++out_adress_data_list_buffer)++;
			}
			else
			{
				(*out_adress_data_list_buffer)++;
			}
			pattern_s_content >>= 1;
			pattern_c_content >>= 1;
		}
	}

	*out_adress_data_list = adress_data_list_length_per2;
}

std::string libsdnn::base::sd::SDNN_SD_SINGLE::GetDesensitizationList(void)
{
	std::string buffer = "[";
	for (unsigned int i = 0; i < desensitize_number_ - 1; i++)
	{
		buffer += "(" + std::to_string(s_list_[i]) + "," + std::to_string(c_list_[i]) + "),";
	}
	buffer += "(" + std::to_string(s_list_[desensitize_number_ - 1]) + "," + std::to_string(c_list_[desensitize_number_ - 1]) + ")]";
	return buffer;
}