#include <iostream>
#include <string>
#include <vector>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <algorithm>
#include <fstream>
#include <map>

using namespace std;

int global_LHS = 0;
int indLHS;
vector<vector<string> > result;

vector<vector<string> > getData() {
    vector<vector<string> > result;

    string line;
    ifstream inFile("breast-cancer-wisconsin.data");
    if (inFile.is_open()) {
        while ( getline (inFile,line) ){
            vector<string> currLine;
            while ( true ) {
                int split = line.find(',');
                if ( split==-1 ) {
                    break;
                }
                string str1 = line.substr(0,split);
                currLine.push_back(str1);
                line = line.substr(split+1, line.length()-split-1);
            }
            currLine.push_back(line);
            if ( !currLine.empty() ) {
                result.push_back( currLine );
            }
        }
        inFile.close();
    }

    return result;
}

void printRes(vector<vector<string> > result) {
    int n = result.size();
    int m = result[0].size();

    for ( int i=0 ; i<n ; i++, cout << endl  ) {
        for ( int j=0 ; j<m ; j++) {
            cout << result[i][j] << " ### ";
        }
    }
}

bool cmp(vector<string> a, vector<string> b) {
    // only makes sense when
    if ( a[global_LHS].find(".")==-1 ) {
        // float
        return ( atof(a[global_LHS].c_str()) < atof(b[global_LHS].c_str()) );
    }
    else {
        // integer
        return ( atoi(a[global_LHS].c_str()) < atoi(b[global_LHS].c_str()) );
    }
}

int cdValid(vector<vector<string> > result, int indLHS, vector<int> indRHS) {
    // the index of LHS and RHS attributes

    global_LHS = indLHS;

    // first sort on LHS attribute
    sort(result.begin(), result.end(), cmp );

    // then do the validation algorithm
    int n = result.size();
    bool lastAllEq = true, currAllEq = true;
    bool currMustEq = false;

    // generate last tuple
    string lastTuple = "";
    for ( int j=0 ; j<indRHS.size() ; j++ ) {
        lastTuple = lastTuple + result[0][indRHS[j]];
    }

    map<string, bool> BList;
    BList[lastTuple] = true;
    for ( int i=1 ; i<n ; i++ ) {
        // generate this partial tuple
        string currTuple = "";
        for ( int j=0 ; j<indRHS.size() ; j++ ) {
            currTuple = currTuple + result[i][indRHS[j]];
        }

        // case 1:
        if ( !BList[currTuple] && result[i][indLHS] != result[i-1][indLHS] ) {
            // dif A, new B
            lastAllEq = currAllEq;
            currAllEq = true;
            currMustEq = false;

            BList[currTuple] = true;
            lastTuple = currTuple;
            continue;
        }
        // case 2:
        else if ( !BList[currTuple] &&  result[i][indLHS] == result[i-1][indLHS] ) {
            // same A, new B
            if ( !currMustEq ) {

                currAllEq = false;

                BList[currTuple] = true;
                lastTuple = currTuple;
                continue;
            }
        }
        // case 3:
        else if ( BList[currTuple] &&  result[i][indLHS] != result[i-1][indLHS] ) {
            if ( currAllEq && lastTuple==currTuple ) {
                // dif A, old B
                currMustEq = true;
                lastTuple = currTuple;
                continue;
            }
        }
        // case 4:
        else if ( BList[currTuple] &&  result[i][indLHS] == result[i-1][indLHS] ) {
            if ( currAllEq && lastTuple==currTuple ) {
                // same A, old B
                currMustEq = true;

                lastTuple = currTuple;
                continue;
            }
        }
        // if we are here, these would be a violation
        return i;   // return the index of the tuple that went wrong
    }

    return 0;   // no violation
}

void dfs(int step, int last, int depth, vector<int> &indRHS, vector<bool> used, vector<int> &candi ) {
    if ( step==depth ) {
        if ( cdValid(result, indLHS, indRHS)==0 ) {
            cout << "FOUND left:" << indLHS << " right:";
            for ( int i=0 ; i<indRHS.size() ; i++ ) {
                cout << indRHS[i] << " ";
            }
            cout << endl;
        }
        return;
    }

    for ( int i=last+1 ; i<candi.size() ; i++ ) {
        if ( !used[i] && candi[i]!=indLHS ) {
            indRHS.push_back( candi[i] );
            used[i] = true;
            dfs(step+1, i, depth, indRHS, used, candi);
            used[i] = false;
            indRHS.pop_back();
        }
    }
}

int main() {
    result = getData();

    // set parameter
    int aa[11]={1,2,3,4,5,6,7,8,9,10};
    for ( indLHS=1 ; indLHS<=10 ; indLHS++ ) {
        vector<int> candi(aa,aa+10);
        vector<int> indRHS;
        // cd-validation{
        vector<bool> used(20,false);
        for ( int depth=1 ; depth<=3 ; depth++ ) {
            dfs(0, -1, depth, indRHS, used, candi);
        }
    }


    //printRes(result);
    // sort on LHS
    //global_LHS = 3; sort(result.begin(), result.end(), cmp ); printRes(result);
}
