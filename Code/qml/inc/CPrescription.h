/**
* @CPrescription.h
* @author Joao Miranda and Duarte Rodrigues
* @date 12-01-2022
*
* @brief Contains prototypes of the interface functions with the CPrescription table. 
*/

#ifndef _CPRESCRIPTION_H_
#define _CPRESCRIPTION_H_

#include "CDatabase.h"
#include <string>
#include <list>

class CPrescription
{
   public:
      void insert(CDatabase &db, int idPatient, int idTime, int idMedicine, int quantity);

      void updateQuantity(CDatabase &db, int idPatient, int idTime, int idMedicine, int quantity);
      void updateQuantity(CDatabase &db, int idPatient, int idTime, int idMedicine, int quantity, std::string signal);

      int getQuantity(CDatabase &db, int idPatient, int idTime, int idMedicine);
      std::string getQuantity(CDatabase &db, int idTime);
      std::string getMedicine(CDatabase &db, int idTime);
      std::list <int> getPatient(CDatabase &db, int idTime);
      std::string getQuantityMedicine(CDatabase &db, int idPatient, int idTime);
      int getNumberElements(CDatabase &db);


      void remove(CDatabase &db, int idPatient, int idTime, int idMedicine);
};

#endif /* _CPRESCRIPTION_H_ */
