#include "../inc/smd.h"
#include "../inc/auxiliaries.h"

#include <QDebug>
 #include <syslog.h>

#include <pthread.h>
#include <sched.h>
#include <unistd.h>
#include <time.h>           /*localtime*/
#include <signal.h>
#include <errno.h>
#include <string>
#include <sstream>
#include <sys/stat.h>
#include <iostream>
#include <vector>
#include <algorithm>
#include <list>
#include <fcntl.h>           /*Device Driver Buzzer*/
#include <iomanip>

#define TIMER1 1
#define TIMER2 2
#define TIMEOUT 3
#define TIMEFINGER 4

#define PIN1 RPI_GPIO_P1_12 //GPIO 18 (PIN 12)
#define PIN2 RPI_V2_GPIO_P1_35 //GPIO 35 (PIN 19)
#define PWM_CHANNEL0 0
#define PWM_CHANNEL1 1

/**< Message Queues */
#define MSGQOBJ_VERPAT "/rfid_queue_verpat"                     /**< Message queue used to send the RFID searched and matched */
#define MSGQOBJ_NEWPAT "/rfid_queue_newpat"                   /**< Message queue used to send the RFID added or error message */
#define MSGQOBJ_VERCAR "/finger_queue_vercar"                 /**< Message queue used to send the finger searched and matched */
#define MSGQOBJ_NEWCAR "/finger_queue_newcar"              /**< Message queue used to send the finger added or error message */
#define MAX_MSG_LEN 	10000                                               /**< max length of a message queue*/

int timeout = 0;

/**< Condution Variables */
pthread_cond_t timer1_var = PTHREAD_COND_INITIALIZER;
pthread_cond_t timer2_var = PTHREAD_COND_INITIALIZER;
pthread_cond_t rfid_update = PTHREAD_COND_INITIALIZER;
pthread_cond_t finger_update = PTHREAD_COND_INITIALIZER;

/**< Global Variables*/
char BuzzOn = '1';
char BuzzOff = '0';

CSMD::CSMD(QObject *parent)
                : QObject(parent)
{
     qDebug("CSMD Constructor");

     caregiverON=0;

     /**< Open file descriptor to write on buzzer */
     //system("insmod /etc/buzz.ko");
     file_descriptor_buzzer = open("/dev/buzzer0", O_WRONLY);

     /**< Open file descriptor to write on pwm driver */
     //system("insmod /etc/pwmPinDriver01.ko");
     file_descriptor_motors = open("/dev/my_pwm_driver", O_WRONLY);

      /**<Sincronize motors*/
     write(file_descriptor_motors, "1u", strlen("1u")+1);
     write(file_descriptor_motors, "2u", strlen("2u")+1);

     if(!time.getNumberElements(db))
     {
          std::cout << "Table Time" << std::endl;
          for(int i = 0; i < 24; i++)
              time.insert(db,(i+1),i,0);
      }

     struct tm actualtime = getTimeDate();
     /**<Get current actual_time*/
     this->actual_time =  time.getID(db,actualtime.tm_hour,0);

     std::cout << "Actual time: " << this->actual_time << std::endl;

      if(!caregiver.getNumberElements(db))
      {
          std::cout << "Table Caregiver" << std::endl;
          caregiver.insert(db);
          caregiver.insert(db);

          caregiver.updatePin(db,1,"0000");

         db.print("Caregiver");
          //caregiver.updatePin(db,2,"2222");
     }

      if(!patient.getNumberElements(db))
      {
          std::cout << "Table Patient" << std::endl;
          patient.insert(db,1);
          patient.insert(db,2);


          patient.updateRFID(db,1,"A31CD604");
          patient.updateRFID(db,2,"03B09105");
     }

      if(!medicine.getNumberElements(db))
      {
          std::cout << "Table Medicine" << std::endl;
          medicine.insert(db,1);
          medicine.insert(db,2);

          medicine.updateQuantity(db,1,5);
          medicine.updateQuantity(db,2,5);
     }

      if(!prescription.getNumberElements(db))
      {
          for(int i = 0; i < 24; i++)
          {
              prescription.insert(db,1,i+1,1,0);
              prescription.insert(db,1,i+1,2,0);
              prescription.insert(db,2,i+1,1,0);
              prescription.insert(db,2,i+1,2,0);
          }
          prescription.updateQuantity(db,1,time.getHour(db,((this->actual_time)+1)),1,1);
          prescription.updateQuantity(db,1,time.getHour(db,((this->actual_time)+1)),2,1);
          prescription.updateQuantity(db,2,time.getHour(db,((this->actual_time)+1)),2,1);

          prescription.updateQuantity(db,1,time.getHour(db,((this->actual_time)+2)),1,3);
          prescription.updateQuantity(db,1,time.getHour(db,((this->actual_time)+2)),2,1);
          prescription.updateQuantity(db,2,time.getHour(db,((this->actual_time)+2)),1,3);
          prescription.updateQuantity(db,2,time.getHour(db,((this->actual_time)+2)),2,1);

          prescription.updateQuantity(db,1,time.getHour(db,((this->actual_time)+3)),1,1);
          prescription.updateQuantity(db,1,time.getHour(db,((this->actual_time)+3)),2,1);
      }

      /**<Pick diference between actual time and next Hour*/
      int seconds = diffSeconds(time.getHour(db,((this->actual_time)+1)),time.getMinute(db,((this->actual_time)+1)));

      std::cout << "Seconds: " << seconds << std::endl;

      create_timer(timerStartMed,TIMER1);
      create_timer(timerTimeout,TIMER2);
      create_timer(timerSensor,TIMEOUT);

      /**<The actual time there are not medication. Just next time*/
      //set_timer(timerStartMed,seconds);

      /**<Debug*/
      set_timer(timerStartMed,1);

      createSemaphores();
      create_mqueues();
      createThreads();

//      db.print("Medicine");
//      db.print("MedicationTime");
        //db.print("Patient");
        //db.print("Prescription");

        //prescription.remove(db,1,1,1);

     //db.print("Prescription");
      //db.print("PatientNotification");
}

