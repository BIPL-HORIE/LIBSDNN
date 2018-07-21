#include "LIBSDNN_PARAMETERS.h"
#include "LIBPARAMETERS.h"
#include "LIBSDNN_LEXIAL.h"
#include <fstream>
#include <omp.h>
using namespace libsdnn::parameter_property;

//type of issue
const std::string PARAM_SDNN_TYPE::CNT_SDNN_TYPE_::pattern_recognition_("pattern_recognition");
const std::string PARAM_SDNN_TYPE::CNT_SDNN_TYPE_::function_approximation_("function_approximation");
const std::vector<std::string> PARAM_SDNN_TYPE::CNT_SDNN_TYPE_::list_({pattern_recognition_,function_approximation_});
PARAM_PROPERTY_SET PARAM_SDNN_TYPE::property_("ISSUE\\type", CNT_SDNN_TYPE_::function_approximation_, std::make_unique<libsdnn::parameters::judgement::JUDGEMENT_STRING_LIST>(CNT_SDNN_TYPE_::list_));

//case of function approximation
//output range
PARAM_PROPERTY_SET PARAM_NN_PP_OUTPUT_RANGE::property_("ISSUE\\FA\\output_range", "[0, 1]", std::make_unique<libsdnn::parameters::judgement::JUDGEMENT_IS_RANGE>());
//step size of output
PARAM_PROPERTY_SET PARAM_NN_PP_OUTPUT_QUANTIZATION_STEP_SIZE::property_("ISSUE\\FA\\required_step_size", "0,01", std::make_unique<libsdnn::parameters::judgement::JUDGEMENT_IS_IN_RANGE<double>>(0.0001, 1));

//case of pattern recognition
//number of classes
PARAM_PROPERTY_SET PARAM_NN_SP_CATEGORY_NUMBER::property_("ISSUE\\PR\\class_number", "2", std::make_unique<libsdnn::parameters::judgement::JUDGEMENT_IS_BIGGER_THAN<unsigned int>>(2));

//multi-class recognition
const std::string PARAM_NN_SP_MULTI_CLASS_RECOGNITION_METHOD::CNT_NN_SP_MULTI_CLASS_RECOGNITION_METHOD_::ovo_("1v1");
const std::string PARAM_NN_SP_MULTI_CLASS_RECOGNITION_METHOD::CNT_NN_SP_MULTI_CLASS_RECOGNITION_METHOD_::ovr_("1vR");
const std::vector<std::string> PARAM_NN_SP_MULTI_CLASS_RECOGNITION_METHOD::CNT_NN_SP_MULTI_CLASS_RECOGNITION_METHOD_::list_({ ovo_,ovr_ });
PARAM_PROPERTY_SET PARAM_NN_SP_MULTI_CLASS_RECOGNITION_METHOD::property_("ISSUE\\PR\\multi_class_recognition", CNT_NN_SP_MULTI_CLASS_RECOGNITION_METHOD_::ovo_, std::make_unique<libsdnn::parameters::judgement::JUDGEMENT_STRING_LIST>(CNT_NN_SP_MULTI_CLASS_RECOGNITION_METHOD_::list_));

//number of inputs
PARAM_PROPERTY_SET PARAM_INPUT_NUMBER::property_("ISSUE\\input_number", "2", std::make_unique<libsdnn::parameters::judgement::JUDGEMENT_IS_BIGGER_THAN<unsigned int>>(2));

//pattern coding
//number of elements of coded pattern
PARAM_PROPERTY_SET PARAM_SD_PC_N::property_("SDNN\\PC\\n", "128", std::make_unique<PARAM_SD_PC_N::JUDGEMENT_N_>());
bool PARAM_SD_PC_N::JUDGEMENT_N_::judgement(libsdnn::parameters::PARAMETERS *parameters, std::string parameter_name, std::string default_parameter_content)const
{
	int n;
	parameters->ReadParameter(n, parameter_name, default_parameter_content);
	return (n >= 2 && (n % 2 == 0));
}

