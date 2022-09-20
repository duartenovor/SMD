/**
* @CPatientNotification.h
* @author Joao Mitanda and Duarte Rodrigues
* @date 9-01-2022
*
* @brief Contains prototypes of the interface functions with the PatientNotification table. 
*/

#ifndef _CPATIENTNOTIFICATION_H_
#define _CPATIENTNOTIFICATION_H_

#include "CDatabase.h"
//#include <ctime>

class CPatientNotification
{
   public:
      void insert(CDatabase &db, int idPatient, int idTime, int day, int month, int year);
      
      /*void updateDate(CDatabase &db, int idPatientNotification, int idPatient, int hour, int day, int month, int year);*/

      std::string get(CDatabase &db, int idPatient);

      void remove(CDatabase &db, int idPatientNotification);
};

#endif /* _CPATIENTNOTIFICATION_H_ */