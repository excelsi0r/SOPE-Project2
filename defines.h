#define FIFON "/tmp/fifoN"
#define FIFOS "/tmp/fifoS"
#define FIFOW "/tmp/fifoO"
#define FIFOE "/tmp/fifoE"

#define LOG   "/tmp/log.txt"

#define ENTERING 1 
#define EXITING 2
#define FULL -1
#define CLOSED -2

#define MAX_BUF 64

typedef struct 
{
  int id;
  char port;
  float park_time;
  char fifo[MAX_BUF];
} Vehicle;