CSMD::~CSMD()
{
    qDebug( "CSMD Destructor");
    deleteSemaphores();

    close(file_descriptor_buzzer);
}

void *CSMD::tPatRFIDFunction(void *arg)
{
    CSMD *smd;
    smd = (CSMD *)arg;
    int id;
    char buffer[MAX_MSG_LEN];
    while(1)
    {
        /*Code RFID -> MQueue*/
        if(mq_receive(smd->msgq_id_verpat, buffer, MAX_MSG_LEN, NULL) == -1)
        {
                // get error from errno
                int err = errno;
                // is the queue empty?
                if(err == EAGAIN)
                    continue;	// no more to read

                perror("In mq_receive()");
                mq_close(smd->msgq_id_verpat);
                exit(1);
        }

        std::string rfid(buffer);

        std::cout << "tPatRFIDFunction: new RFID: " << rfid << std::endl;

        if((pthread_mutex_trylock(&(smd->mCaregiverOn))) != 0)
            continue;

        pthread_mutex_lock (&(smd->mPatientdb));
        id = smd->patient.getID(smd->db,rfid);
        pthread_mutex_unlock (&(smd->mPatientdb));


        std::cout << "ID: " << id << std::endl;
        std::cout << "SIZE: " << smd->actualPrescription.size() << std::endl;

        if(id > 0)
        {
            pthread_mutex_lock (&(smd->mPrescriptiondb));
            smd->patientLogged = std::find_if((smd->actualPrescription).cbegin(), (smd->actualPrescription).cend(), [&id] (const prescription_t& s) { return s.pat_ID == id;});

           /**< If it is not clear (SO, THIS PATIENT NEED TO TAKE MEDICATION) */
           if (smd->patientLogged != smd->actualPrescription.cend())
           {
                std::cout << "This patient need to take medication" << std::endl;
                std::cout << "patientLogged ID: " << smd->patientLogged->pat_ID << std::endl;
                std::cout << "patientLogged MED SIZE: " << smd->patientLogged->med_ID.size() << std::endl;

               sem_post(&(smd->sSupplyMedication));
           }

           pthread_mutex_unlock (&(smd->mPrescriptiondb));
        }

        pthread_mutex_unlock (&(smd->mCaregiverOn));

    }
    pthread_exit(nullptr);
}

void *CSMD::tCarFingerFunction(void *arg)
{
    CSMD *smd;
    smd = (CSMD *)arg;

    char buffer[MAX_MSG_LEN];
    int fingerID = 0;
    int id;

    /*Code FINGERPRINT*/
    while(1)
    {
        if(mq_receive(smd->msgq_id_vercar, buffer, MAX_MSG_LEN, NULL) == -1)
        {
            // get error from errno
            int err = errno;

            // is the queue empty?
            if(err == EAGAIN)
                continue;	// no more to read

            perror("In mq_receive()");
            mq_close(smd->msgq_id_vercar);
            exit(1);
        }

        //else, receive some value
        fingerID = atoi(buffer);
        std::cout << "tCarFingerFunction: new FINGER: " << fingerID << std::endl;

        if((pthread_mutex_trylock(&(smd->mCaregiverOn))) != 0)
            continue;

        std::cout << "Entrei tCarFingerFunction" << std::endl;

        pthread_mutex_lock (&(smd->mCaregiverdb));
        id = smd->caregiver.getID(smd->db,fingerID);
        pthread_mutex_unlock (&(smd->mCaregiverdb));

        std::cout << id << std::endl;

        if(id > 0)
        {
            smd->caregiverON = 1;
            pthread_mutex_unlock (&(smd->mCaregiverOn));
            emit smd->setCaregiverID(id);
            emit smd->sendToQml("mainMenuPage.qml");
            sem_post(&(smd->sCaregiver));
        }
        else
            pthread_mutex_unlock (&(smd->mCaregiverOn));

        std::cout << "Sai tCarFingerFunction" << std::endl;
    }
    pthread_exit(nullptr);
}

void *CSMD::tCaregiverFunction(void *arg)
{
    CSMD *smd;
    smd = (CSMD *)arg;

    while(1)
    {
        sem_wait(&(smd->sCaregiver));
        sem_post(&(smd->sBuzzer));
        pthread_mutex_lock (&(smd->mCaregiverOn));
        std::cout << "tCaregiverFunction entra" << std::endl;
        while(smd->caregiverON) {
            usleep(10);
        }
        std::cout << "tCaregiverFunction sai" << std::endl;
        pthread_mutex_unlock (&(smd->mCaregiverOn));
       sem_post(&(smd->sBuzzer));
    }
    pthread_exit(nullptr);
}

