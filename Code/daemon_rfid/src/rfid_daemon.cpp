/**
* @file rfid_daemon.cpp
* @author Joao Miranda and Duarte Rodrigues
* @date 19-01-2021
*
* @brief Process that reads the rfid sensor
*/

#include <fcntl.h>			/**<file descriptor */
#include <mqueue.h>   		/**< mq_* functions */
#include <sys/syslog.h>		/**< syslog */
#include <signal.h>			/**< signals */
#include <unistd.h>			/**< usleep */
#include <string.h>			/**< strlen */

/**< Includes used to RFID use */
#include "MFRC522.h"

#define MSGQOBJ_VERPAT "/rfid_queue_verpat"		/**< Message queue used to send the RFID searched and matched */
#define MSGQOBJ_NEWPAT "/rfid_queue_newpat"		/**< Message queue used to send the RFID added or error message */

/**< mqd_t type, which is used for message queue descriptors */
mqd_t msgq_id_verpat;
mqd_t msgq_id_newpat;
mqd_t msgq_id;

/**< Variable that indicates the state of program {1->run}*/
int state = 1;

/**<Prototype Functions about init and close tools used*/
void create_daemon();
void create_mqueues();
void delete_mqueues();

/**< Function that help to change the state of the system */
static void sig_handler(int sig);

int main(int argc, char *argv[])
{	
	/*< Init RFID sensor*/
	MFRC522 mfrc;
	mfrc.PCD_Reset();
	mfrc.PCD_Init();

	signal(SIGUSR1,sig_handler);	/**< SIGNAL TO INSERT NEW PATIENT */
	signal(SIGUSR2,sig_handler);	/**< SIGNAL TO INSERT NEW PATIENT */
    signal(SIGTERM,sig_handler);    /**< catch KILL SIGNAL */

	char msg[64];

	syslog(LOG_ERR, "%s\n","DAEMON!\n");

	create_daemon();
	create_mqueues();

	/**< Send PID to main program*/
	sprintf(msg, "%d", (int)getpid());
	mq_send(msgq_id_newpat, msg, strlen(msg)+1, 1);

	syslog(LOG_ERR, "%s\n","lets go RFID!\n");

	/* Define default opeartion as search RFID*/
	msgq_id = msgq_id_verpat;

	struct timespec tm;
	tm.tv_sec = 5;

	while(state)
	{
		if(!mfrc.PICC_IsNewCardPresent())
		  continue;

		syslog(LOG_ERR, "%s","New card detected!\n");

		if( !mfrc.PICC_ReadCardSerial())
		  continue;

		msg[0] = '\0';

		for(int i = 0; i < mfrc.uid.size; i++)
		{
			if(mfrc.uid.uidByte[i] < 0x10){
			  	sprintf(msg + strlen(msg), "%s","0");
			  	sprintf(msg + strlen(msg), "%X",mfrc.uid.uidByte[i]);
		  	}
		  	else{
			  	//sprintf(msg + strlen(msg), "%s"," ");
			  	sprintf(msg + strlen(msg), "%X", mfrc.uid.uidByte[i]);
			  }
		}

		syslog(LOG_ERR, "RFID: %s", msg);

		int result = mq_timedsend (msgq_id, msg, strlen(msg)+1, 1, &tm);

		usleep(2000000);

		/* Define default opeartion as search RFID*/
		msgq_id = msgq_id_verpat;
	}

	/**< After the kill command is sent, used resources are terminated, such as mqueue */
	delete_mqueues();
	syslog(LOG_ERR, "%s\n", "Daemon: Closed");
			
	exit(EXIT_SUCCESS);		
}

/**
 * create daemon
 * -> create the dameon
 * @return -
 */
void create_daemon(){
	/*** Deamon ***/
	pid_t pid, sid;

	pid = fork(); 			// create a new process

	if (pid < 0) 			// on error exit
	{ 
		syslog(LOG_ERR, "%s\n", "fork");
		exit(EXIT_FAILURE);
	}

	if (pid > 0)
	{  
		printf("Deamon PID: %d\n", pid);	
		exit(EXIT_SUCCESS); // parent process (exit)
	}
	
	sid = setsid(); 		// create a new session
	if (sid < 0) 			// on error exit
	{ 
		syslog(LOG_ERR, "%s\n", "setsid");
		exit(EXIT_FAILURE);
	}
	
	// make '/' the root directory
	if (chdir("/") < 0) 	// on error exit
	{ 
		syslog(LOG_ERR, "%s\n", "chdir");
		exit(EXIT_FAILURE);
	}
	umask(0);
	close(STDIN_FILENO);  // close standard input file descriptor
	close(STDOUT_FILENO); // close standard output file descriptor
	close(STDERR_FILENO); // close standard error file descriptor
}

/**
 * create mqueues
 * -> create the two mqueues 
 * @return -
 */
void create_mqueues(){

	struct mq_attr attr;

	attr.mq_flags = 0;
    attr.mq_maxmsg = 1;
    attr.mq_msgsize = 10;
    attr.mq_curmsgs = 0;

	msgq_id_verpat = mq_open(MSGQOBJ_VERPAT, O_RDWR | O_CREAT | O_NONBLOCK, S_IRWXU | S_IRWXG, &attr);

	if (msgq_id_verpat == (mqd_t)-1) {
    	syslog(LOG_ERR, "%s\n", "In mq_open()");
    	mq_close(msgq_id_verpat);
    	exit(1);
	}

	msgq_id_newpat = mq_open(MSGQOBJ_NEWPAT, O_RDWR | O_CREAT | O_NONBLOCK, S_IRWXU | S_IRWXG, &attr);

	if (msgq_id_newpat == (mqd_t)-1) {
    	syslog(LOG_ERR, "%s\n", "In mq_open()");
    	mq_close(msgq_id_newpat);
    	exit(1);
	}

	syslog(LOG_ERR, "msgq_id_newpat %d\n", msgq_id_newpat);
}

/**
 * delete mqueues
 * -> delete both mqueues after kill command
 * @return -
 */
void delete_mqueues(){

	mq_close(msgq_id_verpat);
	if(mq_unlink(MSGQOBJ_VERPAT) == -1)
		syslog(LOG_ERR, "%s\n", "In mq_unlink");

	mq_close(msgq_id_newpat);
	if(mq_unlink(MSGQOBJ_NEWPAT) == -1)
		syslog(LOG_ERR, "%s\n", "In mq_unlink");
}

/**
 * sig_handler
 * -> Handler that picks up system signals 
 * {SIGUSR1 - SIGNAL TO INSERT NEW PATIENT}
 * {SIGTERM - KILL SIGNAL}
 * @return making use of the variables, the state of the next function 
 * to be executed or if it is necessary to shut down the system 
 */
static void sig_handler(int sig){
	switch(sig) 
	{
		case SIGUSR1:
			msgq_id = msgq_id_newpat;
			break;
		case SIGUSR2:
			syslog(LOG_ERR, "SIGUSR2 receive -> SEARCH");
			msgq_id = msgq_id_verpat;
			break;
		case SIGTERM:
			state = 0;
	}
}
