/*#include <iostream>
#include <string>
#include <unistd.h>
#include <vector>
#include <stdio.h>
#include <dirent.h>
#include <cstring>

using namespace std;

int main() {
	string path, lem = "";
	cin >> path;
	vector<string> lems;
	vector<string> pt1, pt2;	
	for (int i = 0; i < path.length(); i++) {
		if (path[i] == '/'){
			if (lem != "/") lems.push_back(lem);
			lem = "/";
		} else {
			lem += path[i];
		}
	}
	if (lem.length() > 0) lems.push_back(lem);
	for (auto s: lems) cout << "[" << s << "] ";
	cout << "\n"; 
	DIR *dir = opendir(lems[0].c_str());
	for (dirent *de = readdir(dir); de != NULL; de = readdir(dir)) {
		if (strcmp(".",de->d_name) == 0 || strcmp("..", de->d_name) == 0) continue;
		cout << "[" << string(de -> d_name) << "] ";
	}
	closedir(dir);
	cout << "\n";
}*/
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <string>
#include <string.h>
#include <iostream>

using namespace std;

bool str_check(string text, string sample, int pos_t, int pos_s) {
	bool visit[text.length() + 1][sample.length() + 1] = {false};
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
		return ((sample[pos_s] == text[pos_t]) && (str_check(text, sample, pos_t+1, pos_s+1)));
	
	return false;
}

void walk_recursive(string const &dirname, vector<string> &ret, int counter, int max_count, vector<string> lems) {
    //printf("walk_recursive(%s)\n", dirname.c_str());
    if (counter < max_count) {
    	DIR *dir = opendir(dirname.c_str());
    	if (dir == nullptr) {
        	perror(dirname.c_str());
        	return;
    	}
    	for (dirent *de = readdir(dir); de != NULL; de = readdir(dir)) {
        	if (strcmp(".",de->d_name) == 0 || strcmp("..", de->d_name) == 0) continue;
        	if (string(de->d_name)[0] == '.') continue;
        	//cout << de->d_name << " " << lems[counter] << " " << str_check(string(de->d_name), lems[counter], 0, 0) << "\n";
        	if (counter == max_count - 1 & str_check(string(de->d_name), lems[counter], 0, 0)) {ret.push_back(dirname + "/" + de->d_name);}
        	if (de->d_type == DT_DIR & str_check(string(de->d_name), lems[counter], 0, 0)) walk_recursive(dirname + "/" + de->d_name, ret, counter + 1, max_count, lems);
    	}
    	closedir(dir);	
    } else {
    	return;
    }
    
}
    
vector<string> walk(string const &dirname, int counter, int max_count, vector<string> lems) {
    vector<string> ret;
    walk_recursive(dirname, ret, counter, max_count, lems);
    return ret;
}

int main(int argc, char **argv) {
    const char *dirname = argc > 1 ? argv[1] : ".";
    string path, lem = "";
		cin >> path;
		vector<string> lems;
		vector<string> pt1, pt2;	
		for (int i = 0; i < path.length(); i++) {
			if (path[i] == '/'){
				if (lem != "/") lems.push_back(lem);
				lem = "";
			} else {
				lem += path[i];
			}
		}
		if (lem.length() > 0) lems.push_back(lem);
    auto names = walk(dirname, 0, 2, lems);
    for (auto const &name: names) {
        printf("'%s'\n", name.c_str());
    }
}

