#include <iostream>
#include <mqueue.h>   /* mq_* functions */
#include <string.h>

#define MSGQOBJ_VERPAT "/rfid_queue_verpat"
#define MAX_MSG_LEN 	10000 	/* max length of a message queue*/

int main()
{
	mqd_t msgq_id;

	char buffer[MAX_MSG_LEN];

	msgq_id = mq_open(MSGQOBJ_VERPAT, O_RDWR | O_NONBLOCK);
	if (msgq_id == (mqd_t)-1) {
    	perror("In mq_open()");
    	mq_close(msgq_id);
    	exit(1);
	}

	printf("msgq_id: %d\n",msgq_id);

	//puts("OLA1");

	while(1)
	{
		//puts("OLA2");

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
