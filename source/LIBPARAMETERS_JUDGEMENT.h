//# Licensed under the BSD 3-Clause License - https://opensource.org/licenses/BSD-3-Clause

#pragma once
#include <vector>
#include <string>

namespace libsdnn
{
	namespace parameters
	{
		class PARAMETERS;

		namespace judgement
		{
			class JUDGEMENT
			{
			private:
				std::string list_;
			public:
				JUDGEMENT(const std::string &list) :list_(list) {}
				JUDGEMENT(const std::vector<std::string> &list) {
					for (unsigned int i = 0; i < list.size(); i++)
						list_ += list[i] + "\n";
				}
				virtual ~JUDGEMENT() {}
				virtual bool judgement(PARAMETERS *parameters, const std::string parameter_name, const std::string default_parameter_content = "")const;
				void coutList(void)const;
				std::string GetList(void)const { return list_; };
			};

			template<typename content_type>
			class JUDGEMENT_IS_IN_RANGE :public JUDGEMENT
			{
			private:
				content_type min_;
				content_type max_;
			public:
				JUDGEMENT_IS_IN_RANGE(content_type min, content_type max);
				~JUDGEMENT_IS_IN_RANGE() {}
				bool judgement(PARAMETERS *parameters, const std::string parameter_name, const std::string default_parameter_content = "")const;
			};

			template<typename content_type>
			class JUDGEMENT_IS_BIGGER_THAN :public JUDGEMENT
			{
			private:
				content_type min_;
			public:
				JUDGEMENT_IS_BIGGER_THAN(content_type min);
				~JUDGEMENT_IS_BIGGER_THAN() {}
				bool judgement(PARAMETERS *parameters, const std::string parameter_name, const std::string default_parameter_content = "")const;
			};

			class JUDGEMENT_IS_RANGE :public JUDGEMENT
			{
			public:
				JUDGEMENT_IS_RANGE();
				~JUDGEMENT_IS_RANGE() {}
				bool judgement(PARAMETERS *parameters, const std::string parameter_name, const std::string default_parameter_content = "")const;
			};

			class JUDGEMENT_FILE_EXIST :public JUDGEMENT
			{
			public:
				JUDGEMENT_FILE_EXIST();
				~JUDGEMENT_FILE_EXIST() {}
				bool judgement(PARAMETERS *parameters, const std::string parameter_name, const std::string default_parameter_content = "")const;
			};

			class JUDGEMENT_STRING_LIST :public JUDGEMENT
			{
			private:
				std::vector<std::string> judgement_list_;
			public:
				JUDGEMENT_STRING_LIST(const std::vector<std::string> judgement_list);
				~JUDGEMENT_STRING_LIST() {}
				bool judgement(PARAMETERS *parameters, const std::string parameter_name, const std::string default_parameter_content = "")const;
			};

			class JUDGEMENT_YN :public JUDGEMENT
			{
			public:
				JUDGEMENT_YN();
				~JUDGEMENT_YN() {}

				class CNT_YN_
				{
				private:
					CNT_YN_() = delete;
				public:
					static const std::string y_;
					static const std::string n_;
				};

				bool judgement(PARAMETERS *parameters, const std::string parameter_name, const std::string default_parameter_content = "")const;
			};

			class JUDGEMENT_VECTOR_SIZE :public JUDGEMENT
			{
			private:
				int vector_size_;
			public:
				JUDGEMENT_VECTOR_SIZE(int vector_size);
				~JUDGEMENT_VECTOR_SIZE() {}
				bool judgement(PARAMETERS *parameters, const std::string parameter_name, const std::string default_parameter_content = "")const;

			};
			class JUDGEMENT_RANDOMSEED :public JUDGEMENT
			{
			public:
				JUDGEMENT_RANDOMSEED() :JUDGEMENT("String \"hardware_entropy\", or a unsigned integer array ([uint1, uint2,,,])") {}
				~JUDGEMENT_RANDOMSEED() {}

				bool judgement(parameters::PARAMETERS *parameters, const std::string parameter_name, const std::string default_parameter_content = "")const;
			};
		}
	}
}

