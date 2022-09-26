#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>

#define FILE_NAME_TEMP "file_%d_%d.txt"

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

void test(int clientNum) {

    double duration;
    clock_t start, end;
    time_t first, second;

    int fileNum = 3000;
    int perProcess = fileNum / clientNum;

    int processid = 0;
    int i = 0;
    first = time(NULL);
    while(++i <= clientNum) {
        if((processid=fork()) > 0) {
		}
		else {
            start = clock();
	    	int pid = getpid();
            for (int j = 0; j < perProcess; j ++) {
                write_file(pid, j);
                read_file(pid, j);
            }
    
            end = clock();



            // printf("pid:%d, diff:%lu\n", pid, end - start);
            exit((end - start) / CLOCKS_PER_SEC);
		}
    }

    if (processid > 0) {
        int ret = 1;
        int status = 0;
        int total = 0;
        for (int k = 0; k < clientNum; k ++) {
            ret = wait(&status);
            // printf("k:%d, ret:%d\n", k, WEXITSTATUS(status));
            total += WEXITSTATUS(status) ;
        }
        second = time(NULL);
        printf("ClientNum: %d, clock diff: %d s\n", clientNum, total);
        printf("ClientNum: %d, difference: %f s\n", clientNum, difftime(second, first));
        // end = clock();
        // printf("pid:%d, start:%lu, end:%lu\n", processid, start, end);
        // duration = (double)(end - start) / CLOCKS_PER_SEC;
	    // printf("ClientNum: %d, duration: %f ms\n", clientNum, duration * 1000);
    }


    // if (processid > 0) {
    //     int status = 0;
    //     while ((wait(&status)) > 0) ;
    //     end = clock();
    //     printf("start:%l, end:%l", start, end);
    //     duration = (double)(end - start) / CLOCKS_PER_SEC;
	//     printf("ClientNum: %d, duration: %f ms\n", clientNum, duration * 1000);
    // }


}


int main(int argc, char * argv[])
{
    if(argc > 1){
    int clientNum = atoi(argv[1]);
    printf("clientNum: %d\n", clientNum);
    test(clientNum);
    } else {
        printf("should have at least 1 arg!\n");
    }
    // int clientNum = 1;

    // printf("Please set the number[1,100] of currenct read&write clients:");
    // scanf("%d", &clientNum);

    // while(clientNum < 1 || clientNum > 100) {
    //     printf("Please insert a number between 1 and 100:");
	// 	scanf("%d", &clientNum);
    // }

    // printf("The number you entered is: %d\n", clientNum);

    // for (int i = 10; i <= 20; i += 5) {
        // test(i);
    // }


    
    return 0; 
}
