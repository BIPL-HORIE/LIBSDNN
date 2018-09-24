//# Licensed under the BSD 3-Clause License - https://opensource.org/licenses/BSD-3-Clause

#pragma once
#include <string>
#include <vector>
#include <random>
#include <memory>
namespace libsdnn
{
	namespace parameters
	{
		class PARAMETERS;
	}
	namespace base
	{
		class NEURON_OUTPUT
		{
		public:
			unsigned int index;
			int potential;
		};

		class SDNN_PP_VIRTUAL
		{
		private:
			SDNN_PP_VIRTUAL(const SDNN_PP_VIRTUAL&) = delete;
			void operator=(const SDNN_PP_VIRTUAL&) = delete;

			double DetermineOutputAstarisk(int *adress_data_list);

			void TrainAstarisk(int *adress_data_list, double target);

		protected:
			std::unique_ptr<int[]> w_;

			int w_number_per_neuron_;
			int output_neuron_number_;

			double (SDNN_PP_VIRTUAL::*DetermineOutputPointer)(int *adress_data_list);
			void (SDNN_PP_VIRTUAL::*TrainPointer)(int* adress_data_list, double target);

			int initial_value_min;
			int initial_value_max;

		public:
			SDNN_PP_VIRTUAL();
			virtual ~SDNN_PP_VIRTUAL();

			virtual void InitPP(parameters::PARAMETERS &sdnn_parameter, const int input_pattern_length);
			double DetermineOutput(int *adress_data_list) { return (this->*DetermineOutputPointer)(adress_data_list); }
			void Train(int* adress_data_list, double target) { return (this->*TrainPointer)(adress_data_list, target); }
			void Reset(std::mt19937 &mt);
			void LoadW(std::vector<int> w);
			void GetW(std::vector<int> &w);
			int GetWNumber(void);

			void GetPotential(std::vector<int> &out_potential, int* adress_data_list);
		};

		typedef void(SDNN_PP_VIRTUAL::*Train_callback)(int *adress_data_list, double target);
		typedef double(SDNN_PP_VIRTUAL::*DetermineOutput_callback)(int *adress_data_list);
	}
}