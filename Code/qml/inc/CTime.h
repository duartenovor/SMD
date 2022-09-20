/**
* @CTime.h
* @author Joao Mitanda and Duarte Rodrigues
* @date 9-01-2022
*
* @brief Contains prototypes of the interface functions with the MedicationTime table. 
*/

#include "CDatabase.h"

#ifndef _CTIME_H_
#define _CTIME_H_

class CTime
{
   public:
      void insert(CDatabase &db, int idTime, int med_hour, int med_min);
      int getID(CDatabase &db, int med_hour, int med_min);
      int getHour(CDatabase &db, int idTime);
      int getMinute(CDatabase &db, int idTime);
      int getNumberElements(CDatabase &db);
      
      void remove(CDatabase &db, int idTime);
};

#endif /* _CTIME_H_ */
