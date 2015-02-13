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

using namespace std;

struct CompareDeg {
    bool operator()(pair<int,int> p1, pair<int,int> p2) {
    	return p1.second < p2.second;
    }
};

int main() {
	int a = 107;
	int b = 40;
	int c = (a&b);
	cout << bitset<8>(16) << endl;


	priority_queue<pair<int,int>, vector< pair<int,int> >, CompareDeg> kkk;
	kkk.push(make_pair(1,2));
	kkk.push(make_pair(5,4));
	kkk.push(make_pair(3,8));
	kkk.push(make_pair(4,1));
	while ( !kkk.empty() ) {
		cout << kkk.top().second << " ";
		kkk.pop();
	}
}