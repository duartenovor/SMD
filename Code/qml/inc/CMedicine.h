/**
* @CMedicine.h
* @author Joao Miranda and Duarte Rodrigues
* @date 11-01-2022
*
* @brief Contains prototypes of the interface functions with the Medicine table. 
*/

#ifndef _CMedicine_H_
#define _CMedicine_H_

#include "CDatabase.h"
#include <string>

class CMedicine
{
   public:
      void insert(CDatabase &db, int idMedicine);
      
      void updateName(CDatabase &db, int idMedicine, std::string name);
      void updateQuantity(CDatabase &db, int idMedicine, int quantity, std::string signal);
      void updateQuantity(CDatabase &db, int idMedicine, int quantity);

      int getNumberElements(CDatabase &db);
      std::string getName(CDatabase &db, int idMedicine);
      int getQuantity(CDatabase &db, int idMedicine);

      void remove(CDatabase &db, int idMedicine);
};

#endif /*_CMedicine_H_ */
