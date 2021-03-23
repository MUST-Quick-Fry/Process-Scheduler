#include<stdio.h>
#include<unistd.h>
#include<time.h>

int main(){

    time_t start = time(NULL);
    printf("Before sleep...\n");
    
    int i = 0;
    while( i < 100 ){
        sleep(1);
	printf("Sleep %d sec.\n", i);

	i++;
    }

    time_t end = time(NULL);
    printf("After %ld seconds. Wake up and die...\n", end-start);

    return 0;
}
