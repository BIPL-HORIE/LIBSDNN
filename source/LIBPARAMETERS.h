/*! @file LIBPARAMETERS.h
@brief Class and functions for loading parameter file.

@author Kazumasa HORIE
@date 2015/12/1~
*/
#pragma once

#define HARDWARE_ENTROPY "hardware_entropy"

#include <memory>
#include <string>
#include <vector>
namespace libsdnn
{
	namespace parameters
	{
		namespace judgement
		{
			//! forward declaration for JUDGEMENT class
			class JUDGEMENT;
		}
		/*!
		@brief class for parameters of SDNN
		@code
		<usage1>
		libsdnn::parameters::PARAMETERS test_parameter;
		test_parameter.LoadFile("setting.txt");
		int parameter_input_number;
		test_parameter.ReadParameter(parameter_input_number,"SDNN\\input_number","2");
		std::cout << parameter_input_number << std::endl;
		@endcode
		@code
		<usage2>
		libsdnn::parameters::PARAMETERS test_parameter;
		test_parameter.LoadFile("setting.txt");
		std::vector<double> output_range;
		test_parameter.ReadParameter(output_range,"SDNN\\PP\\output_range","[-0.2,1.2]");
		if(output_range.size() >= 2)
			std::cout << output_range[0] << "," << output_range[1] << std::endl;
		@endcode
		@author Kazumasa HORIE
		@date 2015/12/1~
		*/
		class PARAMETERS
		{
		private:
			//! Pointer2Implementation forward declaration
			class PARAMETERS_ROOT;
			//! Pointer2ImplementationÅ@pointer & root of parameter tree
			std::unique_ptr<PARAMETERS_ROOT> pimpl_;

			//! ban constracter
			PARAMETERS(const PARAMETERS&) = delete;
			//! ban = operator
			void operator=(const PARAMETERS&) = delete;

		public:
			//! constracter
			PARAMETERS(void);
			//! deconstracter
			~PARAMETERS(void);

			/*! @brief load parameters according to parameter file
			@param[in] parameter_file_name name of the parameter file.
			@return void
			*/
			void LoadFile(const std::string parameter_file_name);

			/*! @brief load parameters according to stringstream 
			@param[in] parameter_stream string stream storing parameters.
			@return void
			*/
			void LoadFile(std::stringstream &parameter_stream);

			/*! @brief add/overwrite parameter
			@param[in] parameter_name name of parameter
			@param[in] parameter_content content of parameter
			@return void
			*/
			void OverwriteParameter(const std::string parameter_name, const std::string &parameter_content);

			/*! @brief add/overwrite parameter
            This function can use std::string for parameter content.
			@param[in] parameter_name name of parameter
			@param[in] parameter_content content of parameter
			@return voidÅD
			*/
			template<typename content_type>
			void OverwriteParameter(const std::string parameter_name, const std::vector<content_type> parameter_content);

			/*! @brief read parameter
			If content of the parameter are not suitable, this function reset the parameter and require you to retype suitable content.
			@param[out] out_parameter_content output (you can use int, double, unsigned int, std::string)
			@param[in] parameter_name name of parameter
			@param[in] default_parameter_content default content
			@param[in] judgement adress for JUDGEMENT class (you can use nullptr)
			@return void
			*/
			template<typename content_type>
			void ReadParameter(content_type &out_parameter_content, const std::string parameter_name, const std::string default_parameter_content = "", judgement::JUDGEMENT *judgement = nullptr);

			/*! @brief read parameter
			If content of the parameter are not suitable, this function reset the parameter and require you to retype suitable content.
			@param[out] out_parameter_content output (you can use std::vector<hoge>)
			@param[in] parameter_name name of parameter
			@param[in] default_parameter_content default content
			@param[in] judgement adress for JUDGEMENT class (you can use nullptr)
			@return void
			*/
			template<typename content_type>
			void ReadParameter(std::vector<content_type> &out_parameter_content, const std::string parameter_name, const std::string default_parameter_content = "", judgement::JUDGEMENT *judgement = nullptr);

			bool IsExistParameter(const std::string parameter_name);

			/*! @brief get list of parameters
			@param[out] out_parameter_list parameter list
			@warning out_parameter_list will be cleared.
			@return void
			*/
			void ListParameter(std::string &out_parameter_list)const;

			friend std::ostream& operator<<(std::ostream& os, const PARAMETERS& parameter)
			{
				std::string list;
				parameter.ListParameter(list);
				os << list;
				return os;
			}
		};
	}
}