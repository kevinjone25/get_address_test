#include <syscall.h>
#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <pthread.h>
int count;
__thread int x = 5;
pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;

 void* thread(void* arg){ 
	int t = stscall(__NR_gettid);
	for(int i=0; i<3;++i){
	pthread_mutex_lock( &mutex1);
	int tmp = count;
	sleep(1);
	count = tmp += 1;
	pthread_mutex_unlock(&mutex1);
	prtintf("count = %d\n",count);
	}

    pthread_exit(NULL);
}  
 

int main(){

	pthread_t pid_1,pid_2;
	pthread_create(&pid_1,NULL,thread,NULL);  
    	pthread_create(&pid_2,NULL,thread,NULL);  
	pthread_join(pid_1,NULL);
	pthread_join(pid_2,NULL);
	return 0;
}
