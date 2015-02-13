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

int global_LHS;

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

bool cmp(vector<string> a, vector<string> b) {
    // only makes sense when
    return ( atoi(a[global_LHS].c_str()) < atoi(b[global_LHS].c_str()) );
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

int cdValid(vector<vector<string> > data) {
    // the index of LHS and RHS attributes
    int indLHS = 0;
    vector<int> indRHS;
    
    //***
    indRHS.push_back(1);
    indRHS.push_back(2);
    //***
    global_LHS = indLHS;
    
    // first sort on LHS attribute
    sort(data.begin(), data.end(), cmp );
    printRes( data );
    
    // then do the validation algorithm
    int n = data.size();
    bool lastAllEq = true, currAllEq = true;
    bool currMustEq = false;
    
    // generate last tuple
    string lastTuple = "";
    for ( int j=0 ; j<indRHS.size() ; j++ ) {
        lastTuple = lastTuple + data[0][indRHS[j]];
    }
    
    map<string, bool> BList;
    BList[lastTuple] = true;
    for ( int i=1 ; i<n ; i++ ) {
        // generate this partial tuple
        string currTuple = "";
        for ( int j=0 ; j<indRHS.size() ; j++ ) {
            currTuple = currTuple + data[i][indRHS[j]];
        }
        
        // case 1:
        if ( !BList[currTuple] && data[i][indLHS] != data[i-1][indLHS] ) {
            // dif A, new B
            lastAllEq = currAllEq;
            currAllEq = true;
            currMustEq = false;
            
            BList[currTuple] = true;
            lastTuple = currTuple;
            continue;
        }
        // case 2:
        else if ( !BList[currTuple] &&  data[i][indLHS] == data[i-1][indLHS] ) {
            // same A, new B
            if ( !currMustEq ) {
                
                currAllEq = false;
                
                BList[currTuple] = true;
                lastTuple = currTuple;
                continue;
            }
        }
        // case 3:
        else if ( BList[currTuple] &&  data[i][indLHS] != data[i-1][indLHS] ) {
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
        else if ( BList[currTuple] &&  data[i][indLHS] == data[i-1][indLHS] ) {
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


int main() {
    vector<vector<string> > table(5,vector<string>(3,""));

    table[0][0] = "1"; table[0][1] = "4"; table[0][2] = "3";
    table[1][0] = "1"; table[1][1] = "2"; table[1][2] = "3";
    table[2][0] = "2"; table[2][1] = "6"; table[2][2] = "5";
    table[3][0] = "2"; table[3][1] = "4"; table[3][2] = "3";
    table[4][0] = "2"; table[4][1] = "4"; table[4][2] = "3";

    cout << cdValid(table) << endl;
}
