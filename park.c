#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <semaphore.h>
#include "defines.h"

int free_places;
int closed;
int log_park;
pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mlog = PTHREAD_MUTEX_INITIALIZER;


void write_park_log(Vehicle * v, int state);

/**Park Helper to process Vehicle upon creation and when the ports are still oepn*/
void * tpark_helper(void * avg)
{
  
  Vehicle vehicle = *(Vehicle *) avg; 
  int fd_write;
  int park_state = 0;
  pthread_detach(pthread_self());
  
  printf("Reaching Vehicle to park %d\n", vehicle.id);
  
  fd_write = open(vehicle.fifo, O_WRONLY);
  
  pthread_mutex_lock(&m);
  if(free_places > 0 && closed != 1)
  {
    
    park_state = ENTERING;
    free_places--;
    write(fd_write, &park_state, sizeof(int));
    
    
    pthread_mutex_lock(&mlog);
    write_park_log(&vehicle, park_state);
    pthread_mutex_unlock(&mlog);
    
    pthread_mutex_unlock(&m);
    
   	struct timespec ts;
    ts.tv_sec = (time_t) (vehicle.park_time / vehicle.tps);
    ts.tv_nsec = (long) (((vehicle.park_time / vehicle.tps)-ts.tv_sec) * 1000000000);
    nanosleep(&ts, NULL);
    
    free_places++;
    park_state = EXITING;

  }
  else if(closed == 1)
  {
  	pthread_mutex_unlock(&m);
    park_state = CLOSED;
  }
  else if(free_places <= 0)
  {
  	pthread_mutex_unlock(&m);
    park_state = FULL;
  }
  write(fd_write, &park_state, sizeof(int));

	pthread_mutex_lock(&mlog);
  write_park_log(&vehicle, park_state);
  pthread_mutex_unlock(&mlog);
   
  close(fd_write);
  pthread_exit(NULL);
}

