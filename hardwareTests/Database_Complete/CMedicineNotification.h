#define _CMEDICINENOTIFICATION_H_
#ifndef _CMEDICINENOTIFICATION_H_

enum class type
{
    NO_STOCK,
    LOW_STOCK
};

class CMedicineNotification
{
   public:
      void insert(CDatabase &db, int idMedicine);

      void updateType(CDatabase &db, int idMedicine, type t);

      int getType(CDatabase &db, int idMedicine);

      void remove(CDatabase &db, int idMedicine);
}

#endif /* _CPMEDICINENOTIFICATION_H_ */