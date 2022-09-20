#include <iostream>

#include "FingerprintReader.h"


int main()
{
    cout << "Fingerprint Starting Test ... " << endl;
	CFingerprint f;

    cout << "-------------Delete all templates------------ " << endl;
    f.removeAllFingerPrint();
    cout << "All templates removed! " << endl;

    cout << "--------------New fingerprint---------------- " << endl;
    f.addFingerPrint();
    usleep(2000000);
    cout << "--------------New fingerprint---------------- " << endl;
    f.addFingerPrint();
    usleep(2000000);
    cout << "--------------New fingerprint---------------- " << endl;
    f.addFingerPrint();
    usleep(2000000);
    cout << "------------Search fingerprint---------------" << endl;
    f.searchFingerPrint();
    usleep(2000000);
    cout << "------------Search fingerprint---------------" << endl;
    f.searchFingerPrint();
    usleep(2000000);
    cout << "------------Search fingerprint---------------" << endl;
    f.searchFingerPrint();

	return 0;
}