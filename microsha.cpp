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
#include <sys/time.h>
#include <sys/resource.h>

using namespace std;

bool check_meta_symb(string s) {
	for (int i = 0; i < s.length(); i++) {
		if (s[i] == '?' || s[i] == '*') return true;
	}
	return false;
}

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
				if (str_check(string(de->d_name) + '\0', sample + '\0')) res.push_back(string(par_dir + de->d_name));
			} else {
				if (str_check(string(de->d_name) + '\0', sample + '\0')) {
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

void sigfunc(int val) {

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
				int in = open(conveer[p.first],O_WRONLY | O_CREAT | O_TRUNC, 0666)	;
				conveer[p.first] = nullptr;
				to_close.push_back(in);
				dup2(in, 1);
			}
		}
		int code = execvp(conveer[0], &conveer[0]);
		if (code == -1) {
			perror(conveer[0]);
		}
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
	  vector<int> to_close = {};
	  if (pid == 0) {
	  	dup2(fd_in, 0);
	  	if (i < n-1) dup2(pip[1], 1);
	  	close(pip[0]);
	  	for (auto p: route[i]) {
				if (p.second == 0) {
					int out = open(conveer[i][p.first], O_RDONLY);
					conveer[i][p.first] = nullptr;
					to_close.push_back(out);
					dup2(out, 0);	 
				} else {
					int in = open(conveer[i][p.first], O_WRONLY | O_CREAT | O_TRUNC, 0666);
					conveer[i][p.first] = nullptr;
					to_close.push_back(in);
					dup2(in, 1);
				}
			}
	  	execvp(conveer[i][0], &conveer[i][0]);
	  	exit(0);
	  } else {
	  	int status;
	  	wait(&status);
	  	close(pip[1]);
	  	fd_in = pip[0];
	  	
			for (auto cl: to_close) {
				close(cl);
			}
	  }
	}
}

bool check_route(vector<vector<pair<int, int>>> route) {
	int start_0 = 0, start_1 = 0, end_0 = 0, end_1 = 0;
	int n = route.size() - 1, all = 0;
	for (int i = 0; i <= n; i++) {
		all += route[i].size();
		if (i == 0) {
			for (auto p: route[i]) {
				if (p.second == 0) start_0 += 1;
				else start_1 += 1;
			}
		}
		if (i == n) {
			for (auto p: route[i]) {
				if (p.second == 0) end_0 += 1;
				else end_1 += 1;
			}
		}
	}
	if (n == 0) {
		if (start_1 <= 1 && start_0 <= 1) return true;
		cout << "недопустимое количество перенапрвлений\n";
		return false; 
	}	
	if (all <= 2 && start_0 <= 1 && start_1 == 0 && end_0 == 0 && end_1 <= 1 && all == start_0 + end_1) {
		return true;
	}
	cout << "перенаправления недопустимо в конвеере\n";
	return false;
}


int main() {
	string s, word;	
	char *home  = get_current_dir_name();
	char *dir = get_current_dir_name();
	cout << dir << "> ";
	signal(SIGINT, sigfunc);	
	double last_sys = 0, last_usr = 0;
	while (getline(cin, s)) {
		struct timeval start;
		gettimeofday(&start, nullptr);
		bool lst_route = false, need_time = false;
		struct rusage info;
		getrusage(RUSAGE_CHILDREN, &info);
		last_sys = (double)info.ru_stime.tv_sec + (double)info.ru_stime.tv_usec / 1000000.0;
		last_usr = (double)info.ru_utime.tv_sec + (double)info.ru_utime.tv_usec / 1000000.0;
		vector<string> samples = {}, dirs = {}, res = {};
		vector<vector<pair<int, int>>> route = {}; // {position, <0, >1}
		vector<vector<string>> lems = {};
		vector<vector<char *>> conveer = {};
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
				int start_size = lems[lems_size].size();
				find_mets(samples, lems[lems_size], dirs);
				if (lems[lems_size].size() == start_size) {
					lems[lems_size].push_back(word);
				}
				continue;
			}
			if (word == "|") {
				lems.push_back({});
				route.push_back({});
				lems_size += 1;
				continue;
			}
			if (word == "<" && !lst_route) {
				route[lems_size].push_back({lems[lems_size].size(), 0});
				lst_route = true;
				continue;
			}
			if (word == ">" && !lst_route) {
				route[lems_size].push_back({lems[lems_size].size(), 1});
				lst_route = true;
				continue;
			}
			if (word == "time") {
				need_time = true;
				continue;
			}	
			lst_route = false;
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
		if (!check_route(route)) goto skip;
		
		if (conveer.size() == 1) { 
			if (lems[0][0] == "cd") {
				if (conveer[0].size() == 2) {
					chdir(home);
				} else {
					int code = chdir(conveer[0][1]);
					if (code == -1) {
						perror(conveer[0][1]);
					}
				}
			} else if (lems[0][0] == "pwd") {
				cout << dir << "\n";
			} else {
				cmd_conveer(conveer[0], route[0]);
			}
		}
		else cmd_conveer(conveer, route);
		
		if (need_time) {
			struct timeval end;
			gettimeofday(&end, nullptr);
			double start_time = (double)start.tv_sec + (double)start.tv_usec / 1000000.0;
			double end_time = (double)end.tv_sec + (double)end.tv_usec / 1000000.0;
			getrusage(RUSAGE_CHILDREN, &info);
			double sys_time = (double)info.ru_stime.tv_sec + (double)info.ru_stime.tv_usec / 1000000.0 - last_sys;
			double usr_time = (double)info.ru_utime.tv_sec + (double)info.ru_utime.tv_usec / 1000000.0 - last_usr;
			last_sys = (double)info.ru_stime.tv_sec + (double)info.ru_stime.tv_usec / 1000000.0;
			last_usr = (double)info.ru_utime.tv_sec + (double)info.ru_utime.tv_usec / 1000000.0;
			printf("\nreal: %.3lf \nuser: %.3lf \nsys: %.3f\n", end_time - start_time, usr_time, sys_time);
		}

		skip: free(dir);
		dir = get_current_dir_name();
		cout << dir << "> ";
	}
	free(dir);
	free(home); 
}


