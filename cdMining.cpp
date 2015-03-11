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

//ofstream testFile("llllllog.txt");

long long nodesLeft;
long long maxState;

long long global_LHS;

bool cmp(vector<string> a, vector<string> b) {
    // only makes sense when
    return ( atoi(a[global_LHS].c_str()) < atoi(b[global_LHS].c_str()) );
}

struct CompareDeg {
    bool operator()(pair<long long,long long> p1, pair<long long,long long> p2) {
    	return p1.second < p2.second;
    }
};

// data structure: use hashtable to save the state for each node
// for each node: 0--unidentified, 1--true,  2--false
// the validation algorithm for each node: cdValid(vector<vector<long long>>)
map<long long, short> nodeValid;

// vector newValidated saves the newly validately nodes
vector<long long> newValidated;

// vector validRHS/invalidRHS saves the all the valid/invalid RHS attribute combinations
vector<long long> validRHS, invalidRHS;

// use this priority queeue to get the nodes with largest degree
priority_queue<pair<long long,long long>, vector<pair<long long,long long> >, CompareDeg > largestDeg;


void foundValidNode(long long state) {
	if ( nodeValid[state] || (state&1)==1 ) {
		return;
	}
	nodeValid[state] = 1;
	newValidated.push_back( state );
	validRHS.push_back( state );
	nodesLeft--;
}


void foundInvalidNode(long long state) {
	if ( nodeValid[state] || (state&1)==1 ) {
		return;
	}
	nodeValid[state] = 2;
	newValidated.push_back( state );
	invalidRHS.push_back( state );
	nodesLeft--;
}

