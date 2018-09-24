//# Licensed under the BSD 3-Clause License - https://opensource.org/licenses/BSD-3-Clause

#pragma once

#include<memory>
namespace libsdnn
{
	namespace parameters
	{
		class PARAMETERS;
	}
	namespace base
	{
		class SDNN_SD_VIRTUAL;
		void FactorySD(std::unique_ptr<SDNN_SD_VIRTUAL> &out_sd_pointer, parameters::PARAMETERS &sdnn_parameter);

		class SDNN_PP_VIRTUAL;
		void FactoryPP(std::unique_ptr<SDNN_PP_VIRTUAL> &out_nn_pointer, parameters::PARAMETERS &sdnn_parameter);
	}
}