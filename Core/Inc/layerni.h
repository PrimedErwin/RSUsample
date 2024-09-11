#pragma once
#include "basic.h"

#define FRAME_START 0x7E //0111 1110 GB/T 20851.2
#define FRAME_END 0x7E
//#ifndef HAL_CRC_MODULE_ENABLED
//	#define HAL_CRC_MODULE_ENABLED
//#endif


extern uint8_t lane_dir;//1=OBU->RSU, 0=RSU->OBU
extern uint8_t LSDU_exist;// 1=yes 0=no
extern uint8_t CR;//0 = command, 1 = response
extern uint8_t private_lane;//MAC address = 0xff then this is valid, 1=looking for lane, 0=not

typedef struct{
	uint8_t macadd[4];
	uint8_t macctl;
}Macs;
/*
ENUM for tokubezu parameters
enum unit is U
remember to transfer
*/
typedef enum{
	private_LPDU_exist = 0x0A,
	unprivate_LPDU_exist = 0x02,
	private_LPDU_noexist = 0x08,
	unprivate_LPDU_noexist = 0x00
	//private means looking for private connection
}macctl_down;
//down for rsu transmit

typedef enum{
	LPDU_exist = 0x07,
	LPDU_noexist = 0x05
}macctl_up;
//up for obu transmit

typedef enum{
    OBU_init = 99,
    OBU_VST = 100,
    OBU_baseinfo,
    OBU_00190002,
    OBU_EF04,
    OBU_EF04update,
    OBU_0019,
    OBU_0002,
    RSU_BST,
    RSU_baseinfo,
    RSU_00190002,
    RSU_EF04,
    RSU_EF04update,
    RSU_0019,
    RSU_0002,
		RSU_release
}Commu_Stage;

/*
Communication Stage records what step has reached right now, and switches when recved msg that lead to current stage.
for example, communicate with double piece(?) OBU on term1(without ACK type):
RSU->BST, read file 0015 0019
OBU->VST
(To lane controller: OBU exist)
RSU->(recorded private mac)get vehicle info
OBU->vehicle info
RSU->read file 0019 0002
OBU->0019 0002
RSU->read file EF04 byte 315-405
OBU->EF04 byte 315-405
(To lane controller: Trade)
RSU->update EF04
OBU->update ensured
RSU->initiate trade, update 0019
OBU->update ensured
RSU->traded, read 0002
OBU->0002
(To lane controller: Finished)
RSU->release connection

As described above, we index these stage with current status:
for OBU: OBU_VST, OBU_baseinfo, OBU_00190002, OBU_EF04, OBU_EF04update, OBU_0019, OBU_0002
for RSU: RSU_.............................................................................,RSU_release
*/

void recv_parse(uint8_t *frame, uint8_t *mac_address, uint8_t *mac_control, uint8_t *LSDU, uint8_t *FCS, uint8_t *LSDU_len);
void recv_mac_control(uint8_t mac_control);
void recv_build(uint8_t *frame, uint8_t *LSDU, Macs *mac, uint8_t LSDU_len);
void LSDU_build(uint8_t *LSDU, uint8_t *Msg, uint8_t Msg_len);
/*
void ACK_data_trans(Macs *mac, uint8_t *frame);
void ACK_data_recv(Macs *mac, uint8_t *frame);
void ACK_data_status(Macs *mac, uint8_t *status);
void ACK_reply_trans(Macs *mac, uint8_t *frame);
void ACK_reply_recv(Macs *mac, uint8_t *frame);
void ACK_reply_status(Macs *mac, uint8_t *status);
void ACK_save(Macs *mac, uint8_t *frame);
void ACK_save_status(Macs *mac, uint8_t *status);
*/
