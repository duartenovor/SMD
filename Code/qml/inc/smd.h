#ifndef _SMD_H_
#define _SMD_H_

#include <QObject>
#include <QString>
#include <QQmlProperty>

#include <semaphore.h> /**< semaphore functions */
#include <mqueue.h>      /**< mq_* functions */

#include "CDatabase.h"
#include "CCaregiver.h"
#include "CMedicine.h"
#include "CPatient.h"
#include "CPatientNotification.h"
#include "CPrescription.h"
#include "CTime.h"

typedef struct  {
    int pat_ID;
    std::list<int> med_ID;
    std::list<int> med_quatity;
}prescription_t;

typedef struct {
    std::list<int> idNotList;
    std::list<int>::iterator it;
}notification_t;

class CSMD  : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool caregiverState WRITE setCaregiverState NOTIFY setCaregiverStateChanged)

    signals:

        /**< QML Text Page*/
        void sendText(QString text);
        void showBack();

        /**< QML Notification Page*/
       void sendNotification(QString text);
       void previousButton(bool visible);
       void nextButton(bool visible);
       void removeButton(bool visible);

        void sendToQml(QString text);
        void setCaregiverID(int id);
        void setPatients(int id);
        void removePatients(int id);

        void setCaregiverStateChanged();



    public:
        CSMD(QObject *parent = nullptr);
        ~CSMD();

         /**< QML Caregiver Functions*/
        Q_INVOKABLE int checkCaregiver(int id, QString pin);
        Q_INVOKABLE void updateFingerprint(int id);
        Q_INVOKABLE void updatePin(int id, QString pin);
        Q_INVOKABLE bool caregiverExists(int id);
        Q_INVOKABLE bool removeCaregiver(int id);
        Q_INVOKABLE void addCaregiver(int id);
        Q_INVOKABLE void supplyMedicationDecision(int id);

         /**< QML Patient Functions*/
        Q_INVOKABLE void removePatient(int id);
        Q_INVOKABLE void updateRFID(int id);

          /**< QML Medicine Functions*/
        Q_INVOKABLE int getMedicineQuantity(int id);
        Q_INVOKABLE int updateMedicineQuantity(int id, QString signal);

        /**< QML Prescriptions Functions*/
        Q_INVOKABLE int getMedicinePrescription(int idPat, int idMed, int idTime);
        Q_INVOKABLE int updatePrescription(int idPat, int idMed, int idTime, QString signal);

        /**< QML Notification Functions*/
        Q_INVOKABLE void getNotification(int idPatient);
        Q_INVOKABLE void decrementNofication();
        Q_INVOKABLE void incrementNofication();
        Q_INVOKABLE void removeNotification();


        /**< QML Shared Variables*/
        void setCaregiverState(bool state);

    private:
        /**< pThreads Declaration */
        pthread_t tPatRFID;
        pthread_t tCarFinger;
        pthread_t  tCaregiver;
        pthread_t  tStartMedication;
        pthread_t  tTimeout;
        pthread_t tBuzzer;
        pthread_t  tSupplyMedication;

        pthread_t tRfidQML;
        pthread_t tFingerQML;

        /**< Mutex Declaration */
        pthread_mutex_t timer1_lock =  PTHREAD_MUTEX_INITIALIZER;
        pthread_mutex_t timer2_lock =  PTHREAD_MUTEX_INITIALIZER;
        pthread_mutex_t mPrescriptiondb =  PTHREAD_MUTEX_INITIALIZER;
        pthread_mutex_t mPatientdb =  PTHREAD_MUTEX_INITIALIZER;
        pthread_mutex_t mTimedb =  PTHREAD_MUTEX_INITIALIZER;
        pthread_mutex_t mCaregiverdb =  PTHREAD_MUTEX_INITIALIZER;
        pthread_mutex_t mCaregiverOn =  PTHREAD_MUTEX_INITIALIZER;
        pthread_mutex_t mRfidUpdate =  PTHREAD_MUTEX_INITIALIZER;
        pthread_mutex_t mFingerUpdate =  PTHREAD_MUTEX_INITIALIZER;

        /**< Semaphore Declarations*/
        sem_t sBuzzer;
        sem_t sSupplyMedication;
        sem_t sCaregiver;

        /**< Message Queue Declarations*/
            /**< Patient -> RFID*/
        mqd_t msgq_id_verpat;
        mqd_t msgq_id_newpat;
            /**< Caregiver -> Fingerprint*/
        mqd_t msgq_id_vercar;
        mqd_t msgq_id_newcar;

        /* pThreads functions */
        static void *tPatRFIDFunction(void *arg);
        static void *tCarFingerFunction(void *arg);
        static void *tCaregiverFunction(void *arg);
        static void *tStartMedicationFunction(void *arg);
        static void *tTimeoutFunction(void *arg);
        static void *tBuzzerFunction(void *arg);
        static void *tSupplyMedicationFunction(void *arg);

         static void *tRfidQMLFunction(void *arg);
         static void *tFingerQMLFunction(void *arg);

        /* pThreads Creation */
        void createThreads();
        /*Semaphores Creation*/
        void createSemaphores();
        /*Semaphores Delete*/
        void deleteSemaphores();

        /*Create Mqueues*/
        void create_mqueues();

        /**< Private Functions */
        bool removeFromFingerprint(int fingerprintID);

        /**< Timer ID*/
        timer_t timerStartMed;
        timer_t timerTimeout;
        timer_t timerSensor;

        /*timer creation*/
        void create_timer(timer_t &timer_id, int sival_int);
        void set_timer(timer_t timer_id, unsigned i);
        static void thread_handler(union sigval arg);

        /*Principal Database Class*/
        CDatabase db;

        /*Interface Database Class*/
        CCaregiver caregiver;
        CMedicine medicine;
        CPatient patient;
        CPatientNotification patNot;
        CPrescription prescription;
        CTime time;

        /*Device Driver*/
        int file_descriptor_buzzer;
        int file_descriptor_motors;

        /*Daemon PIDs*/
        int pid_rfid;
        int pid_finger;

        /* variables Main Program*/
        std::list<prescription_t> actualPrescription;
        std::_List_const_iterator<prescription_t> patientLogged;

        /**< List and pointer to list position that contain atual */
        notification_t notificationsQML;

        /* variables QML*/
        int qmlPatientID;
        int qmlCaregiverID;

        int actual_time;
        bool caregiverON; /*ON = 1 | OFF = 0*/
};

#endif
