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
#include <bitset>
#include <set>

using namespace std;

int getState(vector<int> atts) {
	int ans = 0;

	for ( int i=0 ; i<atts.size() ; i++ ) {
		ans += (1<<atts[i]);
	}

	return ans;
}

bool containsAtt(int state, int col) {
	int digit = (1<<col);
	return ( (state & digit) == digit );
}


int main() {
	int N, M, LHSCol;

	vector<vector<int> > data(N, vector<int>(M, 0));

	// read the input table
	ifstream inFile("sData.txt");

	inFile >> N >> M >> LHSCol;
	cout << N << " " << M << endl;

	cout << "Reading table..........DONE" << endl;
	// for all the inference edges
	map<int, vector<int> > neighbours;

	// readin all functional dependencies
	// save them in a map
	map<int, int> fdList;
	int fdCount, odCount;
	inFile >> fdCount;
	for ( int i=0 ; i<fdCount ; i++ ) {
		int col1, col2;
		inFile >> col1 >> col2;
		fdList[col1] = col2;
		cout << col1 << " " << col2 << endl;

		// add edges
		for ( int state=1 ; state < (1<<M) ; state++ ) {
			if ( containsAtt(state, col1) && !containsAtt(state, col2) ) {
				int destState = state + ( 1 << col2 );
				neighbours[state].push_back( destState);
				neighbours[destState].push_back( state );
				cout << "edge " << bitset<8>(state) << " " << bitset<8>(destState) << endl;
			}
		}
	}

	// readin all order dependencies
	set<int> odList;
	inFile >> odCount;
	for ( int i=0 ; i<odCount ; i++ ) {
		int col;
		inFile >> col;
		odList.insert(col);
		cout << "OD " << "A ~> " << col << endl;

		// add edges
		for ( int state=0 ; state < (1<<M) ; state++ ) {
			if ( !containsAtt(state, col) ) {
				int destState = state + ( 1<< col );
				neighbours[state].push_back( destState );
				neighbours[destState].push_back( state );
				cout << "od edge " << bitset<8>(state) << " " << bitset<8>(destState) << endl;
			}
		}
	}

	vector<int> LHS(N, 0);
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

/*
	// data structure: use hashtable to save the state for each node
	// for each node: 0--unidentified, 1--true,  2--false
	// the validation algorithm for each node: cdValid(vector<vector<int>>)
	map<int, short> nodeValid;
	
	// first check every single attribute as RHS
	// **********DO---cdValid(LHSCol, RHSCols);
	for ( int j=0 ; j<M ; j++ ) {
		int tmp[1] = {j};
		vector<int> tmpV(tmp, tmp+1);
	1	if ( cdValid(data, LHSCol, tmpV) ) {
			nodeValid[getState(tmpV)] = 1;
		}
		else {
			nodeValid[getState(tmpV)] = 2;
		}
	}

	// then check the RHS with every attribute
	// at most M attributes on the RHS, so valid state range is [0, 2^M-1]
	if ( 1 ) {
		vector<int> tmpV;
		for ( int i=0 ; i<M ; i++ ) {
			tmpV.push_back(i);	
		}
		if ( cdValid(data, LHSCol, tmpV) ) {
			nodeValid[getState(tmpV)] = 1;
		}
		else {
			nodeValid[getState(tmpV)] = 2;
		}
	}

	// keep discovering new nodes
	int nodesLeft = (1<<M)-1-M-1;

	// construct inference edges


	// maintain a set of directly verifiable nodes
	set<int> reachableNodes;

	while ( nodesLeft ) {
		while ( )
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

	return 0;
}
