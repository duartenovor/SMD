/**
* @CPatient.h
* @author Joao Mitanda and Duarte Rodrigues
* @date 9-01-2022
*
* @brief Contains prototypes of the interface functions with the Patient table.
*/

#ifndef _CPATIENT_H_
#define _CPATIENT_H_

#include "CDatabase.h"
#include <string>

class CPatient
{
   public:
      void insert(CDatabase &db, int idPatient);
      
      void updateName(CDatabase &db, int idPatient, std::string name);
      void updateRFID(CDatabase &db, int idPatient, std::string rfidTag);

      std::string getName(CDatabase &db, int idPatient);
      std::string getRFID(CDatabase &db, int idPatient);
      int getID(CDatabase &db, std::string rfidTag);

      void remove(CDatabase &db, int idPatient);
   
};

#endif /* _CPATIENT_H_ */

