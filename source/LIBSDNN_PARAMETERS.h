//# Licensed under the BSD 3-Clause License - https://opensource.org/licenses/BSD-3-Clause

#pragma once

#include<string>
#include<vector>
#include<memory>
#include"LIBPARAMETERS_JUDGEMENT.h"
#include"LIBPARAMETERS.h"

namespace libsdnn
{
	namespace parameter_property
	{
		class PARAM_PROPERTY_SET
		{
		public:
			PARAM_PROPERTY_SET(std::string name, std::string def, std::unique_ptr<parameters::judgement::JUDGEMENT> judgement) :name_(name), default_(def), judgement_(std::move(judgement)) {}
			const std::string name_;
			const std::string default_;
			std::unique_ptr<parameters::judgement::JUDGEMENT> judgement_;
			template<typename content_type>
			void Read(content_type &output, parameters::PARAMETERS &parameter)
			{
				parameter.ReadParameter(output, name_, default_, judgement_.get());
			}
			std::string Check(parameters::PARAMETERS &parameter)
			{
				if (parameter.IsExistParameter(name_))
					return (!judgement_->judgement(&parameter, name_, default_)) ? "Parameter<" + name_ + "> is incorrect.\nYou can set the following parameters.\n" + judgement_->GetList() : "";
				else
					return "Parameter<" + name_ + "> does not exist";
			}
		};

		class PARAM_SDNN_TYPE 
		{
		private:
			PARAM_SDNN_TYPE() = delete;
		public:
			class CNT_SDNN_TYPE_
			{
			private:
				CNT_SDNN_TYPE_() = delete;
			public:
				static const std::string pattern_recognition_;
				static const std::string function_approximation_;
				static const std::vector<std::string> list_;
			};
			static PARAM_PROPERTY_SET property_;
		};

		class PARAM_INPUT_NUMBER/**/
		{
		private:
			PARAM_INPUT_NUMBER() = delete;
		public:
			static PARAM_PROPERTY_SET property_;
		};

		class PARAM_SD_METHOD
		{
		private:
			PARAM_SD_METHOD() = delete;
		public:
			static PARAM_PROPERTY_SET property_;
			class CNT_SD_METHOD
			{
			private:
				CNT_SD_METHOD() = delete;
			public:
				static const std::string mutual_;
				static const std::string file_;
				static const std::string saved_;
				static const std::vector<std::string> list_;
			};

			class PARAM_FILE //file_
			{
			private:
				PARAM_FILE() = delete;
			public:
				static PARAM_PROPERTY_SET property_;
			};
			
			class PARAM_LINE //saved
			{
			private:
				PARAM_LINE() = delete;
			public:
				static PARAM_PROPERTY_SET property_;
			};
		};

		class PARAM_SD_PC_N/**/
		{
		private:
			PARAM_SD_PC_N() = delete;

		public:
			static PARAM_PROPERTY_SET property_;

			class JUDGEMENT_N_ :public libsdnn::parameters::judgement::JUDGEMENT
			{
			public:
				JUDGEMENT_N_() :libsdnn::parameters::judgement::JUDGEMENT("integer over 2") {}
				~JUDGEMENT_N_() {}
				bool judgement(libsdnn::parameters::PARAMETERS *parameters, const std::string parameter_name, const std::string default_parameter_content = "")const;
			};
		};

		class PARAM_SD_PC_TYPE
		{
		private:
			PARAM_SD_PC_TYPE() = delete;

		public:
			class CNT_INPUT_TYPE_
			{
			private:
				CNT_INPUT_TYPE_() = delete;
			public:
				static const std::string numerical_;
				static const std::string symbol_;
			};

			class CNT_PC_METHOD_
			{
			private:
				CNT_PC_METHOD_() = delete;
			public:
				static const std::string random_inverse_;
				static const std::string correlation_matrix_;
				static const std::string saved_;
			};

