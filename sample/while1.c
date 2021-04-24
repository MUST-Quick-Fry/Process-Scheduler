#include<stdio.h>
#include<sys/unistd.h>
#include<time.h>
int main(void){
	int cnt=0;
	while(1){
		printf("%d\n",cnt++);
		sleep(1);
	}
	return 0;
}