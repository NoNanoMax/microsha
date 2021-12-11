#include <iostream>
#include <stdlib.h>
#include <string>
#include <string.h>
#include <pthread.h>
#include <mutex>
#include <vector>
#include <deque>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>

using namespace std;

mutex get_s;
mutex write_s;
deque<string> S	;

void find_mets(string dirname, int recurs) {
		DIR* dir = opendir(dirname.c_str());
		if (dir == nullptr) return;
		for (auto de = readdir(dir); de != nullptr; de = readdir(dir)) {
			if (!strcmp(de->d_name, ".") or !strcmp(de->d_name, "..")) continue;
			if (de->d_type == DT_REG) {
				get_s.lock();
				S.push_front(dirname + "/" + de->d_name);
				get_s	.unlock();
			} else if (de->d_type == DT_DIR and recurs) {
				find_mets(dirname + "/" + de->d_name, recurs);
			}
		}
		closedir(dir);
		return;
}

struct f {
	string dirname;
	int recurs;
};
	

void* finder_mets(void* args) {
	f* f_args = (f*)args;
	auto dirname = f_args->dirname;  
	auto recurs = f_args->recurs;
	find_mets(dirname, recurs);
	S.push_front(".");
	return nullptr;	
}

void finder_examples(vector<int> pi, string example, string to_search) {
	// testing pref function	
	int pi_s = 0;
	for (int i = 0; i < to_search.length(); i++) {
		while (pi_s > 0 && to_search[i] != example[pi_s]	) {
			pi_s = pi[pi_s - 1];
		}
		if (to_search[i] == example[pi_s]) {
			pi_s ++;
		}
		if (pi_s < example.length()) continue;
		cout << i << ": [";
		for (int j = i - example.length() + 1; j <= i; j ++){
			cout << to_search[j];
		}
		cout << "]\n";
	}
	return;
}

struct f_d {
	string example;
	vector<int> pi;
};

void* finder_examples(void* args) {
	f_d* f_args = (f_d*)args;
	auto example = f_args->example;
	auto pi = f_args->pi;
	while (true) {
		get_s.lock();
		if (S.empty()) {
			get_s.unlock();
			continue;
		}
		string dirname = S.back();
		if (dirname == ".") {
			get_s.unlock();
			break;
		}
		S.pop_back();
		get_s.unlock();
		auto file = open(dirname.c_str(), O_RDONLY);
		if (file < 0) continue;
		struct stat for_size;
    fstat(file, &for_size);
    auto file_size = for_size.st_size;
    auto to_search = (char*)mmap(NULL, file_size, PROT_READ, MAP_PRIVATE, file, 0);
    int line = 0, pi_s = 0;
    for (int i = 0; i < file_size; i++) {
    	if (to_search[i] == '\n') {
    		line ++;
    		pi_s = 0;
    		continue;
    	}
			while (pi_s > 0 && to_search[i] != example[pi_s]) {
				pi_s = pi[pi_s - 1];
			}
			if (to_search[i] == example[pi_s]) {
				pi_s ++;
			}
			if (pi_s < example.length()) continue;
			write_s.lock();
			cout << "found [" << example << "] in line " << line << " in file " << dirname << "\n";
			write_s.unlock();
			while (i < file_size and to_search[i] != '\n') i++;
			line += 1;
			pi_s = 0;
		}
		munmap(to_search, file_size);
		close(file);
	}
	return nullptr;
}

int main(int argc, char **argv) {
	string main_dir;
	string example = "";
	int n = 1, recurs = 0;
	for (int i = 1; i < argc; i++) {
		if (argv[i][0] == '-') {
			if (argv[i][1] == 't') n = atoi(argv[i] + 2);
			if (argv[i][1] == 'n') recurs = 1;	
		} else {
			if (example.length() == 0) {
				example = string(argv[i]);
			} else {
				main_dir = string(argv[i]);
			}
		}
	}
	vector<int> pi(example.length());
	for (int i = 1; i < example.length(); i++) {
		int j = pi[i - 1];
		while (j > 0 and example[i] != example[j]) {
			j = pi[j - 1];
		}
		if (example[i] == example[j]) {
			j ++;
		}
		pi[i] = j;
	}
	f fin = {main_dir, recurs};
	pthread_t a;
	pthread_create(&a, nullptr, finder_mets, (void *)&fin);
	
	f_d info = {example, pi};
	
	vector<pthread_t> p(n);
	for (int i = 0; i < n; i++) {
		pthread_create(&p[i], NULL, finder_examples, (void*)&info);
	} 
	
	pthread_join(a, nullptr); 
	
	for (int i = 0; i < n; i++) {
		pthread_join(p[i], nullptr); 
	} 
}
