#include <syscall.h>
#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <pthread.h>

__thread int shared_var;

#define __NR_get_address 449
#define MAX_BUF_SIZE 32

enum MODE {
    BY_SEGMENT = 0,
    BY_VIRTUAL_ADDRESS = 1,
};

struct Segment {
    unsigned long int start_addr;
    unsigned long int end_addr;
    char seg_name[MAX_BUF_SIZE];
    char lib_name[MAX_BUF_SIZE];
};

struct ProcessSegments {
    pid_t pid;
    struct Segment code_seg;
    struct Segment data_seg;
    struct Segment heap_seg;
    struct Segment stack_seg;
    struct Segment mmap_segs[MAX_BUF_SIZE];
    int mmap_seg_count;
};

struct AddrInfo {
    unsigned long int virt_addr;
    unsigned long int phys_addr;
};

unsigned long int get_phys_addr(unsigned long int virt_addr) {
    struct AddrInfo addr_info;
    addr_info.virt_addr = virt_addr;

    syscall(__NR_get_address, BY_VIRTUAL_ADDRESS, (void *) &addr_info);
    
    return addr_info.phys_addr;
}

void get_thraed_seg(){

    struct ProcessSegments therad_segs;
    
    // at here to call syscall to get thread segemnet
    // get thread tid
    int tid = syscall(__NR_gettid);
    
    // call get_address syscall
    syscall(__NR_get_address, BY_SEGMENT, (void *) &thread_segs);
    printf("%s: %lx-%lx (%lx-%lx)\n", thread_segs.code_seg.seg_name, thread_segs.code_seg.start_addr, thread_segs.code_seg.end_addr, get_phys_addr(thread_segs.code_seg.start_addr), get_phys_addr(thread_segs.code_seg.end_addr));
    printf("%s: %lx-%lx (%lx-%lx)\n", thread_segs.data_seg.seg_name, thread_segs.data_seg.start_addr, thread_segs.data_seg.end_addr, get_phys_addr(thread_segs.data_seg.start_addr), get_phys_addr(thread_segs.data_seg.end_addr));
    printf("%s: %lx-%lx (%lx-%lx)\n", thread_segs.heap_seg.seg_name, thread_segs.heap_seg.start_addr, thread_segs.heap_seg.end_addr, get_phys_addr(thread_segs.heap_seg.start_addr), get_phys_addr(therad_segs.heap_seg.end_addr));
    printf("%s: %lx-%lx (%lx-%lx)\n", thread_segs.stack_seg.seg_name, thread_segs.stack_seg.start_addr, thread_segs.stack_seg.end_addr, get_phys_addr(thread_segs.stack_seg.start_addr), get_phys_addr(thread_segs.stack_seg.end_addr));
    
    for (int i = 0; i < thread_segs.mmap_seg_count; i++) {
        if (strcmp(thread_segs.mmap_segs[i].lib_name, "NULL") != 0) {
            printf("%s (%s): %lx-%lx (%lx-%lx)\n", thread_segs.mmap_segs[i].seg_name, thread_segs.mmap_segs[i].lib_name, thread_segs.mmap_segs[i].start_addr, thread_segs.mmap_segs[i].end_addr, get_phys_addr(thread_segs.mmap_segs[i].start_addr), get_phys_addr(therad_segs.mmap_segs[i].end_addr));
        } else {            
            printf("%s: %lx-%lx (%lx-%lx)\n", thread_segs.mmap_segs[i].seg_name, thread_segs.mmap_segs[i].start_addr, thread_segs.mmap_segs[i].end_addr, get_phys_addr(thread_segs.mmap_segs[i].start_addr), get_phys_addr(thread_segs.mmap_segs[i].end_addr));
        }
    }

    int local_var = 0;
    printf("local_var: %lx (%lx)\n", (unsigned long int) &local_var, get_phys_addr((unsigned long int) &local_var));
    //print the TLS address
    printf("TLS address: %p\n" , &shared_var);
    
    pthread_exit(NULL);
}


int main() {
    pthread_t pid_1,pid_2,pid_3;
    struct ProcessSegments process_segs;
    
	  pthread_create(&pid_1,NULL,get_thread_seg,NULL);  
    pthread_create(&pid_2,NULL,get_thread_seg,NULL);
    pthread_create(&pid_3,NULL,get_thread_seg,NULL);
    
    process_segs.pid = getpid();
    
    syscall(__NR_get_address, BY_SEGMENT, (void *) &process_segs);
    
    printf("%s: %lx-%lx (%lx-%lx)\n", process_segs.code_seg.seg_name, process_segs.code_seg.start_addr, process_segs.code_seg.end_addr, get_phys_addr(process_segs.code_seg.start_addr), get_phys_addr(process_segs.code_seg.end_addr));
    printf("%s: %lx-%lx (%lx-%lx)\n", process_segs.data_seg.seg_name, process_segs.data_seg.start_addr, process_segs.data_seg.end_addr, get_phys_addr(process_segs.data_seg.start_addr), get_phys_addr(process_segs.data_seg.end_addr));
    printf("%s: %lx-%lx (%lx-%lx)\n", process_segs.heap_seg.seg_name, process_segs.heap_seg.start_addr, process_segs.heap_seg.end_addr, get_phys_addr(process_segs.heap_seg.start_addr), get_phys_addr(process_segs.heap_seg.end_addr));
    printf("%s: %lx-%lx (%lx-%lx)\n", process_segs.stack_seg.seg_name, process_segs.stack_seg.start_addr, process_segs.stack_seg.end_addr, get_phys_addr(process_segs.stack_seg.start_addr), get_phys_addr(process_segs.stack_seg.end_addr));

    for (int i = 0; i < process_segs.mmap_seg_count; i++) {
        if (strcmp(process_segs.mmap_segs[i].lib_name, "NULL") != 0) {
            printf("%s (%s): %lx-%lx (%lx-%lx)\n", process_segs.mmap_segs[i].seg_name, process_segs.mmap_segs[i].lib_name, process_segs.mmap_segs[i].start_addr, process_segs.mmap_segs[i].end_addr, get_phys_addr(process_segs.mmap_segs[i].start_addr), get_phys_addr(process_segs.mmap_segs[i].end_addr));
        } else {            
            printf("%s: %lx-%lx (%lx-%lx)\n", process_segs.mmap_segs[i].seg_name, process_segs.mmap_segs[i].start_addr, process_segs.mmap_segs[i].end_addr, get_phys_addr(process_segs.mmap_segs[i].start_addr), get_phys_addr(process_segs.mmap_segs[i].end_addr));
        }
    }

    int local_var = 0;
    printf("local_var: %lx (%lx)\n", (unsigned long int) &local_var, get_phys_addr((unsigned long int) &local_var));
    
    pthread_join(pid_1,NULL);
	  pthread_join(pid_2,NULL);
    pthread_join(pid_3,NULL);

    return 0;
}
