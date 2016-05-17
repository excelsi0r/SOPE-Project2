#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, const char * argv[])
{
    long tps;
    double time_unit;
    
    if(argc != 3)
    {
      printf("Usage %s <GENERATOR_TIME> <TIME_UNIT_BETWEEN_ACTIONS>\n", argv[0]);
      return 1;
    }
    
    tps = sysconf(_SC_CLK_TCK);
    time_unit = (double) atoi (argv[2])/tps;
    
    
    
    return 0;
}
