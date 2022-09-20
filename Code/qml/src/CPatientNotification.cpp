/**
* @CPatientNotification.cpp
* @author Joao Mitanda and Duarte Rodrigues
* @date 9-01-2022
*
* @brief Contains the definition of functions from the CPatientNotification.h file 
*/

#include "../inc/CPatientNotification.h"
#include "../inc/auxiliaries.h"

#include <iostream>

void CPatientNotification::insert(CDatabase &db, int idPatient, int idMedicine, int quantity, int idTime, int day, int month, int year, int type)
{
    std::cout << "-----Insert Patient Notification-----" << std::endl;
    db.insert("PatientNotification","NULL,"+ std::to_string(idPatient)+","+std::to_string(idMedicine)+","+std::to_string(quantity)+","+std::to_string(idTime)+","+std::to_string(day)+","
        +std::to_string(month)+","+std::to_string(year)+","+std::to_string(type));

    //db.print("PatientNotification");
}

/*void CPatientNotification::updateDate(CDatabase &db, int idPatientNotification, int idPatient, int hour, int day, int month, int year)
{
	//db.update("PatinetNotification");
}*/

std::list<int> CPatientNotification::getIds(CDatabase &db, int idPatient)
{
    std::string d_text;
    std::list <int> id;

    db.get("PatientNotification","notification_id","patient_id",std::to_string(idPatient),d_text);

    if(d_text != "") {
        d_text.pop_back();
    }

    string_to_vector(d_text, id);

    return id;
}

std::string CPatientNotification::get(CDatabase &db, int idNotification)
{
    std::string day, month, year, time, d_hour, d_minute, quantity, idMedicine, type;
    db.get("PatientNotification","time_id","notification_id",std::to_string(idNotification),time);
    db.get("PatientNotification","day","notification_id",std::to_string(idNotification),day);
    day.pop_back();
    db.get("PatientNotification","month","notification_id",std::to_string(idNotification),month);
    month.pop_back();
    db.get("PatientNotification","year","notification_id",std::to_string(idNotification),year);
    year.pop_back();
    db.get("PatientNotification","medicine_id","notification_id",std::to_string(idNotification),idMedicine);
    idMedicine.pop_back();
    db.get("PatientNotification","quatity","notification_id",std::to_string(idNotification),quantity);
    quantity.pop_back();
    db.get("PatientNotification","type","notification_id",std::to_string(idNotification),type);

//    if(time == "" || day == "" || month == "" || year == "")
//        return std::to_string(NOT_FIND);

    db.get("MedicationTime","med_hour","time_id",time,d_hour);
    d_hour.pop_back();
    db.get("MedicationTime","med_min","time_id",time,d_minute);
    d_minute.pop_back();
//    if(d_hour == "" || d_minute == "")
//        return std::to_string(NOT_FIND);

    switch(stoi(type))
    {
        case 0:     /**< Notification caused by lake of stock */
            type = "Lake of Stock\n";
            break;
        case 1:      /**< Notification caused by time expire */
            type = "Time Expire\n";
    }
    type += " The patient did not take the medicine " + idMedicine + " (" + quantity + ") on " + day + "/" + month + "/" + year + " at "+ d_hour + ":" + d_minute;

    return type;
}

void CPatientNotification::remove(CDatabase &db, int idNotification)
{
    db.remove("PatientNotification","notification_id",std::to_string(idNotification));
}

void CPatientNotification::removePatient(CDatabase &db, int idPatient)
{
    db.remove("PatientNotification","patient_id",std::to_string(idPatient));
}
