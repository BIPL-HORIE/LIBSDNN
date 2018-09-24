//# Licensed under the BSD 3-Clause License - https://opensource.org/licenses/BSD-3-Clause

#include "LIBSDNN_FACTORY.h"

#include "LIBSDNN_PARAMETERS.h"
#include "LIBSDNN_PARAMETERS.h"
#include "LIBPARAMETERS_JUDGEMENT.h"
#include "LIBSDNN_UTILITY.h"

#include "LIBSDNN_PP_VIRTUAL.h"
#include "LIBSDNN_PP_FA.h"
#include "LIBSDNN_PP_PC.h"

#include "LIBSDNN_SD_VIRTUAL.h"
#include "LIBSDNN_SD_SINGLE.h"

#include <string>
#include <iostream>

void libsdnn::base::FactorySD(std::unique_ptr<SDNN_SD_VIRTUAL> &out_sd_pointer, parameters::PARAMETERS &sdnn_parameter)
{
	try
	{
		out_sd_pointer = std::make_unique<sd::SDNN_SD_SINGLE>();
	}
	catch (std::bad_alloc)
	{
		std::cout << "memory allocation error" << std::endl;
		libsdnn::utility::Wait();
		exit(1);
	}
}

void libsdnn::base::FactoryPP(std::unique_ptr<SDNN_PP_VIRTUAL> &out_nn_pointer, parameters::PARAMETERS &sdnn_parameter)
{
	std::string issue_type;
	parameter_property::PARAM_SDNN_TYPE::property_.Read(issue_type, sdnn_parameter);

	try {
		if (issue_type == parameter_property::PARAM_SDNN_TYPE::CNT_SDNN_TYPE_::function_approximation_)
		{
			out_nn_pointer = std::make_unique<nn::SDNN_PP_FA>();
		}
		else if (issue_type == parameter_property::PARAM_SDNN_TYPE::CNT_SDNN_TYPE_::pattern_recognition_)
		{
			std::string multi_class_recognition_method;
			parameter_property::PARAM_NN_SP_MULTI_CLASS_RECOGNITION_METHOD::property_.Read(multi_class_recognition_method, sdnn_parameter);

			if (multi_class_recognition_method == parameter_property::PARAM_NN_SP_MULTI_CLASS_RECOGNITION_METHOD::CNT_NN_SP_MULTI_CLASS_RECOGNITION_METHOD_::ovr_)
			{
				out_nn_pointer = std::make_unique<nn::SDNN_PP_PC_1vR>();
			}
			else if (multi_class_recognition_method == parameter_property::PARAM_NN_SP_MULTI_CLASS_RECOGNITION_METHOD::CNT_NN_SP_MULTI_CLASS_RECOGNITION_METHOD_::ovo_)
			{
				out_nn_pointer = std::make_unique<nn::SDNN_PP_PC_1v1>();
			}
			else
			{
				utility::error::BugFound(0x1ff01);
			}
		}
		else
		{
			utility::error::BugFound(0x1ff02);
		}
	}
	catch (std::bad_alloc)
	{
		std::cout << "memory allocation error" << std::endl;
		libsdnn::utility::Wait();
		exit(1);
	}
}