#include <stdio.h> 
#include <stdlib.h> 
#include <time.h> 
#include <sys/times.h>
#include <unistd.h>
#include <errno.h>

#define DEFAULT_TIME 4.5

int main(int argc, char *argv[]){
	int i = 0;
	unsigned long long total = 0;
	struct tms currentTMS;
	double target = DEFAULT_TIME;
	clock_t targetClock;
	long clockTicks = sysconf(_SC_CLK_TCK);
	
	if (argc == 2){
		errno = 0;
		target = strtod(argv[1], NULL);
		if (errno || target <= 0){
			target = DEFAULT_TIME;
		}
	}
	targetClock = (clock_t)(target * clockTicks);
	
	srand(time(NULL));
	while (1){
		i = (i + 1) % 100;
		if (i == 0){
			times(&currentTMS);
			if ((currentTMS.tms_utime + currentTMS.tms_stime) >= targetClock){
				break;
			}
		}
		total += ( (rand() % 2) ?
				  (-1 * rand() % 10) :
				  ( rand() % 10) );
	}
	return 0;
}