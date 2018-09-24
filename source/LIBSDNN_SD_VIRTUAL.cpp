//# Licensed under the BSD 3-Clause License - https://opensource.org/licenses/BSD-3-Clause

#include "LIBSDNN_SD_VIRTUAL.h"

#include "LIBSDNN_PARAMETERS.h"
#include "LIBSDNN_LEXIAL.h"
#include "LIBSDNN_UTILITY.h"


libsdnn::base::SDNN_SD_VIRTUAL::SDNN_SD_VIRTUAL() :input_number_(0), desensitize_number_(0),desensitized_pattern_neuron_number_(0)
{
	SDNN_SD_VIRTUAL::SelectiveDesensitizationPointer = (&SDNN_SD_VIRTUAL::SelectiveDesensitizationAstarisk);
}

libsdnn::base::SDNN_SD_VIRTUAL::~SDNN_SD_VIRTUAL()
{}

void libsdnn::base::SDNN_SD_VIRTUAL::SelectiveDesensitizationAstarisk(int *out_adress_data_list, const double *input)
{
	libsdnn::utility::error::BugFound(0x1ff08);
}

void libsdnn::base::SDNN_SD_VIRTUAL::InitSD(parameters::PARAMETERS &sdnn_parameter)
{
	libsdnn::utility::error::BugFound(0x1ff08);
}

int libsdnn::base::SDNN_SD_VIRTUAL::GetSDPatternNeuronNumber(void)
{
	return desensitized_pattern_neuron_number_;
}

std::string libsdnn::base::SDNN_SD_VIRTUAL::GetDesensitizationList(void)
{
	libsdnn::utility::error::BugFound(0x1ff08);
	return "";
}
