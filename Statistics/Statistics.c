#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>
#include <math.h>
#include <unistd.h>

#define MAX 256

/* Substract the m characters from a string src of length n */
char* substr(const char *src, int m, int n)
{
    int len = n - m;
 
    char *dest = (char*)malloc(sizeof(char) * (len + 1));

    for (int i = m; i < n && (*(src + i) != '\0'); i++) {
        *dest = *(src + i);
        dest++;
    }
 
    *dest = '\0';
 
    return dest - len;
}


int main(int argc, char **argv)
{
	if (argc != 3){
		perror("Wrong number of arguments. Follow: \"./executable_name frequency directory_name\"\n");
		exit(EXIT_FAILURE);
	}
	
	FILE * file_res = fopen("statistical_results.txt", "a"); 
	char cwd[MAX];
	char * dirname = argv[2];
	DIR* dir = opendir(strcat(strcat(getcwd(cwd, sizeof(cwd)), "/"), dirname));
	struct dirent *cur;

	while ((cur = readdir(dir))) {
		if (cur->d_name[0] != '.') {  //Don't display hidden files
			
			char str[MAX] = "";
			float mean, range, standard_deviation;
			float ci_lower, ci_upper; //Confidence interval lower and upper limits
			float sum = 0, max = -1, min = -1, variance = 0, tmp;
			int sample_size = 0;

			/* Open the current file */
			char * filename = (char *) malloc(sizeof(char) * (strlen(dirname)+strlen(cur->d_name)+2));
			strcpy(filename, dirname);
			strcat(filename, "/");
			strcat(filename, cur->d_name);

			FILE * file = fopen(filename, "r");
			if (file == NULL) {
				perror("Error while opening file");
				exit(EXIT_FAILURE);
			}
	
			/* Computation of the mean and the range */
			while (fgets(str, MAX, file) != NULL){
				tmp = atof((const char *) str);
				printf("%2.f\n", tmp);
				if (max < tmp) max = tmp;
				if (min == -1 || min > tmp) min = tmp;
				sum += tmp;
				sample_size++;
			}
			mean = sum / sample_size;
			range = max - min;

			/* Go back to the beginning of the file */
			if (fseek(file, 0, SEEK_SET) != 0){
				perror("FSEEK error");
			}

			/* Computation of the variance deviation and the confidence interval */
			while (fgets(str, MAX, file) != NULL){
				tmp = atof((const char *) str);
				variance += (pow(tmp - mean, 2)); //Square the difference of the sample mean and each individual result
			}
			variance /= sample_size;
			standard_deviation = sqrt(variance);
			ci_lower = mean - (2.58 * (standard_deviation / sqrt(sample_size)));
			ci_upper = mean + (2.58 * (standard_deviation / sqrt(sample_size)));
			
			fclose(file);
			
			free(filename);

			fprintf(file_res, "%s %d - mean : %f, range, %f, standard deviation : %f, confidence interval : [%f, %f]\n", substr(cur->d_name, 3, strlen(cur->d_name)-4), atoi(argv[1]), mean, range, standard_deviation, ci_lower, ci_upper);
		}
	}

	closedir(dir);
	
	fclose(file_res);
	return 0;
}

