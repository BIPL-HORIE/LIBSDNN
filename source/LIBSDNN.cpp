#include "LIBSDNN.h"
#include "LIBSDNN_PP_VIRTUAL.h"
#include "LIBSDNN_SD_VIRTUAL.h"
#include "LIBSDNN_FACTORY.h"
#include "LIBSDNN_PARAMETERS.h"
#include "LIBSDNN_PC.h"

#include "LIBSDNN_LEXIAL.h"
#include "LIBSDNN_UTILITY.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <omp.h>

namespace libsdnn
{
	std::string CheckParameterFile(const std::string parameter_filename)
	{
		libsdnn::parameters::PARAMETERS check_parameter;
		check_parameter.LoadFile(parameter_filename);
		std::string result_buffer;

		// Issue
		if ((result_buffer = parameter_property::PARAM_INPUT_NUMBER::property_.Check(check_parameter)) != "")
			return result_buffer+ "\n";

		if ((result_buffer = parameter_property::PARAM_SDNN_TYPE::property_.Check(check_parameter)) != "")
			return result_buffer + "\n";

		std::string issue_type;
		parameter_property::PARAM_SDNN_TYPE::property_.Read(issue_type, check_parameter);

		if (issue_type == parameter_property::PARAM_SDNN_TYPE::CNT_SDNN_TYPE_::function_approximation_)
		{
			if ((result_buffer = parameter_property::PARAM_NN_PP_OUTPUT_RANGE::property_.Check(check_parameter)) != "")
				return result_buffer + "\n";

			std::vector<double> output_range;
			parameter_property::PARAM_NN_PP_OUTPUT_RANGE::property_.Read(output_range, check_parameter);
			auto judgement_buffer = std::make_unique<libsdnn::parameters::judgement::JUDGEMENT_IS_IN_RANGE<double>>(0.000001, (output_range[1] - output_range[0]));
			parameter_property::PARAM_NN_PP_OUTPUT_QUANTIZATION_STEP_SIZE::property_.judgement_ = std::move(judgement_buffer);
			if ((result_buffer = parameter_property::PARAM_NN_PP_OUTPUT_QUANTIZATION_STEP_SIZE::property_.Check(check_parameter)) != "")
				return result_buffer + "\n";
		}
		else if (issue_type == parameter_property::PARAM_SDNN_TYPE::CNT_SDNN_TYPE_::pattern_recognition_)
		{
			if ((result_buffer = parameter_property::PARAM_NN_SP_CATEGORY_NUMBER::property_.Check(check_parameter)) != "")
				return result_buffer + "\n";

			if ((result_buffer = parameter_property::PARAM_NN_SP_MULTI_CLASS_RECOGNITION_METHOD::property_.Check(check_parameter)) != "")
				return result_buffer + "\n";
		}
		else
		{
			libsdnn::utility::error::BugFound(0x206);
		}

		//SDNN\PC

		if ((result_buffer = parameter_property::PARAM_SD_PC_N::property_.Check(check_parameter)) != "")
			return result_buffer + "\n";

		int input_number, n;
		parameter_property::PARAM_INPUT_NUMBER::property_.Read(input_number, check_parameter);
		parameter_property::PARAM_SD_PC_N::property_.Read(n, check_parameter);
		auto judgement_buffer = std::make_unique<libsdnn::parameter_property::PARAM_SD_PC_TYPE::JUDGEMENT_PC_TYPE_>(input_number, n);
		parameter_property::PARAM_SD_PC_TYPE::property_.judgement_ = std::move(judgement_buffer);
		if ((result_buffer = parameter_property::PARAM_SD_PC_TYPE::property_.Check(check_parameter)) != "")
			return result_buffer + "\n";

		//SDNN\\SD
		if ((result_buffer = parameter_property::PARAM_SD_METHOD::property_.Check(check_parameter)) != "")
			return result_buffer + "\n";
		std::string method;
		parameter_property::PARAM_SD_METHOD::property_.Read(method, check_parameter);
		if (method == parameter_property::PARAM_SD_METHOD::CNT_SD_METHOD::file_)
		{
			if ((result_buffer = parameter_property::PARAM_SD_METHOD::PARAM_FILE::property_.Check(check_parameter)) != "")
				return result_buffer + "\n";
		}

		//SDNN\\NN
		if ((result_buffer = parameter_property::PARAM_NN_INITIAL_VALUE_RANGE::property_.Check(check_parameter)) != "")
			return result_buffer + "\n";

		//APP
		if ((result_buffer = parameter_property::PARAM_OPTION_MULTI_THREAD_USE::property_.Check(check_parameter)) != "")
			return result_buffer + "\n";

		return "OK";
	}

