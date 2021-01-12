#include "firmwaresend.h"
#include "can.h"
#include "QThread"


FirmwareSend::FirmwareSend(QObject *parent) : QObject(parent){

}

bool FirmwareSend::canInit = false;

bool FirmwareSend::CheckHelloMsg(TPCANMsg *recvMsg, tagTPCANTimestamp *timeStamp)
{
    if(CAN_read(PCAN_USBBUS1, recvMsg, timeStamp) == PCAN_ERROR_OK)
    {
        //Check Hello message from controller
        if(recvMsg->ID == 0x00 && recvMsg->LEN == 0x08
                && recvMsg->DATA[0] == 0x48 && recvMsg->DATA[1] == 0x65 && recvMsg->DATA[2] == 0x6C
                && recvMsg->DATA[3] == 0x6C && recvMsg->DATA[4] == 0x6F && recvMsg->DATA[5] == 0x20
                && recvMsg->DATA[6] == 0x21 && recvMsg->DATA[7] == 0x21)
        {
            return true;
        }
    }
    return false;
}


uint32_t FirmwareSend::CheckSum(uint8_t *data, uint8_t dataSize)
{
    uint32_t checkSum = 0;

    for(uint8_t i = 0; i < dataSize; i++)
    {
        checkSum += data[i];
    }

    return checkSum;
}

uint16_t FirmwareSend::calculate_flashCheckSum(void)
{
    TPCANMsg recvMsg;
    tagTPCANTimestamp timeStamp;
    TPCANStatus status;
    uint16_t checkSum = 0;

    do
    {
        status = CAN_read(PCAN_USBBUS1, &recvMsg, &timeStamp);

        //Read check sum from the flash memory
        if(status == PCAN_ERROR_OK && recvMsg.ID == 0x300)
        {
            checkSum = (recvMsg.DATA[1] << 8) | recvMsg.DATA[2];
        }

    }while(recvMsg.ID != 0x300);

    return checkSum;
}

void FirmwareSend::setFirmwareAddress(QString firmwareAddress)
{
    //Make a substring
    QStringRef firmAddress(&firmwareAddress, 2, 8);
    addressArray = QByteArray::fromHex(firmAddress.toLatin1());
}

