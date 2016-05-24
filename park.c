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
  
  Vehicle vehicle = *(Vehicle *) avg; 
  int fd_write;
  int park_state = 0;
  
  pthread_detach(pthread_self());
  
  fd_write = open(vehicle.fifo, O_WRONLY);
  
  if(free_places > 0 && closed != 1)
  {
    park_state = ENTERING;
    write(fd_write, &park_state, sizeof(int));
    free_places--;
    sleep(vehicle.park_time);
    free_places++;
    park_state = EXITING;
    write(fd_write, &park_state, sizeof(int));
  }
  else if(closed == 1)
  {
    park_state = CLOSED;
    write(fd_write, &park_state, sizeof(int));
  }
  else if(free_places <= 0)
  {
    park_state = FULL;
    write(fd_write, &park_state, sizeof(int));
  }
  
  
  close(fd_write);
  return NULL;
}

void * tcontroller_N(void * avg)
{
  int fd;
  Vehicle vehicle;
   
  mkfifo(FIFON, 0600);
  fd = open(FIFON, O_RDONLY); 
   
  
  while(read(fd, &vehicle, sizeof(vehicle)) != 0)
  {
     if(vehicle.id == -1)
     {
      break;
     }
     else
     {
        pthread_t tid;
        pthread_create(&tid, NULL, tpark_helper, &vehicle);      
     }
  }
 

  close(fd);
  pthread_exit(0);
  return NULL;
}

void * tcontroller_E(void * avg)
{
  int fd;
  Vehicle vehicle; 
   
  mkfifo(FIFOE, 0600);
  fd = open(FIFOE, O_RDONLY);   

  while(read(fd, &vehicle, sizeof(vehicle)) != 0)
  {
     if(vehicle.id == -1)
     {
      break;
     }
     else
     {
        pthread_t tid;
        pthread_create(&tid, NULL, tpark_helper, &vehicle);      
     }
  }
  
  
  close(fd);
  pthread_exit(0);
  return NULL;
}

void * tcontroller_W(void * avg)
{
  int fd;
  Vehicle vehicle; 
   
  mkfifo(FIFOW, 0600);
  fd = open(FIFOW, O_RDONLY);   

  while(read(fd, &vehicle, sizeof(vehicle)) != 0)
  {
     if(vehicle.id == -1)
     {
      break;
     }
     else
     {
        pthread_t tid;
        pthread_create(&tid, NULL, tpark_helper, &vehicle);      
     }
  }
  
  
  close(fd);
  pthread_exit(0);
  return NULL;
}

void * tcontroller_S(void * avg)
{
  int fd;
  Vehicle vehicle; 
   
  mkfifo(FIFOS, 0600);
  fd = open(FIFOS, O_RDONLY);   

  while(read(fd, &vehicle, sizeof(vehicle)) != 0)
  {
     if(vehicle.id == -1)
     {
      break;
     }
     else
     {
        pthread_t tid;
        pthread_create(&tid, NULL, tpark_helper, &vehicle);      
     }
  }
  
  close(fd);
  pthread_exit(0);
  return NULL;
}

int main(int argc, const char * argv[])
{
    //variables declaration
    int fplaces;
    int duration;
    int fdN, fdS, fdW, fdE;
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
    
    //creating threads and executing
    pthread_create(&tidN, NULL, tcontroller_N, NULL); 
    pthread_create(&tidS, NULL, tcontroller_S, NULL); 
    pthread_create(&tidW, NULL, tcontroller_W, NULL); 
    pthread_create(&tidE, NULL, tcontroller_E, NULL); 
    
    closed = 0;
    sleep(duration);
    closed = 1;
    
    //creating the stop vehicle and closing program
    vehicle_stop->id = -1;
    fdN = open(FIFON, O_WRONLY);
    fdS = open(FIFOS, O_WRONLY);
    fdW = open(FIFOW, O_WRONLY);
    fdE = open(FIFOE, O_WRONLY);
    write(fdN, &vehicle_stop, sizeof(Vehicle));
    write(fdS, &vehicle_stop, sizeof(Vehicle));
    write(fdW, &vehicle_stop, sizeof(Vehicle));
    write(fdE, &vehicle_stop, sizeof(Vehicle));
    free(vehicle_stop);
    close(fdN);
    close(fdS);
    close(fdW);
    close(fdE);
    unlink(FIFON);
    unlink(FIFOS);
    unlink(FIFOW);
    unlink(FIFOE);
    
    
    pthread_join(tidN,NULL);
    pthread_join(tidS,NULL);
    pthread_join(tidW,NULL);
    pthread_join(tidE,NULL);
    
    pthread_exit(NULL);
    return 0;
}