PARAM_PROPERTY_SET PARAM_SD_PC_TYPE::property_("SDNN\\PC\\input_type_and_creation_method", "[]", std::make_unique<libsdnn::parameter_property::PARAM_SD_PC_TYPE::JUDGEMENT_PC_TYPE_>(0,0));
const std::string PARAM_SD_PC_TYPE::CNT_INPUT_TYPE_::numerical_("NUMERICAL");
const std::string PARAM_SD_PC_TYPE::CNT_INPUT_TYPE_::symbol_("SYMBOLIC");
const std::string PARAM_SD_PC_TYPE::CNT_PC_METHOD_::random_inverse_("RANDOM_INVERSE");
const std::string PARAM_SD_PC_TYPE::CNT_PC_METHOD_::correlation_matrix_("CORRELATION_MATRIX");
const std::string PARAM_SD_PC_TYPE::CNT_PC_METHOD_::saved_("SAVED");

bool libsdnn::parameter_property::PARAM_SD_PC_TYPE::JUDGEMENT_PC_TYPE_::judgement(libsdnn::parameters::PARAMETERS *parameters, std::string parameter_name, std::string default_parameter_content)const
{
	std::vector<std::string> pc_type;
	parameters->ReadParameter(pc_type, parameter_name, default_parameter_content);
	if (input_number_ != pc_type.size())
		return 0;
	else
	{
		std::vector<std::string> type;
		for (unsigned int i = 0; i < pc_type.size(); i++)
		{
			libsdnn::lexial::Split(type, pc_type[i], '(');
			if (type.size() != 3)
				return 0;

			if (type[0] != CNT_INPUT_TYPE_::numerical_ && type[0] != CNT_INPUT_TYPE_::symbol_)
				return 0;

			type[2].erase(--type[2].end());
			type[2].erase(--type[2].end());

			if (type[1] == CNT_PC_METHOD_::random_inverse_)
			{
				std::vector<std::string> q_r;
				libsdnn::lexial::Split(q_r, type[2], ',');
				try
				{
					int q = stoi(q_r[0]);
					if (q <= 0)
						return 0;
				}
				catch (...)
				{
					return 0;
				}
				try
				{
					int r = std::stoi(q_r[1]);
					if (r <= 0 || r >= n_ / 2)
						return 0;
				}
				catch (...)
				{
					return 0;
				}
			}
			else if (type[1] == CNT_PC_METHOD_::correlation_matrix_)
			{
				std::vector<std::string> f_b_i_p;
				libsdnn::lexial::Split(f_b_i_p, type[2], ',');

				try
				{
					int b = stoi(f_b_i_p[1]);
					if (b <= 0)
						return 0;
					int i = stoi(f_b_i_p[2]);
					if (i <= 0)
						return 0;
					double p = stof(f_b_i_p[3]);
					if (p <= 0)
						return 0;
				}
				catch (...)
				{
					return 0;
				}
			}
			else if (type[1] == CNT_PC_METHOD_::saved_)
			{
				std::vector<std::string> f_b_i_p;
				libsdnn::lexial::Split(f_b_i_p, type[2], ',');

				try
				{
					int b = stoi(f_b_i_p[1]);
					if (b <= 0)
						return 0;
					int i = stoi(f_b_i_p[2]);
					if (i <= 0)
						return 0;
					double p = stof(f_b_i_p[3]);
					if (p <= 0)
						return 0;
				}
				catch (...)
				{
					return 0;
				}
			}
			else
				return 0;
		}
	}
	return 1;
}

PARAM_PROPERTY_SET PARAM_SD_PC_RANDOM_SEED::property_("SDNN\\PC\\random_seed", HARDWARE_ENTROPY, std::make_unique<libsdnn::parameters::judgement::JUDGEMENT_RANDOMSEED>());

const std::string libsdnn::parameter_property::PARAM_NN_TRAINING_END::CNT_NN_TRAINING_END_::itr_("iteration");
const std::string libsdnn::parameter_property::PARAM_NN_TRAINING_END::CNT_NN_TRAINING_END_::rmse_("rmse");

