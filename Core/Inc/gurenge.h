#pragma once
#include "layerni.h"

//ê€§œ§…§≥§´§È§‰§√§∆§Ø§Î§Œ§«§∑§Á°°◊‘∑÷§Œ–ÿ§ÀÜñ§§§´§±§ø
//struct below 116 bytes
//from head of paragraph to FCS
//#include <cstdlib>
//#include <cstdint>


class _BST
{
	public:
	uint8_t BST;
	uint8_t BaconID[4];
	uint8_t unixTime[4];
	uint8_t profile_integer;
	uint8_t mandApplications;
	uint8_t option_indicator2;
	uint8_t option_indicator3;
	uint8_t pretreatPara;
	uint8_t option_indicator4;
	uint8_t sysInfo;
	uint8_t len2[2];
	uint8_t len12[2];
	uint8_t len15[2];
	uint8_t len19[2];
	uint8_t profile_list;
	_BST() {}
	_BST(uint8_t *BaconID4, uint8_t *unixTime4, uint8_t profileInteger,
		uint8_t sysinfo, uint8_t *len22, uint8_t *len122, uint8_t *len152, 
		uint8_t *len192)
	{
		BST = 0xC0;
		BaconID[0] = BaconID4[0];
		BaconID[1] = BaconID4[1];
		BaconID[2] = BaconID4[2];
		BaconID[3] = BaconID4[3];
		unixTime[0] = unixTime4[0];
		unixTime[1] = unixTime4[1];
		unixTime[2] = unixTime4[2];
		unixTime[3] = unixTime4[3];
		profile_integer = profileInteger;
		mandApplications = 1;
		option_indicator2 = 0x41;
		option_indicator3 = 0x87;
		pretreatPara = 0x29;
		option_indicator4 = 0;
		sysInfo = sysinfo;
		len2[0] = len22[0];
		len2[1] = len22[1];
		len12[0] = len122[0];
		len12[1] = len122[1];
		len15[0] = len152[0];
		len15[1] = len152[1];
		len19[0] = len192[0];
		len19[1] = len192[1];
		profile_list = 0;
	}
	void msg_construct(uint8_t* msg);
	void update_unix(uint8_t* unix);
	uint8_t lengthwa()
	{
		return 25;
	}
};

void _BST::msg_construct(uint8_t* msg)
{
	msg[0] = BST;

	// øΩ±¥ BaconID
	msg[1] = BaconID[0];
	msg[2] = BaconID[1];
	msg[3] = BaconID[2];
	msg[4] = BaconID[3];

	// øΩ±¥ unixTime
	msg[5] = unixTime[0];
	msg[6] = unixTime[1];
	msg[7] = unixTime[2];
	msg[8] = unixTime[3];

	// ºÃ–¯øΩ±¥∆‰À˚◊÷∂Œ
	msg[9] = profile_integer;
	msg[10] = mandApplications;
	msg[11] = option_indicator2;
	msg[12] = option_indicator3;
	msg[13] = pretreatPara;
	msg[14] = option_indicator4;
	msg[15] = sysInfo;

	// øΩ±¥ len2
	msg[16] = len2[0];
	msg[17] = len2[1];

	// øΩ±¥ len12
	msg[18] = len12[0];
	msg[19] = len12[1];

	// øΩ±¥ len15
	msg[20] = len15[0];
	msg[21] = len15[1];

	// øΩ±¥ len19
	msg[22] = len19[0];
	msg[23] = len19[1];

	// øΩ±¥ profile_list
	msg[24] = profile_list;

}

void _BST::update_unix(uint8_t* unix)
{
	unixTime[0] = unix[0];
	unixTime[1] = unix[1];
	unixTime[2] = unix[2];
	unixTime[3] = unix[3];
}

