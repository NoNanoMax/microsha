#include <iostream>
#include <vector>
#include <deque>
#include <set>

using namespace std;

int main() {
	int n, m, q;
	int c1=0, c2=0, c3=0, c4=0;
	cin >> n >> m;
	vector<int> quer(m);
	deque<int> FIFO;
	set<int> OPT;
	vector<pair<int, int>> LRU(n), LFU(n);
	for (int i = 0; i < n; i++) {
		LRU[i] = {-1, -1};
		LFU[i] = {-1, -1};
	}
	for (int i = 0; i < m; i++) {
		cin >> q;
		quer[i] = q;
	}
	for (int i = 0; i < m; i++) {
		bool is_here = false;
		for (int j = 0; j < (int)FIFO.size(); j++) {
			if (FIFO[j] == quer[i]) {
				is_here = true;
				break;
			}
		}
		if (is_here) {
			cout << quer[i] << "+ ";
			continue;
		}
		if ((int)FIFO.size() == n) {
			FIFO.pop_back();
		}
		FIFO.push_front(quer[i]);
		cout << quer[i] << "- ";
		c1 ++;
	} 
	cout << "\n";
	
	for (int i = 0; i < m; i++) {
		bool is_here = false;
		int min_ind = 0, min_freq = i + 1, null_idx = -1;
		for (int j = 0; j < n; j ++) {
			if (LRU[j].first == quer[i]) {
				is_here = true;
				LRU[j].second = i;
			}
			if (LRU[j].first != -1 && LRU[j].second <= min_freq) {
				if (LRU[j].second == min_freq) {
					if (LRU[j].second < LRU[min_ind].second) {
						min_ind = j;
						min_freq = LRU[j].second;
					}
				} else {
					min_ind = j;
					min_freq = LRU[j].second;
				}
			}
			if (LRU[j].first == -1) {
				if (null_idx == -1) null_idx = j;
			}
		}
		if (is_here) {
			cout << quer[i] << "+ ";
			continue;
		}
		if (null_idx == -1) {
			LRU[min_ind] = {-1, -1};
			null_idx = min_ind;
		}
		LRU[null_idx] = {quer[i], i};
		cout << quer[i] << "- ";
		c2 ++;
	} 
	cout << "\n";
	
	for (int i = 0; i < m; i++) {
		bool is_here = false;
		int min_ind = 0, min_freq = i + 1, null_idx = -1;
		for (int j = 0; j < n; j ++) {
			if (LFU[j].first == quer[i]) {
				is_here = true;
				LFU[j].second += 1;
			}
			if (LFU[j].first != 0 && LFU[j].second <= min_freq) {
				if (LFU[j].second == min_freq) {
					if (LFU[j].first < LFU[min_ind].first) {
						min_ind = j;
						min_freq = LFU[j].second;
					}
				} else {
					min_ind = j;
					min_freq = LFU[j].second;
				}
			}
			if (LFU[j].first == 0) {
				if (null_idx == -1) null_idx = j;
			}
		}
		if (is_here) {
			cout << quer[i] << "+ ";
			continue;
		}
		if (null_idx == -1) {
			LFU[min_ind] = {0, -1};
			null_idx = min_ind;
		}
		LFU[null_idx] = {quer[i], 0};
		cout << quer[i] << "- ";
		c3 ++;
	} 
	cout << "\n"; 
	
	for (int i = 0; i < m; i++) {
		set<int> found;
		if (OPT.count(quer[i]) == 1) {
			cout << quer[i] << "+ ";
			continue;
		}
		int last_find = -1;
		for (int j = i + 1; j < m; j++) {
			if (OPT.count(quer[j]) == 1 && found.count(quer[j]) == 0) {
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
		cout << quer[i] << "- ";
		c4 ++;
	}
	cout << "\n" << c1 << " " << c2 << " " << c3 << " " << c4 << "\n";
}

 
