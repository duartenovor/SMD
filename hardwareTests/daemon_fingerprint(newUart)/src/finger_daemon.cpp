/**
* @file finger_daemon.cpp
* @author Joao Miranda and Duarte Rodrigues
* @date 21-01-2021
*
* @brief Process that reads the fingerprint sensor
*/

#include <fcntl.h>	        /**<file descriptor */
#include <mqueue.h>         /**< mq_* functions */
#include <sys/syslog.h>     /**< syslog */
#include <signal.h>         /**< signals */
#include <stdlib.h>         /**< atoi */
#include <errno.h>          /**< errno */

/**< Includes used to Fingerprint use */
#include "uart.h"
#include "Adafruit_Fingerprint.h"

#define MSGQOBJ_VERCAR "/finger_queue_vercar" /**< Message queue used to send the finger searched and matched */
#define MSGQOBJ_NEWCAR "/finger_queue_newcar" /**< Message queue used to send the finger added or error message */

#define MAX_MSG_LEN 	10000 	/**<  max length of a message queue*/

#define SECONDS 5  /**< Time between process (add/remove/search cant have more time than that) */

/**< mqd_t type, which is used for message queue descriptors */
mqd_t msgq_id_vercar;
mqd_t msgq_id_newcar;

/**< Fingerprint sensor variables*/
Adafruit_Fingerprint *fp;
uint16_t readID;
uint16_t last_Inserted;
uint16_t ID;
int timOverflow = 0;    /**< used to signal that has already reached the timeout*/

/**< Variable that indicates the state of program {1->run}*/
int state = 1;

/**<Prototype Functions about init and close tools used*/
void create_daemon();
void create_mqueues();
void delete_mqueues();
void init_fingerprint();

/**<Prototype Functions about execute search, add and remove*/
int addFingerPrint();
int removeFingerPrint();
int searchFingerPrint();

/**< Enum, pointer array to functions search, add and remove*/
enum State {SEARCH, ADD, REMOVE};
int (*func_ptr[3])() = {searchFingerPrint, addFingerPrint, removeFingerPrint};
int optionFP = SEARCH;

/**< Functions that help to change the state of the system */
static void sig_handler(int sig);
void timer(int s, int usec);

/**
 * main funtion
 * -> will be in a loop checking whether to read, insert or remove
 * any fingerprints until a kill command is sent 
 * @return -
 */
