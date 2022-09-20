/**
* @CPatientNotification.cpp
* @author Joao Mitanda and Duarte Rodrigues
* @date 9-01-2022
*
* @brief Contains the definition of functions from the CPatientNotification.h file 
*/

#include "CPatientNotification.h"
#include <iostream>

void CPatientNotification::insert(CDatabase &db, int idPatient, int idTime, int day, int month, int year)
{
	std::cout << "-----Insert Patient Notification-----" << std::endl;
	db.insert("PatientNotification", std::to_string(idPatient)+","+std::to_string(idTime)+","+std::to_string(day)+","
		+std::to_string(month)+","+std::to_string(year));

	db.print("PatientNotification");
}

/*void CPatientNotification::updateDate(CDatabase &db, int idPatientNotification, int idPatient, int hour, int day, int month, int year)
{
	//db.update("PatinetNotification");
}*/

std::string CPatientNotification::get(CDatabase &db, int idPatient)
{
	std::string day, month, year, time, d_hour, d_minute;
	db.get("PatientNotification","time_id","patient_id",std::to_string(idPatient),time);
	db.get("PatientNotification","day","patient_id",std::to_string(idPatient),day);
	db.get("PatientNotification","month","patient_id",std::to_string(idPatient),month);
	db.get("PatientNotification","year","patient_id",std::to_string(idPatient),year);
	
	if(time == "" || day == "" || month == "" || year == "")
		return std::to_string(NOT_FIND);

	db.get("MedicationTime","med_hour","time_id",time,d_hour);
	db.get("MedicationTime","med_min","time_id",time,d_minute);

	if(d_hour == "" || d_minute == "")
		return std::to_string(NOT_FIND);

	time = "The patient did not take the medication on day " + day + "/" + month + "/" + year + " at time "+ d_hour + ":" + d_minute;

	return time;
}

void CPatientNotification::remove(CDatabase &db, int idPatientNotification)
{
	db.remove("PatientNotification","patient_id",std::to_string(idPatientNotification));
}