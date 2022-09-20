#include <iostream>
#include <string.h>

#include "CDatabase.h"
#include "CCaregiver.h"

#define MAX_STRING		30

/**< Compile:
 * g++ -o db.elf test_database.cpp CDatabase.cpp CCaregiver.cpp -lsqlite3
 */

int main()
{
	std::string text;

	CDatabase db;

  	std::cout << "-----Insert Patient (1)-----" << std::endl;
	db.insert("Patient","NULL,'Joao','01-02-03-04'");
	std::cout << "-----Insert Patient (2)-----" << std::endl;
	db.insert("Patient","NULL,'Rodrigo','10-20-30-40'");
	std::cout << "-----Insert Patient (3)-----" << std::endl;
	db.insert("Patient","NULL,'Manuel','11-22-33-44'");
	std::cout << "-----Print table Patient-----" << std::endl;
	db.print("Patient");

	std::cout << "-----Update Patient (2)-----" << std::endl;
	std::cout << "->Change name to 'Duarte'" << std::endl;
	db.update("Patient","patient_name","'Duarte'","patient_name","'Rodrigo'");
	std::cout << "-----Print table Patient-----" << std::endl;
	db.print("Patient");

	std::cout << "-----Remove Patient (3)-----" << std::endl;
	db.remove("Patient","patient_name","'Manuel'");
	std::cout << "-----Print table Patient-----" << std::endl;
	db.print("Patient");

	std::cout << "-----Insert actual date-----" << std::endl;
	db.insert("MedicationTime","NULL,datetime('now')");
	std::cout << "-----Print table MedicationTime-----" << std::endl;
	db.print("MedicationTime");

	std::cout << "-----Insert Notification to Patient (1)-----" << std::endl;
	db.insert("PatientNotification","1,1");
	std::cout << "-----Print table PatientNotification-----" << std::endl;
	db.print("PatientNotification");

	std::cout << "-----Remove Patient (1)-----" << std::endl;
	std::cout << "->It implies deleting the notification entry made earlier" << std::endl;
	db.remove("Patient","patient_id","1");

	std::cout << "-----Print table PatientNotification-----" << std::endl;
	db.print("PatientNotification");

	return 0;
}