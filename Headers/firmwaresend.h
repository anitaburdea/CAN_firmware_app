#ifndef FIRMWARESEND_H
#define FIRMWARESEND_H

#include <QString>
#include <QObject>
#include "Headers/can.h"

#define MAX_BYTES_TO_SEND       256
#define LENGTH_CAN_MSG          8
#define NUMBER_OF_FRAMES        32

enum MESSAGE_ID
{
    ENTER_BOOTLOADER_ID = 0x00,
    HOST_USER_ADDRESS = 0x10,
    RESET_CURRENT_FRAME_ID = 0x20,
    JUMP_TO_APP_ID = 0x90,
    DATA_TO_SEND_ID = 0x100
};


class FirmwareSend : public QObject
{
    Q_OBJECT
public:
    explicit  FirmwareSend(QObject *parent= nullptr);
    static bool canInit;
    QByteArray addressArray;
signals:
    void file_length(uint16_t length);
    void file_counter(uint16_t counter);
    void CAN_writeStatus(bool canWriteStatus);
public slots:
    void sendFile(QString fileLocation);
    void setFirmwareAddress(QString firmwareAddress);
private:
    uint16_t calculate_flashCheckSum(void);
    uint32_t CheckSum(uint8_t *data, uint8_t dataSize);
    bool CheckHelloMsg(TPCANMsg *recvMsg, tagTPCANTimestamp *timeStamp);
};
#endif // FIRMWARESEND_H