long long cdValid(vector<vector<string> > data, long long LHSCol, vector<long long> RHSCols) {
    // the index of LHS and RHS attributes

    //***
    //***
    global_LHS = LHSCol;

/*
    cout << "RHS ATTs" << endl;
    for ( long long i=0 ; i<RHSCols.size() ; i++ ) {
        cout << RHSCols[i] << " ";
    }
    cout << endl;
*/

    // first sort on LHS attribute
    sort(data.begin(), data.end(), cmp );

    // then do the validation algorithm
    long long n = data.size();
    bool lastAllEq = true, currAllEq = true;
    bool currMustEq = false;

    // generate last tuple
    string lastTuple = "";
    for ( long long j=0 ; j<RHSCols.size() ; j++ ) {
        lastTuple = lastTuple + data[0][RHSCols[j]];
    }

    map<string, bool> BList;
    BList[lastTuple] = true;
    for ( long long i=1 ; i<n ; i++ ) {
        // generate this partial tuple
        string currTuple = "";
        for ( long long j=0 ; j<RHSCols.size() ; j++ ) {
            currTuple = currTuple + data[i][RHSCols[j]];
        }

        //cout << currTuple << endl;
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


long long getState(vector<long long> atts) {
	long long ans = 0;

	for ( long long i=0 ; i<atts.size() ; i++ ) {
		ans += (1<<atts[i]);
	}

	return ans;
}

bool containsAtt(long long state, long long col) {
	long long digit = (1<<col);
	return ( (state & digit) == digit );
}

void printState(long long state) {
    for ( long long i=1 ; i<=16 ; i++ ) {
        if ( ((1<<i) & state) == ( 1<< i ) ) {
            //testFile << i << " ";
        }
    }
    //testFile << "$" << endl;
}

bool cover(long long a, long long b) {
    while ( a || b ) {
        if ( !(a&1) && (b&1) ) {
            return false;
        }
        a /= 2;
        b /= 2;
    }
    return true;
}

string itoa(long long number) {
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
	long long N, M;

	// read the input table
	ifstream inFile("sData.txt");

	inFile >> N >> M;

	vector<vector<long long> > data(N, vector<long long>(M, 0));
	maxState = ( 1<< M ) - 1;


	// for all the inference edges
	map<long long, vector<long long> > neighbours;
	map<long long, long long> nodeDeg;

	// readin all functional dependencies
	// save them in a map
	map<long long, long long> fdList;
	long long fdCount, odCount;
	inFile >> fdCount;
	//cout << "Number of FDs: " << fdCount << endl;
	for ( long long i=0 ; i<fdCount ; i++ ) {
		long long col1, col2;
		inFile >> col1 >> col2;
		fdList[col1] = col2;
		//cout << col1 << " " << col2 << endl;

		// add edges
		for ( long long state=2 ; state < maxState ; state+=2 ) {
			if ( containsAtt(state, col1) && !containsAtt(state, col2) ) {
				long long destState = state + ( 1 << col2 );
				neighbours[state].push_back( destState);
				neighbours[destState].push_back( state );

				nodeDeg[state]++;
				nodeDeg[destState]++;
				//cout << "edge " << bitset<8>(state) << " " << bitset<8>(destState) << endl;
			}
		}
	}

	// readin all order dependencies
	set<long long> odList;
	inFile >> odCount;
	//cout << "Number of ODs: " << odCount << endl;
	for ( long long i=0 ; i<odCount ; i++ ) {
		long long col;
		inFile >> col;
		odList.insert(col);
		//cout << "OD " << "A ~> " << col << endl;

		// add edges
		for ( long long state=2 ; state < maxState ; state+=2 ) {
			if ( !containsAtt(state, col) ) {
				long long destState = state + ( 1<< col );
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
	map<long long,long long>::iterator iter;

	for ( iter=nodeDeg.begin() ; iter!=nodeDeg.end() ; iter++ ) {
		largestDeg.push( make_pair( iter->first, iter->second ) );
	}

	/*
	while ( !largestDeg.empty() ) {
		cout << largestDeg.top().first  << " ! " << largestDeg.top().second << endl;
		largestDeg.pop();
	}
	*/

	for ( long long i=0 ; i<N ; i++ /*,cout << endl*/ ) {
		for ( long long j=0 ; j<M ; j++ ) {
			inFile >> data[i][j];
			//cout << data[i][j] << " ";
		}
	}
	//cout << "read in table..........done" << endl;


	// every time we want a different LHS, just swap it with the first column...ZZZzzz

	// convert data to string
	vector<vector<string> > newData ( N, vector<string>(M, ""));
	for ( long long i=0 ; i<N ; i++ ) {
		for ( long long j=0 ; j<M ; j++ ) {
			newData[i][j] = itoa(data[i][j]);
		}
	}

	long long LHSCol = 0;

/*
    long long tmp[5] = {1,2,3,4,5};
	vector<long long> tmpV(tmp, tmp+5);
    cout << "RESULT :" << cdValid(newData, LHSCol, tmpV) << endl;

	return 0;
*/

	// first check every single attribute as RHS

	for ( long long j=1 ; j<M ; j++ ) {
		long long tmp[1] = {j};
		vector<long long> tmpV(tmp, tmp+1);
		long long currState = getState(tmpV);

		if ( cdValid(newData, LHSCol, tmpV)==0 ) {
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
		vector<long long> tmpV;
		for ( long long i=1 ; i<M ; i++ ) {
			tmpV.push_back(i);
		}
		long long currState = getState(tmpV);
        //cout << "ERROR: " << currState << "  " << cdValid(newData, LHSCol, tmpV) <<endl;
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
	nodesLeft = maxState-M-1;

    cout << "S" << maxState << endl;
    queue<long long> leftOver;
	for ( long long state=2 ; state<maxState ; state+=2 ) {
        //cout << state << " " << nodeValid[state] << " " << nodeDeg[state] <<  endl;
        // add the nodes with no inference edge long longo leftOver set
        if ( nodeValid[state]==0 && nodeDeg[state]==0 ) {
            // not verified and no inference edges
            leftOver.push(state);
        }
	}

	// construct inference edges


	// malong longain a set of directly verifiable nodes
	set<long long> reachableNodes;

	cout << "start searching" << endl;

	long long optimized = 0;

	while ( !largestDeg.empty() || !leftOver.empty() ) {
        cout << largestDeg.size() << " and " << leftOver.size() << endl;
		// while we still have nodes that are not verifed yet
		while ( !newValidated.empty() ) {
			// choose any one node, so why not the first one
			long long p = *newValidated.begin();
			// remove it from the set
			newValidated.erase(newValidated.begin());
			// apply inf rules on this node
			if ( nodeValid[p]==1 ) {
				// there's a VALID node here, use it to expand
				// first apply union rule
				//testFile << "FOUND VALID" << endl;
                //printState(p);

				for ( long long i=0 ; i<validRHS.size() ; i++ ) {
					// go through every valid RHS, find disjolong long ones
					if ( (validRHS[i] & p)==0 && nodeValid[validRHS[i] | p] == 0 && ((validRHS[i] | p)&1)==0 ) {
						// found a new valid node!
						foundValidNode( validRHS[i] | p );
						optimized++;
						//testFile << "SET valid 1" << endl;
                        //printState(validRHS[i] | p);
					}
				}
				// then apply rules with OD/FD
				for ( long long i=0 ; i<neighbours[p].size() ; i++ ) {
					//testFile << "cost 1: " << neighbours[p].size() << endl;
					if ( neighbours[p][i] > p && nodeValid[neighbours[p][i]]==0 && (neighbours[p][i]&1)==0 ) { // as we are going DOWN
						foundValidNode( neighbours[p][i] );
						optimized++;
						//testFile << "SET valid 2" << endl;
						//printState(neighbours[p][i]);
					}
				}
			}
			else if ( nodeValid[p]==2 ) {
				// there's a INVALID node here, use it to expand
				// first apply the union rule
				//testFile << "FOUND IIIIIIIIIIINVALID" << endl;
                printState(p);
				for ( long long i=0 ; i<validRHS.size() ; i++ ) {
					// go through every valid RHS, find disjoint ones
					// p should full contain validRHS[i] (should we use (validRHS[i] & p) + (validRHS[i] ^ p) == p)? )
					if ( (validRHS[i]<p ) && cover(p, validRHS[i]) && nodeValid[p - validRHS[i]]==0 && ((p - validRHS[i])&1)==0 ) {
						// found a new valid node!
						foundInvalidNode( p - validRHS[i] );
						optimized++;
						//testFile << "SET INvalid 1 " << p << " " << validRHS[i] << endl;
						//printState( p - validRHS[i] );
					}
				}
				// then apply rules with OD/FD
				for ( long long i=0 ; i<neighbours[p].size() ; i++ ) {
					if ( neighbours[p][i] < p && nodeValid[neighbours[p][i]]==0 && (neighbours[p][i]&1)==0 ) { // as we are going UP
						//testFile << "cost 1: " << neighbours[p].size() << endl;
						foundInvalidNode( neighbours[p][i] );
						optimized++;
						//testFile << "SET INvalid 2" << endl;
						//printState( neighbours[p][i] );
					}
				}
			}
		}

		if ( nodesLeft<=0 ) {
                cout << "node left" << endl;
			break;
		}

		// if we are here, it means we are stuck and we need to use validation algorithm to break the dilemma
		if ( largestDeg.empty() ) {
            // no more nodes with inference edges
            long long curr = leftOver.front();
            // find a node that is not validated
            while ( !leftOver.empty() && nodeValid[curr]!=0 ) {
                leftOver.pop();
                curr = leftOver.front();
            }
            if ( leftOver.empty() ) {
                // all nodes are verified, stop the loop
                cout << " left over " << endl;
                break;
            }

            vector<long long> tmpV;
            for ( long long i=1 ; i<M ; i++ ) {
                if ( ((1<<i) & curr ) == (1<<i) ) {
                    tmpV.push_back(i);
                }
            }

            long long res = cdValid(newData, LHSCol, tmpV);
            if ( res==0 ) {
                foundValidNode(curr);
            }
            else {
                foundInvalidNode(curr);
            }
            continue;
		}

		while ( !largestDeg.empty() ) {
			long long curr = largestDeg.top().first;
			// cout << curr << " ";
			largestDeg.pop();
			if ( nodeValid[curr]!=0 ) {
				// if node has been verified, skip this node
                //cout << "VERIFIED" << endl;
				continue;
			}
			else {
				vector<long long> tmpV;
				for ( long long i=1 ; i<M ; i++ ) {
					if ( ((1<<i) & curr ) == (1<<i) ) {
						tmpV.push_back(i);
					}
				}
				long long res = cdValid(newData, LHSCol, tmpV);
				if ( res==0 ) {
					foundValidNode(curr);
				}
				else {
					foundInvalidNode(curr);
				}
			}
		}
	}
        cout << largestDeg.size() << " and " << leftOver.size() << endl;


/*
	// use BFS to visit all the nodes, use a queue to do the BFS process
	queue<long long> state;
	// first node, state = 000...000
	state.push(0);
	while ( !state.empty() ) {
		for ( long long i=0 ; i<M ; i++ ) {
			// try to add new attribute
		}

	}
*/

	cout << "the valid RHS are: " << endl;
	for ( long long i=0 ; i<validRHS.size() ; i++ ) {
        //cout << "valid State: " << validRHS[i] << endl;
        //cout << "valid Attributes: " << endl;
		for ( long long j=0 ; j<M ; j++ ) {
			if ( ((1<<j) & validRHS[i] ) == (1<<j) ) {
				cout << j << " ";
			}
		}
		cout << endl;
	}
	cout << optimized << endl;
	cout << "END" << endl;
	cout << "We have optimized " << (double)optimized/(1<<(M-1))*100 << "% of the nodes" << endl;
    printf("Time taken: %.2fs\n", (double)(clock() - tStart)/CLOCKS_PER_SEC);

	getchar();
	getchar();
	return 0;
}
