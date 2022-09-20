#include "CMedicineNotification.h"


void CMedicineNotification::insert(CDatabase &db, int idMedicine)
{
	db.insert("MedicineNotification");
}

void CMedicineNotification::update_type(CDatabase &db, int idMedicine, type t)
{
	db.update("MedicineNotification");
}

int getType(CDatabase &db, int idMedicine)
{
	db.get("MedicineNotification");
}

void CMedicineNotification::remove(CDatabase &db, int idMedicine)
{
	db.remove("MedicineNotification");
}