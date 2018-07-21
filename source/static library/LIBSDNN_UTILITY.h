#pragma once

#include<string>
#include<random>
#include"LIBPARAMETERS.h"
#include<vector>
#include<fstream>

namespace libsdnn
{
	namespace utility
	{
		template<typename file_type>
		std::string OpenFile(file_type &file_stream, const std::string &file_name, const std::ios::openmode mode = (std::ios::openmode)0);

		template<typename memory_type>
		void AllocateMemory(memory_type** &out_memory, int row_size, int column_size);

		template<typename memory_type>
		void AllocateMemory(std::vector<memory_type*> &out_memory, int row_size, int column_size);

		void Wait(void);

		void InitMt(std::mt19937 &out_mt, libsdnn::parameters::PARAMETERS &parameter, const std::string &seed_name, const std::string &default_content);

		namespace error
		{
			void BugFound(int place);
			void AllocationError(int place);
		}
	}
}
