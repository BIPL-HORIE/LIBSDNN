//# Licensed under the BSD 3-Clause License - https://opensource.org/licenses/BSD-3-Clause

#pragma once

#include <string>
#include <memory>
#include <random>
namespace libsdnn
{
	namespace base
	{
		namespace pc
		{
			void MakeRandomPattern(std::vector<bool> &out_pattern, std::mt19937 &mt);
			void MakeRandomPattern(std::vector<bool> &out_pattern, const std::vector<bool> &material, const int r, std::mt19937 &mt);
			void SavePattern(unsigned int* binary, const std::vector<bool> &pattern);
			double CalcCorrelation(const std::vector<bool> & pattern1, const std::vector<bool> & pattern2);
			std::string CorrelationMatrixFile2Strings(const std::string &filename);
		}
		class SDNN_PC
		{
		private:
			int n_;
			int n32_;
			std::vector<unsigned int*> binary_pattern_;

			void RandomInverse(const int q, const int r, std::mt19937 &mt);
			void CorrelationMatrix(const std::vector<std::vector<double>> &correlation_matrix, const int batch_n, const int max_iteration, const double precision, std::mt19937 &mt, const bool multi_core);
			void CorrelationMatrix(const std::string &correlation_matrix_filename, const int batch_n, const int max_iteration, const double precision, std::mt19937 &mt, const bool multi_core);

		public:		
			SDNN_PC(void);
			~SDNN_PC(void);

			void InitPC(const int n, const std::string &pattern_type, std::mt19937 &mt, const bool multi_core = false);
			unsigned int* GetPatternBinary(const int input) { return binary_pattern_[input]; }
			int GetN(void) { return n_; }
			int GetPatternNumber(void) { return static_cast<int>(binary_pattern_.size()); }
		};
	}
}