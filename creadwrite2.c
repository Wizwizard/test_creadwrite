#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <inttypes.h>
#include <time.h>
#include <signal.h>

#define FILE_NAME_TEMP "file_%d_%d.txt" // File name macro expanding with pid

int cpids[100];

/**
 *
 * When called by a process, this function
 * create a file named file_%pid.txt, iterate writing 
 * into it with each line containing a integer prepadding
 * with 0s if the the literal length of the integer 
 * is less than 4.
 * 
 * The function iterates writing 1024 times 
 * with each line of text length 5 byte, so the created
 * file will be of size of 5 * 1024 = 5K byte. 
 */
void write_file(int pid, int i)
{
    char file_name[200];
    char txt[10];
    sprintf(file_name, FILE_NAME_TEMP, pid, i);

    // printf("\\nProcess %d starts writing file %s", pid, file_name);

    int fd = open(file_name, O_WRONLY|O_CREAT|O_TRUNC, 0644);
    if(fd < 0){
        perror("Open file failed");
		exit(1);
    }

    for(int i=1; i<=1024; i++){
        sprintf(txt, "%04d\n", i);
		write(fd, txt, strlen(txt));
    }

    close(fd);

    // printf("Process %d ends writing.\n", pid);
}

void read_file(int pid, int i)
{
	int buffer_size = 6144;
    char file_name[15];
    char file_content[buffer_size];
    struct stat fstat; 

    sprintf(file_name, FILE_NAME_TEMP, pid, i);
    if(stat(file_name, &fstat) != 0) {
        write_file(pid, i);
    }

    // printf("Process %d starts reading file %s\n", pid, file_name); 

    int fd = open(file_name, O_RDONLY);
    int readn = 0; 
    int bufferi = 0;

    while((readn=read(fd, file_content+bufferi, buffer_size-bufferi-1)) != 0) {
        bufferi += readn;
    }

    close(fd);

    // printf("The content of file %s are:\n", file_name);
    // printf("%s", file_content);

    // printf("Process %d ends reading.\n", pid);
}

void sig_handler(int signum)
{
	int i=0;
	// printf("Inside handler function\n");

  	while(cpids[i++] > 0) {
		// printf("Kill child process: %d\n", cpids[i]);
		kill(cpids[i], SIGKILL);
	}
}

/*int64_t millis()
{
    struct timespec now;
    timespec_get(&now, TIME_UTC);
    return ((int64_t) now.tv_sec) * 1000 + ((int64_t) now.tv_nsec) / 1000000;
}*/

/*
 * The main function prompts the user how many 
 * concurrent clients(child processes) write&read 
 * 5k independently files. It detects the validity of
 * the number user sets(between 1 and 100).
 *
 * After the correct clients number, it create the specified
 * number of child processes which will call the write_file()
 * and read_file() sequentially doing the operations.
 */
int main(int argc, char * argv[])
{
    int client_num = atoi(argv[1]);
	int time_bound = atoi(argv[2]);

    printf("cliend_num:%d\n", client_num);

    int cpid = 0;
    int i = 0;

	// signal(SIGALRM,sig_handler); // Register signal handler
	// alarm(5);

	time_t startTime = time(NULL);
    while(++i <= client_num) {
        if((cpid=fork()) > 0) { // Parent process creates child processes
			cpids[i-1] = cpid;
		}
		else { // Child processes do file writing and reading
	    	int pid = getpid();
            int k = 0;
            while(1) {
	    	    write_file(pid, k);
	    	    read_file(pid, k);
                k ++;
            }
	    	// printf("Running in child process %d!\n", pid);

		}
    }

	time_t time_slipped = time(NULL) - startTime;
	if(time_slipped <= time_bound) 
		sleep(time_bound - time_slipped);

	for(i=0; i<client_num; i++) {
		if(cpids[i] != 0) kill(cpids[i], SIGKILL);
	}
    
    return 0; 
}
