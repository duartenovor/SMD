#include <iostream>
#include <mqueue.h>   /* mq_* functions */
#include <string.h>
#include <unistd.h>
#include <signal.h>

#define MSGQOBJ_NEWCAR "/finger_queue_newcar"
#define MAX_MSG_LEN 	10000 	/* max length of a message queue*/

int main()
{
	mqd_t msgq_id;
	int pid = 0;
	char buffer[MAX_MSG_LEN];

	msgq_id = mq_open(MSGQOBJ_NEWCAR, O_RDWR | O_NONBLOCK);
	if (msgq_id == (mqd_t)-1) {
    	perror("In mq_open()");
    	mq_close(msgq_id);
    	exit(1);
	}

	while(!pid)
	{
		if(mq_receive(msgq_id, buffer, MAX_MSG_LEN, NULL) == -1)
		{
			// get error from errno
			int err = errno;

			// is the queue empty?
			if(err == EAGAIN)
				continue;	// no more to read

			perror("In mq_receive()");
    		mq_close(msgq_id);
    		exit(1);
		}

		pid = atoi(buffer);
		printf("PID: %d\n",pid);
	}

	usleep(10000000);
	kill(pid, SIGUSR1);

	printf("START ADD NEW FINGER\n");

	while(1)
	{
		if(mq_receive(msgq_id, buffer, MAX_MSG_LEN, NULL) == -1)
		{
			//puts("OLA3");

			// get error from errno
			int err = errno;

			// is the queue empty?
			if(err == EAGAIN)
				continue;	// no more to read

			perror("In mq_receive()");
    		mq_close(msgq_id);
    		exit(1);
		}

		//else, print queued message
		puts(buffer);
	}

	return 0;
}
