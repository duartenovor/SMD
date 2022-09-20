/**
* @CPatient.cpp
* @author Joao Mitanda and Duarte Rodrigues
* @date 9-01-2022
*
* @brief Contains the definition of functions from the CPatient.h file 
*/

#include "CPatient.h"
#include <iostream>

/**
 * Function that inserts a new patient into the database according id 
 * @return -
 */
void CPatient::insert(CDatabase &db, int idPatient)
{
	std::cout << "-----Insert Patient-----" << std::endl;
	db.insert("Patient", std::to_string(idPatient)+",NULL,NULL");
}

/**
 * Function that changes the name of the desired patient id.  
 * @return -
 */      
void CPatient::updateName(CDatabase &db, int idPatient, std::string name)
{
	std::cout << "-----Update Patient NAME-----" << std::endl;
	db.update("Patient","patient_name","'"+name+"'","patient_id",std::to_string(idPatient));
}

/**
 * Function that changes the RFID tag of the desired patient id.  
 * @return -
 */
void CPatient::updateRFID(CDatabase &db, int idPatient, std::string rfidTag)
{
	std::cout << "-----Update Caregiver RFID TAG-----" << std::endl;
	db.update("Patient","RFID_tag","'"+rfidTag+"'","patient_id",std::to_string(idPatient));
}

/**
 * Function that looks up the name of the patient id in question.  
 * @return string with name
 */
std::string CPatient::getName(CDatabase &db, int idPatient)
{
	std::string d_text;
	db.get("Patient","patient_name","patient_id",std::to_string(idPatient),d_text);

	if(d_text == "")
		d_text = std::to_string(NOT_FIND);

	return d_text;
}

/**
 * Function that looks up the RFID tag of the patient id in question.  
 * @return string with rfid tag
 */
std::string CPatient::getRFID(CDatabase &db, int idPatient)
{
	std::string d_text;
	db.get("Patient","RFID_tag","patient_id",std::to_string(idPatient),d_text);

	if(d_text == "")
		d_text = std::to_string(NOT_FIND);

	return d_text;
}

/**
 * Function that looks up the id of the rfid tag in question.  
 * @return integer id
 */
int CPatient::getID(CDatabase &db, std::string rfidTag)
{
	std::string d_text;
	db.get("Patient","patient_id","RFID_tag","'"+rfidTag+"'",d_text);

	if(d_text == "")
		d_text = std::to_string(NOT_FIND);

	return stoi(d_text);
}

/**
 * Function that remove the patient id in question. 
 * @return -
 */
void CPatient::remove(CDatabase &db, int idPatient)
{
	db.remove("Patient","patient_id",std::to_string(idPatient));
}

//db.print("Patient");