//Selective desensitization
//combination setting method
const std::string PARAM_SD_METHOD::CNT_SD_METHOD::mutual_("mutual");
const std::string PARAM_SD_METHOD::CNT_SD_METHOD::file_("file");
const std::string PARAM_SD_METHOD::CNT_SD_METHOD::saved_("saved");
const std::vector<std::string> PARAM_SD_METHOD::CNT_SD_METHOD::list_({ mutual_, file_, saved_ });
PARAM_PROPERTY_SET PARAM_SD_METHOD::property_("SDNN\\SD\\combination_setting", CNT_SD_METHOD::mutual_, std::make_unique<libsdnn::parameters::judgement::JUDGEMENT_STRING_LIST>(CNT_SD_METHOD::list_));

//filename for setting combination
PARAM_PROPERTY_SET PARAM_SD_METHOD::PARAM_FILE::property_("SDNN\\SD\\filename", "", std::make_unique<libsdnn::parameters::judgement::JUDGEMENT_FILE_EXIST>());

//for savefile
PARAM_PROPERTY_SET PARAM_SD_METHOD::PARAM_LINE::property_("SDNN\\SD\\line", "[(0,1),(1,0)]", nullptr);

//parameters for parallel perceptron
PARAM_PROPERTY_SET PARAM_NN_RANDOM_SEED::property_("SDNN\\NN\\random_seed", HARDWARE_ENTROPY, std::make_unique<libsdnn::parameters::judgement::JUDGEMENT_RANDOMSEED>());

PARAM_PROPERTY_SET PARAM_NN_INITIAL_VALUE_RANGE::property_("SDNN\\NN\\initial_value_range", "[-5,5]", std::make_unique<libsdnn::parameters::judgement::JUDGEMENT_IS_RANGE>());

//OPTION progression
const std::string PARAM_OPTION_PRINT_PROGRESSION::CNT_OPTION_PRINT_PROGRESSION::y_("Y");
const std::string PARAM_OPTION_PRINT_PROGRESSION::CNT_OPTION_PRINT_PROGRESSION::n_("N");
const std::vector<std::string> PARAM_OPTION_PRINT_PROGRESSION::CNT_OPTION_PRINT_PROGRESSION::list_({y_,n_});
PARAM_PROPERTY_SET PARAM_OPTION_PRINT_PROGRESSION::property_("APP\\print_progression", CNT_OPTION_PRINT_PROGRESSION::y_, std::make_unique<libsdnn::parameters::judgement::JUDGEMENT_STRING_LIST>(CNT_OPTION_PRINT_PROGRESSION::list_));

//OPTION multi_threads
const std::string PARAM_OPTION_MULTI_THREAD_USE::CNT_OPTION_MULTI_THREAD_USE::y_("Y");
const std::string PARAM_OPTION_MULTI_THREAD_USE::CNT_OPTION_MULTI_THREAD_USE::n_("N");
const std::vector<std::string> PARAM_OPTION_MULTI_THREAD_USE::CNT_OPTION_MULTI_THREAD_USE::list_({ y_,n_ });
PARAM_PROPERTY_SET PARAM_OPTION_MULTI_THREAD_USE::property_("APP\\multi_thread", CNT_OPTION_MULTI_THREAD_USE::n_, std::make_unique<libsdnn::parameters::judgement::JUDGEMENT_STRING_LIST>(CNT_OPTION_MULTI_THREAD_USE::list_));

//thread number
PARAM_PROPERTY_SET PARAM_OPTION_MULTI_THREAD_USE::PARAM_OPTION_MULTI_THREAD_NUMBER::property_("APP\\thread_number", "2", std::make_unique<libsdnn::parameters::judgement::JUDGEMENT_IS_IN_RANGE<unsigned int>>(1, omp_get_num_procs()));

PARAM_PROPERTY_SET PARAM_OPTION_SAVE_FILENAME::property_("APP\\autosave_filename", "autosave.bin", nullptr);