	bool CheckCondition(const std::string &condition)
	{
		std::vector<std::string> split_condition;
		libsdnn::lexial::Split(split_condition, condition, '(');

		if (split_condition.size() != 2)
			return false;
		if (split_condition[0] == libsdnn::parameter_property::PARAM_NN_TRAINING_END::CNT_NN_TRAINING_END_::itr_)
		{
			std::string buffer = split_condition[1];
			buffer.erase(--buffer.end());
			try
			{
				unsigned int count = stoul(buffer);
			}
			catch (...)
			{
				return false;
			}
		}
		else if (split_condition[0] == libsdnn::parameter_property::PARAM_NN_TRAINING_END::CNT_NN_TRAINING_END_::rmse_)
		{
			std::string buffer = split_condition[1];
			buffer.erase(--buffer.end());
			std::vector<std::string> argument;
			lexial::Split(argument, buffer, ',');
			if (argument.size() != 2)
				return false;
			try
			{
				double rmse = stof(argument[0]);
				unsigned int count = stoul(argument[1]);
				if (rmse <= 0)
					return false;
			}
			catch (...)
			{
				return false;
			}
		}
		else
			return false;
		return true;
	}

}

class libsdnn::SDNN::SDNN_PIMPL
{
private:
	std::unique_ptr<libsdnn::base::SDNN_SD_VIRTUAL> sd_;
	std::unique_ptr<libsdnn::base::SDNN_PP_VIRTUAL> nn_;

	std::vector<int> adress_data_list_;
	std::vector<std::string> train_method_;

	enum STATEMENT
	{
		instance,
		inited,
	};
	STATEMENT sdnn_statement_;

	void TrainOnce(const std::vector<std::vector<double>>input, const std::vector<double> target)
	{
		const std::vector<double> *input_buffer = input.data();
		const double *target_buffer = target.data();
		for (unsigned int sample_counter = 0; sample_counter < input.size(); sample_counter++)
		{
			sd_->SelectiveDesensitization(adress_data_list_.data(), input_buffer++->data());
			nn_->Train(adress_data_list_.data(), *target_buffer++);
		}
	}

	void (SDNN_PIMPL::*OutProgression)(double input1, double input2);
	void OutProgressionNon(double input1, double input2) {}
	void OutProgressionDefault(double input1, double input2) { std::cout << "\t\t\t\r" << input1 << "/" << input2 << "\r"; }

	SDNN_PIMPL(const SDNN_PIMPL&) = delete;
	void operator=(const SDNN_PIMPL&) = delete;

	
public:
	SDNN_PIMPL() :sd_(nullptr), nn_(nullptr), sdnn_statement_(instance){ OutProgression = (&SDNN_PIMPL::OutProgressionDefault); }
	~SDNN_PIMPL() {}

	parameters::PARAMETERS sdnn_parameter_;

