#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "defines.h"

int free_places;
int closed;

void * tpark_helper(void * avg)
{
  pthread_detach(pthread_self());
  return NULL;
}

void * tcontroller(void * avg)
{
  char port = *(char *) avg;
  int fd;
  Vehicle vehicle;
   
  if(port == 'N')
  {
    mkfifo(FIFON, 0600);
    fd = open(FIFON, O_WRONLY);
  }
  else if(port == 'S')
  {
    mkfifo(FIFOS, 0600);
    fd = open(FIFOS, O_WRONLY);
  }
  else if(port == 'W')
  {
    mkfifo(FIFOW, 0600);
    fd = open(FIFOW, O_WRONLY);
  }
  else if(port == 'E')
  {
     mkfifo(FIFOE, 0600);
     fd = open(FIFOE, O_WRONLY);
  }
  
  while(read(fd, &vehicle, sizeof(vehicle) != 0))
  {
     if(vehicle.id == -1)
     {
      //parque fechado
     }
     else if(closed == 1)
     {
      //send back
     }
     else
     {
        pthread_t tid;
        pthread_create(&tid, NULL, tpark_helper, vehicle);     
     }
  }
  
  close(fd);
  return NULL;
}


void create_port_threads(pthread_t* tidN, pthread_t* tidS, pthread_t* tidW, pthread_t* tidE)
{
    char* c = (char *) malloc(sizeof(char));
    (*c) = 'N';
    pthread_create(tidN, NULL, tcontroller, c);
    (*c) = 'S';
    pthread_create(tidS, NULL, tcontroller, c);
    (*c) = 'W';
    pthread_create(tidW, NULL, tcontroller, c);
    (*c) = 'E';
    pthread_create(tidE, NULL, tcontroller, c);
    free(c);
}

int main(int argc, const char * argv[])
{
    //variables declaration
    int fplaces;
    int duration;
    int fdN;
    pthread_t tidN, tidS, tidW, tidE;
    Vehicle* vehicle_stop = (Vehicle*) malloc(sizeof(Vehicle));
    
    //test arguments
    if(argc != 3)
    {
      printf("Usage %s <FREE_SPOTS_NUMBER> <TIME_UNTIL_CLOSE>\n", argv[0]);
      free(vehicle_stop);
      return 1;
    }
    
    //variable initialization
    free_places = fplaces = atoi(argv[1]);
    duration = atoi(argv[2]);
    
    printf("%d, %d\n",free_places,fplaces);
    
    //creating threads and executing
    create_port_threads(&tidN, &tidS, &tidW, &tidE);
    closed = 0;
    sleep(duration);
    closed = 1;
    
    //creating the stop vehicle and closing program
    fdN = open(FIFON, O_WRONLY);
    vehicle_stop->id = -1;
    write(fdN, &vehicle_stop, sizeof(Vehicle));
    free(vehicle_stop);
    close(fdN);
    unlink(FIFON);
    /*falta funçao para fechar threads*/

    return 0;
}
