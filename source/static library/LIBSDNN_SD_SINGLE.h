#pragma once
#include <memory>

#include "LIBSDNN_SD_VIRTUAL.h"


namespace libsdnn
{
	namespace base
	{
		class SDNN_PC;

		namespace sd
		{
			class SDNN_SD_SINGLE : public SDNN_SD_VIRTUAL
			{
			private:
				SDNN_SD_SINGLE(const SDNN_SD_SINGLE&) = delete;
				void operator=(const SDNN_SD_SINGLE&) = delete;

				std::vector<unsigned int> s_list_;
				std::vector<unsigned int> c_list_;

				std::vector<SDNN_PC> pattern_s_;
				std::vector<SDNN_PC> pattern_c_;

				int n_;
				int n32_;

				int adress_data_list_length_per2;

				void MakePatternAndListWithMutual(void);
				void MakePatternAndListWithFile(std::ifstream &sd_file);
				void MakePatternAndListWithLine(std::vector<std::string> &line);

				void SelectiveDesensitizationAstarisk(int* out_adress_data_list, const double* input);
				void SelectiveDesensitizationAstarisk_MultiThread(int* out_adress_data_list, const double* input);

			public:
				SDNN_SD_SINGLE(void);
				~SDNN_SD_SINGLE(void);

				void InitSD(parameters::PARAMETERS &sdnn_parameter);

				std::string GetDesensitizationList(void);
			};
		}
	}
}