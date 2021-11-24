#include <syscall.h>
#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <pthread.h>

__thread int x = 5;

 void* thread_1(void* arg){  
    x += 1;
    int t = syscall(__NR_gettid);
    printf("__thread 1 value: %d\naddress: %p\ntid: %d\n", x, &x, t);
    pthread_exit(NULL);
}  
 void* thread_2(void* arg){  
    sleep(1);
    int t = syscall(__NR_gettid);
    printf("__thread 2 value: %d\naddress: %p\ntid: %d\n", x, &x, t);
    pthread_exit(NULL);
}  

int main(){

	pthread_t pid_1,pid_2;
	pthread_create(&pid_1,NULL,thread_1,NULL);  
    	pthread_create(&pid_2,NULL,thread_2,NULL);  
	pthread_join(pid_1,NULL);
	pthread_join(pid_2,NULL);
	return 0;
}
