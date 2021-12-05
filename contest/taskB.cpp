#include <iostream>
#include <vector>
#include <deque>
#include <unordered_set>

using namespace std;

int main() {
	int n, m;
	long long q;
	int c4=0;
	cin >> n >> m;
	vector<long long> quer(m);
	unordered_set<long long> OPT;
	for (int i = 0; i < m; i++) {
		cin >> q;
		quer[i] = q;
	}
	
	for (int i = 0; i < m; i++) {
		unordered_set<long long> found;
		if (OPT.find(quer[i]) != OPT.end()) {
			continue;
		}
		int last_find = -1;
		for (int j = i + 1; j < m; j++) {
			if (OPT.find(quer[j]) != OPT.end() && found.find(quer[j]) == found.end()) {
				found.insert(quer[j]);
				last_find = quer[j];
			}
		}
		if (OPT.size() != found.size()) {
			for (auto a: OPT) {
				if (found.count(a) == 0) {
					last_find = a;
					break;
				}
			}
		}
		//cout << "[" << last_find << "] ";
		if ((int)OPT.size() == n) {
			OPT.erase(last_find);
		}
		OPT.insert(quer[i]);
		c4 ++;
	}
	cout << c4 << "\n";
}

 
