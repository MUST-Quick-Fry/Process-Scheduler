#include<stdio.h>
#include<unistd.h>

int main(int argc, char *argv[]){
     
    execl("/bin/kill","kill","-TSTP", argv[1], NULL);
    return 0;
}