void *CSMD::tStartMedicationFunction(void *arg)
{
    CSMD *smd;
    smd = (CSMD *)arg;

    std::list <int> vect_id;
    std::list <int> listMedQuant;
    prescription_t p;

    while(1)
    {
        pthread_mutex_lock (&(smd->timer1_lock));                                 // ensure we have exclusive access to whathever comprises the condition
        pthread_cond_wait (&timer1_var,&(smd->timer1_lock));               // Wakeup at least one of the threads that are waiting on the condition (if any)
        pthread_mutex_unlock (&(smd->timer1_lock));                             // allow others to proceed

        std::cout <<"Entrei tStartMedicationFunction"<<std::endl;

        pthread_mutex_lock (&(smd->mPrescriptiondb));
        pthread_mutex_lock (&(smd->mPatientdb));

        std::cout << smd->actual_time << std::endl;
        std::cout << smd->actual_time << std::endl;
        syslog(LOG_ERR, "Actual time id: %d\n",smd->actual_time);

        smd->actualPrescription.clear();
        vect_id = smd->prescription.getPatient(smd->db,smd->actual_time);

//        for (auto const &i: vect_id) {
//            std::cout << "ID " <<  i << std::endl;
//        }

        std::cout << "SIZE: " <<  vect_id.size() << std::endl;

        for (std::list<int>::iterator it = vect_id.begin(); it != vect_id.end(); it++)
        {
            p.pat_ID = *it;
            usleep(1000000);
            emit smd->setPatients( p.pat_ID);
            std::cout << "emit cpp: " << p.pat_ID << std::endl;

            std::string med_quantity = smd->prescription.getQuantityMedicine(smd->db,p.pat_ID,smd->actual_time);
            string_to_vector(med_quantity, listMedQuant);
            std::list<int>::iterator it1 = listMedQuant.begin();
            while(it1 != listMedQuant.end())
            {
                p.med_ID.push_back(*it1);
                it1++;
                p.med_quatity.push_back(*it1);
                it1++;
            }
            smd->actualPrescription.push_back(p);
            p.med_ID.clear();
            p.pat_ID = 0;
            p.med_quatity.clear();
            listMedQuant.clear();
        }

        pthread_mutex_unlock (&(smd->mPatientdb));
        pthread_mutex_unlock (&(smd->mPrescriptiondb));

        if(smd->actualPrescription.size() != 0)
        {
            std::cout << "sem_post" << std::endl;
            sem_post(&(smd->sBuzzer));
        }

//        pthread_mutex_lock (&(smd->mTimedb));
//        int seconds = diffSeconds(smd->time.getHour(smd->db,((smd->actual_time)+1)),smd->time.getMinute(smd->db,((smd->actual_time)+1)));
//        pthread_mutex_unlock (&(smd->mTimedb));

        //smd->create_timer(seconds,TIMER1);
        //smd->create_timer((seconds+1800),TIMER2);
        //smd->create_timer(10,TIMER2);

        /**< Only Test (Debug)*/
        smd->set_timer(smd->timerStartMed, 60); /**< 1 min */
        smd->set_timer(smd->timerTimeout, 30);  /**< 30 sec */
    }
    pthread_exit(nullptr);
}

void *CSMD::tTimeoutFunction(void *arg)
{
    CSMD *smd;
    smd = (CSMD *)arg;
    while(1)
    {
        pthread_mutex_lock (&(smd->timer2_lock));                                  // ensure we have exclusive access to whathever comprises the condition
        pthread_cond_wait (&timer2_var,&(smd->timer2_lock));               // Wakeup at least one of the threads that are waiting on the condition (if any)
        pthread_mutex_unlock (&(smd->timer2_lock));                             // allow others to proceed

         std::cout <<"Entrei tTimeoutFunction"<<std::endl;

        prescription_t p;
         std::list<prescription_t>::iterator it;
        struct tm actualtime = getTimeDate();

        pthread_mutex_lock (&(smd->mPrescriptiondb));

         while(smd->actualPrescription.size())
         {
                it = smd->actualPrescription.begin();
                p = *it;
                emit smd->removePatients(p.pat_ID);
                (*it).med_ID.clear();
                (*it).med_quatity.clear();
                smd->actualPrescription.erase(it);

                pthread_mutex_lock (&(smd->mPatientdb));
                pthread_mutex_lock (&(smd->mTimedb));

                std::list<int>::iterator itMedId;
                std::list<int>::iterator itMedQuantity;
                for (itMedId = p.med_ID.begin(), itMedQuantity = p.med_quatity.begin(); itMedId != p.med_ID.end(); itMedId++,  itMedQuantity++)
                {
                    smd->patNot.insert(smd->db,p.pat_ID,*itMedId,*itMedQuantity,smd->actual_time,actualtime.tm_mday,(1+actualtime.tm_mon),(1900+actualtime.tm_year),1);
                }

                p.pat_ID=0;
                p.med_ID.clear();
                p.med_quatity.clear();

                pthread_mutex_unlock (&(smd->mTimedb));
                pthread_mutex_unlock (&(smd->mPatientdb));
         }
         pthread_mutex_unlock (&(smd->mPrescriptiondb));

        smd->actual_time++;
        if(smd->actual_time > 24)
            smd->actual_time = 0;
        std::cout << "sem_post" << std::endl;
        sem_post(&(smd->sBuzzer));
    }
    pthread_exit(nullptr);
}

void *CSMD::tBuzzerFunction(void *arg)
{
    CSMD *smd;
    smd = (CSMD *)arg;

    std::cout << "sem_wait" << std::endl;

    while(1)
    {
        sem_wait(&(smd->sBuzzer));
        std::cout <<"Entrei tBuzzerFunction"<<std::endl;

        pthread_mutex_lock (&(smd->mPrescriptiondb));

        int nPatient = smd->actualPrescription.size();

        pthread_mutex_unlock (&(smd->mPrescriptiondb));

        if(nPatient==0 || smd->caregiverON)
        {
            write(smd->file_descriptor_buzzer, &BuzzOff, 1);
            std::cout << "Buzzer off" << std::endl;
        }
        else
        {
            write(smd->file_descriptor_buzzer, &BuzzOn, 1);
            std::cout << "Buzzer on" << std::endl;
        }
    }
}

