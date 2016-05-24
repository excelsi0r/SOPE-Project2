#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include "defines.h"


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

void generate_time(Vehicle* v)
{
  v->park_time = 2.0;
  return;
}

void * tvehicle(void * avg)
{
  int park_state = 0;
  int fd_write, fd_read;
  Vehicle v = *(Vehicle *) avg;    
  
  pthread_detach(pthread_self());
  printf("%d - %c - %f - %s\n",v.id ,v.port,v.park_time, v.fifo);
  
  // gerar restantes caracteristicas, porta, tempo estacionamento
  //criar fifo unico para leitura, abrir RW para não bloquear (nome do FIFO unico imporatante)
  //cuidado semaforo 
  //sem_wait() iniciar a 1
  
  
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
    
    mkfifo(v.fifo, 0600);
    fd_read = open(v.fifo, O_RDONLY);
  
   if(fd_read != -1)
    {
      read(fd_read, &park_state, sizeof(int));
      printf("park available entering\n");      
      read(fd_read, &park_state, sizeof(int));
      printf("exiting\n");
      
    }
  }
  else
  {
    printf("Couldnt find fifo\n");
  }
  
  close(fd_read);
  unlink(v.fifo);
  //sem_post
  
  
  pthread_exit(0);
  return NULL;
}



int main(int argc, const char * argv[])
{
    srand(time(NULL));
    
    //variables declaration
    long tps;
    double time_unit;
    pthread_t tid;
    double elapsed_time = 0.0;
    int id = 0;
    double duration;
    Vehicle* v = (Vehicle*) malloc(sizeof(Vehicle));
    
    //test arguments
    if(argc != 3)
    {
      printf("Usage %s <GENERATOR_TIME> <TIME_UNIT_BETWEEN_ACTIONS>\n", argv[0]);
      free(v);
      return 1;
    }
    
    //variables initialization
    tps = sysconf(_SC_CLK_TCK);
    time_unit = (double) atoi (argv[2])/tps;
    duration = (double) atoi(argv[1]);
    
    
     while(elapsed_time <= duration)
     {
        int rn = rand() % 10;
        if(rn < 3)
        {
          sleep(time_unit);
          elapsed_time += time_unit;
        }              
        else if(rn < 5)
        {
            sleep(2*time_unit);
            elapsed_time += 2*time_unit;
        }
        else{}
        
        v->id = id;
        id++;
        generate_port(v);
        generate_time(v);
        generate_fifo_name(v);
        pthread_create(&tid, NULL, tvehicle, v);     
        
     }
     
     free(v);
     pthread_exit(NULL);
}
  
