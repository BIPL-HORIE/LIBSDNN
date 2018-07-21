/*! @file LIBSDNN_LEXIAL.h
@brief lexial processing
@author Kazumasa HORIE
@date 2015/12/1~
@version 0.8
*/

#include "LIBSDNN_LEXIAL.h"
#include <iostream>
#include <sstream>
namespace libsdnn
{
	namespace lexial
	{
		void Split(std::vector<std::string> &out_vector, const std::string &material, const char divide_charactor)
		{
			out_vector.clear();
			out_vector.shrink_to_fit();
			std::string::size_type p = material.find(divide_charactor);
			std::string pop_buffer = material;
			while (p != std::string::npos)
			{
				out_vector.push_back(pop_buffer.substr(0, p));
				pop_buffer = (pop_buffer.substr(p + 1));
				p = pop_buffer.find(divide_charactor);
			}
			out_vector.push_back(pop_buffer);
		}
		void DeleteCharactor(std::string &material, const std::string delete_charactor)
		{
			size_t position;
			while ((position = material.find_first_of(delete_charactor)) != std::string::npos)
			{
				material.erase(position, 1);
			}
		}
		int CountCharactor(const std::string material, const std::string count_charactor)
		{
			std::string material_buffer = material;
			DeleteCharactor(material_buffer, count_charactor);
			return static_cast<int>(material.size() - material_buffer.size());
		}
	}
}