void *CSMD::tSupplyMedicationFunction(void *arg)
{
    CSMD *smd;
    smd = (CSMD *)arg;
    prescription_t p;
    std::list<int>::iterator it;
    int pastTime = smd->actual_time;
    int medicineID = 0;
    int quantity = 0;
    int stock = 0;
    char servo[4];

    while(1)
    {
        sem_wait(&(smd->sSupplyMedication));
        struct tm actualtime = getTimeDate();

        std::cout <<"Entrei tSupplyMedicationFunction"<<std::endl;

        pastTime = smd->actual_time;
        pthread_mutex_lock (&(smd->mPrescriptiondb));
        p = *(smd->patientLogged);
        smd->actualPrescription.erase(smd->patientLogged);
        pthread_mutex_unlock (&(smd->mPrescriptiondb));

       emit smd->removePatients(p.pat_ID);

        while(p.med_ID.size())
        {
            it = p.med_ID.begin();
            medicineID = *it;
            p.med_ID.erase(it);
            it = p.med_quatity.begin();
            quantity = *it;
            p.med_quatity.erase(it);

            pthread_mutex_lock(&(smd->mPatientdb));
            stock = smd->medicine.getQuantity(smd->db,medicineID);
            pthread_mutex_unlock(&(smd->mPatientdb));

            while(stock>0 && quantity>0)
            {
                sprintf(servo,"%dd",medicineID);
                write(smd->file_descriptor_motors,servo, strlen(servo)+1);
                usleep(1000000);
                sprintf(servo,"%du",medicineID);
                write(smd->file_descriptor_motors,servo, strlen(servo)+1);
                usleep(1000000);

                quantity--;
                stock--;
            }

            if(quantity>0)
            {
                pthread_mutex_lock (&(smd->mPatientdb));
                pthread_mutex_lock (&(smd->mTimedb));
                smd->patNot.insert(smd->db,p.pat_ID,medicineID,quantity,pastTime,actualtime.tm_mday,(1+actualtime.tm_mon),(1900+actualtime.tm_year),0);
                pthread_mutex_unlock (&(smd->mTimedb));
                pthread_mutex_unlock (&(smd->mPatientdb));
            }

            pthread_mutex_lock(&(smd->mPatientdb));
            smd->medicine.updateQuantity(smd->db,medicineID,stock);
            pthread_mutex_unlock(&(smd->mPatientdb));
        }
        std::cout << "sem_post" << std::endl;
        sem_post(&(smd->sBuzzer));

    }
    pthread_exit(nullptr);
}

void CSMD::createThreads()
{
    /*create Threads*/
    //Create thread attribute structure
    pthread_attr_t tattr;
    //Create struct to establish priority
    sched_param param;
    //Initialize thread attribute structure
    pthread_attr_init(&tattr);
    /* safe to get existing scheduling param */
    pthread_attr_getschedparam (&tattr, &param);
    //Set the scheduling policy to round robin
    pthread_attr_setschedpolicy(&tattr, SCHED_RR);
    //Set to joinable
    pthread_attr_setdetachstate(&tattr, PTHREAD_CREATE_JOINABLE);
//   param.sched_priority=50;
//  pthread_attr_setschedparam(&tattr, &param);

    pthread_create(&tFingerQML, NULL, tFingerQMLFunction, this);
    pthread_create(&tRfidQML, NULL, tRfidQMLFunction, this);

    pthread_create(&tSupplyMedication, NULL, tSupplyMedicationFunction, this);
    pthread_create(&tTimeout, NULL, tTimeoutFunction, this);
    pthread_create(&tBuzzer, NULL, tBuzzerFunction, this);

    pthread_create(&tCaregiver, NULL, tCaregiverFunction, this);
    pthread_create(&tCarFinger, NULL, tCarFingerFunction, this);
    pthread_create(&tPatRFID, NULL, tPatRFIDFunction, this);

    pthread_create(&tStartMedication, NULL, tStartMedicationFunction, this);
}

void CSMD::createSemaphores()
{
    /*create Semaphores*/
    /* initialize a private semaphore */
    int pshared = 0;
    int value = 0;
    int ret = sem_init(&sBuzzer, pshared, value);
    if(ret!=0)
        std::cout << "Error creating 'sBuzzer' Semaphore";
    ret = sem_init(&sSupplyMedication, pshared, value);
    if(ret!=0)
        std::cout << "Error creating 'sSupplyMedication' Semaphore";
    ret = sem_init(&sCaregiver, pshared, value);
    if(ret!=0)
        std::cout << "Error creating 'sCaregiver' Semaphore";

     std::cout << "Init Semaphore" << std::endl;
}

void CSMD::deleteSemaphores()
{
     sem_destroy(&sBuzzer);
     sem_destroy(&sSupplyMedication);
}

void CSMD::create_mqueues()
{
    char buffer[MAX_MSG_LEN];

    msgq_id_verpat = mq_open(MSGQOBJ_VERPAT, O_RDWR | O_NONBLOCK);
        if (msgq_id_verpat == (mqd_t)-1) {
            std::cout << "In mq_open()";
            mq_close(msgq_id_verpat);
            exit(1);
        }

    msgq_id_newpat = mq_open(MSGQOBJ_NEWPAT, O_RDWR | O_NONBLOCK);
        if (msgq_id_newpat == (mqd_t)-1) {
            std::cout << "In mq_open()";
            mq_close(msgq_id_newpat);
            exit(1);
        }

    pid_rfid = 0;
    while(!pid_rfid)
    {
            if(mq_receive(msgq_id_newpat, buffer, MAX_MSG_LEN, NULL) == -1)
            {
                // get error from errno
                int err = errno;

                // is the queue empty?
                if(err == EAGAIN)
                    continue;	// no more to read

                perror("In mq_receive()");
                mq_close(msgq_id_newpat);
                exit(1);
            }

            pid_rfid = atoi(buffer);
            std::cout << "PID RFID: " << pid_rfid  << std::endl;
    }

    msgq_id_vercar = mq_open(MSGQOBJ_VERCAR, O_RDWR | O_NONBLOCK);
        if (msgq_id_vercar == (mqd_t)-1) {
            perror("In mq_open()");
            mq_close(msgq_id_vercar);
            exit(1);
         }

    msgq_id_newcar = mq_open(MSGQOBJ_NEWCAR, O_RDWR | O_NONBLOCK);
        if (msgq_id_newcar == (mqd_t)-1) {
            perror("In mq_open()");
            mq_close(msgq_id_newcar);
            exit(1);
        }

    pid_finger = 0;
    while(!pid_finger)
    {
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

            pid_finger = atoi(buffer);
            std::cout << "PID FINGERPRINT: " << pid_finger  << std::endl;
    }
}

