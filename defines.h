#define FIFON "/tmp/fifoN"
#define FIFOS "/tmp/fifoS"
#define FIFOW "/tmp/fifoO"
#define FIFOE "/tmp/fifoE"

#define GENERATOR_LOG   "generate_log.log"
#define PARK_LOG        "park_log.log"
#define LOG             "general_log.log"

#define ENTERING 1 
#define EXITING 2
#define FULL -1
#define CLOSED -2

#define MAX_BUF 64
#define LONG_BUF 256

typedef struct 
{
  int id;
  char port;
  double park_time;
  long initial_tick;
  char fifo[MAX_BUF];
  long tps;
} Vehicle;
