#include "CCaregiver.h"
#include <iostream>

void CCaregiver::insert(CDatabase &db, int idCaregiver)
{
	
}

void CCaregiver::updateName(int idCaregiver, std::string name)
{
	/*std::cout << "-----Update Caregiver (2)-----" << std::endl;
	db.update("Caregiver","NOME",name,"caregiver_id","Pedro");
	std::cout << "---Caregivers on the system---" << std::endl;
	db.print("Caregiver");*/
}

void CCaregiver::updatePin(int idCaregiver, std::string pin)
{
	/*std::cout << "-----Insert Caregiver (1)-----" << std::endl;
	db.insert("Caregiver","NULL,1,'Joao','0000'");
	std::cout << "-----Insert Caregiver (2)-----" << std::endl;
	db.insert("Caregiver","NULL,1,'Duarte','0000'");

	std::cout << "---Caregivers on the system---" << std::endl;
	db.print("Caregiver");*/

	//db.insert("Patient","NULL,'RITA','00-00-00-00'");
	//db.print("Patient");

	/*db.insert("Medicine","NULL,'BEN',10");
	db.insert("Medicine","NULL,'BRUF',10");*/
	//db.print("Medicine");

	/*db.insert("MedicineNotification","'2'");
	db.print("MedicineNotification");

	db.remove("Medicine","medicine_name","'BRUF'");
	db.print("MedicineNotification");*/

	/*db.insert("MedicationTime","NULL,datetime('now')");
	db.insert("MedicationTime","NULL,datetime('now')");*/
	//db.print("MedicationTime");

/*	db.insert("PatientNotification","1,1");
	db.print("PatientNotification");

	db.remove("Patient","patient_name","'RITA'");
	db.print("PatientNotification");*/

	//db.insert("Prescription","1,1,2,20");
	/*db.print("Prescription");
	db.remove("Patient","patient_name","'RITA'");
	db.print("Patient");
	db.print("Prescription");*/
}

void CCaregiver::updateFingerprint(int idCaregiver, int idFinger)
{

}

std::string CCaregiver::getName(int idCaregiver)
{
	return "a";
}

std::string CCaregiver::getPin(int idCaregiver)
{
	return "a";
}

int CCaregiver::getFingerprint(int idCaregiver)
{
	return 0;
}

int CCaregiver::getID(int idFinger)
{
	return 0;
}

int CCaregiver::getID(std::string pin)
{
	return 0;
}

void CCaregiver::remove(int idCaregiver)
{
   
}