class _GetSecureRq
{
public:
	_GetSecureRq();
	_GetSecureRq(uint8_t *RndRSU8, uint8_t keyIDforAuthen1, uint8_t keyIDforEncrypt1);
	uint8_t action_request;
	uint8_t did;
	uint8_t actionType;
	//uint8_t accessCredential[8];
	uint8_t getsecurerq;
	uint8_t keyIDforEncryptOp;
	uint8_t fid;
	uint8_t offset[2];
	uint8_t length;
	uint8_t RndRSU[8];
	uint8_t keyIDforAuthen;
	uint8_t keyIDforEncrypt;
	void msg_construct(uint8_t* msg);
	uint8_t lengthwa()
	{
		return 19;
	}
};

_GetSecureRq::_GetSecureRq()
{

}

_GetSecureRq::_GetSecureRq(uint8_t *RndRSU8, uint8_t keyIDforAuthen1,
	uint8_t keyIDforEncrypt1)
{
	action_request = 0x05;
	did = 1;
	actionType = 0;
	//accessCredential
	getsecurerq = 0x14;
	keyIDforEncryptOp = 0x80;
	fid = 1;
	offset[0] = 0;
	offset[1] = 0;
	length = 0x10;
	RndRSU[0] = RndRSU8[0];
	RndRSU[1] = RndRSU8[1];
	RndRSU[2] = RndRSU8[2];
	RndRSU[3] = RndRSU8[3];
	RndRSU[4] = RndRSU8[4];
	RndRSU[5] = RndRSU8[5];
	RndRSU[6] = RndRSU8[6];
	RndRSU[7] = RndRSU8[7];
	keyIDforAuthen = keyIDforAuthen1;
	keyIDforEncrypt = keyIDforEncrypt1;
}

void _GetSecureRq::msg_construct(uint8_t* msg)
{
	msg[0] = action_request;
	msg[1] = did;
	msg[2] = actionType;

	msg[3] = getsecurerq;
	msg[4] = keyIDforEncryptOp;
	msg[5] = fid;

	// øΩ±¥ offset£®2◊÷Ω⁄£©
	msg[6] = offset[0];
	msg[7] = offset[1];

	msg[8] = length;

	// øΩ±¥ RndRSU£®8◊÷Ω⁄£©
	for (int i = 0; i < 8; ++i) {
		msg[9 + i] = RndRSU[i];
	}

	msg[17] = keyIDforAuthen;
	msg[18] = keyIDforEncrypt;

}

//Additional APDU inst required
class _TransferChannelRq
{
public:
	_TransferChannelRq();
	_TransferChannelRq(uint8_t APDULIST1);
	uint8_t action_request;
	uint8_t did;
	uint8_t actionType;
	uint8_t ChannelRq;
	uint8_t channelID;
	uint8_t APDULIST;
	
	void msg_construct(uint8_t* msg);
	uint8_t lengthwa()
	{
		return 6;
	}
};

_TransferChannelRq::_TransferChannelRq()
{
}

_TransferChannelRq::_TransferChannelRq(uint8_t APDULIST1)
{
	action_request = 0x05;
	did = 1;
	actionType = 0x03;
	ChannelRq = 0x18;
	channelID = 0x01;
	APDULIST = APDULIST1;
}

void _TransferChannelRq::msg_construct(uint8_t* msg)
{
	msg[0] = action_request;
	msg[1] = did;
	msg[2] = actionType;
	msg[3] = ChannelRq;
	msg[4] = channelID;
	msg[5] = APDULIST;

}

class _EventReportRq
{
public:
	_EventReportRq();
	uint8_t event_report_request;
	uint8_t did;
	uint8_t eventType;
	void msg_construct(uint8_t* msg);
	uint8_t lengthwa()
	{
		return 3;
	}
};

_EventReportRq::_EventReportRq()
{
	event_report_request = 0x60;
	did = 1;
	eventType = 0;
}

void _EventReportRq::msg_construct(uint8_t* msg)
{
	msg[0] = event_report_request;
	msg[1] = did;
	msg[2] = eventType;
}