int main(int argc, char *argv[])
{	
	signal(SIGUSR1,sig_handler);	/**< SIGNAL TO INSERT NEW CAREGIVER */
	signal(SIGUSR2,sig_handler);	/**< SIGNAL TO REMOVE A CAREGIVER */
    signal(SIGTERM,sig_handler);    /**< catch KILL SIGNAL */

	char msg[64];

	syslog(LOG_ERR, "%s\n","DAEMON!\n");

	create_daemon();
	create_mqueues();

	init_fingerprint();

    /*remove all finger*/
    //fp->emptyDatabase();

    /**< Send PID to main program*/
	sprintf(msg, "%d", (int)getpid());
	mq_send(msgq_id_newcar, msg, strlen(msg)+1, 1);

	syslog(LOG_ERR, "%s\n","lets go Fingerprint!\n");

    optionFP = SEARCH;

	while(state)
	{	
		/**< restart timer*/
        timOverflow = 0;
        timer(SECONDS, 0);
		(*func_ptr[optionFP])();
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
    attr.mq_msgsize = 64;
    attr.mq_curmsgs = 0;

	msgq_id_vercar = mq_open(MSGQOBJ_VERCAR, O_RDWR | O_CREAT | O_NONBLOCK, S_IRWXU | S_IRWXG, &attr);

	if (msgq_id_vercar == (mqd_t)-1) {
    	syslog(LOG_ERR, "%s\n", "In mq_open()");
    	mq_close(msgq_id_vercar);
    	exit(1);
	}

    attr.mq_msgsize = 64;

	msgq_id_newcar = mq_open(MSGQOBJ_NEWCAR, O_RDWR | O_CREAT | O_NONBLOCK, S_IRWXU | S_IRWXG, &attr);

	if (msgq_id_newcar == (mqd_t)-1) {
    	syslog(LOG_ERR, "%s\n", "In mq_open()");
    	mq_close(msgq_id_newcar);
    	exit(1);
	}
}

/**
 * delete mqueues
 * -> delete both mqueues after kill command
 * @return -
 */
void delete_mqueues(){
    mq_close(msgq_id_vercar);
    if(mq_unlink(MSGQOBJ_VERCAR) == -1)
        syslog(LOG_ERR, "%s\n", "In mq_unlink msgq_id_vercar");

    mq_close(msgq_id_newcar);
    if(mq_unlink(MSGQOBJ_NEWCAR) == -1)
        syslog(LOG_ERR, "%s\n", "In mq_unlink msgq_id_newcar");
}

/**
 * init fingerprint
 * -> establish the UART connection that the sensor uses and
 * and verify that it has been successfully established.
 * @return -
 */
void init_fingerprint(){

	char aux [64];
    strcpy(aux,"/dev/serial/by-id/usb-FTDI_FT232R_USB_UART_AB0K40B5-if00-port0");

    fp = new Adafruit_Fingerprint(aux);

    fp->begin();

    if (fp->verifyPassword())
        syslog(LOG_ERR, "Found fingerprint sensor!");
    else
    {
        syslog(LOG_ERR, "Did not Find fingerprint sensor!");
    }

    ID = 1;
}

/**
 * add fingerprint
 * -> add a new fingerprint in the next available position 
 * @return the errors or the id entered 
 */
int addFingerPrint(){

    int error = 0;
    syslog(LOG_ERR, "addFingerPrint");
    struct timespec tm;
    tm.tv_sec = 5;

    char msg[64];
    ID = 1;

    syslog(LOG_ERR, "Place finger");
    sprintf(msg, "Place finger");
    mq_timedsend (msgq_id_newcar, msg, strlen(msg)+1, 1, &tm);

    //search where it can be stored!
    while (fp->loadModel(ID)==FINGERPRINT_OK){
        ID++;
    }

    int p = -1;
    while (p != FINGERPRINT_OK && !timOverflow)
    {
        p = fp->getImage();
        switch (p)
        {
        case FINGERPRINT_OK:
            syslog(LOG_ERR, "Image taken");
            break;
        case FINGERPRINT_NOFINGER:
            break;
        case FINGERPRINT_PACKETRECIEVEERR:
            syslog(LOG_ERR,"Communication error");
            error = 1;
            break;
        case FINGERPRINT_IMAGEFAIL:
            syslog(LOG_ERR,"Imaging error");
            error = 1;
            break;
        default:
            syslog(LOG_ERR,"Unknown error");
            error = 1;
            break;
        }
    }

    if(timOverflow || error){
    	/*error message*/
        syslog(LOG_ERR,"TIMEOUT");
    	sprintf(msg, "ERROR");
		mq_timedsend (msgq_id_newcar, msg, strlen(msg)+1, 1, &tm);
        optionFP = SEARCH;
    	return -1;
    }

    /***************************/
    /*   Convert fingerprint   */
    /***************************/

    // OK success!
    p = fp->image2Tz(1);
    switch (p)
    {
        case FINGERPRINT_OK:
            syslog(LOG_ERR,"Image converted");
            break;
        case FINGERPRINT_IMAGEMESS:
            syslog(LOG_ERR,"Image too messy");
            sprintf(msg, "ERROR");
            mq_timedsend (msgq_id_newcar, msg, strlen(msg)+1, 1, &tm);
            optionFP = SEARCH;
            return p;
        case FINGERPRINT_PACKETRECIEVEERR:
            syslog(LOG_ERR,"Communication error");
            sprintf(msg, "ERROR");
            mq_timedsend (msgq_id_newcar, msg, strlen(msg)+1, 1, &tm);
            optionFP = SEARCH;
            return p;
        case FINGERPRINT_FEATUREFAIL:
            syslog(LOG_ERR,"Could not find fingerprint features");
            sprintf(msg, "ERROR");
            mq_timedsend (msgq_id_newcar, msg, strlen(msg)+1, 1, &tm);
            optionFP = SEARCH;
            return p;
        case FINGERPRINT_INVALIDIMAGE:
            syslog(LOG_ERR,"Could not find fingerprint features");
            sprintf(msg, "ERROR");
            mq_timedsend (msgq_id_newcar, msg, strlen(msg)+1, 1, &tm);
            optionFP = SEARCH;
            return p;
        default:
            syslog(LOG_ERR,"Unknown error");
            sprintf(msg, "ERROR");
            mq_timedsend (msgq_id_newcar, msg, strlen(msg)+1, 1, &tm);
            optionFP = SEARCH;
            return p;
    }

    timer(SECONDS, 0);

    syslog(LOG_ERR, "Remove finger");
    sprintf(msg, "Remove finger");
    mq_timedsend (msgq_id_newcar, msg, strlen(msg)+1, 1, &tm);
    p = 0;
    while (p != FINGERPRINT_NOFINGER && !timOverflow) {
        p = fp->getImage();
    }

    if(timOverflow){
        /*error message*/
        syslog(LOG_ERR,"TIMEOUT");
        sprintf(msg, "ERROR");
        mq_timedsend (msgq_id_newcar, msg, strlen(msg)+1, 1, &tm);
        optionFP = SEARCH;
        return -1;
    }

    timer(SECONDS, 0);

    syslog(LOG_ERR, "Place same finger again");
    sprintf(msg, "Place same finger again");
    mq_timedsend (msgq_id_newcar, msg, strlen(msg)+1, 1, &tm);


    /***************************/
    /*    Reread fingerprint   */
    /***************************/
    while (p != FINGERPRINT_OK && !timOverflow)
    {
        p = fp->getImage();
        switch (p)
        {
        case FINGERPRINT_OK:
            syslog(LOG_ERR,"Image taken");
            break;
        case FINGERPRINT_NOFINGER:
            break;
        case FINGERPRINT_PACKETRECIEVEERR:
            syslog(LOG_ERR,"Communication error");
            error = 1;;
            break;
        case FINGERPRINT_IMAGEFAIL:
            syslog(LOG_ERR,"Imaging error");
            error = 1;
            break;
        default:
            syslog(LOG_ERR,"Unknown error");
            error = 1;
            break;
        }
    }

    if(timOverflow || error)
    {
    	/*error message*/
        syslog(LOG_ERR,"TIMEOUT");
    	sprintf(msg, "ERROR");
		mq_timedsend (msgq_id_newcar, msg, strlen(msg)+1, 1, &tm);
        optionFP = SEARCH;
    	return -1;
    }

    /***************************/
    /*   Convert fingerprint   */
    /***************************/
    // OK success!
    p = fp->image2Tz(2);
    switch (p)
    {
    case FINGERPRINT_OK:
        syslog(LOG_ERR,"Image converted");
        break;
    case FINGERPRINT_IMAGEMESS:
        syslog(LOG_ERR,"Image too messy");
        sprintf(msg, "ERROR");
        mq_timedsend (msgq_id_newcar, msg, strlen(msg)+1, 1, &tm);
        optionFP = SEARCH;
        return p;
    case FINGERPRINT_PACKETRECIEVEERR:
        syslog(LOG_ERR,"Communication error");
        sprintf(msg, "ERROR");
        mq_timedsend (msgq_id_newcar, msg, strlen(msg)+1, 1, &tm);
        optionFP = SEARCH;
        return p;
    case FINGERPRINT_FEATUREFAIL:
        syslog(LOG_ERR,"Could not find fingerprint features");
        sprintf(msg, "ERROR");
        mq_timedsend (msgq_id_newcar, msg, strlen(msg)+1, 1, &tm);
        optionFP = SEARCH;
        return p;
    case FINGERPRINT_INVALIDIMAGE:
        syslog(LOG_ERR,"Could not find fingerprint features");
        sprintf(msg, "ERROR");
        mq_timedsend (msgq_id_newcar, msg, strlen(msg)+1, 1, &tm);
        optionFP = SEARCH;
        return p;
    default:
        syslog(LOG_ERR,"Unknown error");
        sprintf(msg, "ERROR");
        mq_timedsend (msgq_id_newcar, msg, strlen(msg)+1, 1, &tm);
        optionFP = SEARCH;
        return p;
    }

    // OK converted!
    //printf("Creating model for #%d\n", ID);

    /***************************/
    /*  Check if fingers match */
    /***************************/

    p = fp->createModel();
    if (p == FINGERPRINT_OK)
    {
        syslog(LOG_ERR,"Prints matched!");
    }
    else if (p == FINGERPRINT_PACKETRECIEVEERR)
    {
        syslog(LOG_ERR,"Communication error");
        sprintf(msg, "ERROR");
        mq_timedsend (msgq_id_newcar, msg, strlen(msg)+1, 1, &tm);
        optionFP = SEARCH;
        return p;
    }
    else if (p == FINGERPRINT_ENROLLMISMATCH)
    {
        syslog(LOG_ERR,"Fingerprints did not match");
        sprintf(msg, "ERROR");
        mq_timedsend (msgq_id_newcar, msg, strlen(msg)+1, 1, &tm);
        optionFP = SEARCH;
        return p;
    }
    else
    {
        syslog(LOG_ERR,"Unknown error");
        sprintf(msg, "ERROR");
        mq_timedsend (msgq_id_newcar, msg, strlen(msg)+1, 1, &tm);
        optionFP = SEARCH;
        return p;
    }

    /***************************/
    /*       Store Model       */
    /***************************/
    if(p == FINGERPRINT_OK)
    {
        p = fp->storeModel(ID);
        if (p == FINGERPRINT_OK)
        {
            syslog(LOG_ERR,"Stored in ID: %d!\n",ID);
            /*fp->getTemplateCount();
            syslog(LOG_ERR, "Num Templates: %d", fp->templateCount);*/
            /*MQUEUE SEND ID INSERTED*/
            sprintf(msg, "SUCCESS %d",ID);
            mq_timedsend (msgq_id_newcar, msg, strlen(msg)+1, 1, &tm);
            optionFP = SEARCH;
            /*time to retire the finger*/
            usleep(1000000);
            return p;
        }
        else if (p == FINGERPRINT_PACKETRECIEVEERR)
        {
            syslog(LOG_ERR,"Communication error");
            sprintf(msg, "ERROR");
            mq_timedsend (msgq_id_newcar, msg, strlen(msg)+1, 1, &tm);
            optionFP = SEARCH;
            return p;
        }
        else if (p == FINGERPRINT_BADLOCATION)
        {   
            syslog(LOG_ERR,"Could not store in that location");
            sprintf(msg, "ERROR");
            mq_timedsend (msgq_id_newcar, msg, strlen(msg)+1, 1, &tm);
            optionFP = SEARCH;
            return p;
        }
        else if (p == FINGERPRINT_FLASHERR)
        {
            syslog(LOG_ERR,"Error writing to flash");
            sprintf(msg, "ERROR");
            mq_timedsend (msgq_id_newcar, msg, strlen(msg)+1, 1, &tm);
            optionFP = SEARCH;
            return p;
        }
        else
        {
            syslog(LOG_ERR,"Unknown error");
            sprintf(msg, "ERROR");
            mq_timedsend (msgq_id_newcar, msg, strlen(msg)+1, 1, &tm);
            optionFP = SEARCH;
            return p;
        }
    }
    return 0;
}

/**
 * remove fingerprint
 * -> delete delete a specific fingerprint based on the id received by mqueue 
 * @return the errors or success
 */
int removeFingerPrint(){
	int removeID = 0;
	char msg[64];
	char buffer[MAX_MSG_LEN];

	while(!removeID && !timOverflow)
	{
		/*mq_receive*/
		if(mq_receive(msgq_id_newcar, buffer, MAX_MSG_LEN, NULL) == -1)
		{
			// get error from errno
			int err = errno;

			// is the queue empty?
			if(err == EAGAIN)
				continue;	// no more to read

			perror("In mq_receive()");
			mq_close(msgq_id_newcar);
			exit(1);
		}
		removeID = atoi(buffer);
	}

	if(timOverflow)
	{
		/*error message*/
    	sprintf(msg, "ERROR");
		mq_send(msgq_id_newcar, msg, strlen(msg)+1, 1);
        optionFP = SEARCH;
    	return -1;
	}

    if(fp->deleteModel(removeID) == FINGERPRINT_OK)
        sprintf(msg, "SUCCESS");
    else
        sprintf(msg, "ERROR");

    /*fp->getTemplateCount();
    syslog(LOG_ERR, "Num Templates: %d", fp->templateCount);*/
        
    syslog(LOG_ERR, "%s: %d", msg, removeID);
    mq_send(msgq_id_newcar, msg, strlen(msg)+1, 1);

    optionFP = SEARCH;
    /*time to retire the finger*/
    usleep(1000000);

    return 0;
}

/**
 * search fingerprint
 * -> function that will be running most of the time. 
 * Searches based on the finger stuck to the sensor, if any are saved. 
 * @return the errors or success
 */
int searchFingerPrint(){

    /***************************/
    /*     Read fingerprint    */
    /***************************/
    struct timespec tm;
    tm.tv_sec = 1;
    int p = -1;
    char msg[64];

    while (p != FINGERPRINT_OK && !timOverflow)
    {
        p = fp->getImage();
    }

    if(timOverflow)
    	return -1;

    timer(SECONDS, 0);

    p = fp->image2Tz(1);

    int found = fp->fingerFastSearch();

    if(found == FINGERPRINT_OK)
    {   
    	if(fp->confidence>100)
    	{   
            syslog(LOG_ERR,"Fingerprint found, ID: %d", fp->fingerID);
    		sprintf(msg, "%d", fp->fingerID);
            mq_timedsend(msgq_id_vercar, msg, strlen(msg)+1, 1, &tm);
		}
        return 1;
    }
    else if(found == FINGERPRINT_NOTFOUND)
    {
        syslog(LOG_ERR,"Finger not found!\n\n");
        return -1;
    }
    else if(found == FINGERPRINT_PACKETRECIEVEERR)
    {
        syslog(LOG_ERR,"Communication error!\n\n");
        return -1;
    }
    return 0;
}

/**
 * sig_handler
 * -> Handler that picks up system signals 
 * {SIGUSR1 - SIGNAL TO INSERT NEW CAREGIVER}
 * {SIGUSR2 - SIGNAL TO REMOVE A CAREGIVER}
 * {SIGALRM - TIMEOUT}
 * {SIGTERM - KILL SIGNAL}
 * @return making use of the variables, the state of the next function 
 * to be executed or if it is necessary to shut down the system 
 */
static void sig_handler(int sig){
	switch(sig) 
	{
		case SIGUSR1:
			/* SIGNAL TO INSERT NEW CAREGIVER */
            timer(0, 5);
			optionFP = ADD;
			break;
		case SIGUSR2:
			/* SIGNAL TO REMOVE A CAREGIVER */
            timer(0, 5);
			optionFP = REMOVE;
			break;
		case SIGALRM:
			timOverflow = 1;
            timer(SECONDS, 0);
			break;
        case SIGTERM:
            timer(0, 5);
            state = 0;
            break;
	}
}

/**
 * timer
 * -> Timer used to generate the timeout between runs. 
 * It can be recharged in seconds or microseconds. 
 * As soon as the timer triggers, the SIGALRM signal will be activated. 
 * @return 
 */
void timer(int s, int usec){
	struct itimerval itv;

	// set handling of SIGALRM
	signal(SIGALRM,sig_handler);

	// period between successive timer interrupts
	itv.it_interval.tv_sec = 0;
	itv.it_interval.tv_usec = 0;

	// period between now and the first timer interrupt
	itv.it_value.tv_sec = s;
	itv.it_value.tv_usec = usec;
	setitimer (ITIMER_REAL, &itv, NULL);
}