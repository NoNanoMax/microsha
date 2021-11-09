#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string>
#include <vector>
#include <string>
#include <iostream>

using namespace std;

int main() {
	int n = 2;
	vector<vector<string>> args {{"ls", "-l"}, {"sort", "-R"}};
	vector<vector<char *>> args_ch = {};
	for (int i = 0; i < n; i++) {
		args_ch.push_back({});
		for (int j = 0; j < args[i].size(); j++) {
			args_ch[i].push_back((char *)args[i][j].c_str());
		}
		args_ch[i].push_back(nullptr);
	}
	int pip[2], fd_in = 0;
	for (int i = 0; i < args_ch.size(); i++) {
	  pipe(pip);
	  pid_t pid = fork();
	  if (pid == 0) {
	  	dup2(fd_in, 0);
	  	if (i < n-1) dup2(pip[1], 1);
	  	close(pip[0]);
	  	execvp(args_ch[i][0], &args_ch[i][0]);
	  	exit(0);
	  } else {
	  	int status;
	  	wait(&status);
	  	close(pip[1]);
	  	fd_in = pip[0];
	  }
	}
}