	template<typename parameter>
	void InitSDNN(parameter &parameter_filename)
	{
		if (sdnn_statement_ != instance)
			utility::error::BugFound(0x10105);
		try
		{
			sdnn_parameter_.LoadFile(parameter_filename);

			base::FactorySD(sd_, sdnn_parameter_);
			base::FactoryPP(nn_, sdnn_parameter_);

			sd_->InitSD(sdnn_parameter_);
			nn_->InitPP(sdnn_parameter_, sd_->GetSDPatternNeuronNumber());

			adress_data_list_.resize(sd_->GetSDPatternNeuronNumber() + 2);

			std::string print_progression;
			parameter_property::PARAM_OPTION_PRINT_PROGRESSION::property_.Read(print_progression, sdnn_parameter_);
			if (print_progression == parameter_property::PARAM_OPTION_PRINT_PROGRESSION::CNT_OPTION_PRINT_PROGRESSION::n_)
			{
				OutProgression = (&SDNN_PIMPL::OutProgressionNon);
			}
			sdnn_statement_ = inited;

			std::string use_multi_thread;
			parameter_property::PARAM_OPTION_MULTI_THREAD_USE::property_.Read(use_multi_thread, sdnn_parameter_);
			if(use_multi_thread == parameter_property::PARAM_OPTION_MULTI_THREAD_USE::CNT_OPTION_MULTI_THREAD_USE::y_)
			{
#ifdef _OPENMP
				unsigned int thread_number;
				parameter_property::PARAM_OPTION_MULTI_THREAD_USE::PARAM_OPTION_MULTI_THREAD_NUMBER::property_.Read(thread_number, sdnn_parameter_);
				omp_set_num_threads(thread_number);
#else
				std::cout << "please compile with -openmp"<< std::endl;
#endif

			}
		}
		catch (...)
		{
			utility::error::BugFound(0x1ff04);
		}
	}

	double Estimate(const std::vector<double> &input)
	{
		sd_->SelectiveDesensitization(adress_data_list_.data(), input.data());
		return nn_->DetermineOutput(adress_data_list_.data());
	}

	void GetPotential(std::vector<int> &out_potential, const std::vector<double> input)
	{
		sd_->SelectiveDesensitization(adress_data_list_.data(), input.data());
		nn_->GetPotential(out_potential, adress_data_list_.data());
	}

	void TrainOneSample(const std::vector<double> &input, const double target)
	{
		if (sdnn_statement_ == instance)
		{
			utility::error::BugFound(0x10106);
		}
		sd_->SelectiveDesensitization(adress_data_list_.data(), input.data());
		nn_->Train(adress_data_list_.data(), target);
	}

	void Train(const std::vector<std::vector<double>>input, const std::vector<double> target, const std::string &completion_condition)
	{	
		if (input.size() != target.size())
		{
			utility::error::BugFound(0x10100);
		}
		if (sdnn_statement_ == instance)
		{
			utility::error::BugFound(0x10106);
		}

		std::string condition_buffer = completion_condition;
		while (!CheckCondition(condition_buffer))
		{
			std::cout << "training-completion condition is set with incorrect strings." << std::endl;
			std::cout << "You can set the following:" << std::endl;
			std::cout << "iteration(n): repeats training process n times" << std::endl;
			std::cout << "rmse(p,m): repeats training process until RMSE is lower than p (up to m times).(only for function approximation issue)" << std::endl;
			std::cout << "Type a correct string: ";
			std::cin >> condition_buffer;
			std::cin.ignore();
		}
		std::vector<std::string> split_condition;
		lexial::Split(split_condition, completion_condition, '(');

		if (split_condition[0] == libsdnn::parameter_property::PARAM_NN_TRAINING_END::CNT_NN_TRAINING_END_::itr_)
		{
			split_condition[1].erase(--split_condition[1].end());
			int count = stoi(split_condition[1]);
			for (int i = 1; i <= count; i++)
			{
				(this->*OutProgression)(i, count);
				TrainOnce(input, target);
			}
		}
		else if (split_condition[0] == libsdnn::parameter_property::PARAM_NN_TRAINING_END::CNT_NN_TRAINING_END_::rmse_)
		{
			split_condition[1].erase(--split_condition[1].end());
			std::vector<std::string> argument;
			lexial::Split(argument, split_condition[1], ',');

			double rmse = stof(argument[0]);
			int count = stoi(argument[1]);
			double rmse_buffer;
			for (int i = count; i > 0; i--)
			{
				rmse_buffer = 0;
				TrainOnce(input, target);
				for (unsigned int l_counter = 0; l_counter < input.size(); l_counter++)
				{
					double result = Estimate(input[l_counter]);
					rmse_buffer += (result - target[l_counter]) * (result - target[l_counter]);
				}
				rmse_buffer /= input.size();
				rmse_buffer = sqrt(rmse_buffer);
				(this->*OutProgression)(rmse_buffer, rmse);
				if (rmse_buffer <= rmse)
					break;
			}
		}

		std::string save_filename;
		parameter_property::PARAM_OPTION_SAVE_FILENAME::property_.Read(save_filename, sdnn_parameter_);
		Save(save_filename);
	}

