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

using namespace std;

bool check_meta_symb(string s) {
	for (int i = 0; i < s.length(); i++) {
		if (s[i] == '?' || s[i] == '*') return true;
	}
	return false;
}

bool str_check(string text, string sample, int pos_t, int pos_s) {
	bool visit[text.length() + 1][sample.length() + 1] = {false};
	if (visit[pos_t][pos_s]) return false;
	visit[pos_t][pos_s] = true;
	if (pos_t == text.length() || pos_s == sample.length()) {
		if (pos_t == text.length() && pos_s == sample.length()) return true;
		else return false;
	}
	if (sample[pos_s] == '?' or sample[pos_s] == '?') return str_check(text, sample, pos_t+1, pos_s+1);
	if (sample[pos_s] == '*') {
		for (int i = pos_t; i <= text.length(); i++)
			if (str_check(text, sample, i, pos_s+1)) return true;
	}
	if (sample[pos_s] != '?' && sample[pos_s] != '*')
		return ((sample[pos_s] == text[pos_t]) && (str_check(text, sample, pos_t+1, pos_s+1)));
	
	return false;
}

bool str_check(string text, string sample) {
	return str_check(text, sample, 0, 0);
}

void find_mets(string sample, vector<string> &res, vector<string> &dirs, bool is_last) {
	//
	vector<string> new_dir;
	for (string dirname: dirs) {
		//cout << "I'm in " << dirname << " now\n";
		DIR *dir = opendir(dirname.c_str());
		if (!dir) continue;
		string par_dir = dirname;
		if (par_dir == ".") par_dir = "";
		else if (par_dir != "/") par_dir += "/";
		for (dirent *de = readdir(dir); de != NULL; de = readdir(dir)) {
			if (strcmp(".",de->d_name) == 0 || strcmp("..", de->d_name) == 0) continue;
			if (is_last) {
				if (str_check(string(de->d_name), sample)) res.push_back(string(par_dir + de->d_name));
			} else {
				if (str_check(string(de->d_name), sample)) {
					if (de->d_type == DT_DIR) new_dir.push_back(par_dir + de->d_name);
				}
			}
		}
		closedir(dir);
	}
	dirs = new_dir;
	return;
}

void find_mets(vector<string> samples, vector<string> &res, vector<string> &dirs) {
	if (samples.size() == 0) {
		find_mets("*", res, dirs, true);
	}
	for (int i = 0; i < samples.size(); i++) {
		//cout << "dirs: ";
		//for (auto dir: dirs) cout << "[" << dir << "] ";
		//cout << "\n";
		bool is_last = (i == (samples.size() - 1));
		find_mets(samples[i], res, dirs, is_last);
	}	
}

void cmd_conveer(vector<char *> conveer, vector<pair<int, int>> route) {
	// если всего одна команда
	vector<int> to_close = {};
	pid_t pid = fork();
	if (pid == 0) {
		for (auto p: route) {
			if (p.second == 0) {
				int out = open(conveer[p.first], O_RDONLY);
				conveer[p.first] = nullptr;
				to_close.push_back(out);
				dup2(out, 0);	 
			} else {
				int in = open(conveer[p.first], O_CREAT | O_WRONLY | O_TRUNC);
				conveer[p.first] = nullptr;
				to_close.push_back(in);
				dup2(in, 1);
			}
		}
		int code = execvp(conveer[0], &conveer[0]);
	} else {
		int status;
		pid_t waitd = wait(&status);
		return;
	}
	for (auto cl: to_close) {
		close(cl);
	}
}

void cmd_conveer(vector<vector<char *>> conveer, vector<vector<pair<int,int>>> route) {
	int pip[2], fd_in = 0;
	int n = conveer.size();
	for (int i = 0; i < conveer.size(); i++) {
	  pipe(pip);
	  pid_t pid = fork();
	  if (pid == 0) {
	  	dup2(fd_in, 0);
	  	if (i < n-1) dup2(pip[1], 1);
	  	close(pip[0]);
	  	execvp(conveer[i][0], &conveer[i][0]);
	  	exit(0);
	  } else {
	  	int status;
	  	wait(&status);
	  	close(pip[1]);
	  	fd_in = pip[0];
	  }
	}
}


int main() {
	string s, word;	
	//char *dir = get_current_dir_name();
	while (true) {
		vector<string> samples = {}, dirs = {}, res = {};
		vector<vector<pair<int, int>>> route = {}; // {position, <0, >1}
		vector<vector<string>> lems = {};
		vector<vector<char *>> conveer = {};
		getline(cin, s);
		if (s == "END") return 0;
		stringstream x;
		x << s;
		lems.push_back({});
		route.push_back({});
		int lems_size = 0;
		while (x >> word) {
			if (check_meta_symb(word)) {
				//получено выражение с метасимволами
				dirs.clear();
				string lem = "";
				int start = 0;
				if (word[0] == '/') {
					start = 1;
					dirs.push_back("/");
				} else if (word[0] == '.') {
					start = 2;
					dirs.push_back(".");
				} else {
					start = 0;
					dirs.push_back(".");
				}	
				for (int i = start; i < word.length(); i++) {
					if (word[i] != '/') lem += word[i];
					else {
						if (lem.length() > 0) samples.push_back(lem);
						lem = "";
					}
				}
				if (lem.length() > 0) samples.push_back(lem);
				find_mets(samples, lems[lems_size], dirs);
				continue;
			}
			if (word == "|") {
				lems.push_back({});
				route.push_back({});
				lems_size += 1;
				continue;
			}
			if (word == "<") {
				route[lems_size].push_back({lems[lems_size].size(), 0});
				continue;
			}
			if (word == ">") {
				route[lems_size].push_back({lems[lems_size].size(), 1});
				continue;
			}
			lems[lems_size].push_back(word);
		}
		
		int conv_size = 0;
		for (int i = 0; i < lems.size(); i++) {
			if (lems[i].size() == 0) continue;
			conveer.push_back({});
			for (int j = 0; j < lems[i].size(); j++) {
				conveer[conv_size].push_back((char *) lems[i][j].c_str());
			}
			conveer[conv_size].push_back(nullptr);
			conv_size += 1;
		}	 
	
		if (conveer.size() == 1) cmd_conveer(conveer[0], route[0]);
		else cmd_conveer(conveer, route);
	}
	//free(dir);
}


