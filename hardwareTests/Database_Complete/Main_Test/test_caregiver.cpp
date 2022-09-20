#include <iostream>
#include <string.h>

#include "CDatabase.h"
#include "CCaregiver.h"

/**< Compile:
 * g++ -o db.elf test_caregiver.cpp CDatabase.cpp CCaregiver.cpp -lsqlite3
 */

int main()
{
	std::string text;

	CDatabase db;
	CCaregiver c;
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

	pin1 = c.getPin(db, '*');
/*	pin2 = c.getPin(db, id2);

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

	std::cout << "REMOVER ID1" << std::endl;*/

	c.remove(db, id1);

	return 0;
}