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

using namespace std;

bool str_check(string text, string sample, int pos_t, int pos_s) {
	bool visit[text.length() + 1][sample.length() + 1];
	if (visit[pos_t][pos_s]) return false;
	visit[pos_t][pos_s] = true;
	if (pos_t == text.length() || pos_s == sample.length()) {
		if (pos_t == text.length() && pos_s == sample.length()) return true;
		else return false;
	}
	if (sample[pos_s] == '?') return str_check(text, sample, pos_t+1, pos_s+1);
	if (sample[pos_s] == '*') {
		for (int i = pos_t; i <= text.length(); i++)
			if (str_check(text, sample, i, pos_s+1)) return true;
	}
	if (sample[pos_s] != '?' && sample[pos_s] != '*')
		return ((sample[pos_s] == text[pos_t]) && (str_check(text, sample, pos_t+1, pos_s+1))));
	
	return false;
}

int main() {
	string s, word;	
	char *dir = get_current_dir_name();
	while (true) {
		cout << dir << ">";
		getline(cin, s);
		vector<string> words;
		vector<char *> c_words;   
		stringstream x;
		x << s;            
		while (x >> word){
			words.push_back(word);
		}
		for (int i = 0; i < words.size(); i++) {
			c_words.push_back((char *)words[i].c_str());
		}
		if (words[0] == "cd"){
			if (chdir(c_words[1]) != 0) perror("cd");
			dir = get_current_dir_name();	 
		} else {
		    pid_t pid = fork();
		    if (pid == 0) {
		    	if (execvp(c_words[0], &c_words[0]) == -1) perror(c_words[0]);
		    } else {
		    	int status;
        	pid_t waitd = wait(&status);
		    }
		}
	}
}


