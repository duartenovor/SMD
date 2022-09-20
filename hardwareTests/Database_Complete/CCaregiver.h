/**
* @CCaregiver.h
* @author Joao Miranda and Duarte Rodrigues
* @date 2-01-2022
*
* @brief Contains prototypes of the interface functions with the Caregiver table. 
*/

#ifndef _CCAREGIVER_H_
#define _CCAREGIVER_H_

#include "CDatabase.h"
#include <string>

class CCaregiver
{
   public:
      int insert(CDatabase &db);
      
      void updateName(CDatabase &db, int idCaregiver, std::string name);
      void updatePin(CDatabase &db, int idCaregiver, std::string pin);
      void updateFingerprint(CDatabase &db, int idCaregiver, int idFinger);

      std::string getName(CDatabase &db, int idCaregiver);
      std::string getPin(CDatabase &db, int idCaregiver);
      int getFingerprint(CDatabase &db, int idCaregiver);
      int getID(CDatabase &db, int idFinger);
      int getID(CDatabase &db, std::string pin);

      void remove(CDatabase &db, int idCaregiver);
};

#endif /*_CCAREGIVER_H_ */