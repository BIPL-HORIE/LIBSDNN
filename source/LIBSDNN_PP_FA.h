//# Licensed under the BSD 3-Clause License - https://opensource.org/licenses/BSD-3-Clause

#pragma once
#include "LIBSDNN_PP_VIRTUAL.h"
#include <vector>
namespace libsdnn
{
	namespace parameters
	{
		class PARAMETERS;
	}
	namespace base
	{
		namespace nn
		{
			class SDNN_PP_FA :public SDNN_PP_VIRTUAL
			{
			private:
				class CHOOSE_LESS_POTENTIAL;

				double a_;
				double b_;
			
				double DetermineOutputAstarisk(int *adress_data_list);
				double DetermineOutputAstarisk_MultiThread(int *adress_data_list);

				void TrainAstarisk(int *adress_data_list, double target);
				void TrainAstarisk_MultiThread(int *adress_data_list, double target);

			public:

				SDNN_PP_FA();
				~SDNN_PP_FA() {}
				void InitPP(parameters::PARAMETERS &sdnn_parameter, const int input_pattern_length);
			};
		}
	}
}