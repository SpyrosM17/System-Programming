// numerical integration: parallel version with processes and message queues
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <math.h>

// timer
double get_wtime(void)
{
  struct timeval t;
  gettimeofday(&t, NULL);

  return (double)t.tv_sec + (double)t.tv_usec*1.0e-6;
}

// structure for message queue
struct mesg_buffer {
  long mesg_type;
  double mesg_data;
};

struct mesg_buffer message;


double f(double x)
{
  return log(x)*sqrt(x);
}

#define SIZE  2	// number of workers

void worker(int id)
{
	double a = 1.0;
	double b = 4.0;
	unsigned long const n = 1e9;
	const double dx = (b-a)/n;

  key_t key;
  int msgid;

  // ftok to generate unique key
  key = ftok("progfile", 65);

  // open message queue
  msgid = msgget(key, 0666);

	double S = 0;

	for (unsigned long i = id; i < n; i+=SIZE) {
		double xi = a + (i + 0.5)*dx;
		S += f(xi);
	}
	S *= dx;

  // put the result in the message queue
  message.mesg_type = id+1; // +1 to avoid 0
  message.mesg_data = S;
  msgsnd(msgid, &message, sizeof(message), 0);
}

int main()
{
  int i, pid;
  int msgid;
  key_t key;

  // ftok to generate unique key
  key = ftok("progfile", 65);

  // create message queue
  if ((msgid = msgget(key, IPC_CREAT | 0666)) < 0) {
    perror("msgget");
    exit(1);
  }


  double t0 = get_wtime();
  for (i = 0; i < SIZE; i++)
  {
    pid = fork();
    if (pid == 0)
    {
      worker(i);
      exit(0);
    }
  }

  double total_S = 0;
  for (i = 0; i < SIZE; i++) {
    msgrcv(msgid, &message, sizeof(message), i+1, 0); // or just 0 instead of i+1 for all messages
    total_S += message.mesg_data;
  }

  while ((pid = waitpid(-1, NULL, 0)) > 0);
  double t1 = get_wtime();
  printf("Time=%lf seconds, Result=%.8f\n", t1-t0, total_S);

  // destroy the message queue
  msgctl(msgid, IPC_RMID, NULL);

  return 0;
}
