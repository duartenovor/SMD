/**
* @CMedicine.cpp
* @author Joao Miranda and Duarte Rodrigues
* @date 11-01-2022
*
* @brief Contains the definition of functions from the CMedicine.h file 
*/

#include "../inc/CMedicine.h"
#include <iostream>

/**
 * Function that inserts a new medicine into the database 
 * @return -
 */
void CMedicine::insert(CDatabase &db, int idMedicine)
{
    //std::cout << "-----Insert Medicine-----" << std::endl;
	db.insert("Medicine",std::to_string(idMedicine)+",NULL,0");
}

/**
 * Function that changes the name of the desired medicine id.  
 * @return -
 */
void CMedicine::updateName(CDatabase &db, int idMedicine, std::string name)
{
	std::cout << "-----Update Medicine NAME-----" << std::endl;
	db.update("Medicine","medicine_name","'"+name+"'","medicine_id",std::to_string(idMedicine));
}

/**
 * Function that changes the quantity of the desired caregiver id.
 * Update Medicine Set quantity = quantity "signal"(+/-) "quantity" Where medicine_id = "idMedicine"
 * @return -
 */
void CMedicine::updateQuantity(CDatabase &db, int idMedicine, int quantity)
{
	std::cout << "-----Update Medicine quantity-----" << std::endl;
    db.update("Medicine","quantity","'"+std::to_string(quantity)+"'","medicine_id",std::to_string(idMedicine));
}

/**
 * Function that changes the quantity of the desired caregiver id.
 * Update Medicine Set quantity = quantity "signal"(+/-) "quantity" Where medicine_id = "idMedicine"
 * @return -
 */
void CMedicine::updateQuantity(CDatabase &db, int idMedicine, int quantity, std::string signal)
{
    std::cout << "-----Update Medicine quantity-----" << std::endl;
    db.update("Medicine","quantity","quantity"+signal+"'"+std::to_string(quantity)+"'","medicine_id",std::to_string(idMedicine));
}

/**
 * Function that looks up the number of elements
 * @return integer with number of elements
 */
int CMedicine::getNumberElements(CDatabase &db)
{
    std::string d_text;

    db.getTotalElements("Medicine",d_text);

    if(d_text == "")
        d_text = std::to_string(NOT_FIND);

    return stoi(d_text);
}

/**
 * Function that looks up the name of the medicine id in question.  
 * @return string with name
 */
std::string CMedicine::getName(CDatabase &db, int idMedicine)
{
	std::string d_text;
	db.get("Medicine","medicine_name","medicine_id",std::to_string(idMedicine),d_text);

	if(d_text == "")
		d_text = std::to_string(NOT_FIND);

	return d_text;
}

/**
 * Function that looks up the quantity of the medicine id in question.  
 * @return integer with quantity
 */
int CMedicine::getQuantity(CDatabase &db, int idMedicine)
{
	std::string d_text;
	db.get("Medicine","quantity","medicine_id",std::to_string(idMedicine),d_text);

	if(d_text == "")
		d_text = std::to_string(NOT_FIND);

	return stoi(d_text);
}

/**
 * Function that remove the medicine id in question. 
 * @return -
 */
void CMedicine::remove(CDatabase &db, int idMedicine)
{
	db.remove("Medicine","medicine_id",std::to_string(idMedicine));
}
