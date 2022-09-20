#include <iostream>
#include <string.h>

#include "CDatabase.h"
//#include "CCaregiver.h"
#include "CPatient.h"
#include "CTime.h"
#include "CPatientNotification.h"
#include "CMedicine.h"
#include "CPrescription.h"

#include <stdio.h>
#include <ctime>


/**< Compile:
 * g++ -o db.elf test_database.cpp CDatabase.cpp CPatientNotification.cpp CPatient.cpp CTime.cpp -lsqlite3
 */

int main()
{
	//------------Current Time--------
	time_t now = time(0);
	// current date/time based on current system
	tm *ltm = localtime(&now);

	CDatabase db;
	CMedicine m;

	//-------------Medicine-----------
	m.insert(db,1);
	m.insert(db,2);

	m.updateName(db,1,"Ben");
	m.updateName(db,2,"Brufen");

	m.updateQuantity(db,1,100,"+");
	m.updateQuantity(db,2,40,"+");

	m.updateQuantity(db,2,30,"-");

	//-----------Time---------------

	CTime t;
	t.insert(db,1,6,30);
	t.insert(db,2,0,30);
	t.insert(db,3,ltm->tm_hour,ltm->tm_min);

	//-----------Patient-----------

	CPatient p;

	p.insert(db,1);
	p.insert(db,2);

	p.updateName(db, 1, "Joao");
	p.updateName(db, 2, "Duarte");

	p.updateRFID(db, 1, "11-11-00-00");
	p.updateRFID(db, 2, "AA-BB-CC-DD");

	//-----------Prescription----

	CPrescription pres;

	pres.insert(db,1,2,1,40);

	//p.remove(db,1);

	pres.insert(db,2,2,1,40);

	std::cout << "----------> " << pres.getQuantity(db,2,2,1) << std::endl;

	pres.updateQuantity(db,2,2,1,200);

	std::cout << "----------> " << pres.getQuantity(db,2,2,1) << std::endl;

	pres.remove(db,2,2,1);


} 


/*CCaregiver c;
	int id1, id2;
	int finger1, finger2;
	std::string pin1;
	std::string pin2;

	id1 = c.insert(db);
	id2 = c.insert(db);

	std::cout << "---->" << id1 << std::endl;
	std::cout << "---->" << id2 << std::endl;

	c.updatePin(db, id1, "kk7k");
	c.updateName(db, id1, "Joao");  
	c.updateFingerprint(db, id1, 7777); 

	c.updatePin(db, id2, "1234");
	c.updateName(db, id2, "HHH");  
	c.updateFingerprint(db, id2, 9990); 

	std::cout << "---->" << c.getName(db, id1) << std::endl;
	std::cout << "---->" << c.getName(db, id2) << std::endl;

	pin1 = c.getPin(db, id1);
	pin2 = c.getPin(db, id2);

	std::cout << "---->" << pin1 << std::endl;
	std::cout << "---->" << pin2 << std::endl;

	finger1 = c.getFingerprint(db, id1);
	finger2 = c.getFingerprint(db, id2);

	std::cout << "---->" << finger1 << std::endl;
	std::cout << "---->" << finger2 << std::endl;

	id1 = c.getID(db, finger1);
	id2 = c.getID(db, finger2); 

	std::cout << "---->" << id1 << std::endl;
	std::cout << "---->" << id2 << std::endl;

	id1 = c.getID(db, pin1);
	id2 = c.getID(db, pin2); 

	std::cout << "---->" << id1 << std::endl;
	std::cout << "---->" << id2 << std::endl;

	std::cout << "REMOVER ID1" << std::endl;

	c.remove(db, id1);*/


	/*CPatient p;

	p.insert(db,1);
	p.insert(db,2);

	p.updateName(db, 1, "Joao");
	p.updateName(db, 2, "Duarte");

	p.updateRFID(db, 1, "11-11-00-00");
	p.updateRFID(db, 2, "AA-BB-CC-DD");

	std::cout << "---->" << p.getName(db, 1) << std::endl;
	std::cout << "---->" << p.getName(db, 2) << std::endl;

	std::string rfid1 = p.getRFID(db, 1);
	std::string rfid2 = p.getRFID(db, 2);

	std::cout << "---->" << rfid1 << std::endl;
	std::cout << "---->" << rfid2 << std::endl;

	int id1 = p.getID(db, rfid1);
	int id2 = p.getID(db, "AB-CD-EF-GH"); 

	std::cout << "---->" << id1 << std::endl;
	std::cout << "---->" << id2 << std::endl;

	std::cout << "REMOVER ID1" << std::endl;
	p.remove(db, 1);

	return 0;*/

	/*CTime t;

	t.insert(db,1,0,0);
	t.insert(db,2,0,30);
	t.insert(db,3,1,0);

	std::cout << t.getID(db,0,30) << std::endl;

	std::cout << t.getHour(db,2) << std::endl;
	
	t.remove(db,3);*/

	/*time_t now = time(0);
	// current date/time based on current system
	tm *ltm = localtime(&now);
	// print various components of tm structure.
	std::cout << "Year:" << 1900 + ltm->tm_year<< std::endl;
	std::cout << "Month: "<< 1 + ltm->tm_mon<< std::endl;
	std::cout << "Day: "<< ltm->tm_mday << std::endl;
	std::cout << "Time: "<< ltm->tm_hour << ":";
	std::cout << ltm->tm_min << ":";
	std::cout << ltm->tm_sec << std::endl;

	std::cout << "----------CODE---------" << std::endl;

	CPatient p;
	p.insert(db,1);
	p.insert(db,2);
	p.updateName(db, 1, "Joao");
	p.updateName(db, 2, "Duarte");
	p.updateRFID(db, 1, "11-11-00-00");
	p.updateRFID(db, 2, "AA-BB-CC-DD");

	CTime t;
	t.insert(db,1,6,30);
	t.insert(db,2,0,30);
	t.insert(db,3,ltm->tm_hour,ltm->tm_min);

	CPatientNotification n;
	n.insert(db,1,1,ltm->tm_mday,(1 + ltm->tm_mon),(1900 + ltm->tm_year));

	std::cout << n.get(db,1) << std::endl;

	n.remove(db,1);

	return 0;*/


	/*CMedicine m;

	m.insert(db,1);
	m.insert(db,2);

	m.updateName(db,1,"Ben");
	m.updateName(db,2,"Brufen");

	m.updateQuantity(db,1,100,"+");
	m.updateQuantity(db,2,40,"+");

	m.updateQuantity(db,2,30,"-");

	std::cout << m.getName(db,1) << std::endl;
	std::cout << m.getName(db,2) << std::endl;

	std::cout << m.getQuantity(db,1) << std::endl;
	std::cout << m.getQuantity(db,2) << std::endl;

	m.remove(db,1);*/