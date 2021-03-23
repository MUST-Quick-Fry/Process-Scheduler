#include<stdio.h>
#include<unistd.h>

int main(int argc, char *argv[]){
     
    execl("/bin/kill","kill","-CONT", argv[1], NULL);
    return 0;
}
