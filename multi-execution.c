#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

 
int main(int argc, char *argv[]) {
	int number = 1;
	int status;

	pid_t wpid;

	if(argc < 2) {
		printf("Need these arguments: number_flow");
        exit(EXIT_FAILURE);
	}

	number = atoi(argv[1]);

	int count = 0;
	int flag = 1;
	while(flag) {
		pid_t pid = fork();

		if(pid < 0) {
			flag = 0;
			perror("Fork error");
		}
		if(pid == 0) {
			flag = 0;
			system("cd ../.. && ./waf --run tcpflow && cd -");
			exit(EXIT_SUCCESS);
			//execl("../../waf", "waf", "--run", "tcpflow", NULL);
			//printf("Simulation number %d created.", i);
		}
		else {
			count++;
			if(count == number)
				flag = 0;
		}
	}

	while ((wpid = wait(&status)) > 0);

	// now all childs are finished

	printf("Looking for stats files...");

	for(int i = 0; i < number; ++i) {
		
	}

	exit(EXIT_SUCCESS);
}