void CSMD::thread_handler(union sigval arg)
{
  switch (arg.sival_int)
  {
      case TIMER1:
          pthread_cond_signal(&timer1_var);
        break;
      case TIMER2:
          pthread_cond_signal(&timer2_var);
        break;
      case TIMEOUT:
           timeout = 1;
         break;
  }
}

void CSMD::create_timer(timer_t &timer_id, int sival_int)
{
  struct sigevent se;
  int status;
  /*
   * Set the sigevent structure to cause the signal to be
   * delivered by creating a new thread.
   */
  se.sigev_notify = SIGEV_THREAD;
  se.sigev_value.sival_ptr = timer_id;
  se.sigev_notify_function = thread_handler;
  se.sigev_notify_attributes = NULL;
  se.sigev_value.sival_int = sival_int;

  status = timer_create(CLOCK_REALTIME, &se, &timer_id);
  if (status == -1)
  {
      qCritical("Error: Create timer");
      exit(EXIT_FAILURE);
  }
}

void CSMD::set_timer(timer_t timer_id, unsigned i)
{
    struct itimerspec ts;
    int status;

    ts.it_value.tv_sec = i;
    ts.it_value.tv_nsec = 0;
    ts.it_interval.tv_sec = 0;
    ts.it_interval.tv_nsec = 0;

    status = timer_settime(timer_id, 0, &ts, 0);
    if (status == -1)
    {
         qCritical("Error: Set timer");
         exit(EXIT_FAILURE);
    }
 }

void CSMD::supplyMedicationDecision(int id)
{
    pthread_mutex_lock (&(mPrescriptiondb));
    patientLogged = std::find_if((actualPrescription).cbegin(), (actualPrescription).cend(), [&id] (const prescription_t& s) { return s.pat_ID == id;});

   /**< If it is not clear (SO, THIS PATIENT NEED TO TAKE MEDICATION) */
   if (patientLogged != actualPrescription.cend())
   {
        std::cout << "This patient need to take medication" << std::endl;
        std::cout << "patientLogged ID: " << patientLogged->pat_ID << std::endl;
        std::cout << "patientLogged MED SIZE: " << patientLogged->med_ID.size() << std::endl;

       sem_post(&sSupplyMedication);
   }

   pthread_mutex_unlock (&mPrescriptiondb);
}

void CSMD::setCaregiverState(bool state)
{
    std::cout << "caregiver off" << std::endl;
    caregiverON = state;
    std::cout << caregiverON << std::endl;
}

/**
 * function checkCaregiver
 * Function used to check if the caregiver inserted the data was correct (compare with database)
 * @return -
 */
int CSMD::checkCaregiver(int id, QString pin)
{
    qDebug() << "Check Caregiver";
    qDebug() << id << pin;
    int idDB;

    /**< Take the id related with */
    pthread_mutex_lock (&mCaregiverdb);
    idDB = caregiver.getID(db,pin.toStdString());
    pthread_mutex_unlock (&mCaregiverdb);

     caregiverON = (idDB == id);
     std::cout << "Caregiver On " << caregiverON << std::endl;

     if (caregiverON)
         sem_post(&sCaregiver);

     return caregiverON;
}

/**
 * function updatePin
 * This function is called to update the pin on Database (Table Caregiver)
 * @return -
 */
void CSMD::updatePin(int id, QString pin)
{
    qDebug() << "updatePin " << id << " " << pin;
    pthread_mutex_lock (&mCaregiverdb);
    if(caregiver.getID(db,pin.toStdString())==-1)
    {
        caregiver.updatePin(db, id, pin.toStdString());
        emit sendText("Sucess:\nPIN Update Successfully");
    }
    else
        emit sendText("Error:\nCannot Insert");
    pthread_mutex_unlock (&mCaregiverdb);

    emit showBack();
}

/**
 * function getMedicineQuantity
 * This function is called to update the information displayed on Screen
 * @return actual quantity of medicine based on IDMedicine
 */
int CSMD::getMedicineQuantity(int id)
{
    int quantity;

    pthread_mutex_lock (&mPrescriptiondb);
    quantity = medicine.getQuantity(db, id);
    pthread_mutex_unlock (&mPrescriptiondb);

    return quantity;
}

/**
 * function updateMedicineQuantity
 * When the caregiver click on "+" or "-" the quantity will be updated on database (table medicine),
 * based on signal and idMedicine
 * @return actual quantity of medicine
 */
int  CSMD::updateMedicineQuantity(int id, QString signal)
{
    int quantity;
    pthread_mutex_lock (&mPrescriptiondb);
    medicine.updateQuantity(db, id, 1, signal.toStdString());
    quantity = medicine.getQuantity(db, id);
    pthread_mutex_unlock (&mPrescriptiondb);

    return quantity;
}

/**
 * function getMedicinePrescription
 * This function is called to update the information displayed on Screen
 * @return actual quantity of medicine,patient and time desire
 */
int CSMD::getMedicinePrescription(int idPat, int idMed, int idTime)
{
    int quantity;

    pthread_mutex_lock (&mPrescriptiondb);
    quantity = prescription.getQuantity(db, idPat, idTime, idMed);
    pthread_mutex_unlock (&mPrescriptiondb);

    return quantity;
}

/**
 * updatePrescription
 * When the caregiver click on "+" or "-" the quantity will be updated on database (table prescription),
 * based on idPat, idMed, idTime and signal
 * @return actual quantity of medicine,patient and time desire
 */
int CSMD::updatePrescription(int idPat, int idMed, int idTime, QString signal)
{
    int quantity;
    pthread_mutex_lock (&mPrescriptiondb);
    prescription.updateQuantity(db, idPat, idTime, idMed, 1, signal.toStdString());
    quantity = prescription.getQuantity(db, idPat, idTime, idMed);
    pthread_mutex_unlock (&mPrescriptiondb);

    return quantity;
}

