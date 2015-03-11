Code for algorithms used in clustering dependency.

cdValid.cpp---contains the validation algorithm for clustering dependency

cdMining.cpp--contains the mining algorithm for clustering dependency

syntheticData.cpp--to generate the sample data for validation/mining algorithm

cdMiningNaive--Check EVERY node to find out all the valid CDs

cdMiningTest--For testing only

MANUAL:

1) g++ -o input syntheticData.cpp

2) ./input -N -M -fdCount -odCount -cdCount

    eg. ./input 1000 12 2 2 2

3) g++ -o mining cdMining.cpp
    ./Mining
