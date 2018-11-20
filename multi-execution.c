#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
<<<<<<< HEAD
#include <assert.h>


char** str_split(char* a_str, const char a_delim)
{
    char** result    = 0;
    size_t count     = 0;
    char* tmp        = a_str;
    char* last_comma = 0;
    char delim[2];
    delim[0] = a_delim;
    delim[1] = 0;

    /* Count how many elements will be extracted. */
    while (*tmp)
    {
        if (a_delim == *tmp)
        {
            count++;
            last_comma = tmp;
        }
        tmp++;
    }

    /* Add space for trailing token. */
    count += last_comma < (a_str + strlen(a_str) - 1);

    /* Add space for terminating null string so caller
       knows where the list of returned strings ends. */
    count++;

    result = malloc(sizeof(char*) * count);

    if (result)
    {
        size_t idx  = 0;
        char* token = strtok(a_str, delim);

        while (token)
        {
            assert(idx < count);
            *(result + idx++) = strdup(token);
            token = strtok(0, delim);
        }
        assert(idx == count - 1);
        *(result + idx) = 0;
    }

    return result;
}
 
int main(int argc, char *argv[]) {
	int sim_number = 1;
	int flow_number = 1;
=======

 
int main(int argc, char *argv[]) {
	int number = 1;
>>>>>>> c56f7a57ac79b2a65026a2ff87ea5cc70cd2436a
	int status;

	pid_t wpid;

<<<<<<< HEAD
	if(argc < 3) {
=======
	if(argc < 2) {
>>>>>>> c56f7a57ac79b2a65026a2ff87ea5cc70cd2436a
		printf("Need these arguments: number_flow");
        exit(EXIT_FAILURE);
	}

<<<<<<< HEAD
	sim_number = atoi(argv[1]);
	flow_number = atoi(argv[2]);

	system("rm -rf OutputFiles");
	system("mkdir OutputFiles");
=======
	number = atoi(argv[1]);
>>>>>>> c56f7a57ac79b2a65026a2ff87ea5cc70cd2436a

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
<<<<<<< HEAD
			char str[12];
			char buff[255];
			sprintf(str, "%d", count);
			
			strcpy(buff, "cd ../.. && ./waf --run \"TCPFlowSimulation --SimNumber=");
			strcat(buff, str);
			strcat(buff, " --FlowNumber=");

			sprintf(str, "%d", flow_number);

			strcat(buff, str);

			strcat(buff,"\" && cd -");

			system(buff);

=======
			system("cd ../.. && ./waf --run tcpflow && cd -");
>>>>>>> c56f7a57ac79b2a65026a2ff87ea5cc70cd2436a
			exit(EXIT_SUCCESS);
			//execl("../../waf", "waf", "--run", "tcpflow", NULL);
			//printf("Simulation number %d created.", i);
		}
		else {
			count++;
<<<<<<< HEAD
			if(count == sim_number)
=======
			if(count == number)
>>>>>>> c56f7a57ac79b2a65026a2ff87ea5cc70cd2436a
				flag = 0;
		}
	}

	while ((wpid = wait(&status)) > 0);

	// now all childs are finished

<<<<<<< HEAD
	printf("Looking for stats files...\n");

	double results[flow_number];

	for(int i = 0; i < flow_number; i++) {
		results[i] = 0;
	}

	for(int i = 0; i < sim_number; i++) {
		char str[12];
		char path[255];
		sprintf(str, "ris%d.txt", i);

		strcpy(path, "./OutputFiles/");
		strcat(path, str);
	
		/* declare a file pointer */
		FILE    *infile;
		char    *buffer;
		long    numbytes;

		/* open an existing file for reading */
		infile = fopen(path, "r");

		/* quit if the file does not exist */
		if(infile == NULL) {
			printf("Unable to open %s.\n", path);
			exit(EXIT_FAILURE);
		}

		/* Get the number of bytes */
		fseek(infile, 0L, SEEK_END);
		numbytes = ftell(infile);

		/* reset the file position indicator to 
		the beginning of the file */
		fseek(infile, 0L, SEEK_SET);	

		/* grab sufficient memory for the 
		buffer to hold the text */
		buffer = (char*)calloc(numbytes, sizeof(char));	

		/* memory error */
		if(buffer == NULL)
		return 1;

		/* copy all the text into the buffer */
		fread(buffer, sizeof(char), numbytes, infile);
		fclose(infile);

		/* confirm we have read the file by
		outputing it to the console */

		char** ris = str_split(buffer, '\n');

		for(int j = 0; *(ris + j); j++) {
			double d = 0;

			sscanf(ris[j], "%lf", &d);

			results[j] += d;

			free(*(ris+j));
		}

		free(ris);

		/* free the memory we used for the buffer */
		free(buffer);
	}

	for(int i = 0; i < flow_number; i++) {
		results[i] = results[i] / sim_number;
		printf("%lf\t", results[i]);
	}

	printf("\n");

=======
	printf("Looking for stats files...");

	for(int i = 0; i < number; ++i) {
		
	}

>>>>>>> c56f7a57ac79b2a65026a2ff87ea5cc70cd2436a
	exit(EXIT_SUCCESS);
}