/**
 * removePatient
 * remove a information about the patient on database and in atual prescritpion list
 * @return -
 */
void CSMD::removePatient(int id)
{
    qDebug() << "Remove Patient: " << id;

    emit removePatients(id);

    /**< Remove data from Table Patient*/
    pthread_mutex_lock (&mPatientdb);
    patient.updateRFID(db,id,"NULL");
    patient.updateName(db,id,"NULL");
    pthread_mutex_unlock (&mPatientdb);

     /**< Remove all prescriptions from Table Prescription*/
    for(int i = 0; i < 24; i++)
    {
        pthread_mutex_lock (&mPrescriptiondb);
        pthread_mutex_lock (&mPatientdb);
        prescription.updateQuantity(db,id,i+1,1,0);
        prescription.updateQuantity(db,id,i+1,2,0);
        pthread_mutex_unlock (&mPatientdb);
        pthread_mutex_unlock (&mPrescriptiondb);
    }
    qDebug() << "Search: ";

    qDebug() << "actualPrescription.size(): "  << actualPrescription.size();

     /**< Remove information from vactor of atual prescritpion*/
    pthread_mutex_lock (&mPrescriptiondb);
    for(std::list<prescription_t>::iterator it = actualPrescription.begin(); it != actualPrescription.end(); it++)
    {
        if((*it).pat_ID == id)          /**< if Patient to remove exists on list to take medication, he is removed*/
        {
            (*it).med_ID.clear();
            (*it).med_quatity.clear();
            actualPrescription.erase(it);
            break;
        }
    }
    pthread_mutex_unlock (&mPrescriptiondb);

    /**< Remove all notification from database*/
    pthread_mutex_lock (&mPatientdb);
    pthread_mutex_lock (&mTimedb);
    patNot.removePatient(db,id);
    pthread_mutex_unlock (&mTimedb);
    pthread_mutex_unlock (&mPatientdb);

    qDebug() << "actualPrescription.size(): "  << actualPrescription.size();
    emit sendText("Success:\nPatient Successfully Removed");
    emit showBack();
}

/**
 * tRfidQMLFunction
 * will make use of the RFID daemon and return the RFID tag to insert into the database and thus update the patient
 * @return -
 */
void *CSMD::tRfidQMLFunction(void *arg)
{
    CSMD *smd;
    smd = (CSMD *)arg;
    char buffer[MAX_MSG_LEN];
    int id = 0;

     while(1)
     {
         pthread_mutex_lock (&(smd->mRfidUpdate));                                  // ensure we have exclusive access to whathever comprises the condition
         pthread_cond_wait (&rfid_update,&(smd->mRfidUpdate));               // Wakeup at least one of the threads that are waiting on the condition (if any)
         pthread_mutex_unlock (&(smd->mRfidUpdate));                               // allow others to proceed

         /**< Clean Mqeue*/
         mq_receive(smd->msgq_id_newpat, buffer, MAX_MSG_LEN, NULL);

         emit smd->sendText("Waiting New Bracelet");
         kill(smd->pid_rfid, SIGUSR1);
         std::string rfid;
         timeout = 0;
         smd->set_timer(smd->timerSensor,10);                                                 /**< Define TIMEOUT as 10seconds */
         id = -1;
         std::cout << "updateRFID: " << std::endl;
         while(!(rfid.size()) && !timeout)
         {
                 if(mq_receive(smd->msgq_id_newpat, buffer, MAX_MSG_LEN, NULL) == -1)
                 {
                     // get error from errno
                     int err = errno;

                     // is the queue empty?
                     if(err == EAGAIN)
                         continue;	// no more to read

                     perror("In mq_receive()");
                     mq_close(smd->msgq_id_newpat);
                     exit(1);
                 }
                 rfid.append(buffer);
                 std::cout << "RFID: " << rfid << std::endl;
                 pthread_mutex_lock (&(smd->mPatientdb));
                 id = smd->patient.getID(smd->db,rfid);
                 pthread_mutex_unlock (&(smd->mPatientdb));
         }

         /**< Clear Timer */
         smd->set_timer(smd->timerSensor,0);

         if (id == smd->qmlPatientID )                                                                                   /**< Same patient RFID */
                emit smd->sendText("Error:\nTrying Update With Current Patient Bracelet");
         else if(!timeout && id < 1)                                                                                        /**< No RFID on database and time not exceded*/
         {
             pthread_mutex_lock (&(smd->mPatientdb));
             smd->patient.updateRFID(smd->db,smd->qmlPatientID,rfid);
             pthread_mutex_unlock (&(smd->mPatientdb));
             emit smd->sendText("Sucess:\nNew Bracelet Added");
         }
         else if(id >= 1)                                                                                                           /**< other patient with same RFID*/
             emit smd->sendText("Error:\nOther Patient With same Bracelet");
         else                                                                                                                             /**< Time exceded*/
             emit smd->sendText("Error:\nTime Excedded");

         kill(smd->pid_rfid, SIGUSR2);
         emit smd->showBack();
     }
      pthread_exit(nullptr);
}

/**
 * tFingerQMLFunction
 * will make use of the Fingerprint daemon and return the Fingerprint to insert into the database and thus update the caregiver
 * It may be necessary to erase the sensor fingerprint if any.
 * @return -
 */
