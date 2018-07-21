#pragma once
#include <string>
#include <memory>
#include "LIBPARAMETERS_JUDGEMENT.h"

namespace libsdnn
{
	namespace parameters
	{
		class PARAMETERS;
	}
	namespace base
	{
		class SDNN_PC;
		class SDNN_SD_VIRTUAL
		{
		private:
			SDNN_SD_VIRTUAL(const SDNN_SD_VIRTUAL&) = delete;
			void operator=(const SDNN_SD_VIRTUAL&) = delete;

		protected:
			unsigned int input_number_;
			std::vector<int> quantized_input_;
			enum INPUT_TYPE_
			{
				NUMERICAL = 0,
				SYMBOL = 1,
			};
			std::vector<INPUT_TYPE_> input_type_;
			std::vector<int> q_;

			int desensitized_pattern_neuron_number_;
			unsigned int desensitize_number_;

			void SelectiveDesensitizationAstarisk(int* out_adress_data_list, const double *input);
			void (SDNN_SD_VIRTUAL::*SelectiveDesensitizationPointer)(int* out_adress_data_list, const double *input);

		public:
			SDNN_SD_VIRTUAL(void);
			virtual ~SDNN_SD_VIRTUAL(void);

			virtual void InitSD(parameters::PARAMETERS &sdnn_parameter);
			void SelectiveDesensitization(int* out_adress_data_list, const double* input) { (this->*SelectiveDesensitizationPointer)(out_adress_data_list, input); }
			int GetSDPatternNeuronNumber(void);
			virtual std::string GetDesensitizationList(void);
		};

		typedef void(SDNN_SD_VIRTUAL::*SelectiveDesensitization_callback)(int* out_adress_data_list, const double *input);
	}
}