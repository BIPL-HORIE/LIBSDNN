#include "mex.h"
#include "LIBSDNN.h"
#pragma comment(lib,"LIBSDNN.lib")

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

void sdnnTrain(const std::string &setting_filename,mwSize input_number, double *train_sample, double *train_target, mwSize train_n, const std::string &save_filename, const std::string &complete_condition)
{
    libsdnn::SDNN sdnn;
    sdnn.InitSDNN(setting_filename);   

    std::vector<std::vector<double>> input_list;
    std::vector<double> target_list;
    int c = 0;
    for(mwSize j = 0; j < train_n; j++)
    {
        std::vector<double> input_buffer;
        for(mwSize i = 0; i < input_number; i++)
        {
            input_buffer.push_back(train_sample[c++]);
        }
        input_list.push_back(input_buffer);
        target_list.push_back(train_target[j]);
    }
    mexPrintf("%d\n",train_n);
	sdnn.Train(input_list,target_list,complete_condition);
    sdnn.Save(save_filename);
}

void mexFunction(int nlhs, mxArray *plhs[],
                 int nrhs, const mxArray *prhs[])
{
    if(nrhs!=5) 
    {
        mexErrMsgIdAndTxt("libsdnn:SDNN:nrhs",
                      "Five inputs required.");
    }

    if(nlhs!=0)
    {
        mexErrMsgIdAndTxt("libsdnn:SDNN:nlhs",
                      "No output required.");
    }    
    
    if ( mxIsChar(prhs[0]) != 1)
    {
      mexErrMsgIdAndTxt( "libsdnn:SDNN:sdnnTrainAndEstimate:inputNotString",
              "Input must be a string.");
    }
        
    if( !mxIsDouble(prhs[1]) || 
        mxIsComplex(prhs[1]))
    {
        mexErrMsgIdAndTxt("libsdnn:SDNN:sdnnTrainAndEstimate:notDouble",
            "Input matrix must be type double.");
    }
    
    if( !mxIsDouble(prhs[2]) || 
        mxIsComplex(prhs[2]))
    {
        mexErrMsgIdAndTxt("libsdnn:SDNN:sdnnTrainAndEstimate:notDouble",
            "Input matrix must be type double.");
    }
    
    if ( mxIsChar(prhs[3]) != 1)
    {
      mexErrMsgIdAndTxt( "libsdnn:SDNN:sdnnTrainAndEstimate:inputNotString",
              "Input must be a string.");
    }

    if ( mxIsChar(prhs[4]) != 1)
    {
      mexErrMsgIdAndTxt( "libsdnn:SDNN:sdnnTrainAndEstimate:inputNotString",
              "Input must be a string.");
    }
    
    if(mxGetM(prhs[1]) < 2)
    {
        mexErrMsgIdAndTxt("libsdnn:SDNN:sdnnTrainAndEstimate:input_number < 2",
            "Input number must be 2 and more.");       
    }
    
    if(mxGetN(prhs[1])!=mxGetN(prhs[2]))
    {
        mexErrMsgIdAndTxt("libsdnn:SDNN:sdnnTrainAndEstimate:sample_number_collumped",
                      "input_sample_number != target_sample_number.");
    }
    
    double *train_sample;
    train_sample = mxGetPr(prhs[1]);
    double *train_target;
    train_target = mxGetPr(prhs[2]);
        
    mwSize train_n = (mwSize)mxGetN(prhs[1]);
   
    char *setting_filename = mxArrayToString(prhs[0]);   
    char *condition = mxArrayToString(prhs[3]);
    char *save_filename = mxArrayToString(prhs[4]);

    std::ifstream test_file(setting_filename);
    if(test_file.is_open() != 1)
    {
         mxFree(setting_filename);
         mexErrMsgIdAndTxt( "libsdnn:SDNN:sdnnTrainAndEstimate:is_not_open",
              "Could not open setting file");
    }
    test_file.close();
    
    const std::string buffer(libsdnn::CheckParameterFile(setting_filename));
    mexPrintf("parameters...%s\n",buffer.c_str());
    if(buffer != "OK")
    {
         mexErrMsgIdAndTxt( "libsdnn:SDNN:sdnnTrainAndEstimate:incorrect_parameter",
              buffer.c_str());
    }
        
    if(!libsdnn::CheckCondition(condition))
    {
         mexErrMsgIdAndTxt( "libsdnn:SDNN:sdnnTrainAndEstimate:incorrect_condition",
              buffer.c_str());
    }
    mexPrintf("complete condition...OK\n");
    
    sdnnTrain(setting_filename,mxGetM(prhs[1]),train_sample,train_target,train_n,save_filename,condition);   
    mxFree(setting_filename);
    mxFree(save_filename);
}