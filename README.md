Code for algorithms used in clustering dependency.

cdValid.cpp---contains the validation algorithm for clustering dependency

cdMining.cpp--contains the mining algorithm for clustering dependency

syntheticData.cpp--to generate the sample data for validation/mining algorithm

MANUAL:

1) g++ -o input syntheticData.cpp

2) ./input -N -M -fdCount -cdCount

<<<<<<< HEAD
eg. ./input 100 6 2 2
=======
    eg. ./input 100 6 2 2
>>>>>>> 9100b1e249d31fc2a224518c1f038c99c6cc2a96

3) g++ -o mining cdMining.cpp
    ./Mining
