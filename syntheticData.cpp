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
#include <time.h>

using namespace std;


ofstream testLog("log.txt");

string to_string(int num) {
    if ( num==0 ) {
        return "0";
    }

    string result;
    while ( num!=0 ) {
        result = (char)(num%10+'0') + result;
        num /= 10;
    }

    return result;
}

bool cmp(vector<int> a, vector<int> b) {
    return a[0] < b[0];
}

void generate_CD(vector<vector<int> > &data, vector<int> colIDs) {
    int N = data.size();
    //int cols = colIDs.size();

    // remove duplicates
    int pFast = 0, pSlow = 0;
    while ( pFast < colIDs.size() ) {
    	while ( pFast < colIDs.size() && ( colIDs[pFast]==colIDs[pFast-1] || colIDs[pFast]==0 ) ) {
    		pFast++;
    	}
    	if ( pFast < colIDs.size() ) {
    		colIDs[pSlow++] = colIDs[pFast++];
    	}
    }

    int cols = pSlow;
    for ( int i=0 ; i<cols ; i++ ) cout << colIDs[i] << " ~ "; cout << endl;
	// we dont consider the value of A here
	// genenate the tuples row by row
	// first row
	string lastRowValue = "";
	for ( int j=0 ; j<cols ; j++ ) {
		int jj = colIDs[j];
		data[0][jj] = rand() % 6;
		lastRowValue = lastRowValue + "_" + to_string(data[0][jj]);
	}

	bool lastAllEq = true;
	bool reset = false;
	// remaining rows
	for ( int i=1 ; i<N ; i++ ) {
		string currRowValue = "";

		if ( !reset && data[i][0]!=data[i-1][0] && rand()%2==0 ) {
            // keep this cluster all equal
            do {
                currRowValue = "";
                for ( int j=0 ; j<cols ; j++ ) {
                    int jj = colIDs[j];
                    data[i][jj] = data[i-1][jj] + rand() % 2;
                    currRowValue = currRowValue + "_" + to_string(data[i][jj]);
                }
            }while ( !lastAllEq && data[i][0]!=data[i-1][0] && currRowValue==lastRowValue );
            lastRowValue = currRowValue;

            while ( i+1<N && data[i+1][0]==data[i][0] ) {
                i++;
                for ( int j=0 ; j<cols ; j++ ) {
                    int jj = colIDs[j];
                    data[i][jj] = data[i-1][jj];
                }
                lastRowValue = currRowValue;
            }

            lastAllEq = true;
            reset = false;
            continue;
		}

        // generate new value
		for ( int j=0 ; j<cols ; j++ ) {
			int jj = colIDs[j];
			data[i][jj] = data[i-1][jj] + rand() % 2;
			currRowValue = currRowValue + "_" + to_string(data[i][jj]);
		}

		if ( currRowValue == lastRowValue ) {
			// same value, redo
			i--;
			reset = true;
			continue;
		}

		// testLog << "CURR " << currRowValue << endl;

		lastRowValue = currRowValue;
		lastAllEq = false;
		reset = false;
	}
}



int main(int argc, char* argv[]) {
	int N, M, fdCount, odCount, cdCount;

	N = atoi(argv[1]);
	M = atoi(argv[2]);
	fdCount = atoi(argv[3]);
	odCount = atoi(argv[4]);
	cdCount = atoi(argv[5]);

	ofstream outFile("sData.txt");
	outFile << N << " " << M << endl;

    srand(time(NULL));

    // matrix initialization
    vector<vector<int> > data ( N, vector<int>(M, 0) );

    // generate column A, as the LHS
    for ( int i=0 ; i<N ; i++ ) {
        data[i][0] = rand()%(N/3);
    }

    // sort the table by A
    sort(data.begin(), data.end(), cmp);

	// add some random data
    // for each row, either generate some random data, OR, use an identical row from previous ones
	int upperBound = N/M+1;
	int seed = sqrt(M)+3;
    for ( int j=1 ; j<M ; j++ ) {
        data[0][j] = rand() % seed;
    }
	for ( int i=1 ; i<N ; i++ ) {
        if ( rand()%N < N/3+5 ) {
            int tmp = rand()%4;
            for ( int j=1 ; j<M ; j++ ) {
                data[i][j] = data[max(i-tmp,0)][j];
            }
		}
        else
        {
            for ( int j=1 ; j<M ; j++ ) {
                data[i][j] = data[i-1][j] + rand() % 2;
            }
        }
	}

    // add CD
    // outFile << cdCount << "\n\n";
    if ( 1 ) {
    	vector<int> list;
    	for ( int i=0 ; i<cdCount ; i++ ) {
            int col = rand() % (M-1) + 1;
    		list.push_back( col );
    	}
    	sort( list.begin(), list.end() );
    	generate_CD( data, list );
    }

	// add FD
	outFile << fdCount << endl;
	for ( int timer=0 ; timer<odCount ; timer++ ) {
		int col1 = rand() % (M-1) + 1;
		int col2 = rand() % (M-1) + 1;
		if ( col1==col2 ) {
            timer--;
            continue;
		}
		if ( col1 > col2 ) {
            col1 = col1+col2;
            col2 = col1-col2;
            col1 = col1-col2;
		}
		for ( int i=0 ; i<N ; i++ ) {
			data[i][col2] = (data[i][col1] * 32767) % (2*upperBound);
		}
/*
		for ( int i=0 ; i<N ; i++ ) {
			if ( i==0 || data[i][0]!=data[i-1][0] ) {
				data[i][col] = 100 + rand() % upperBound;
			}
			else {
				data[i][col] = data[i-1][col];
			}
		}
*/
		// generate FD: col1 -> col2
		outFile << col1 << " " << col2 << endl;
	}
	outFile << endl;

	// add OD
	outFile << odCount << endl;
	for ( int timer=0 ; timer<odCount ; timer++ ) {
		int col = rand() % (M-1) + 1;
		for ( int i=0 ; i<N ; i++ ) {
			if ( i==0 ) {
				data[i][col] = rand() % 5;
			}
			else if ( data[i][0]!=data[i-1][0] ) {
				data[i][col] = data[i-1][col] + rand() % 5;
			}
			else {
				data[i][col] = data[i-1][col];
			}
		}
		// generate OD: {A<} ~> {C<=}, {A=} ~> {C=}
		outFile << col << " ";
	}
	outFile << "\n\n";


	// print
	for ( int i=0 ; i<N ; i++ ) {
		for ( int j=0 ; j<M ; j++ ) {
			outFile << data[i][j] << " ";
		}
		outFile << endl;
	}

	cout << "Data generated, see sData.txt" << endl;

    outFile.close();
    testLog.close();
	return 0;
}