	void Save(const std::string &filename)
	{
		if (sdnn_statement_ == instance)
		{
			utility::error::BugFound(0x10107);
		}
		try
		{
			std::string method;
			parameter_property::PARAM_SD_METHOD::property_.Read(method, sdnn_parameter_);
			if(method != parameter_property::PARAM_SD_METHOD::CNT_SD_METHOD::saved_)
			{
				sdnn_parameter_.OverwriteParameter(parameter_property::PARAM_SD_METHOD::property_.name_, parameter_property::PARAM_SD_METHOD::CNT_SD_METHOD::saved_);
				sdnn_parameter_.OverwriteParameter(parameter_property::PARAM_SD_METHOD::PARAM_LINE::property_.name_, sd_->GetDesensitizationList());
			}
			
			int cm_flag = 0;
			std::vector<std::string> pc_type_list;
			parameter_property::PARAM_SD_PC_TYPE::property_.Read(pc_type_list, sdnn_parameter_);
			for (auto ptl_itr = pc_type_list.begin(); ptl_itr != pc_type_list.end(); ptl_itr++)
			{
				if (ptl_itr->find(parameter_property::PARAM_SD_PC_TYPE::CNT_PC_METHOD_::correlation_matrix_) != std::string::npos)
				{
					std::vector<std::string> setting_buffer_vector;
					libsdnn::lexial::Split(setting_buffer_vector, *ptl_itr, '(');
					setting_buffer_vector[2].erase(--setting_buffer_vector[2].end());
					setting_buffer_vector[2].erase(--setting_buffer_vector[2].end());

					std::vector<std::string> f_b_i_p;
					libsdnn::lexial::Split(f_b_i_p,setting_buffer_vector[2], ',');

					std::string line_buffer = libsdnn::base::pc::CorrelationMatrixFile2Strings(f_b_i_p[0]);
					*ptl_itr = setting_buffer_vector[0] + "(" + parameter_property::PARAM_SD_PC_TYPE::CNT_PC_METHOD_::saved_ + "(" + line_buffer + "," + f_b_i_p[1] + "," + f_b_i_p[2] + "," + f_b_i_p[3] + "))";
					cm_flag = 1;
				}
			}
			if(cm_flag == 1)
				sdnn_parameter_.OverwriteParameter(parameter_property::PARAM_SD_PC_TYPE::property_.name_, pc_type_list);

			std::ofstream parameter_file;
			utility::OpenFile(parameter_file, filename, std::ios::binary);
			std::stringstream buffer_stream;
			buffer_stream << sdnn_parameter_;
			size_t parameter_length = buffer_stream.str().size();

			parameter_file.write((char*)&parameter_length, sizeof(int));
			parameter_file.write((char*)buffer_stream.str().c_str(), sizeof(char)*parameter_length);

			std::vector<int> w;
			nn_->GetW(w);

			for (unsigned int i = 0; i < w.size(); i++)
			{
				parameter_file.write((const char*)&w[i], sizeof(int));
			}
		}
		catch (...)
		{
			utility::error::BugFound(0x1ff06);
		}
	}

