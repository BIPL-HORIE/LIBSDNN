/*! @file LIBSDNN_LEXIAL.h
@brief lexial processing
@author Kazumasa HORIE
@date 2015/12/1~
*/

#pragma once
#include <string>
#include <vector>
namespace libsdnn
{
	namespace lexial
	{
		/*! @brief split string
		@param[out] out_vector output
		@param[in] material 
		@param[in] divide_charactor 
		@return void
		*/
		void Split(std::vector<std::string> &out_vector, const std::string &material, const char divide_charactor);

		/*! @brief 
		@param[in,out] material 
		@param[in] delete_charactor
		@return void
		*/
		void DeleteCharactor(std::string &material, const std::string delete_charactor);

		/*! @brief 
		@param[in] material 
		@param[in] count_charactor 
		@return 
		*/
		int CountCharactor(const std::string material, const std::string count_charactor);
	}
}

