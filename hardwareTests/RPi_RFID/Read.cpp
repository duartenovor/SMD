#include "MFRC522.h"
#include <unistd.h>

int main()
{
  MFRC522 mfrc;

  mfrc.PCD_Init();

  printf("RFID Module\n");

  while(1){
    // Look for a card
    if(!mfrc.PICC_IsNewCardPresent())
      continue;

    printf("New card detected!\n");

    if( !mfrc.PICC_ReadCardSerial())
      continue;

    printf("UID: ");

    // Print UID
    for(byte i = 0; i < mfrc.uid.size; ++i){
      if(mfrc.uid.uidByte[i] < 0x10){
      	printf(" 0");
      	printf("%X",mfrc.uid.uidByte[i]);
      }
      else{
      	printf(" ");
      	printf("%X", mfrc.uid.uidByte[i]);
      }
    }
    printf("\n");
    usleep(1000000);
  }
  return 0;
}
