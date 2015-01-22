#include <iostream>
#include <string>
#include <vector>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <algorithm>
#include <map>

using namespace std;

bool cmp(vector<int> a, vector<int> b) {
    return a[0] < b[0];
}

void generate_CD(vector<vector<int> > &data, vector<int> colIDs) {
    int N = data.size();
    int cols = colIDs.size();

	// we dont consider the value of A here
	// genenate the tuples row by row
	// first row
	string lastRowValue = "";
	for ( int j=0 ; j<cols ; j++ ) {
		int jj = colIDs[j];
		data[0][jj] = rand() % 6;
		lastRowValue = lastRowValue + "_" + to_string(data[0][jj]);	
	}
	// remaining rows
	for ( int i=1 ; i<N ; i++ ) {
		string currRowValue = "";
		for ( int j=0 ; j<cols ; j++ ) {
			int jj = colIDs[j];
			data[i][jj] = data[i-1][jj] + rand() % 2;
			currRowValue = currRowValue + "_" + to_string(data[i][jj]);
		}
		
		if ( currRowValue == lastRowValue ) {
			// same value, redo
			i--;
			continue;
		}
		
		lastRowValue = currRowValue;
	}
}



int main() {
    srand(time(NULL));
    // set the parameter: # of rows and # of attributes
    int N = 15;
    int M = 6;

    // matrix initialization
    vector<vector<int> > data ( N, vector<int>(M, 0) );

    // generate column A, as the LHS
    for ( int i=0 ; i<N ; i++ ) {
        data[i][0] = rand()%10;
    }

    // sort the table by A
    sort(data.begin(), data.end(), cmp);

	// add some random data
	int upperBound = N*2;
	for ( int i=0 ; i<N ; i++ ) {
		for ( int j=1 ; j<M ; j++ ) {
			data[i][j] = rand() % upperBound;
		}
	}

	// add FD
	for ( int timer=0 ; timer<5 ; timer++ ) {
		int col = rand() % (M-1) + 1;
		for ( int i=0 ; i<N ; i++ ) {
			if ( i==0 || data[i][0]!=data[i-1][0] ) {
				data[i][col] = 100 + rand() % upperBound;
			}
			else {
				data[i][col] = data[i-1][col];
			}
		}
	}

	// add OD
	for ( int timer=0 ; timer<5 ; timer++ ) {
		int col = rand() % (M-1) + 1;
		for ( int i=0 ; i<N ; i++ ) {
			if ( i==0 ) {
				data[i][col] = 200 + rand() % 5;
			}
			else if ( data[i][0]!=data[i-1][0] ) {
				data[i][col] = data[i-1][col] + rand() % 5;
			}
			else {
				data[i][col] = data[i-1][col];
			}
		}

	}

    // add CD 
	int aa[3] = {2,3,4};
	vector<int> tmp(aa,aa+3);
	generate_CD( data, tmp );
	
	// print
	for ( int i=0 ; i<N ; i++ ) {
		for ( int j=0 ; j<M ; j++ ) {
			cout << data[i][j] << " ";
		}
		cout << endl;
	}

	return 0;
}

