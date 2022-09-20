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
#include <list>

class CPatientNotification
{
   public:
      void insert(CDatabase &db, int idPatient, int idMedicine, int quantity, int idTime, int day, int month, int year, int type);
      
      /*void updateDate(CDatabase &db, int idPatientNotification, int idPatient, int hour, int day, int month, int year);*/

      std::list<int> getIds(CDatabase &db, int idPatient);
      std::string get(CDatabase &db, int idNotification);

      void remove(CDatabase &db, int idNotification);
      void removePatient(CDatabase &db, int idPatient);
};

#endif /* _CPATIENTNOTIFICATION_H_ */
