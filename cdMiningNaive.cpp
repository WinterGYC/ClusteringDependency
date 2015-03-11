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
#include <ctime>

using namespace std;

int nodesLeft;

int global_LHS;

bool cmp(vector<string> a, vector<string> b) {
    // only makes sense when
    return ( atoi(a[global_LHS].c_str()) < atoi(b[global_LHS].c_str()) );
}

struct CompareDeg {
    bool operator()(pair<int,int> p1, pair<int,int> p2) {
    	return p1.second < p2.second;
    }
};

// data structure: use hashtable to save the state for each node
// for each node: 0--unidentified, 1--true,  2--false
// the validation algorithm for each node: cdValid(vector<vector<int>>)
map<int, short> nodeValid;

// vector newValidated saves the newly validately nodes
vector<int> newValidated;

// vector validRHS/invalidRHS saves the all the valid/invalid RHS attribute combinations
vector<int> validRHS, invalidRHS;

// use this priority queeue to get the nodes with largest degree
priority_queue<pair<int,int>, vector<pair<int,int> >, CompareDeg > largestDeg;


void foundValidNode(int state) {
	if ( nodeValid[state] || (state&1)==1 ) {
		return;
	}
	nodeValid[state] = 1;
	newValidated.push_back( state );
	validRHS.push_back( state );
	nodesLeft--;
}


void foundInvalidNode(int state) {
	if ( nodeValid[state] || (state&1)==1 ) {
		return;
	}
	nodeValid[state] = 2;
	newValidated.push_back( state );
	invalidRHS.push_back( state );
	nodesLeft--;
}

int cdValid(vector<vector<string> > data, int LHSCol, vector<int> RHSCols) {
    // the index of LHS and RHS attributes

    //***
    //***
    global_LHS = LHSCol;
/*
    cout << "RHS ATTs" << endl;
    for ( int i=0 ; i<RHSCols.size() ; i++ ) {
        cout << RHSCols[i] << " ~ ";
    }
    cout << endl;
*/
    // first sort on LHS attribute
    sort(data.begin(), data.end(), cmp );

    // then do the validation algorithm
    int n = data.size();
    bool lastAllEq = true, currAllEq = true;
    bool currMustEq = false;

    // generate last tuple
    string lastTuple = "";
    for ( int j=0 ; j<RHSCols.size() ; j++ ) {
        lastTuple = lastTuple + data[0][RHSCols[j]];
    }


    map<string, bool> BList;
    BList[lastTuple] = true;
    for ( int i=1 ; i<n ; i++ ) {
        // generate this partial tuple
        string currTuple = "";
        for ( int j=0 ; j<RHSCols.size() ; j++ ) {
            currTuple = currTuple + data[i][RHSCols[j]];
        }

        //if ( i<10 ) cout << currTuple << endl;
        // case 1:
        if ( !BList[currTuple] && data[i][LHSCol] != data[i-1][LHSCol] ) {
            // dif A, new B
            lastAllEq = currAllEq;
            currAllEq = true;
            currMustEq = false;

            BList[currTuple] = true;
            lastTuple = currTuple;
            continue;
        }
        // case 2:
        else if ( !BList[currTuple] && data[i][LHSCol] == data[i-1][LHSCol] ) {
            // same A, new B
            if ( !currMustEq ) {

                currAllEq = false;

                BList[currTuple] = true;
                lastTuple = currTuple;
                continue;
            }
        }
        // case 3:
        else if ( BList[currTuple] &&  data[i][LHSCol] != data[i-1][LHSCol] ) {
            if ( currAllEq && lastTuple==currTuple ) {
                // dif A, old B
                currMustEq = true;
                lastAllEq = currAllEq;
                currAllEq = true;

                lastTuple = currTuple;
                continue;
            }
        }
        // case 4:
        else if ( BList[currTuple] &&  data[i][LHSCol] == data[i-1][LHSCol] ) {
            if ( currAllEq && lastTuple==currTuple ) {
                // same A, old B
                currMustEq = true;
                currAllEq = true;

                lastTuple = currTuple;
                continue;
            }
        }
        // if we are here, these would be a violation
        return i;   // return the index of the tuple that went wrong
    }

    return 0;   // no violation
}


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

string itoa(int number) {
	string result = "";

	if ( number==0 ) {
		return "0";
	}
	while ( number!=0 ) {
		result = result + char( number % 10 + '0');
		number /= 10;
	}

	reverse(result.begin() , result.end());
	return result;
}


