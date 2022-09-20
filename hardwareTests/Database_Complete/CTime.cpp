/**
* @CTime.cpp
* @author Joao Mitanda and Duarte Rodrigues
* @date 9-01-2022
*
* @brief Contains the definition of functions from the CTime.h file 
*/

#include "CTime.h"
#include <iostream>

/**
 * Function that inserts a new time into the database 
 * @return -
 */
void CTime::insert(CDatabase &db, int idTime, int med_hour, int med_min)
{
	std::cout << "-----Insert Time-----" << std::endl;
	db.insert("MedicationTime", std::to_string(idTime)+","+std::to_string(med_hour)+","+std::to_string(med_min));
}

/**
 * Function that looks up the id of hour and minute in question.  
 * @return integer id
 */
int CTime::getID(CDatabase &db, int med_hour, int med_min)
{
	std::string d_text;
	db.get("MedicationTime","time_id","med_hour",std::to_string(med_hour)+" AND med_min = "+std::to_string(med_min),d_text);

	if(d_text == "")
		d_text = std::to_string(NOT_FIND);

	return stoi(d_text);
}

/**
 * Function that looks up the Hour of the idTime in question.  
 * @return string with "hour:minute"
 */
std::string CTime::getHour(CDatabase &db, int idTime)
{
	std::string d_hour, d_minute;
	db.get("MedicationTime","med_hour","time_id",std::to_string(idTime),d_hour);

	db.get("MedicationTime","med_min","time_id",std::to_string(idTime),d_minute);

	if(d_hour == "" || d_minute == "")
		return std::to_string(NOT_FIND);

	d_hour = d_hour + ":" + d_minute;

	return d_hour;
}

/**
 * Function that remove the id time in question. 
 * @return -
 */
void CTime::remove(CDatabase &db, int idTime)
{
   db.remove("MedicationTime","time_id",std::to_string(idTime));
}

//db.print("MedicationTime");