void *CSMD::tFingerQMLFunction(void *arg)
{
    CSMD *smd;
    smd = (CSMD *)arg;
    char buffer[MAX_MSG_LEN];
    char printBuffer[64];
    int id = -1;
    int fingerRemove = 0;
    bool returnValue = true;

    while(1)
    {
        pthread_mutex_lock (&(smd->mFingerUpdate));                                  // ensure we have exclusive access to whathever comprises the condition
        pthread_cond_wait (&finger_update,&(smd->mFingerUpdate));           // Wakeup at least one of the threads that are waiting on the condition (if any)
        pthread_mutex_unlock (&(smd->mFingerUpdate));                              // allow others to proceed

        std::cout << "tFingerQMLFunction" << std::endl;

        kill(smd->pid_finger, SIGUSR1);
        char *token;

        /**< Search the database if there are any fingerprints saved that need to be deleted from the sensor */
        pthread_mutex_lock (&(smd->mCaregiverdb));
        fingerRemove = smd->caregiver.getFingerprint(smd->db, smd->qmlCaregiverID);
        pthread_mutex_unlock (&(smd->mCaregiverdb));

        std::cout << "Finger to remove: " << fingerRemove << std::endl;

        do
        {
                if(mq_receive(smd->msgq_id_newcar, buffer, MAX_MSG_LEN, NULL) == -1)
                {
                    // get error from errno
                    int err = errno;

                    // is the queue empty?
                    if(err == EAGAIN)
                        continue;	// no more to read

                    perror("In mq_receive()");
                    mq_close(smd->msgq_id_newcar);
                    exit(1);
                }
                strcpy(printBuffer,buffer);
                token = strtok(buffer, " ");
                if(!strcmp(token,"ERROR"))
                    emit smd->sendText("Error:\nFailed To Update Fingerprint");
                else if(!strcmp(token,"SUCCESS"))
                {
                     /**< The received string (on Success) is SUCCESS ID */
                    id = atoi(strtok(NULL, " "));
                    std::cout << "Inserted ID: " << id << std::endl;
                    pthread_mutex_lock (&(smd->mCaregiverdb));
                    smd->caregiver.updateFingerprint(smd->db, smd->qmlCaregiverID, id);
                    pthread_mutex_unlock (&(smd->mCaregiverdb));
                    if(fingerRemove>0)
                        returnValue = smd->removeFromFingerprint(fingerRemove);
                    if(returnValue)
                        emit smd->sendText("Sucess:\nNew Fingerprint Added");
                    else
                        emit smd->sendText("Error:\nFailed To Update Fingerprint");
                }
                else
                    emit smd->sendText(printBuffer);
        }
        while((strcmp(token,"ERROR")!=0 && strcmp(token,"SUCCESS")!=0));

        if(token!=nullptr)
            free(token);
        emit smd->showBack();
    }
    pthread_exit(nullptr);
}

/**
 * updateRFID
 * When the caregiver asks to update the patient's RFID this function triggers the tRfidQMLFunction thread
 * @return -
 */
void CSMD::updateRFID(int id)
 {
     qmlPatientID = id;
     pthread_cond_signal(&rfid_update);
 }

/**
 * updateFingerprint
 * When the caregiver asks to update the own Fingerprint this function triggers the tFingerQMLFunction thread
 * @return -
 */
void CSMD::updateFingerprint(int id)
{
    qmlCaregiverID = id;
     std::cout << "updateFingerprint" << std::endl;
    pthread_cond_signal(&finger_update);
}

/**
 * updateFingerprint
 * Checks if caregiver exists (can login).
 * Therefore, checks if there is any pin stored in that id.
 * @return true (if exists) and false (if not exists)
 */
 bool CSMD::caregiverExists(int id)
{
     return(caregiver.getPin(db,id) != "");
}

 /**
  * removeCaregiver
  * Function that makes the removal of a caregiver. If there is more than one saved, removal is allowed.
  * If the removal is approved, based on the id to remove, it clears the data from the database (idfingerprint to 0 and empty pin).
  * It makes use of the Fingerprint daemon to remove the sensor memory and checks that there were no problems.
  * @return true (remove was success) and false (if remove was not success)
  */
bool CSMD::removeCaregiver(int id)
 {
        std::cout << "removeCaregiver: " << id << std::endl;
        int count = 0;
        int fingerRemove = 0;
        bool returnValue = true;

        /**< Total number of elements | Number of elements not defined */
        pthread_mutex_lock (&(mCaregiverdb));
        count = caregiver.getNumberElements(db) - caregiver.getElementsNDef(db);
        pthread_mutex_unlock (&(mCaregiverdb));

        if(count > 1)                                                                                                 /**< Exist at least 2 caregivers */
        {
            pthread_mutex_lock (&(mCaregiverdb));
            fingerRemove = caregiver.getFingerprint(db, id);
            pthread_mutex_unlock (&(mCaregiverdb));
            std::cout << "Finger to remove: " << fingerRemove << std::endl;
            if(fingerRemove>0)                                                                                  /**< If the caregiver has some fingerprint saved on Database */
                returnValue = removeFromFingerprint(fingerRemove);

            pthread_mutex_lock (&(mCaregiverdb));
            caregiver.updatePin(db,id,"");
            caregiver.updateFingerprint(db, id , 0);
            pthread_mutex_unlock (&(mCaregiverdb));

            if(returnValue)
                emit sendText("Sucess:\nCaregiver Remove Successfully");
            else
                emit sendText("Error:\nCannot Remove Caregiver");

            emit showBack();
            return true;
        }

        emit sendText("Error:\nCannot Remove Caregiver\nJust One Caregiver");
        emit showBack();

        return false;
 }

/**
 * addCaregiver
 * add a new caregiver.
 * For that, update the pin for "0000".
 * Show that information on screen
 * @return -
 */
void CSMD::addCaregiver(int id)
{
     std::cout << "addCaregiver: " << id << std::endl;
     std::string pin = "0000";

     pthread_mutex_lock (&(mCaregiverdb));

     while(caregiver.getID(db,pin) != -1)
     {
         srand(::time(NULL));                                     // Seed the time
         int finalNum = rand()%(9999-0+1)+0;
         std::stringstream ss;
         ss << std::setw(4) << std::setfill('0') << finalNum;
         pin = ss.str();
     }

     caregiver.updatePin(db,id,pin);
     pthread_mutex_unlock (&(mCaregiverdb));

     emit sendText(QString::fromStdString("Success:\nCarregiver " +  std::to_string(id) + " Add successfully with pin " + pin));
     emit showBack();
}

