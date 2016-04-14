#include "filelock.h"
#include <stdio.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/prctl.h>

int main()
{
	int i;
	int result;
	filelock_mutex_t locktest;
	filelock_mutex_init(&locktest);
	pid_t pid[4] = {-1, -1, -1, -1};
    for(i = 0; i < 4; i++) {
        pid[i] = fork();
        if(0 == pid[i] || -1 == pid[i]) 
            break;
    }
    if(0 == pid[0]) {
        filelock_mutex_rlock(&locktest, LOCK_BLOCK);
        printf("000\n");
        sleep(2);
        filelock_mutex_unlock(&locktest);
    }
    if(0 == pid[1]) {
        filelock_mutex_wlock(&locktest, LOCK_BLOCK);
        printf("111\n");
        sleep(2);
        filelock_mutex_unlock(&locktest);
    }
    if(0 == pid[2]) {
        filelock_mutex_rlock(&locktest, LOCK_BLOCK);
        printf("222\n");
        sleep(2);
        filelock_mutex_unlock(&locktest);
    }
    if(0 == pid[3]) {
        filelock_mutex_wlock(&locktest, LOCK_UNBLOCK);
        printf("333\n");
        sleep(2);
        filelock_mutex_unlock(&locktest);
    }
    if(pid[0] != 0 && pid[1] != 0 && pid[2] != 0 && pid[3] != 0) {
        for(i = 0; i < 4; i++) {
            waitpid(pid[i], NULL, 0);
        }
        result = filelock_mutex_unlock(&locktest);
        printf("%d\n", result);
    }
    filelock_mutex_destroy(&locktest);
	return 0;
}