void FirmwareSend::sendFile(QString fileLocation){

    FILE *fp;
    TPCANMsg recvMsg;
    TPCANMsg transMsg;
    tagTPCANTimestamp timeStamp;
    unsigned char str[8];
    uint8_t i = 0;
    uint16_t fileIndex = 0;
    int fileLen = 0;
    uint8_t counter = 0;

    QByteArray fLocatotion=fileLocation.toLatin1();
    const char *file=fLocatotion.data();

    fopen_s(&fp, file, "rb");
    if(fp == NULL){
        printf("Error opening file\n");
    }
    fseek(fp, 0, SEEK_END);
    fileLen=(int)ftell(fp);
    emit file_length(fileLen);

    //if user want to resend another firmware app we don't need to initialize CAN again
    if(!canInit)
    {
        if(CAN_init(PCAN_USBBUS1, PCAN_BAUD_1M, PCAN_TYPE_DNG_SJA, 0x00, 0x00) == PCAN_ERROR_OK)
        {
            if(FirmwareSend::CheckHelloMsg(&recvMsg,&timeStamp))
            {
                canInit = true;
            }
        }
        else
        {
            canInit = false;
            emit CAN_writeStatus(false);
        }
    }

    if(canInit)
    {
        //Send message to enter in bootloader mode
        transMsg.MSGTYPE = PCAN_MESSAGE_STANDARD;
        transMsg.ID = ENTER_BOOTLOADER_ID;
        transMsg.LEN = 0;

        if(CAN_write(PCAN_USBBUS1, &transMsg) == PCAN_ERROR_OK)
        {
            //Write firmware address
            transMsg.ID = HOST_USER_ADDRESS;
            transMsg.LEN = 4;
            std::memcpy(transMsg.DATA, addressArray, 4);

            QThread::msleep(10);

            if(CAN_write(PCAN_USBBUS1, &transMsg) == PCAN_ERROR_OK)
            {
                //Reset CAN buffer on controller
                transMsg.ID = RESET_CURRENT_FRAME_ID;
                transMsg.LEN = 0;

                if(CAN_write(PCAN_USBBUS1, &transMsg) == PCAN_ERROR_OK)
                {
                    QThread::msleep(10);

                    while(fileLen > 0)
                    {
                        if(fileLen > MAX_BYTES_TO_SEND)
                        {
                            //Flash programming is done by writing frames of 32 frames
                            for(i = 0; i < NUMBER_OF_FRAMES; i++)
                            {
                                //read from file
                                fseek(fp, fileIndex, SEEK_SET);
                                fread(str, 1, LENGTH_CAN_MSG, fp);
                                fileIndex += LENGTH_CAN_MSG;

                                QThread::msleep(10);

                                //send file
                                transMsg.ID = DATA_TO_SEND_ID + i;
                                transMsg.LEN = LENGTH_CAN_MSG;

                                std::reverse_copy(str, str + LENGTH_CAN_MSG, transMsg.DATA);

                                if(CAN_write(PCAN_USBBUS1, &transMsg) == PCAN_ERROR_OK)
                                {
                                    emit file_counter(fileIndex);
                                }
                                else
                                {
                                    emit CAN_writeStatus(false);
                                }
                            }
                            fileLen -= i*LENGTH_CAN_MSG;
                        }
                        //Writing last frames in flash
                        else if(fileLen < MAX_BYTES_TO_SEND)
                        {
                            for(i = 0; i < fileLen/LENGTH_CAN_MSG + 1; i++)
                            {
                                //read from file
                                fseek(fp, fileIndex, SEEK_SET);
                                fread(str, 1, LENGTH_CAN_MSG, fp);
                                fileIndex += LENGTH_CAN_MSG;

                                //send file
                                transMsg.ID = DATA_TO_SEND_ID + i;
                                transMsg.LEN = LENGTH_CAN_MSG;

                                std::reverse_copy(str, str + LENGTH_CAN_MSG, transMsg.DATA);

                                if(i == fileLen/LENGTH_CAN_MSG)
                                {
                                    //Set empty bytes with 0xFF
                                    memset(transMsg.DATA, 0xFF, sizeof(LENGTH_CAN_MSG - (fileLen % LENGTH_CAN_MSG)));
                                }
                                QThread::msleep(10);

                                if(CAN_write(PCAN_USBBUS1, &transMsg) == PCAN_ERROR_OK)
                                {
                                    emit file_counter(fileIndex);
                                }
                                else
                                {
                                    emit CAN_writeStatus(false);
                                }
                            }
                            //send empty frames
                            for(i = fileLen/LENGTH_CAN_MSG + 1; i < NUMBER_OF_FRAMES; i++)
                            {
                                transMsg.MSGTYPE = PCAN_MESSAGE_STANDARD;
                                transMsg.ID = DATA_TO_SEND_ID + i;
                                transMsg.LEN = LENGTH_CAN_MSG;
                                memset(transMsg.DATA, 0xFF, LENGTH_CAN_MSG);

                                QThread::msleep(10);

                                if(CAN_write(PCAN_USBBUS1, &transMsg) == PCAN_ERROR_OK)
                                {
                                    emit file_counter(fileIndex);
                                }
                                else
                                {
                                    emit CAN_writeStatus(false);
                                }
                            }
                            fileLen = 0;
                        }
                    }
                }
            }

            //Reset current fram on controller
            transMsg.ID = RESET_CURRENT_FRAME_ID;
            transMsg.LEN = 0;

            if(CAN_write(PCAN_USBBUS1, &transMsg) != PCAN_ERROR_OK)
            {
                emit CAN_writeStatus(false);
            }

            //jump to user app
            transMsg.ID = JUMP_TO_APP_ID;
            transMsg.LEN = 0;

            QThread::msleep(10);
            if(CAN_write(PCAN_USBBUS1, &transMsg) == PCAN_ERROR_OK)
            {
                //New firmware available on controller
                emit CAN_writeStatus(true);
            }
            else
            {
                emit CAN_writeStatus(false);
            }

            /*Reset current frame on controller twice
            Without this cannot jump to user app*/
            transMsg.ID = RESET_CURRENT_FRAME_ID;
            transMsg.LEN = 0;

            while(counter < 2)
            {
                QThread::msleep(10);

                if(CAN_write(PCAN_USBBUS1, &transMsg) == PCAN_ERROR_OK)
                {
                    counter++;
                }
                else
                {
                    emit CAN_writeStatus(false);
                }
            }
        }
        else
        {
            emit CAN_writeStatus(false);
        }
    }
    else
    {
        emit CAN_writeStatus(false);
    }

    fclose(fp);
    CAN_reset(PCAN_USBBUS1);
}