	void Load(const std::string &filename)
	{
		if (sdnn_statement_ != instance)
		{
			utility::error::BugFound(0x10108);
		}
		try
		{
			std::ifstream parameter_file;
			utility::OpenFile(parameter_file, filename, std::ios::binary);
			int parameter_length;
			parameter_file.read((char*)&parameter_length, sizeof(int));
			char* test = new char[parameter_length + 1];
			parameter_file.read(test, sizeof(char)*parameter_length);
			std::stringstream str;
			test[parameter_length] = '\0';
			str << test;
			InitSDNN(str);

			std::vector<int> w;
			nn_->GetW(w);

			int w_buffer;
			std::vector<int> w_vector;
			w_vector.clear();
			for (int i = 0; i < nn_->GetWNumber(); i++)
			{
				parameter_file.read((char*)&w_buffer, sizeof(w_buffer));
				if (parameter_file.bad())
					throw "file error";
				w_vector.push_back(w_buffer);
			}
			nn_->LoadW(w_vector);
		}
		catch (...)
		{
			utility::error::BugFound(0x1ff07);
		}
	}

	void Reset(void)
	{
		sdnn_statement_ = instance;
		sd_.reset();
		nn_.reset();
	}

	void GetParameter(std::string &out_parameter, const std::string &parameter_name)
	{
		sdnn_parameter_.ReadParameter(out_parameter, parameter_name,"");
	}

	void GetW(std::vector<int> &out_w)
	{
		nn_->GetW(out_w);
	}

};

template void libsdnn::SDNN::SDNN_PIMPL::InitSDNN(const std::string& filename);
template void libsdnn::SDNN::SDNN_PIMPL::InitSDNN(std::stringstream& filename);

libsdnn::SDNN::SDNN():pimpl_(new SDNN_PIMPL)
{}

libsdnn::SDNN::~SDNN()
{}

void libsdnn::SDNN::InitSDNN(const std::string &parameter_filename)
{
	pimpl_->InitSDNN(parameter_filename);
}

double libsdnn::SDNN::Estimate(const std::vector<double> &input)
{
	return pimpl_->Estimate(input);
}

void libsdnn::SDNN::GetPotential(std::vector<int> &out_potential, const std::vector<double> input)
{
	pimpl_->GetPotential(out_potential, input);
}

void libsdnn::SDNN::TrainOneSample(const std::vector<double> &input, const double target)
{
	pimpl_->TrainOneSample(input, target);
}

void libsdnn::SDNN::Train(const std::vector<std::vector<double>>input, const std::vector<double> target, const std::string &completion_condition)
{
	pimpl_->Train(input, target,completion_condition);
}

void libsdnn::SDNN::Save(const std::string &filename)
{
	pimpl_->Save(filename);
}
void libsdnn::SDNN::Load(const std::string &filename)
{
	pimpl_->Load(filename);
}

void libsdnn::SDNN::Reset(void)
{
	pimpl_->Reset();
}

template<typename content_type>
void libsdnn::SDNN::GetParameter(content_type &out_parameter, const std::string &parameter_name)
{
	pimpl_->sdnn_parameter_.ReadParameter(out_parameter, parameter_name,"");
}

void libsdnn::SDNN::GetW(std::vector<int> &out_w)
{
	pimpl_->GetW(out_w);
}


template void libsdnn::SDNN::GetParameter(std::string &out_parameter, const std::string &parameter_name);
template void libsdnn::SDNN::GetParameter(int &out_parameter, const std::string &parameter_name);
template void libsdnn::SDNN::GetParameter(double &out_parameter, const std::string &parameter_name);
template void libsdnn::SDNN::GetParameter(unsigned int &out_parameter, const std::string &parameter_name);
template void libsdnn::SDNN::GetParameter(std::vector<std::string> &out_parameter, const std::string &parameter_name);
template void libsdnn::SDNN::GetParameter(std::vector<int> &out_parameter, const std::string &parameter_name);
template void libsdnn::SDNN::GetParameter(std::vector<double> &out_parameter, const std::string &parameter_name);
template void libsdnn::SDNN::GetParameter(std::vector<unsigned int> &out_parameter, const std::string &parameter_name);
