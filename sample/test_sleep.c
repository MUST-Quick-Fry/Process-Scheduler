#include<stdio.h>
#include<unistd.h>
#include<time.h>

int main(){

    time_t start = time(NULL);
    printf("Before sleep...\n");

    sleep(3);

    time_t end = time(NULL);
    printf("After %ld seconds. Wake up and die...\n", end-start);

    return 0;
}
