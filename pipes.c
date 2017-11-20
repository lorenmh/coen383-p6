#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <assert.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/select.h>

#define BUFFER_SIZE		100
#define READ_END		0
#define WRITE_END		1
#define NUM_PIPES		5
#define MAX_SLEEP		2
#define PIPE_DURAITON	30


fd_set input, input_fd;
FILE* output;
char buffer[BUFFER_SIZE];
char terminal_input[BUFFER_SIZE];
int fd[NUM_PIPES][2];
int user_input, nread;
int timeout = 0;
struct timeval start_t;
struct itimerval timer;
time_t start;

void readPipe(int pipeEnd, int pipe){
	if(!timeout){
		struct timeval curr_time;
        gettimeofday(&curr_time, NULL);
        float current_read_time = (float)((curr_time.tv_sec - start_t.tv_sec) + ((curr_time.tv_usec - start_t.tv_usec)/1000000.));
        
        read(pipeEnd, buffer, BUFFER_SIZE);
        if (pipe == 4){
			fprintf(output, "%6.3f: User Input: %s", current_read_time, buffer);
        } else{
       		fprintf(output, "%6.3f %s\n", current_read_time, buffer);
        }
	}
}

void writePipe(int* pd){
	if(!timeout){
		close(pd[READ_END]);
		write(pd[WRITE_END], buffer, BUFFER_SIZE);
	}
}

void interruptHandler(int signal){
	assert(signal == SIGALRM);
	timeout = 1;
	exit(0);
}

int main(){
	output = fopen("output.txt", "w");

	time(&start); //get start time
	timer.it_value.tv_sec = PIPE_DURAITON;	//30 second timeout
	timer.it_value.tv_usec = 0;
	setitimer(ITIMER_REAL, &timer, NULL);
	gettimeofday(&start_t, NULL);
	signal(SIGALRM, interruptHandler);
	srand(time(NULL));

	FD_ZERO(&input_fd);
	FD_SET(0, &input_fd);

	int i, j, pipeNumber;
	pid_t pid;

	for(i = 0; i < NUM_PIPES; i++){
		if(pipe(fd[i]) == -1) {
			perror("Pipe error");
			exit(1);
		}
		
		FD_SET(fd[i][READ_END], &input_fd);
		
		pid = fork();
		if(pid  == -1) {
			perror("Fork error");
			exit(1);
		}

		if(pid == 0) {
			fflush(stdout);
			break;
		}

	}

	while(!timeout){

		//Parent Process
		if(pid > 0){
			input = input_fd;

			pipeNumber = select(FD_SETSIZE, &input, NULL, NULL, &timeout);
			if(pipeNumber < 0){
				perror("Pipe Number error");
				exit(1);
			} else if (pipeNumber == 0){
				perror("Nothing to read");
			} else{
				for(i = 0; i < NUM_PIPES; i++){
					if(FD_ISSET(fd[i][READ_END], &input)) {
						readFromPipe(fd[i][READ_END], i);
					}
				}
			}
		} else{ // Child Process
			int message_count = 1;
			inputs = inputfds;

			if(i == 4) {
				struct timeval curr_time;
		        gettimeofday(&curr_time, NULL);
		        float current_read_time = (float)((curr_time.tv_sec - start_t.tv_sec) + ((curr_time.tv_usec - start_t.tv_usec)/1000000.));

				fgets(terminal_input, BUFFER_SIZE, stdin);
				snprintf(buffer, BUFFER_SIZE, "%6.3f  User Input: %s", curtime, terminal_input);
				writeToPipe(fd[i]);
			}
			else {

				struct timeval curr_time;
		        gettimeofday(&curr_time, NULL);
		        float current_read_time = (float)((curr_time.tv_sec - start_t.tv_sec) + ((curr_time.tv_usec - start_t.tv_usec)/1000000.));
		        
				snprintf(buffer, BUFFER_SIZE, "%6.3f  Child: %d Message: %d",curtime, i, message_count++);
				writeToPipe(fd[i]);
				sleep(rand()%3);
			}
		} 

	}

	exit(0);
	fclose(output);
}