#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<sys/wait.h>
#include<sys

int cnt=0;

int main(){
     int pid, shmid;
     
     
     cnt=(int *)
     pid =fork();
     
     if(pid<0){
         perror("problem in creating child\n");
         exit(0);
     }
     else if(pid==0){
     	printf("child process is executing\n");
     	for(int i=0;i<5;i++){
     	int temp=*cnt;
     	temp--;
     	*cnt=temp;
     	printf("child count: %d\n",cnt);
     	sleep(1);
     	}
     }else{
     	printf("parent process executing \n");
     	for(int i=0;i<5;i++){
     	temp++;
     	*cnt=temp;
     	printf("parent count: %d\n",cnt);
     	sleep(1);
     	}
     	wait(NULL);
     	}
     	shmdt(cnt);
     	
     	shmctl(shmid, RMID, NULL);
     	printf("\n final count: %d\n",cnt);
     return 0;
}
