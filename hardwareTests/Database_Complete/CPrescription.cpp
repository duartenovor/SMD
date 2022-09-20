/**
* @CCaregiver.cpp
* @author Joao Miranda and Duarte Rodrigues
* @date 12-01-2022
*
* @brief Contains the definition of functions from the CPrescription.h file 
*/

#include "CPrescription.h"
#include <iostream>

/**
 * Function that inserts a new prescription into the database 
 * @return -
 */
void CPrescription::insert(CDatabase &db, int idPatient, int idTime, int idMedicine, int quantity)
{
	std::cout << "-----Insert Prescription-----" << std::endl;
	db.insert("Prescription", std::to_string(idPatient)+","+std::to_string(idMedicine)+","+std::to_string(idTime)+","+std::to_string(quantity));
}

/**
 * Function that changes the quantity of the desired prescription(idPatient,idTime,idMedicine).  
 * @return -
 */
void CPrescription::updateQuantity(CDatabase &db, int idPatient, int idTime, int idMedicine, int quantity)
{
	std::cout << "-----Update quantity-----" << std::endl;
	db.update("Prescription","quantity",std::to_string(quantity),"patient_id",std::to_string(idPatient)
		+" AND "+"medicine_id = "+std::to_string(idMedicine)+" AND "+"time_id = "+std::to_string(idTime));
}

/**
 * Function that looks up the quantity of the prescription(idPatient,idTime,idMedicine) in question.  
 * @return integer with quantity
 */
int CPrescription::getQuantity(CDatabase &db, int idPatient, int idTime, int idMedicine)
{
	std::string d_text;

	db.get("Prescription","quantity","patient_id",std::to_string(idPatient)+" AND "+"medicine_id = "+std::to_string(idMedicine)
		+" AND "+"time_id = "+std::to_string(idTime),d_text);

	if(d_text == "")
		d_text = std::to_string(NOT_FIND);

	return stoi(d_text);
}	

/**
 * Function that remove the prescription(idPatient,idTime,idMedicine) in question.  
 * @return -
 */
void CPrescription::remove(CDatabase &db, int idPatient, int idTime, int idMedicine)
{
	db.remove("Prescription","patient_id",std::to_string(idPatient) +" AND "+"medicine_id = "+std::to_string(idMedicine)
		+" AND "+"time_id = "+std::to_string(idTime));

	db.print("Prescription");
}	

/*db.print("Medicine");
db.print("MedicationTime");
db.print("Patient");
db.print("Prescription");*/
