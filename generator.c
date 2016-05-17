#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

int main(int argc, const char * argv[])
{
    long tps;
    double time_unit;
    pthread_t tid;
    double time = 0.0;
    double duration = (double) atoi(argv[1]);
    
    
    if(argc != 3)
    {
      printf("Usage %s <GENERATOR_TIME> <TIME_UNIT_BETWEEN_ACTIONS>\n", argv[0]);
      return 1;
    }
    
    tps = sysconf(_SC_CLK_TCK);
    time_unit = (double) atoi (argv[2])/tps;

     
    
    return 0;
}