/**
 * removeFromFingerprint
 * When it is necessary to remove information in memory from the fingerprint, this function is called.
 * Send by message queue the id of the fingerprint to be sent and wait for the return after deleting in the daemon.
 * @return true (remove was success) and false (if remove was not success)
 */
bool CSMD::removeFromFingerprint(int fingerprintID)
 {
    kill(pid_finger, SIGUSR2);
    char msg[64];
    char buffer[MAX_MSG_LEN];
    buffer[0] ='\0';
    bool returnValue = false;

    std::cout << "removeFingerInfo" << std::endl;

    sprintf(msg, "%d", fingerprintID);
    mq_send(msgq_id_newcar, msg, strlen(msg)+1, 1);

    /**< Wait for the daemon to receive the message addressed to it and then mqreceive it and not receive it in this process. */
    struct mq_attr mqa;
    do {
        mq_getattr(msgq_id_newcar, &mqa);
    }while(mqa.mq_curmsgs);

    do
    {
            if(mq_receive(msgq_id_newcar, buffer, MAX_MSG_LEN, NULL) == -1)
            {
                // get error from errno
                int err = errno;

                // is the queue empty?
                if(err == EAGAIN)
                    continue;	// no more to read

                perror("In mq_receive()");
                mq_close(msgq_id_newpat);
                exit(1);
            }

            if(!strcmp(buffer,"ERROR"))
                returnValue = false;
            else if(!strcmp(buffer,"SUCCESS"))
                returnValue = true;
    }
    while((strcmp(buffer,"ERROR")!=0 && strcmp(buffer,"SUCCESS")!=0));

    return returnValue;
 }

void CSMD::getNotification(int idPatient)
{
    std::string msg;
    notificationsQML.idNotList.clear();

    pthread_mutex_lock (&mPatientdb);
    pthread_mutex_lock (&mTimedb);

    notificationsQML.idNotList = patNot.getIds(db,idPatient);

    notificationsQML.it = notificationsQML.idNotList.begin();

    emit previousButton(false);

    std::cout << "getNotification: Nots na lista:" << std::endl;
//    for (auto v : notificationsQML.idNotList)
//            std::cout << v << "\n";

    switch(notificationsQML.idNotList.size())
    {
        case 0:
            emit removeButton(false);
            emit nextButton(false);
            emit sendNotification("Error:\nThere aren't Notifications About This Patient");
            break;
        case 1:
            emit nextButton(false);
        default:
            msg = patNot.get(db,*(notificationsQML.it));
            emit sendNotification(QString::fromStdString(msg));
    }

    pthread_mutex_unlock (&mTimedb);
    pthread_mutex_unlock (&mPatientdb);

}

void CSMD::decrementNofication()
{
    std::string msg;

    emit nextButton(true);

    pthread_mutex_lock (&mPatientdb);
    pthread_mutex_lock (&mTimedb);

    notificationsQML.it--;
    msg = patNot.get(db,*(notificationsQML.it));
    emit sendNotification(QString::fromStdString(msg));

    if(*(notificationsQML.it) == notificationsQML.idNotList.front())
          emit previousButton(false);

    pthread_mutex_unlock (&mTimedb);
    pthread_mutex_unlock (&mPatientdb);
}

void CSMD::incrementNofication()
{
    std::string msg;

    emit previousButton(true);

    pthread_mutex_lock (&mPatientdb);
    pthread_mutex_lock (&mTimedb);

    notificationsQML.it++;
    msg = patNot.get(db,*(notificationsQML.it));
    emit sendNotification(QString::fromStdString(msg));

    if(*(notificationsQML.it) == notificationsQML.idNotList.back())
          emit nextButton(false);

    pthread_mutex_unlock (&mTimedb);
    pthread_mutex_unlock (&mPatientdb);
}

void CSMD::removeNotification()
{
    std::string msg;

    db.print("PatientNotification");

    pthread_mutex_lock (&mPatientdb);
    pthread_mutex_lock (&mTimedb);

    patNot.remove(db,  *(notificationsQML.it));
    db.print("PatientNotification");
//    std::cout << "AFTER remove" << std::endl;

    notificationsQML.it  = notificationsQML.idNotList.erase(notificationsQML.it);
//    std::cout << "Nots na lista:" << std::endl;
//    for (auto v : notificationsQML.idNotList)
//            std::cout << v << "\n";

    switch(notificationsQML.idNotList.size())
    {
        case 0:
            emit removeButton(false);
            emit previousButton(false);
            emit nextButton(false);
            emit sendNotification("There aren't Notifications for this Patient");
            break;
        case 1:
            notificationsQML.it  = notificationsQML.idNotList.begin();
            //std::cout <<  "ID COM APENAS 1: " << *(notificationsQML.it) << std::endl;
            emit previousButton(false);
            emit nextButton(false);
              msg = patNot.get(db,*(notificationsQML.it));
            emit sendNotification(QString::fromStdString(msg));
            break;
        default:
            //std::cout << "AFTER APAGAR ID: " << *(notificationsQML.it) << std::endl;
            if(notificationsQML.it == notificationsQML.idNotList.end())
                notificationsQML.it--;
            //std::cout << "AFTER APAGAR ID: " << *(notificationsQML.it) << std::endl;
            msg = patNot.get(db,*(notificationsQML.it));
            emit sendNotification(QString::fromStdString(msg));
            if(*(notificationsQML.it) == notificationsQML.idNotList.back())
                  emit nextButton(false);
    }

    pthread_mutex_unlock (&mTimedb);
    pthread_mutex_unlock (&mPatientdb);
}

