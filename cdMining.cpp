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
        else if ( !BList[currTuple] &&  data[i][LHSCol] == data[i-1][LHSCol] ) {
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
	//cout << "Number of FDs: " << fdCount << endl;
	for ( int i=0 ; i<fdCount ; i++ ) {
		int col1, col2;
		inFile >> col1 >> col2;
		fdList[col1] = col2;
		//cout << col1 << " " << col2 << endl;

		// add edges
		for ( int state=1 ; state < (1<<M) ; state++ ) {
			if ( 1 && containsAtt(state, col1) && !containsAtt(state, col2) ) {
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
	//cout << "Number of ODs: " << odCount << endl;
	for ( int i=0 ; i<odCount ; i++ ) {
		int col;
		inFile >> col;
		odList.insert(col);
		//cout << "OD " << "A ~> " << col << endl;

		// add edges
		for ( int state=0 ; state < (1<<M) ; state++ ) {
			if ( (state&1)!=1 && !containsAtt(state, col) ) {
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
			newData[i][j] = data[i][j];
		}
	}

	int LHSCol = 0;

	// first check every single attribute as RHS
	for ( int j=0 ; j<M ; j++ ) {
		int tmp[1] = {j};
		vector<int> tmpV(tmp, tmp+1);
		int currState = getState(tmpV);

		if ( cdValid(newData, LHSCol, tmpV)==0 ) {
			//cout << "VALID " << j << endl;
			nodeValid[ currState ] = 1;


			newValidated.push_back( currState );
			validRHS.push_back( currState );
		}
		else {
			nodeValid[ currState ] = 2;

			invalidRHS.push_back( currState );
		}
	}

	// then check the RHS with every attribute
	// at most M attributes on the RHS, so valid state range is [0, 2^M-1]
	if ( 1 ) {
		vector<int> tmpV;
		for ( int i=0 ; i<M ; i++ ) {
			tmpV.push_back(i);	
		}
		int currState = getState(tmpV);

		if ( cdValid(newData, LHSCol, tmpV)==0 ) {
			//cout << "VALID ALL" << endl; 
			nodeValid[ currState ] = 1;

			validRHS.push_back( currState );
		}
		else {
			nodeValid[currState] = 2;

			newValidated.push_back( currState );
			invalidRHS.push_back( currState );
		}
	}

	// keep discovering new nodes
	nodesLeft = (1<<(M-1))-1-M-1;

	// construct inference edges


	// maintain a set of directly verifiable nodes
	set<int> reachableNodes;

	cout << "start searching" << endl;

	while ( nodesLeft ) {
		// while we still have nodes that are not verifed yet
		while ( !newValidated.empty() ) {
			// choose any one node, so why not the first one
			int p = *newValidated.begin();
			// remove it from the set
			newValidated.erase(newValidated.begin());
			// apply inf rules on this node
			if ( nodeValid[p]==1 ) {
				// there's a VALID node here, use it to expand
				// first apply union rule
				for ( int i=0 ; i<validRHS.size() ; i++ ) {
					// go through every valid RHS, find disjoint ones
					if ( (validRHS[i] & p)==0 ) {
						// found a new valid node!
						foundValidNode( validRHS[i] | p );
					}
				}
				// then apply rules with OD/FD
				for ( int i=0 ; i<neighbours[p].size() ; i++ ) {
					if ( neighbours[p][i] > p ) { // as we are going DOWN
						foundValidNode( neighbours[p][i] );
					}
				}
			}
			else if ( nodeValid[p]==2 ) {
				// there's a INVALID node here, use it to expand
				// first apply the union rule
				for ( int i=0 ; i<validRHS.size() ; i++ ) {
					// go through every valid RHS, find disjoint ones
					if ( (validRHS[i]<p ) && ((validRHS[i] & p)==0) ) {
						// found a new valid node!
						foundInvalidNode( p - validRHS[i] );
					}
				}
				// then apply rules with OD/FD
				for ( int i=0 ; i<neighbours[p].size() ; i++ ) {
					if ( neighbours[p][i] < p ) { // as we are going UP
						foundInvalidNode( neighbours[p][i] );
					}
				}
			}
		}

		if ( nodesLeft<=0 ) {
			break;
		}

		// if we are here, it means we are stuck and we need to use validation algorithm to break the dilemma
		while ( !largestDeg.empty() ) {
			int curr = largestDeg.top().first;
			largestDeg.pop();
			if ( nodeValid[curr]!=0 ) {
				// if node has been verifed, skip this node
				continue;
			}
			else {
				vector<int> tmpV;
				for ( int i=1 ; i<M ; i++ ) {
					if ( ((1<<i) & curr ) == (1<<i) ) {
						tmpV.push_back(i);
					}
				}
				int res = cdValid(newData, LHSCol, tmpV);
				if ( res==0 ) {
					foundValidNode(curr);
				}
				else {
					foundInvalidNode(curr);
				}
			}
		}
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

	cout << "the valid RHS are: " << endl;
	for ( int i=0 ; i<validRHS.size() ; i++ ) {
		for ( int j=0 ; j<M ; j++ ) {
			if ( ((1<<j) & validRHS[i] ) == (1<<j) ) {
				cout << j << " ";
			}
		}
		cout << endl;
	}
	cout << endl;
	cout << "END" << endl;
	return 0;
}
