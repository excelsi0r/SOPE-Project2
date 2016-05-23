#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "defines.h"



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
  v->park_time = 0.0;
  return;
}

void * tvehicle(void * avg)
{
  
  int fd ;
  Vehicle v = *(Vehicle *) avg;    
  
  pthread_detach(pthread_self());
  printf("%c\n", v.port);
  
  // gerar restantes caracteristicas, porta, tempo estacionamento
  //criar fifo unico para leitura, abrir RW para não bloquear (nome do FIFO unico imporatante)
  //cuidado semaforo 
  //sem_wait() iniciar a 1
  
  
  if(v.port == 'N')
  {
    fd = open(FIFON, O_WRONLY | O_NONBLOCK);
  }
  else if(v.port == 'S')
  {
    fd = open(FIFOS, O_WRONLY | O_NONBLOCK);

  }
  else if(v.port == 'E')
  {
     fd = open(FIFOE, O_WRONLY | O_NONBLOCK);
  }
  
  else if(v.port == 'W')
  {
    fd = open(FIFOW, O_WRONLY | O_NONBLOCK);
  }
  
  if (fd != -1)
  {
    write(fd, &v, sizeof(Vehicle));
    close(fd);
  }
  else
  {
    printf("what\n");
  }
  
  
  /*
  int fd = open(FIFON, O_WRONLY | O_NONBLOCK);
  if(fd != -1)
  {
    write(fd, &vehicle, sizeof(vehicle));
    close();
    
    read(fifounico, ...,...);
    //entrar parque
    read(....);
  
  }
  */
  //apagar fifo
  //sem_post
  
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
        pthread_create(&tid, NULL, tvehicle, v);     
        
     }
     
     free(v);
     pthread_exit(NULL);
     
    return 0;
}
