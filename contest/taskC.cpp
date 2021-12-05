#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <unistd.h>
#include <map>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>

using namespace std;

long long mod = 1e9 + 7;
long long k = 37;
const int BUFFER = 4096;

struct TAB
{
	long long bias;
	int len;
	long long bias_k;
	int len_k;
};

int XASH(string s) {
	long long h = 0, m = 1, x = 0;
	for (auto c: s) {
		x = (long long)(c - 'A' + 1);
		h = (h + m * x) % mod;
		m = (m * k) % mod;
	}
	return h;
}

int main() {
	int n;
	char ans[BUFFER];
	char* value;
	string s, word;
	getline(cin, s);
	n = atoi(s.c_str());
	map<long long, vector<TAB>> xash;
	long long bias = 0, bias_k = 0;
	int in_v = open("help_v.txt", O_RDWR | O_CREAT | O_TRUNC, 0666);
	int in_k = open("help_k.txt", O_RDWR | O_CREAT | O_TRUNC, 0666);
	for (int i = 0; i < n; i++) {
		int idx = -1, num = 0;
		vector<string> words;
		getline(cin, s);
		stringstream x;
		x << s;
		while (x >> word) words.push_back(word);
		long long hash = XASH(words[1]);
		for (auto info: xash[hash]) {
			string a;
			pread(in_k, ans, info.len_k, (size_t) info.bias_k);
			for (int j = 0; j < info.len_k; j++) {
				a += ans[j];
			}
			if (words[1] == a) {
				idx = num;
				break;
			}
			num ++;
		}
		if (words[0] == "ADD") {
			if (idx != -1) {
				cout << "ERROR\n";
				continue;
			} 
			xash[hash].push_back({bias, (int)words[2].length(), bias_k, (int)words[1].length()});
			value = (char *)words[2].c_str();
			pwrite(in_v, value, words[2].length(), (size_t) bias);
			value = (char *)words[1].c_str();
			pwrite(in_k, value, words[1].length(), (size_t) bias_k);
			bias += words[2].length();
			bias_k += words[1].length();
		}
		if (words[0] == "PRINT") {
			if (idx == -1) {
				cout << "ERROR\n";
				continue;
			}
			string a;
			auto info = xash[hash][idx];
			pread(in_v, ans, info.len, (size_t) info.bias);
			cout << words[1] << " ";
			for (int j = 0; j < info.len; j++) {
				a += ans[j];
			}
			cout << a << "\n";
		}
		if (words[0] == "DELETE") {
			if (idx == -1) {
				cout << "ERROR\n";
				continue;
			}
			xash[hash].erase(xash[hash].begin() + idx);
		}
		if (words[0] == "UPDATE") {
			if (idx == -1) {
				cout << "ERROR\n";
				continue;
			}
			value = (char *)words[2].c_str();
			pwrite(in_v, value, words[2].length(), (size_t) bias);
			xash[hash][idx].bias = bias;
			xash[hash][idx].len = words[2].length();
			bias += words[2].length();
		}
	}
}



