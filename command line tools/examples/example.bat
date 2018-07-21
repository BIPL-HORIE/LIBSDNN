cd ../
SDNN_Training.exe examples/parameter_file.txt examples/training_sample.csv iteration(300) examples/training_result.bin
SDNN_Testing.exe examples/training_result.bin examples/test_sample.csv examples/result.csv
cd examples