#include "basic.h"

#define PACK_START 0xff
#define PACK_TAIL 0xff
#define MAX_LEN 256

void frame_build(uint8_t rsctl, uint8_t *frame, uint8_t *data, uint16_t data_len);
void frame_parse(uint8_t *frame, uint8_t *data, uint8_t *rsctl, uint16_t frame_len, uint16_t *data_len);
void data_preprocess(uint8_t *data, uint8_t *datawating, uint16_t data_len, uint16_t *len, char dir);
void data_build(uint8_t *data, uint16_t *data_len);

typedef struct
{
	uint8_t FrameType;
	uint8_t RSUStatus;
	uint8_t PSAMNUM;
	uint8_t RSUTerminalId1[15];
	uint8_t RSUTerminalId2[15];
	uint8_t RSUAlgId;
	uint8_t RSUManuID;
	uint8_t RSUIndividualID[10];
	uint8_t RSUVersion[10];
	uint8_t Reserved[15];
}frameB0;

typedef struct
{
	uint8_t FrameType;
	uint8_t OBUID[10];
	uint8_t ErrorCode;
	uint8_t ContractProvider[15];
	uint8_t ContractType;
	uint8_t ContractVersion;
	uint8_t ContractSerialNumber[15];
	uint8_t ContractSignedDate[10];
	uint8_t ContractExpiredDate[10];
	uint8_t Equitmentstatus;
	uint8_t OBUStatus[10];
}frameB2;

typedef struct
{
	uint8_t FrameType; 
	uint8_t OBUID[10];
	uint8_t ErrorCode;
	uint8_t Vehicle_info[85];
}frameB3;

typedef struct
{
	uint8_t FrameType;
	uint8_t OBUID[10];
	uint8_t ErrorCode;
	uint8_t CardType;
	uint8_t CardRestMoney[10];
	uint8_t CardData[50];
	uint8_t ChargeData[50]; 
	uint8_t ReadEf04Status;
	uint8_t Ef04Info[MAX_LEN+5];
}frameB4;

typedef struct
{
	uint8_t FrameType;
	uint8_t OBUID[10];
	uint8_t ErrorCode;
	uint8_t WrFileTime[10];
	uint8_t PSAMNo[15];
	uint8_t TransTime[15];
	uint8_t TransType;
	uint8_t TAC[10];
	uint8_t ICCPayserial[10];
	uint8_t PSAMTransSerial[10];
	uint8_t CardRestMoney[10];
	uint8_t KeyType;
}frameB5;

typedef struct
{
	uint8_t FrameType;
	uint8_t ErrorCode;
	uint8_t ChannelID;
	uint8_t PSAMNO[15];
	uint8_t PSAMVersion;
	uint8_t AreaCode[10];
	uint8_t RandCode[15];
	uint8_t PSAMterminalNo[15];
}frameBD;

typedef struct
{
	uint8_t FrameType;
	uint8_t ErrorCode;
	uint8_t ChannelID;
	uint8_t SW1SW2[10];
}frameBF;

typedef struct
{
	uint8_t FrameType; 
	uint8_t OBUID[10];
	uint8_t ErrorCode;
	uint8_t EF04Info[MAX_LEN+5];
}frameB9;

typedef struct
{
	uint8_t CMDType;
	uint8_t Seconds[4];
	uint8_t Datetime[7];
	uint8_t LaneMode;
	uint8_t WaitTime;
	uint8_t TxPower;
	uint8_t PLLChannelID;
	uint8_t TransClass;
	uint8_t Ef04Opt;
	uint8_t Ef04OffSet[2];
	uint8_t Ef04Len[2];
}frameC0;

typedef struct
{
	uint8_t CMDType;
	uint8_t OBUID[4];
}frameC1;

typedef struct
{
	uint8_t CMDType;
	uint8_t OBUID[4];
	uint8_t StopType;
}frameC2;

typedef struct
{
	uint8_t CMDType;
	uint8_t OBUID[4];
	uint8_t TradeMode;
	uint8_t ConsumeMoney[4];
	uint8_t Station[40];
	uint8_t DateTime[7];
	uint8_t Ef04OffSet[2];
	uint8_t Ef04Len[2];
	uint8_t Ef04Info[MAX_LEN];
}frameC6;

typedef struct
{
	uint8_t CMDType;
	uint8_t OBUID[4];
	uint8_t DateTime[7];
	uint8_t OpMode;
	uint8_t Ef04OffSet[2];
	uint8_t Ef04Len[2];
	uint8_t Ef04Info[MAX_LEN];
}frameC7;

typedef struct
{
	uint8_t CMDType;
	uint8_t AntennaStatus;
}frame4C;

typedef struct
{
	uint8_t CMDType;
	uint8_t ChannelID;
}frameCD;

typedef struct
{
	uint8_t CMDType;
	uint8_t ChannelID;
	uint8_t MacCode[8];
}frameCF;

typedef struct
{
	uint8_t CMDType;
	uint8_t OBUID[4];
	uint8_t Ef04OffSet[2];
	uint8_t Ef04Len[2];
}frameC9;

struct EF04Opt
{
	uint8_t EF04Opt;
	uint16_t EF04offset;
	uint16_t EF04len;
};
