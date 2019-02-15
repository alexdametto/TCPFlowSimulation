#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
	FILE    *infile;

	/* open an existing file for reading */
	infile = fopen("./dataset.txt", "r");

	/* quit if the file does not exist */
	if(infile == NULL) {
		printf("Unable to open\n");
	}
	else {
		/* Get the number of bytes */
		fseek(infile, 0L, SEEK_END);
		long numbytes = ftell(infile);

		/* reset the file position indicator to 
		the beginning of the file */
		fseek(infile, 0L, SEEK_SET);	

		/* grab sufficient memory for the 
		buffer to hold the text */
		char* buffer = (char*)calloc(numbytes, sizeof(char));	

		/* memory error */
		if(buffer == NULL)
			return 1;

		/* copy all the text into the buffer */
		fread(buffer, sizeof(char), numbytes, infile);
		fclose(infile);

		/* confirm we have read the file by
		outputing it to the console */

		char** rows = str_split(buffer, '\n');

		double tempo = 0;
		int count = 0;
		double med = 0;

		for(int j = 0; *(rows + j); j++) {
			int n_packets = 0;

			sscanf(rows[j], "%d", &n_packets);

			printf("%d\n", n_packets);

			med += n_packets;
			
			free(*(rows+j));

			count++;
		}

		free(rows);

		/* free the memory we used for the buffer */
		free(buffer);

		//double med = 0;
		//for(int q = 0; q < count; q++) {
		//	med += resTime[q];
		//}

		med = med / count;

		printf("Media del file è %2f\n", med);
	}

	exit(EXIT_SUCCESS);
}