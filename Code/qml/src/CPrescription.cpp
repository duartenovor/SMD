/**
* @CCaregiver.cpp
* @author Joao Miranda and Duarte Rodrigues
* @date 12-01-2022
*
* @brief Contains the definition of functions from the CPrescription.h file 
*/

#include "../inc/CPrescription.h"
#include "../inc/auxiliaries.h"
#include <iostream>

/**
 * Function that inserts a new prescription into the database 
 * @return -
 */
void CPrescription::insert(CDatabase &db, int idPatient, int idTime, int idMedicine, int quantity)
{
    //std::cout << "-----Insert Prescription-----" << std::endl;
	db.insert("Prescription", std::to_string(idPatient)+","+std::to_string(idMedicine)+","+std::to_string(idTime)+","+std::to_string(quantity));

//    db.print("Medicine");
//    db.print("Patient");
//    db.print("MedicationTime");
//    db.print("Prescription");
}

/**
 * Function that changes the quantity of the desired prescription(idPatient,idTime,idMedicine).  
 * @return -
 */
void CPrescription::updateQuantity(CDatabase &db, int idPatient, int idTime, int idMedicine, int quantity)
{
	db.update("Prescription","quantity",std::to_string(quantity),"patient_id",std::to_string(idPatient)
		+" AND "+"medicine_id = "+std::to_string(idMedicine)+" AND "+"time_id = "+std::to_string(idTime));

    //db.print("Prescription");
}

/**
 * Function that changes the quantity of the desired caregiver id.
 * Update Medicine Set quantity = quantity "signal"(+/-) "quantity" Where medicine_id = "idMedicine"
 * @return -
 */
void CPrescription::updateQuantity(CDatabase &db, int idPatient, int idTime, int idMedicine, int quantity, std::string signal)
{
    db.update("Prescription","quantity","quantity"+signal+"'"+std::to_string(quantity)+"'","patient_id",std::to_string(idPatient)
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
 * Function that looks up the quantity of the prescription idTime in question.
 * @return string with quantity or quantities
 */
std::string CPrescription::getQuantity(CDatabase &db, int idTime)
{
    std::string d_text;

    db.get("Prescription","quantity","time_id",std::to_string(idTime),d_text);

    if(d_text == "")
        d_text = std::to_string(NOT_FIND);
    else{
        d_text.pop_back();
    }
    return d_text;
}

/**
 * Function that looks up the medicines of the prescription idTime in question.
 * @return string with medicine or medicines
 */
std::string CPrescription::getMedicine(CDatabase &db, int idTime)
{
    std::string d_text;

    db.get("Prescription","medicine_id","time_id",std::to_string(idTime),d_text);

    if(d_text == "")
        d_text = std::to_string(NOT_FIND);
    else{
        d_text.pop_back();
    }

    return d_text;
}

/**
 * Function that looks up the medicines and quantity of the prescription idTime and idPatient in question.
 * @return string with medicine or medicines
 */
std::string CPrescription::getQuantityMedicine(CDatabase &db, int idPatient, int idTime)
{
    std::string d_text;

    db.get("Prescription","medicine_id, quantity","time_id",std::to_string(idTime)+" AND patient_id =" +std::to_string(idPatient)
           +" AND "+"time_id = "+std::to_string(idTime)+" AND quantity > 0",d_text);

    if(d_text == "")
        d_text = std::to_string(NOT_FIND);
    else{
        d_text.pop_back();
    }

    return d_text;
}

/**
 * Function that looks up the patients of the prescription idTime in question.
 * @return string with patient or patients
 */
std::list <int> CPrescription::getPatient(CDatabase &db, int idTime)
{
    std::string d_text;
        std::list <int> l;

    db.get("Prescription","patient_id","time_id",std::to_string(idTime)+" AND quantity > 0",d_text);

    if(d_text != "") {
        d_text.pop_back();
    }

    string_to_vector(d_text, l);

    l.sort();
    l.unique();

    return l;
}

/**
 * Function that looks up the number of elements
 * @return integer with number of elements
 */
int CPrescription::getNumberElements(CDatabase &db)
{
    std::string d_text;

    db.getTotalElements("Prescription",d_text);

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
