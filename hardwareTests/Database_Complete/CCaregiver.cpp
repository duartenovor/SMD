/**
* @CCaregiver.cpp
* @author Joao Miranda and Duarte Rodrigues
* @date 2-01-2022
*
* @brief Contains the definition of functions from the CCaregiver.h file 
*/

#include "CCaregiver.h"
#include <iostream>

/**
 * Function that inserts a new caregiver into the database 
 * @return the caregiver id entered 
 */
int CCaregiver::insert(CDatabase &db)
{
	std::cout << "-----Insert Caregiver-----" << std::endl;
	db.insert("Caregiver","NULL,NULL,NULL,'0000'");

	std::string d_text;
	db.getLastInserted("Caregiver","caregiver_id","caregiver_id",d_text);

	return stoi(d_text);
}

/**
 * Function that changes the name of the desired caregiver id.  
 * @return -
 */
void CCaregiver::updateName(CDatabase &db, int idCaregiver, std::string name)
{
	std::cout << "-----Update Caregiver NAME-----" << std::endl;
	db.update("Caregiver","caregiver_name","'"+name+"'","caregiver_id",std::to_string(idCaregiver));
}

/**
 * Function that changes the pin of the desired caregiver id.  
 * @return -
 */
void CCaregiver::updatePin(CDatabase &db, int idCaregiver, std::string pin)
{
	std::cout << "-----Update Caregiver PIN-----" << std::endl;
	db.update("Caregiver","pin_code","'"+pin+"'","caregiver_id",std::to_string(idCaregiver));
}

/**
 * Function that changes the fingerprint of the desired caregiver id.  
 * @return -
 */
void CCaregiver::updateFingerprint(CDatabase &db, int idCaregiver, int idFinger)
{
	std::cout << "-----Update Caregiver FINGERPRINT-----" << std::endl;
	db.update("Caregiver","fingerprint_id",std::to_string(idFinger),"caregiver_id",std::to_string(idCaregiver));

	db.print("Caregiver");
}

/**
 * Function that looks up the name of the caregiver id in question.  
 * @return string with name
 */
std::string CCaregiver::getName(CDatabase &db, int idCaregiver)
{
	std::string d_text;
	db.get("Caregiver","caregiver_name","caregiver_id",std::to_string(idCaregiver),d_text);

	if(d_text == "")
		d_text = std::to_string(NOT_FIND);

	return d_text;
}

/**
 * Function that looks up the pin of the caregiver id in question.  
 * @return string with pin
 */
std::string CCaregiver::getPin(CDatabase &db, int idCaregiver)
{
	std::string d_text;
	db.get("Caregiver","pin_code","caregiver_id",std::to_string(idCaregiver),d_text);

	if(d_text == "")
		d_text = std::to_string(NOT_FIND);

	return d_text;
}

/**
 * Function that looks up the fingerprint id of the caregiver id in question.  
 * @return integer with fingerprint id
 */
int CCaregiver::getFingerprint(CDatabase &db, int idCaregiver)
{
	std::string d_text;
	db.get("Caregiver","fingerprint_id","caregiver_id",std::to_string(idCaregiver),d_text);

	if(d_text == "")
		d_text = std::to_string(NOT_FIND);

	return stoi(d_text);
}

/**
 * Function that looks up the id of the fingeprint id in question.  
 * @return integer id
 */
int CCaregiver::getID(CDatabase &db, int idFinger)
{
	std::string d_text;
	db.get("Caregiver","caregiver_id","fingerprint_id",std::to_string(idFinger),d_text);

	if(d_text == "")
		d_text = std::to_string(NOT_FIND);

	return stoi(d_text);
}

/**
 * Function that looks up the id of the pin in question.  
 * @return integer id
 */
int CCaregiver::getID(CDatabase &db, std::string pin)
{
	std::string d_text;

	db.get("Caregiver","caregiver_id","pin_code","'"+pin+"'",d_text);

	if(d_text == "")
		d_text = std::to_string(NOT_FIND);

	return stoi(d_text);
}

/**
 * Function that remove the caregiver id in question. 
 * @return -
 */
void CCaregiver::remove(CDatabase &db, int idCaregiver)
{
   db.remove("Caregiver","caregiver_id",std::to_string(idCaregiver));
}

//db.print("Caregiver");