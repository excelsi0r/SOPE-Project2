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

int log_generator;
pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;

void write_generator_log(Vehicle * v, int state);

void generate_fifo_name(Vehicle* v)
{
  sprintf(v->fifo,"%s%d","fifo",v->id);
}

void generate_port(Vehicle* v)
{
  int p = rand() % 4;
   
  if(p == 0)
    v->port = 'N';
  else if(p == 1)
    v->port = 'S';
  else if(p == 2)
    v->port = 'E';
  else if(p == 3)
    v->port = 'W';
   
  return;
}

void generate_time(Vehicle* v, long initial_t, double time_unit, long ticks_per_sec)
{
  v->initial_tick = initial_t;
  v->tps = ticks_per_sec;
  v->park_time = ((rand() % 10)+1)*time_unit;
  return;
}

void * tvehicle(void * avg)
{
  int park_state = 0;
  int fd_write, fd_read;
  Vehicle v = *(Vehicle *) avg;    
  
  pthread_detach(pthread_self());
	sem_t * semaphore = sem_open(SEM_NAME, O_CREAT, 0600, 1);
	mkfifo(v.fifo, 0600);
  sem_wait(semaphore);
  if(v.port == 'N')
  {
    fd_write = open(FIFON, O_WRONLY | O_NONBLOCK);
  }
  else if(v.port == 'S')
  {
    fd_write = open(FIFOS, O_WRONLY | O_NONBLOCK);

  }
  else if(v.port == 'E')
  {
     fd_write = open(FIFOE, O_WRONLY | O_NONBLOCK);
  }
  
  else if(v.port == 'W')
  {
    fd_write = open(FIFOW, O_WRONLY | O_NONBLOCK);
  }
  
  
  
  if (fd_write != -1)
  {
    write(fd_write, &v, sizeof(Vehicle));
    close(fd_write);
  
    sem_post(semaphore);
    sem_close(semaphore);
 		fd_read = open(v.fifo, O_RDONLY);
 		
   if(fd_read != -1)
    {
      read(fd_read, &park_state, sizeof(int));
			pthread_mutex_lock(&m);
      write_generator_log(&v, park_state); 
      pthread_mutex_unlock(&m); 
      read(fd_read, &park_state, sizeof(int));   
    }
  }
  else
  {
  	sem_post(semaphore);
    sem_close(semaphore);
   // printf("Parque fechado\n");
   	park_state = CLOSED;
  }
  
  //close(fd_read);
  unlink(v.fifo);
	pthread_mutex_lock(&m);
  write_generator_log(&v, park_state); 
  pthread_mutex_unlock(&m); 
  //sem_post
  
  
  pthread_exit(NULL);
}

void create_generator_log()
{
  FILE* file = fopen(GENERATOR_LOG, "w");
  
  fclose(file);
  log_generator = open(GENERATOR_LOG, O_WRONLY | O_CREAT, 0600);
  char str[] = "t(ticks); id_viat ; destin ; t_estacion ; t_vida ; observ\n";
  write(log_generator, str, strlen(str));
   
}

void write_generator_log(Vehicle * v, int state)
{
  //"t(ticks) ; id_viat ; destin ; t_estacion ; t_vida ; observ\n";
  long ticks = v->initial_tick;
  int id = v->id;
  char port = v->port;
  int estacion = (int) v->park_time * v->tps;
  int tmp_vida = (int) ticks + estacion;
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
    sprintf(str, "%7li ; %7d ; %6c ; %10d ; %6d ; %s\n", ticks, id, port, estacion, tmp_vida, observ);
  }
  else
  {
    sprintf(str, "%7li ; %7d ; %6c ; %10d ;     ?  ; %s\n", ticks, id, port, estacion, observ);
  }
  
  write(log_generator, str, strlen(str));
}


int main(int argc, const char * argv[])
{
    srand(time(NULL));
    
    //variables declaration
    long tps;
    double time_unit;
    pthread_t tid;
    double elapsed_time = 0.0;
    int id = 1;
    double duration;
    Vehicle* v = (Vehicle*) malloc(sizeof(Vehicle));
    
    //test arguments
    if(argc != 3)
    {
      printf("Usage %s <GENERATOR_TIME_SEC> <TICKS_UNIT_BETWEEN_ACTIONS>\n", argv[0]);
      free(v);
      return 1;
    }
    
    //variables initialization
    tps = sysconf(_SC_CLK_TCK);
    time_unit = (double) atoi (argv[2])/tps;
    duration = (double) atoi(argv[1]);
    create_generator_log();
    
     while(elapsed_time <= duration)
     {
        int rn = rand() % 10;
        if(rn < 3)
        {
          struct timespec ts;
          ts.tv_sec = (time_t) time_unit;
          ts.tv_nsec = (long) ((time_unit - ts.tv_sec) * 1000000000);
         	//printf("%li + %li\n", ts.tv_sec, ts.tv_nsec);
          nanosleep(&ts, NULL);
          elapsed_time += time_unit;
        }              
        else if(rn < 5)
        {
            struct timespec ts;
            double timetemp = 2*time_unit;
            ts.tv_sec = (time_t) timetemp;
            ts.tv_nsec = (long) ((timetemp - ts.tv_sec) * 1000000000);
            nanosleep(&ts, NULL);
            sleep(timetemp);
            elapsed_time += timetemp;
        }
        else{}
        
        v->id = id;
        id++;
        generate_port(v);
        generate_time(v,(long) tps*elapsed_time,time_unit, tps);
        generate_fifo_name(v);
        
        pthread_create(&tid, NULL, tvehicle, v);     
        
     }
     
     free(v);
     pthread_exit(NULL);
}
  