			class JUDGEMENT_PC_TYPE_ :public libsdnn::parameters::judgement::JUDGEMENT
			{
			private:
				int input_number_;
				int n_;
			public:
				JUDGEMENT_PC_TYPE_(int input_number, int n) :libsdnn::parameters::judgement::JUDGEMENT(
					"String vector whose element is NUMERICAL(RANDOM_INVERSE(r)) (1 =< r < n/2), NUMERICAL(INTERPOLATION(r)) (r > 2 && n % r == 0), SYMBOL(RANDOM_INVERSE(r)) (1 =< r < n/2),or SYMBOL(CORRELATION_TREE(filename)),\n the number of elements have to equal to the number of inputs.") {
					input_number_ = input_number; n_ = n;
				}
				~JUDGEMENT_PC_TYPE_() {}
				bool judgement(libsdnn::parameters::PARAMETERS *parameters, const std::string parameter_name, const std::string default_parameter_content = "")const;
			};
			static PARAM_PROPERTY_SET property_;
		};

		class PARAM_SD_PC_RANDOM_SEED
		{
		private:
			PARAM_SD_PC_RANDOM_SEED() = delete;

		public:
			static PARAM_PROPERTY_SET property_;
		};

		class PARAM_NN_TRAINING_END/**/
		{
		private:
			PARAM_NN_TRAINING_END() = delete;

		public:
			class CNT_NN_TRAINING_END_
			{
			private:
				CNT_NN_TRAINING_END_() = delete;
			public:
				static const std::string itr_;
				static const std::string rmse_;
			};
		};

		class PARAM_NN_PP_OUTPUT_RANGE/**/
		{
		private:
			PARAM_NN_PP_OUTPUT_RANGE() = delete;
		public:
			static PARAM_PROPERTY_SET property_;
		};

		class PARAM_NN_PP_OUTPUT_QUANTIZATION_STEP_SIZE/**/
		{
		private:
			PARAM_NN_PP_OUTPUT_QUANTIZATION_STEP_SIZE() = delete;
		public:
			static PARAM_PROPERTY_SET property_;
		};

		class PARAM_NN_SP_MULTI_CLASS_RECOGNITION_METHOD/**/
		{
		private:
			PARAM_NN_SP_MULTI_CLASS_RECOGNITION_METHOD() = delete;

		public:
			class CNT_NN_SP_MULTI_CLASS_RECOGNITION_METHOD_
			{
			private:
				CNT_NN_SP_MULTI_CLASS_RECOGNITION_METHOD_() = delete;
			public:
				static const std::string ovo_;
				static const std::string ovr_;
				static const std::vector<std::string> list_;
			};
			static PARAM_PROPERTY_SET property_;
		};

		class PARAM_NN_SP_CATEGORY_NUMBER/**/
		{
		private:
			PARAM_NN_SP_CATEGORY_NUMBER() = delete;
		public:
			static PARAM_PROPERTY_SET property_;
		};

		class PARAM_NN_RANDOM_SEED
		{
		private:
			PARAM_NN_RANDOM_SEED() = delete;

		public:
			static PARAM_PROPERTY_SET property_;
		};

		class PARAM_NN_INITIAL_VALUE_RANGE/**/
		{
		private:
			PARAM_NN_INITIAL_VALUE_RANGE() = delete;

		public:
			static PARAM_PROPERTY_SET property_;
		};

		class PARAM_OPTION_PRINT_PROGRESSION/**/
		{
		private:
			PARAM_OPTION_PRINT_PROGRESSION() = delete;
		public:
			class CNT_OPTION_PRINT_PROGRESSION //YN
			{
			private:
				CNT_OPTION_PRINT_PROGRESSION() = delete;
			public:
				static const std::string y_;
				static const std::string n_;
				static const std::vector<std::string> list_;
			};
			static PARAM_PROPERTY_SET property_;
		};

		class PARAM_OPTION_MULTI_THREAD_USE/**/
		{
		private:
			PARAM_OPTION_MULTI_THREAD_USE() = delete;
		public:
			class CNT_OPTION_MULTI_THREAD_USE //YN
			{
			private:
				CNT_OPTION_MULTI_THREAD_USE() = delete;
			public:
				static const std::string y_;
				static const std::string n_;
				static const std::vector<std::string> list_;
			};
			static PARAM_PROPERTY_SET property_;

			class PARAM_OPTION_MULTI_THREAD_NUMBER
			{
			private:
				PARAM_OPTION_MULTI_THREAD_NUMBER() = delete;
			public:
				static PARAM_PROPERTY_SET property_;
			};
		};

		class PARAM_OPTION_SAVE_FILENAME
		{
		private:
			PARAM_OPTION_SAVE_FILENAME() = delete;
		public:
			static PARAM_PROPERTY_SET property_;
		};
	}
}