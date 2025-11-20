// msgq1.c
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/wait.h>

// structure for message queue
struct mesg_buffer {
	long mesg_type;
	double mesg_data[8];
};


struct mesg_buffer message;

void server()
{
	key_t key;
	int msgid;

	// ftok to generate unique key
	key = ftok("progfile", 65);

	// msgget creates a message queue
	// and returns identifier
	msgid = msgget(key, 0666 | IPC_CREAT);
	message.mesg_type = 1;

	printf("Prepare data\n");
  for (int i = 0; i < 8; i++)
    message.mesg_data[i] = i;

	printf("Send data\n");
	// msgsnd to send message
	msgsnd(msgid, &message, sizeof(message), 0);

	printf("Receive data\n");
  msgrcv(msgid, &message, sizeof(message), 2, 0);

  printf("Display data\n");
  for (int i = 0; i < 8; i++)
    printf("server: mesg_data[%d] = %lf\n", i, message.mesg_data[i]);

  waitpid(-1, NULL, 0);

  // to destroy the message queue
  msgctl(msgid, IPC_RMID, NULL);


}

void client()
{
  key_t key;
  int msgid;

  // ftok to generate unique key
  key = ftok("progfile", 65);

  // msgget creates a message queue
  // and returns identifier
  msgid = msgget(key, 0666 /*| IPC_CREAT*/);

  // msgrcv to receive message
  msgrcv(msgid, &message, sizeof(message), 1, 0);

  // display the message
  for (int i = 0; i < 8; i++)
    printf("client: mesg_data[%d] = %lf\n", i, message.mesg_data[i]);

  for (int i = 0; i < 8; i++)
    message.mesg_data[i] += 100;

  message.mesg_type = 2;
	msgsnd(msgid, &message, sizeof(message), 0);

  exit(1);
}

int main()
{
  int pid;

  pid = fork();
  if (pid > 0)
  {
    server();
  }
  else if (pid == 0)
  {
    sleep(1);
    client();
  }
  else
    perror("fork");


	return 0;
}
