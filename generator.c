#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>
#include "defines.h"


void * tvehicle(void * avg)
{
  
  Vehicle v = *(Vehicle *) avg;  
  pthread_detach(pthread_self());
  
  printf("%c\n", v.port);
  
  // gerar restantes caracteristicas, porta, tempo estacionamento
  
  //criar fifo unico para leitura, abrir RW para não bloquear (nome do FIFO unico imporatante)
  
  //cuidado semaforo 
  //sem_wait() iniciar a 1
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
    
    
    
    //test arguments
    if(argc != 3)
    {
      printf("Usage %s <GENERATOR_TIME> <TIME_UNIT_BETWEEN_ACTIONS>\n", argv[0]);
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
        
        
        Vehicle* v = (Vehicle*) malloc(sizeof(Vehicle));
        v->id = id;
        id++;
        v->port = 'N';
        pthread_create(&tid, NULL, tvehicle, v);
        
        
     }
    return 0;
}
