#include <iostream>
#include <string>
#include <vector>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <algorithm>
#include <map>
#include <fstream>
#include <queue>


using namespace std;

int main() {
	int N, M, LHScol;

	// read the input table
	ifstream inFile("sdata.txt");

	inFile >> N >> M >> LHSCol;
	cout << N << " " << M << endl;
	
	for ( int i=0 ; i<N ; i++ ) {
		for ( int j=0 ; j<M ; j++ ) {
			if ( j==LHSCol ) {
				inFile >> LHS[i];
			}
			else {
				inFile >> data[i][j-(j>LHSCol)];
			}
		}
	}

	// data structure: use hashtable to save the state for each node
	// for each node: 0--unidentified, 1--true,  2--false
	// the validation algorithm for each node: cdValid(vector<vector<int>>)
	map<int, short> nodeValid;
	
	// first check every single attribute as RHS
	// **********DO---cdValid(LHSCol, RHSCols);
	for ( int j=0 ; j<M ; j++ ) {
		int tmp[1] = {j};
		vector<int> tmpV(tmp, tmp+1);
		if ( cdValid(data, LHSCol, tmpV) ) {
			nodeValid[] = 1;
		}
		else {
			nodeValid[] = 2;
		}
	}

	// then check the RHS with every attribute
	// at most M attributes on the RHS, so state valid range is [0, 2^M-1]
	if ( 1 ) {
		vector<int> tmpV;
		for ( int i=0 ; i<M ; i++ ) {
			tmpV.push_back(i);	
		}
		if ( cdValid(data, LHSCol, tmpV) ) {
			nodeValid[(1<<M)-1] = 1;
		}
		else {
			nodeValid[(1<<M)-1] = 2;
		}
	}

	// keep discovering new nodes
	int nodesLeft = (1<<M)-1-M-1;
	while ( nodesLeft ) {
		// first try to discover new CD from 
	}


/*
	// use BFS to visit all the nodes, use a queue to do the BFS process
	queue<int> state;
	// first node, state = 000...000
	state.push(0);
	while ( !state.empty() ) {
		for ( int i=0 ; i<M ; i++ ) {
			// try to add new attribute
		}

	}
*/

}