/**Controller Port North*/
void * tcontroller_N(void * avg)
{
 
  int fd;
  Vehicle vehicle;
  int read_fd;
 
  mkfifo(FIFON, 0600);
  fd = open(FIFON, O_RDONLY); 


  while(1)
  {
  	 read_fd = read(fd, &vehicle, sizeof(Vehicle));
			 if(vehicle.id == 0)
			 {
				break;
			 }
			 else if(read_fd > 0 )
			 {
				  pthread_t tid;
				  pthread_create(&tid, NULL, tpark_helper, &vehicle);      
			 }
		 
  }
 

  close(fd);
  unlink(FIFON);
  printf("exiting N\n");
  pthread_exit(NULL);
}
/**Controller Port East*/
void * tcontroller_E(void * avg)
{
  int fd;
  Vehicle vehicle; 
  int read_fd;
   
  mkfifo(FIFOE, 0600);
  fd = open(FIFOE, O_RDONLY);   

  while(1)
  {
  	 read_fd = read(fd, &vehicle, sizeof(Vehicle));
			 if(vehicle.id == 0)
			 {
				break;
			 }
			 else if(read_fd > 0 )
			 {
				  pthread_t tid;
				  pthread_create(&tid, NULL, tpark_helper, &vehicle);      
			 }
		 
  }
  
  
  close(fd);
  unlink(FIFOE);
  printf("exiting E\n");
  pthread_exit(NULL);
}
/**Controller Port West*/
void * tcontroller_W(void * avg)
{
  int fd;
  Vehicle vehicle; 
  int read_fd;
   
  mkfifo(FIFOW, 0600);
  fd = open(FIFOW, O_RDONLY);   

	while(1)
  {
  	 read_fd = read(fd, &vehicle, sizeof(Vehicle));	 
			 if(vehicle.id == 0)
			 {
				break;
			 }
			 else if(read_fd > 0 )
			 {
				  pthread_t tid;
				  pthread_create(&tid, NULL, tpark_helper, &vehicle);      
			 }
		 
  }
  
  
  close(fd);
  unlink(FIFOW);
  printf("exiting W\n");
  pthread_exit(NULL);
}
/**Controller Port South*/
void * tcontroller_S(void * avg)
{
  int fd;
  Vehicle vehicle; 
  int read_fd;
   
  mkfifo(FIFOS, 0600);
  
  fd = open(FIFOS, O_RDONLY);   

	while(1)
  {
  	 read_fd = read(fd, &vehicle, sizeof(Vehicle));
  	 if(vehicle.id == 0)
			 {
				break;
			 }
			 else if(read_fd > 0 )
			 {
				  pthread_t tid;
				  pthread_create(&tid, NULL, tpark_helper, &vehicle);      
			 
		 }
  }
  
  close(fd);
  unlink(FIFOS);
  printf("exiting S\n");
  pthread_exit(NULL);
}
/**Fuction to create log */
void create_park_log()
{
  FILE* file = fopen(PARK_LOG, "w");
  fclose(file);
  log_park = open(PARK_LOG, O_WRONLY | O_CREAT, 0600);
  char str[] = "t(ticks) ; nlug ; id_viat ; observ\n";
  write(log_park, str, strlen(str));
   
}
/**Fuction to Write in log*/
void write_park_log(Vehicle * v, int state)
{
  long ticks = v->initial_tick;
  int id = v->id;
  char observ[MAX_BUF];
  
  char str[LONG_BUF];
  
  
  switch(state)
  {
    case ENTERING:
    strcpy(observ, "entered");
    break;
    case EXITING:
    strcpy(observ, "exit");
    break;
    case FULL:
    strcpy(observ, "full");
    break;
    case CLOSED:
    strcpy(observ, "close");
    break;
    default:
    return;
    break;
  }
  
  if(state == EXITING)
  {
    sprintf(str, "%7li ; %5d ; %7d ; %s\n", ticks, free_places, id, observ);
  }
  else
  {
    sprintf(str, "%7li ; %5d ; %7d ; %s\n", ticks, free_places, id,observ);
  }
  
  write(log_park, str, strlen(str));
  printf("Vehicle %d state: %s\n",id,observ);
  
}
/**Main fuction to initiate controllers*/
int main(int argc, const char * argv[])
{
    //variables declaration
    int fplaces;
    int duration;
    int fdN, fdS, fdW, fdE;
    pthread_t tidN, tidS, tidW, tidE;
    Vehicle* vehicle_stop = (Vehicle*) malloc(sizeof(Vehicle));
    vehicle_stop->id = 0;
    
    //test arguments
    if(argc != 3)
    {
      printf("Usage %s <FREE_SPOTS_NUMBER> <TIME_UNTIL_CLOSE_IN_SEC>\n", argv[0]);
      free(vehicle_stop);
      return 1;
    }
       
    //variable initialization
    free_places = fplaces = atoi(argv[1]);
    duration = atoi(argv[2]);
    
    if(duration < 1)
    {
      perror("Open park time must be greater than 1\n");
      free(vehicle_stop);
      return 2;
    }
    
    //semaphore
    sem_t* semaphore = sem_open(SEM_NAME, O_CREAT,0600,1);
    create_park_log();
        
    //creating threads and executing
    pthread_create(&tidN, NULL, tcontroller_N, NULL); 
    pthread_create(&tidS, NULL, tcontroller_S, NULL); 
    pthread_create(&tidW, NULL, tcontroller_W, NULL); 
    pthread_create(&tidE, NULL, tcontroller_E, NULL); 
   
   	closed = 0;
    sleep(duration);
    closed = 1;
    
    fdN = open(FIFON, O_WRONLY);
    fdS = open(FIFOS, O_WRONLY);
    fdW = open(FIFOW, O_WRONLY);
    fdE = open(FIFOE, O_WRONLY);
    
    //creating the stop vehicle and closing program
    sem_wait(semaphore); //not sending vehicle at the same time
    write(fdN, vehicle_stop, sizeof(Vehicle));
    write(fdS, vehicle_stop, sizeof(Vehicle));
    write(fdW, vehicle_stop, sizeof(Vehicle));
    write(fdE, vehicle_stop, sizeof(Vehicle));
		sem_post(semaphore);
		sem_close(semaphore);

    free(vehicle_stop);
    close(fdN);
    close(fdS);
    close(fdW);
    close(fdE);
    
    pthread_join(tidN,NULL);
    pthread_join(tidS,NULL);
    pthread_join(tidW,NULL);
    pthread_join(tidE,NULL);
    
    sem_unlink(SEM_NAME);
    //unlink(FIFON); unlink(FIFOS); unlink(FIFOE); unlink(FIFOW);
    pthread_exit(NULL);
    return 0;
}
