#pragma once
#include <string>
#include <vector>
#include <memory>

namespace libsdnn
{
	std::string CheckParameterFile(std::string parameter_filename);

	class SDNN
	{
	private:
		class SDNN_PIMPL;
		std::unique_ptr<SDNN_PIMPL> pimpl_;

		SDNN(const SDNN&) = delete;
		void operator=(const SDNN&) = delete;

	public:
		SDNN();
		~SDNN();

		void InitSDNN(const std::string &parameter_filename);
		void TrainOneSample(const std::vector<double> &input, const double target);
		void Train(const std::vector<std::vector<double>>input, const std::vector<double> target, const std::string &completion_condition);
		double Estimate(const std::vector<double> &input);
		void Save(const std::string &filename);
		void Load(const std::string &filename);
		void Reset(void);

		template <typename content_type>
		void GetParameter(content_type &out_parameter, const std::string &parameter_name);
		void GetPotential(std::vector<int> &out_potential, const std::vector<double> input);
		void GetW(std::vector<int> &out_w);

		//for mex
		void Train4Matlab(double *input, double *target, int target_num, const std::string &completion_condition);
		double Estimate4Matlab(double *input);
	};
}