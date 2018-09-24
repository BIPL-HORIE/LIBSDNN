//# Licensed under the BSD 3-Clause License - https://opensource.org/licenses/BSD-3-Clause

#pragma once
#include "LIBSDNN_PP_VIRTUAL.h"
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
			class SDNN_PP_PC_1vR :public SDNN_PP_VIRTUAL
			{
			private:
				int category_number_;
				double DetermineOutputAstarisk(int *adress_data_list);
				void TrainAstarisk(int *adress_data_list, double target);

			public:
				SDNN_PP_PC_1vR();
				~SDNN_PP_PC_1vR() {}
				void InitPP(parameters::PARAMETERS &sdnn_parameter, const int input_pattern_length);
			};
			class SDNN_PP_PC_1v1 :public SDNN_PP_VIRTUAL
			{
			private:
				int category_number_;
				double DetermineOutputAstarisk(int *adress_data_list);
				void TrainAstarisk(int *adress_data_list, double target);

			public:
				SDNN_PP_PC_1v1();
				~SDNN_PP_PC_1v1() {}
				void InitPP(parameters::PARAMETERS &sdnn_parameter, const int input_pattern_length);
			};
		}
	}
}