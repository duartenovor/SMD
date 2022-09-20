#include <iostream>

#include "FingerprintReader.h"


int main()
{
    cout << "Fingerprint Starting Test ... " << endl;
	CFingerprint f;

    cout << "-------------Delete all templates------------ " << endl;
    f.removeAllFingerPrint();

	return 0;
}