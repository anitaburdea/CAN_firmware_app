#ifndef CAN_H
#define CAN_H

#endif // CAN_H
#include <stdint.h>
#include "PCANBasic.h"

#define LANGUAGE_ENGLISH        0x09

void Check(char* id, TPCANStatus status);
TPCANStatus CAN_init(uint16_t channel, int Bitrate, uint8_t HwType, uint16_t portAddress, uint16_t portIntNumber);
TPCANStatus CAN_write(uint16_t channel, TPCANMsg *msg);
TPCANStatus CAN_read(uint16_t channel, TPCANMsg *msg, tagTPCANTimestamp *timeStamp);
TPCANStatus CAN_reset(uint16_t channel);
