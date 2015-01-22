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
	ifstream fin("fdInput.txt");

	int M = 11;
	int N = 100;
	int table[700][12];

	for ( int i=0 ; i<N ; i++ ) {
		for ( int j=0 ; j<M ; j++ ) {
			fin >> table[i][j];
		}
	}


	for  ( int l=0 ; l<M ; l++ ) {
		for ( int r=0 ; r<M ; r++ ) {
			if ( l==r ) {
				continue;
			}
		

			bool flag = true;
			map<int, int> rValue;
			
			for ( int i=0 ; i<N ; i++ ) {
				int left = table[i][l];
				int right = table[i][r];
				
				if ( rValue[left] == 0 ) {
					rValue[left] = right;
				}
				else if ( rValue[left] != right ) {
					flag = false;
					break;
				}
			}


			if ( flag==true ) {
				cout << l << " " << r << endl;
			}
		}
	}

	return 0;
}
