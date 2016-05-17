#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>


void * tusher(void * avg)
{
  //arrumar
}

void * tcontroller(void * avg)
{
  int close = 0;
  
  //criar extensao para N, S, O
  int fd = open(FIFON, O_RDONLY);
  
  while(read(fd, &vehicle, sizeof(vehicle) != 0)
  {
     if(vehicle.id == -1)
      closed = 1;
     else if(closed)
     {
      //send back
     }
     else
     {
       //criar arrumador e passar vehicle
     }
  }
  close(fd);
  return NULL;

}



int main(int argc, const char * argv[])
{
    //variables declaration
    int dp, fplaces;
    int duration;
    pthread_t tidN, tidS, tidO, tidE;
    int fdN, fdS, fdO, fdE;
    

    //test arguments
    if(argc != 3)
    {
      printf("Usage %s <FREE_SPOTS_NUMBER> <TIME_UNTIL_CLOSE>\n", argv[0]);
      return 1;
    }
    
    //variable initialization
    dp = fplaces = atoi(argv[1]);
    duration = atoi(argv[2]);
    
    //creating fifos
    mkfifo(FIFON, 0600);
    pthread_create(&tidN, NULL, tcontroller, NULL);
    fdN = open(FIFON, O_WRONLY);
    
    mkfifo(FIFOS, 0600);
    pthread_create(&tidS, NULL, tcontroller, NULL);
    fdN = open(FIFOS, O_WRONLY);
    
    mkfifo(FIFOE, 0600);
    pthread_create(&tidE, NULL, tcontroller, NULL);
    fdN = open(FIFOE, O_WRONLY);
    
    mkfifo(FIFOO, 0600);
    pthread_create(&tidO, NULL, tcontroller, NULL);
    fdN = open(FIFOO, O_WRONLY);
    
    sleep(duration);
    
    //falta criar vehicle struct com id = -1;
    write(fdN, &vehicle_stop, sizeof(vehicle));
    close(fdN);
    
    //espera outros threads
    
    unlink(FIFON);
    unlink(FIFOS);
    unlink(FIFOE);
    unlink(FIFOO);
    
    return 0;
}
