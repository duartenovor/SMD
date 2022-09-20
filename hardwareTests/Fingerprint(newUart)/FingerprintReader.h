#ifndef CFingerprintREADER_H
#define CFingerprintREADER_H

#include "uart.h"
#include "Adafruit_Fingerprint.h"

using namespace std;

class CFingerprint{
    public:

        Adafruit_Fingerprint* finger;
        CFingerprint();
        ~CFingerprint();


        void getTemplates();
        int addFingerPrint();
        int removeFingerPrint(int ID);
        int removeAllFingerPrint();
        int searchFingerPrint();

        void setReadID(int ID);
        int getReadID();

    private:
        Adafruit_Fingerprint *fp;

        uint16_t readID;
        uint16_t last_Inserted;
        uint16_t ID;
};

#endif //CFingerprintREADER_H

