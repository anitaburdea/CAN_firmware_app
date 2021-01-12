#include <QCoreApplication>
#include "can.h"

//Checks if the status is an error code and displays its error message
void Check(char* id, TPCANStatus status){

    if (status != PCAN_ERROR_OK)
    {
        char buf[50];
        buf[0] = '\0';
        CAN_GetErrorText(status, LANGUAGE_ENGLISH, buf);
        printf("%s: failed, stat=%d (%s)\n", id, (int)status, buf);
    }
}
TPCANStatus CAN_init(uint16_t channel, int Bitrate, uint8_t HwType,
              uint16_t portAddress, uint16_t portIntNumber){

    TPCANStatus sts;

    sts = CAN_Initialize(channel, Bitrate, HwType, portAddress, portIntNumber);
    if(sts != PCAN_ERROR_OK)
    {
       Check((char *)"CanInitialize", sts);
    }
    printf("Can bus was initialized\n");

    return sts;
}
TPCANStatus CAN_write(uint16_t channel, TPCANMsg *msg){

     TPCANStatus sts;

      printf("Writing a message to the channel and waiting for it to be sent \n");
      sts = CAN_Write(channel, msg);

      if(sts != PCAN_ERROR_OK)
      {
          Check((char *)"CanWrite", sts);
      }
      printf("Can message has sent\n");

      return sts;
}

TPCANStatus CAN_read(uint16_t channel, TPCANMsg *msg, tagTPCANTimestamp *timeStamp)
{
    TPCANStatus sts;

    printf("Reading a message and waiting for it to be read\n");
    sts = CAN_Read(channel, msg, timeStamp);

    if(sts != PCAN_ERROR_OK)
    {
        Check((char *)"CanRead", sts);
    }
    printf("Can message has read\n");

    return sts;
}

TPCANStatus CAN_reset(uint16_t channel){

     TPCANStatus sts;

     printf("Going to reset bus and channel");
     sts = CAN_Reset(channel);

     if(sts != PCAN_ERROR_OK)
     {
         Check((char *)"CanReset", sts);
     }
     printf("Can bus and channel has reseted\n");

     return sts;
}
