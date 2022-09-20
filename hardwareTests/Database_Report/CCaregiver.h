#ifndef _CCAREGIVER_H_
#define _CCAREGIVER_H_

#include "CDatabase.h"
#include <string>

class CCaregiver
{
   public:
      void insert(CDatabase &db, int idCaregiver);
      
      void updateName(int idCaregiver, std::string name);
      void updatePin(int idCaregiver, std::string pin);
      void updateFingerprint(int idCaregiver, int idFinger);

      std::string getName(int idCaregiver);
      std::string getPin(int idCaregiver);
      int getFingerprint(int idCaregiver);
      int getID(int idFinger);
      int getID(std::string pin);

      void remove(int idCaregiver);
};

#endif /*_CCAREGIVER_H_ */