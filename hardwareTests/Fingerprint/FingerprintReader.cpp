#include "FingerprintReader.h"

CFingerprint::CFingerprint() 
{
    char aux [64];
    strcpy(aux,"/dev/serial/by-id/usb-FTDI_FT232R_USB_UART_AB0K40B5-if00-port0");

    fp = new Adafruit_Fingerprint(aux);

    fp->begin(57600);

    if (fp->verifyPassword())
        printf("Found fingerprint sensor! \n");
    else
    {
        printf("Did not Find fingerprint sensor!\n");
    }

    this->ID = 1;
}

CFingerprint::~CFingerprint()
{
    delete fp;
}

int CFingerprint::getReadID(){
    return readID;
}

void CFingerprint::setReadID(int ID){
    this->readID=ID;
}

void CFingerprint::getTemplates(){
    fp->getTemplateCount();
    printf("%d\n",fp->templateCount);
}

int CFingerprint::addFingerPrint()
{
    ID = 1;
    //search where it can be stored!
    while (fp->loadModel(ID)==FINGERPRINT_OK){
        ID++;
    }

    /***************************/
    /*     Read fingerprint    */
    /***************************/

    printf("Place finger\n");
    int p = -1;
    while (p != FINGERPRINT_OK)
    {
        p = fp->getImage();
        switch (p)
        {
        case FINGERPRINT_OK:
            printf("Image taken\n");
            break;
        case FINGERPRINT_NOFINGER:
            break;
        case FINGERPRINT_PACKETRECIEVEERR:
            printf("Communication error");
            break;
        case FINGERPRINT_IMAGEFAIL:
            printf("Imaging error");
            break;
        default:
            printf("Unknown error");
            break;
        }
    }

    /***************************/
    /*   Convert fingerprint   */
    /***************************/

    // OK success!
    p = fp->image2Tz(1);
    switch (p)
    {
        case FINGERPRINT_OK:
            printf("Image converted\n");
            break;
        case FINGERPRINT_IMAGEMESS:
            printf("Image too messy");
            return p;
        case FINGERPRINT_PACKETRECIEVEERR:
            printf("Communication error");
            return p;
        case FINGERPRINT_FEATUREFAIL:
            printf("Could not find fingerprint features");
            return p;
        case FINGERPRINT_INVALIDIMAGE:
            printf("Could not find fingerprint features");
            return p;
        default:
            printf("Unknown error");
            return p;
    }

    printf("Remove finger\n");
    p = 0;
    while (p != FINGERPRINT_NOFINGER) {
        p = fp->getImage();
    }

    printf("Place same finger again\n");

    /***************************/
    /*    Reread fingerprint   */
    /***************************/
    while (p != FINGERPRINT_OK)
    {
        p = fp->getImage();
        switch (p)
        {
        case FINGERPRINT_OK:
            printf("Image taken\n");
            break;
        case FINGERPRINT_NOFINGER:
            break;
        case FINGERPRINT_PACKETRECIEVEERR:
            printf("Communication error");
            break;
        case FINGERPRINT_IMAGEFAIL:
            printf("Imaging error");
            break;
        default:
            printf("Unknown error");
            break;
        }
    }


    /***************************/
    /*   Convert fingerprint   */
    /***************************/
    // OK success!
    p = fp->image2Tz(2);
    switch (p)
    {
    case FINGERPRINT_OK:
        printf("Image converted\n");
        break;
    case FINGERPRINT_IMAGEMESS:
        printf("Image too messy");
        return p;
    case FINGERPRINT_PACKETRECIEVEERR:
        printf("Communication error");
        return p;
    case FINGERPRINT_FEATUREFAIL:
        printf("Could not find fingerprint features");
        return p;
    case FINGERPRINT_INVALIDIMAGE:
        printf("Could not find fingerprint features");
        return p;
    default:
        printf("Unknown error");
        return p;
    }

    // OK converted!
    //printf("Creating model for #%d\n", ID);

    /***************************/
    /*  Check if fingers match */
    /***************************/

    p = fp->createModel();
    if (p == FINGERPRINT_OK)
    {
        printf("Prints matched!\n");
    }
    else if (p == FINGERPRINT_PACKETRECIEVEERR)
    {
        printf("Communication error\n");
        return p;
    }
    else if (p == FINGERPRINT_ENROLLMISMATCH)
    {
        printf("Fingerprints did not match\n");
        return p;
    }
    else
    {
        printf("Unknown error\n");
        return p;
    }

    /***************************/
    /*       Store Model       */
    /***************************/
    if(p == FINGERPRINT_OK)
    {
        p = fp->storeModel(ID);
        if (p == FINGERPRINT_OK)
        {
            printf("Stored in ID: %d!\n",ID);
            return p;
        }
        else if (p == FINGERPRINT_PACKETRECIEVEERR)
        {
            printf("Communication error");
            return p;
        }
        else if (p == FINGERPRINT_BADLOCATION)
        {
            printf("Could not store in that location");
            return p;
        }
        else if (p == FINGERPRINT_FLASHERR)
        {
            printf("Error writing to flash");
            return p;
        }
        else
        {
            printf("Unknown error");
            return p;
        }
    }
}


int CFingerprint::removeFingerPrint(int ID)
{
    return fp->deleteModel(ID);
}

int CFingerprint::removeAllFingerPrint()
{
    return fp->emptyDatabase();

}


int CFingerprint::searchFingerPrint(){

    /***************************/
    /*     Read fingerprint    */
    /***************************/
    int p = -1;

    while (p != FINGERPRINT_OK)
    {
        p = fp->getImage();
    }
    p = fp->image2Tz(1);

    int found = fp->fingerFastSearch();

    int search_id =  fp->fingerID;
    int confidence = fp->confidence;


    printf("Searching for fingerprint...\n");
    if(found == FINGERPRINT_OK)
    {     
        printf("ID: %d \nConfidence: %d\n" ,search_id,confidence);
        return 1;
    }
    else if(found == FINGERPRINT_NOTFOUND)
    {
        printf("Finger not found!\n\n");
        return -1;
    }
    else if(found == FINGERPRINT_PACKETRECIEVEERR)
    {
        printf("Communication error!\n\n");
        return -1;
    }
}
