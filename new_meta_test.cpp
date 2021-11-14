#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <vector>
#include <string>
#include <iostream>
#include <vector>
#include <fcntl.h>
#include <sstream>
#include <dirent.h>
#include <cstring>
#include <typeinfo>
#include <signal.h>

using namespace std;


bool str_check(string text, string sample, int pos_t, int pos_s) {
	//cout << text[pos_t] << " " << sample[pos_s] << "\n";
	if (text[pos_t] == '\0') {
		if (sample[pos_s] != '\0') {
			if (sample[pos_s] == '*') return str_check(text, sample, pos_t, pos_s + 1);
			else return false;
		}
		return true;
	}
	if (text[pos_t] == sample[pos_s]) return str_check(text, sample, pos_t + 1, pos_s + 1);
	if (sample[pos_s] == '?') return str_check(text, sample, pos_t + 1, pos_s + 1);
	if (sample[pos_s] == '*') {
		int i = pos_t;
		while (text[i] != '\0') {
			if (str_check(text, sample, i, pos_s + 1)) return true;
			i ++;
		}
		if (str_check(text, sample, i, pos_t + 1)) return true;
	}
	return false;
}

bool str_check(string text, string sample) {
	return str_check(text, sample, 0, 0);
}


int main() {
	string text = "fadfa" + '\0', sample = "*f?" + '\0';
	//cout << (sample[1] == '\0') << "\n";
	//cout << text.length() << " " << sample.length() << "\n";
	cout << str_check(text, sample) << "\n";
}