int main() {
    clock_t tStart = clock();

	int N, M;

	// read the input table
	ifstream inFile("sData.txt");

	inFile >> N >> M;

	vector<vector<int> > data(N, vector<int>(M, 0));


	// for all the inference edges
	map<int, vector<int> > neighbours;
	map<int,int> nodeDeg;

	// readin all functional dependencies
	// save them in a map
	map<int, int> fdList;
	int fdCount, odCount;
	inFile >> fdCount;
	cout << "Number of FDs: " << fdCount << endl;
	for ( int i=0 ; i<fdCount ; i++ ) {
		int col1, col2;
		inFile >> col1 >> col2;
		fdList[col1] = col2;
		cout << col1 << " " << col2 << endl;

		// add edges
		for ( int state=2 ; state < (1<<M) ; state+=2 ) {
			if ( containsAtt(state, col1) && !containsAtt(state, col2) ) {
				int destState = state + ( 1 << col2 );
				neighbours[state].push_back( destState);
				neighbours[destState].push_back( state );

				nodeDeg[state]++;
				nodeDeg[destState]++;
				//cout << "edge " << bitset<8>(state) << " " << bitset<8>(destState) << endl;
			}
		}
	}

	// readin all order dependencies
	set<int> odList;
	inFile >> odCount;
	cout << "Number of ODs: " << odCount << endl;
	for ( int i=0 ; i<odCount ; i++ ) {
		int col;
		inFile >> col;
		odList.insert(col);
		cout << "OD " << "A ~> " << col << endl;

		// add edges
		for ( int state=2 ; state < (1<<M) ; state+=2 ) {
			if ( !containsAtt(state, col) ) {
				int destState = state + ( 1<< col );
				neighbours[state].push_back( destState );
				neighbours[destState].push_back( state );

				nodeDeg[state]++;
				nodeDeg[destState]++;
				//cout << "od edge " << bitset<8>(state) << " " << bitset<8>(destState) << endl;
			}
		}
	}
	//cout << "od edges..........Established" << endl;


	// convert map to priority queue
	map<int,int>::iterator iter;
	for ( iter=nodeDeg.begin() ; iter!=nodeDeg.end() ; iter++ ) {
		largestDeg.push( make_pair( iter->first, iter->second ) );
	}

	/*
	while ( !largestDeg.empty() ) {
		cout << largestDeg.top().first  << " ! " << largestDeg.top().second << endl;
		largestDeg.pop();
	}
	*/

	for ( int i=0 ; i<N ; i++ /*,cout << endl*/ ) {
		for ( int j=0 ; j<M ; j++ ) {
			inFile >> data[i][j];
			//cout << data[i][j] << " ";
		}
	}
	//cout << "read in table..........done" << endl;


	// every time we want a different LHS, just swap it with the first column...ZZZzzz

	// convert data to string
	vector<vector<string> > newData ( N, vector<string>(M, ""));
	for ( int i=0 ; i<N ; i++ ) {
		for ( int j=0 ; j<M ; j++ ) {
			newData[i][j] = itoa(data[i][j]);
		}
	}

	for ( int i=0 ; i<10 ; i++,cout << endl ) {
		for ( int j=0 ; j<M ; j++ ) {
			cout << newData[i][j] << " ";
		}
	}

	int LHSCol = 0;

	nodesLeft = (1<<M)-1-M-1;



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
    cout << "start searching" << endl;
    cout << "the valid states are:" << endl;
	for ( int state=2 ; state<(1<<M)-1 ; state+=2 ) {
        // add the nodes with no inference edge into leftOver set
        vector<int> tmpV;
		for ( int i=1 ; i<M ; i++ ) {
			if ( ((1<<i) & state ) == (1<<i) ) {
                tmpV.push_back(i);
            }
        }

/*
            for ( int i=0 ; i<tmpV.size() ; i++ ) {
                cout << tmpV[i] << " ";
            }
            cout << endl;
*/
        int res = cdValid(newData, LHSCol, tmpV);

        if ( res==0 ) {
            for ( int i=0 ; i<tmpV.size() ; i++ ) {
                cout << tmpV[i] << " ";
            }
            cout << endl;
        }
	}

	/*

	vector<int> tmp;
	tmp.push_back(6);
	cout << cdValid(newData, LHSCol, tmp) << endl;
*/

    printf("Time taken: %.2fs\n", (double)(clock() - tStart)/CLOCKS_PER_SEC);
	cout << "END" << endl;

	getchar();
	getchar();
	return 0;
}
