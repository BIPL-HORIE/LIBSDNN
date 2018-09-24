//# Licensed under the BSD 3-Clause License - https://opensource.org/licenses/BSD-3-Clause

#include "mex.h"
#include "LIBSDNN.h"
#pragma comment(lib,"LIBSDNN.lib")

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

void sdnnTest(const std::string &load_filename,mwSize input_number, double *test_sample,  mwSize test_n, double *result)
{
    libsdnn::SDNN sdnn;
    sdnn.Load(load_filename);
    
    std::vector<std::vector<double>> input_list;
    int c = 0;
    for(mwSize j = 0; j < test_n; j++)
    {
        std::vector<double> input_buffer;
        for(mwSize i = 0; i < input_number; i++)
        {
            input_buffer.push_back(test_sample[c++]);
        }
        input_list.push_back(input_buffer);
    }   
    for(mwSize i = 0; i < test_n; i++)
    {
        result[i] = sdnn.Estimate(input_list[i]);
    }
}

void mexFunction(int nlhs, mxArray *plhs[],
                 int nrhs, const mxArray *prhs[])
{
    if(nrhs!=2) 
    {
        mexErrMsgIdAndTxt("bipl:sdnn:SDNN:nrhs",
                      "Two inputs required.");
    }

    if(nlhs!=1)
    {
        mexErrMsgIdAndTxt("bipl:sdnn:SDNN:nlhs",
                      "One output required.");
    }    
    
    if ( mxIsChar(prhs[0]) != 1)
    {
      mexErrMsgIdAndTxt( "bipl:sdnn:SDNN:sdnnTrainAndEstimate:inputNotString",
              "Input must be a string.");
    }
        
    if( !mxIsDouble(prhs[1]) || 
        mxIsComplex(prhs[1]))
    {
        mexErrMsgIdAndTxt("bipl:sdnn:SDNN:sdnnTrainAndEstimate:notDouble",
            "Input matrix must be type double.");
    }
           
    double *test_sample;
    test_sample = mxGetPr(prhs[1]);       
    mwSize test_n = (mwSize)mxGetN(prhs[1]);
   
    char *load_filename = mxArrayToString(prhs[0]);   

    std::ifstream test_file(load_filename);
    if(test_file.is_open() != 1)
    {
         mxFree(load_filename);
         mexErrMsgIdAndTxt( "bipl:sdnn:SDNN:sdnnTest:is_not_open",
              "Could not open setting file");
    }
    
    plhs[0] = mxCreateDoubleMatrix(test_n,1,mxREAL);
    double* result = mxGetPr(plhs[0]);
    
    test_file.close();
    sdnnTest(load_filename,mxGetM(prhs[1]),test_sample,test_n,result);   
    mxFree(load_filename);
}