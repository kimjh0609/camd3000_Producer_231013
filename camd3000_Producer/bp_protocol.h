#ifndef __BP_PROTOCOL__
#define __BP_PROTOCOL__

#define UART0_RCV_MAX	(1024 + 128)
#define PKT_HEAD_SIZE	16
#define SEND_MAX_SIZE	(1024 + 128)

#define STX0											0xAA
#define STX1											0x55

#define ETX0											0xAA
#define ETX1											0x66

#define HVID_BP										0x02
#define HDID_BP										0x01

#define BP_PACKET_UPDATE_APP			0x01	//FTP ���
#define BP_PACKET_UPDATE_HAPP			0x21 	// HTTP ��� 
#define BP_PACKET_UPDATE_OBD			0x02
#define BP_PACKET_UPDATE_FBP			0x04
#define BP_PACKET_UPDATE_BLE			0x05
#define BP_PACKET_UPDATE_NODE			0x06
#define BP_PACKET_UPLOAD					0x08
#define BP_PACKET_MAKEFILE				0x10
#define BP_PACKET_CERT						0x20
#define BP_PACKET_MEDIAFILE				0x30

#define BP_PACKET_QM_COMMAND			0x11
#define BP_PACKET_M4_COMMAND			0x22

#define BP_PACKET_QM_SLEEP				0x01
#define BP_PACKET_QM_WAKEUP				0x02
#define BP_PACKET_QM_ATCOMMAND		0x03

#define BP_PACKET_M4_SLEEP				0x01

#define BP_PACKET_M4_SERIAL_NO		0x04
#define BP_PACKET_M4_HOST_IP			0x05
#define BP_PACKET_M4_SERVICE			0x06
#define BP_PACKET_M4_APN					0x07
#define BP_PACKET_M4_JIG_STATUS		0x08

#define BP_PACKET_M4_RESET        0x02
#define BP_PACKET_M4_GET_APN			0x0E
#define BP_PACKET_M4_GET_CONFIG		0x0F
#define BP_PACKET_M4_GET_INFO			0x10
#define BP_PACKET_M4_SET_HWMODE		0x11
#define BP_PACKET_M4_SET_DBG			0x20
#define BP_PACKET_M4_SET_HWVER		0x21

#define BP_PACKET_M4_SET_PAYLOAD	0x31  //PAYLOAD test 

typedef struct _FILE_INFO_ {
	TCHAR szFilePath[4096];    // ������ ��� �� �̸��� �����ϱ� ���� ���ڿ�
	int cur_no;
	int total_no;
	int file_size;
}FILE_INFO;


#pragma pack(push, 1)       // 1����Ʈ ũ��� ����
typedef struct _TRIP_ID_
{
	unsigned char trip_year;                  // ?(1B)        2018 - 2000 : 0 ~ 255
	unsigned char trip_month;                 // ?(1B)        1 ~ 12
	unsigned char trip_day;                   // ?(1B)        1 ~ 31
	unsigned char trip_hour;                  // ?(1B)        0 ~ 23
	unsigned char trip_min;                   // ?(1B)        0 ~ 59
	unsigned char trip_num;                   // ????(1B) 0 ~ 255 ????.
} TRIP_ID, * PTRIP_ID; 

typedef struct _IMU_DATA_
{
	short x_axis;       // ??
	short y_axis;       // ??
	short z_axis;       // ??
} IMU_DATA, * PIMU_DATA;

typedef struct _BP_PACKET_ {
	unsigned char stx1;		// = 0xAA;
	unsigned char stx2;		// = 0x55;

	unsigned char data_len1;
	unsigned char data_len2;

	unsigned char hvid_bp;	// HVID_BP;
	unsigned char hdid_bp;	// HDID_BP;
	unsigned char command;
	unsigned char type;

	unsigned short cur_no;
	unsigned short total_no;

	unsigned char payload;	// data[] + crc + etx1, etx2

//	unsigned short crc;

//	unsigned char etx1;		// = 0xAA;
//	unsigned char etx2;		// = 0x66;
} BP_PACKET, * PBP_PACKET;

typedef struct _BP_INFO_PACKET_ {
	unsigned char file_name[8];
	unsigned char file_type;
	unsigned char pad[3];
	unsigned int  file_len;
	unsigned int  total_no;
	unsigned char file_ver[4];
	unsigned char app_ver[4];
	unsigned short crc;
	unsigned char  dummy[2 + 32];
} BP_INFO_PACKET, * PBP_INFO_PACKET;	// total 64
#pragma pack(pop)   // ���� ������ ���� ����(�⺻��)�� �ǵ���

#define ATI_INFO_MAX 40
#define STATE_BUF_MAX 8
#define MAX_DTC_CODE        10
#define CONFIG_ETC_LEN_MAX 156

#pragma pack(push, 1)       // 1����Ʈ ũ��� ����
typedef struct _CONFIG_DAT_
{
	unsigned int magic;				// 0x5A5AA5A5
	char serial_no[32];			  // 7byte ��� 
	char host_ip[128];
	unsigned short port_no;
	char service_nm[32];
	unsigned char hwVER[2];
	unsigned char dbgFlag;
	unsigned char pad1[5];
	unsigned int service_iot_NO;
	char reserved[64];
	unsigned char device_SN[12];  // �ʿ�� ��ǰ ���� ��ȣ�� ��� (KSTI)
	unsigned char ucEBPServerVersion[2];
	///////////////////////////////////
	unsigned char etc[CONFIG_ETC_LEN_MAX];  //SSL_DATA
//////////////////////////////////
	unsigned short chksum;
} CONFIG_DAT, * PCONFIG_DAT;  // DROM_CONFIG_DAT_SIZE_MAX

typedef struct _QM_INFO_ {
	unsigned int baudrate;

	unsigned int ati_idx;
	unsigned char ati_info[3][ATI_INFO_MAX];	// Manufacturer ID, Device module and Firmware version

	unsigned char imei_gsn[24];		// IMEI of module
	unsigned char imsi_sim[24];		// IMSI of SIM card
	unsigned char cnum[20];		// ��ȭ��ȣ of SIM card

	unsigned char iccid_sim[24]; 		// ICCID number of SIM card

	unsigned char csq_info[24];		// current signal quality
	unsigned char cop_info[24];		// current Network Operator

	unsigned char apn[32];

	char stat[4][STATE_BUF_MAX];			// the network registration status., Open/Connect a Network for MQTT Client status

} QM_INFO, * PQM_INFO;

typedef struct _OBD_TRIP_MAP_
{
	unsigned short	magicA;
	unsigned short	magicB;
	long 			tStamp;

	//
	// Tatal Data
	//
	TRIP_ID trip_id;					// 6
	char padding0[2];

	long igni_off_total_count;			// �� Data
	long igni_on_total_count;			// �� Data

	long acc_total_dist;				// [0.1Km]�� ���� �Ÿ�

	char qm_vendor[8];
	char model[8];
	// Activation- �ܸ��� �� ��Ÿ���� - �õ� On �� �׻� ����
	// firmware version	�߿����� ���� ����. QM Revision(20), APP(4), OBD(4), DB(15)
	char qm_revision[20];				// quectel revision
	char app_version[4];				// Major 2Byte + Minor 2Byte 0xC000 0x0001

	char vehicle_id[17];				// ������ �����ȣ ���� (Vehicle Identification Number)
	char padding1;

	unsigned short fuel_tank_capacity;	// ������ũ �뷮 - 0.1L

	char dev_imei[15];					// IMEI	�ܸ��� ��Ÿ���� IMEI ���� 15�ڸ��� ��.
	char padding2;
	char phone_number[11];				// �ܸ��� ��Ÿ���� PHONE Number 11�ڸ��� ��.
	char padding3;

	///////////////////////////////////////////////////
	// 114 = 2+2+4+6+2+ 4 * 3 + 8 *2 + 20 + 4 + 17+1 + 2 + 15+1  + 11+1   index�� �� �� ���� ���� 
	///////////////////////////////////////////////////
	char config_data1[24];	//
	char config_data2[24];	//BLE_DATA		
	char config_data3[48];	//DC_CONFIG_DATA		
	char config_data_reserved[24 * 6];
	////////////////////////////////////////////////////
	////////////////////////////////////////////////////

	unsigned char imu_event_count;
	IMU_DATA imu_event;

	//
	// Current Data
	//
	unsigned char trip_step;			// trip_step
										// 0 : parking
										// 1 : ignition on
										// 2 : running
										// 3 : event & running
										// 4 : ignition off

#if true//def USE_PROTOCOL_DC
	unsigned short trip_rtd_id;
	unsigned short trip_rld_id;
	unsigned short gps_id;
	unsigned short ka_id;
	unsigned long pk_id;
	unsigned char FaultCnt;
	char resereved[19];					// IMEI	�ܸ��� ��Ÿ���� IMEI ���� 15�ڸ��� ��.
#else
	unsigned long trip_rtd_id;			// (0 ~ 999999) RTD�� �����Ҷ����� 1�� ����, Trip ID ������ ���� �ʱ�ȭ
	unsigned short trip_rld_id;			// (1 ~ 60) RLD�� �����ɶ����� 1�� ����, RTD ������ �ʱ�ȭ
#endif

	volatile unsigned char ignition;	// ���� �õ� ����
	unsigned char state_H;				// 0x5F, 0x6F, 0x7F .... 0x50, 0x60, 0x70
	unsigned char state_L;				// reserver

	unsigned char igni_on_count;		//

	long igni_on_t;						// �õ� ON �ð� �õ��� ���� �ð� - �ʴ��� �ð����� ȯ�갪

	long trip_start_t;					// Trip ���� �ð� - �ʴ��� �ð����� ȯ�갪  ==> ������ ���� ���� ��  �ð� ???
	long igni_off_t;					// Trip ���� �ð� - �ʴ��� �ð����� ȯ�갪

	long trip_run_time;					// �õ� �� ���� �ð� (sec)
	float trip_run_dist; 				// �õ� �� ���� �Ÿ�(�Ǽ�) // [m] 0xFFFFFFFF(12:�ӵ� ���� �̻� )
	float trip_fuel;					//[ml] �õ� �� ���� �Ҹ�
	float	idle_time;			//[sec], 	�õ�  ��ȸ�� �ð�		

	unsigned char	AccPos; 		// Acc					// [%] -1		noData : 0x00
	unsigned char engine_oil_temp;		// �������� ���	�������� �µ��� DTC (standard PID�� �����ϸ� �������� ��տµ�)	1B	BYTE	C	�����µ� �ְ� ����, +40 ����
	unsigned char ibs_info[2];			// IBS info	���͸� ���� �� �ܿ��� �ܿ� ���� (0 ~ 100)%, ���� �ܿ��� (0 ~ 100)% - �� 1Byte ��

	unsigned char mission_oil_temp;		// �̼ǿ��� �µ�	�̼ǿ��� �µ��� DTC (standard PID�� �����ϸ� �������� ��տµ�)	1B	BYTE	C	�̼ǿµ� �ְ� ����. +40 ����
	char	FR_State;		// ���� �ܷ� ���� ����

	unsigned char fuelFL;				// [%]��ũ���� raw ����Ÿ
	unsigned short batt_volt;


	long start_latitude;				// ���� ���� ��ġ ���� long
	long start_longitude;				// ���� ���� ��ġ �浵 long

	long stop_latitude;					// ���� ���� ��ġ ���� long
	long stop_longitude;				// ���� ���� ��ġ �浵 long

	unsigned short imu_warning_param1;	// 2019.03.26 008 version add
	unsigned short imu_warning_param2;	// 2019.03.26 008 version add
	unsigned short batt_warning_param;	// 2019.03.26 008 version add

	//unsigned char padding22[4];
	char obd_version[4];
	char tiObd_version[4];
	char db_version[4];


	char carCode[7];
	unsigned char padding33[1];

	unsigned short igni_on_fuel;		// �õ�ON ������ũ �ܿ���	 �õ�ON ������ ������ũ ��	2B	USHORT	100ml	x10 ����
	IMU_DATA	igni_on_imu;			// �õ�ON IMU ��	�õ�ON IMU ��	6B	USHORT * 3	�� 0.01 Res	����, ����, ���� �� 2Byte - x10 ����

	unsigned short igni_off_fuel;		// �õ�OFF ������ũ �ܿ���	�õ�ON������ ������ũ ��	2B	USHORT	100ml	x10 ����
	IMU_DATA 	igni_off_imu;		// �õ�OFF IMU ��	�õ�ON IMU ��	6B	USHORT * 3	�� 0.01 Res	����, ����, ���� �� 2Byte - x10 ����


	unsigned char coolant_temp;		// �ð��� �µ� 	1B	BYTE	C	�����µ� �ְ� ����, +40 ����

	unsigned char device_tmp;			// CAR (Device) �µ� 1 BYTE  C -40 C ~ , +40���� ����
	char padding4;
	unsigned short tpms[4]; 			// Ÿ�̾� �����   0 not support, val : 1 ~  xxx , unit =  KPA

	unsigned char warm_up_time;			// ���� �ð�	�õ� ON�� ����Ÿ��� 0�� ������ �ð� 1 BYTE ��
	long fuel_cut_time;					// ǻ���� �ð�	���� �����ǰ��� 0�� ���¿��� �ӵ��� 1�̻��� �ð� 1BYTE ��
	char padding5;
	/////////////////////////////////////////////////////////////////////////
	unsigned short cur_rpm;				// Rpm 
	unsigned char cur_speed;			// Speed 
	unsigned char cur_break;			// 

	unsigned char safe_drive_data[12];	// ������� �̺�Ʈ	�ް���/�ް���	12B

	unsigned char accel_pos[60];		// ������	1�� ����	60B


	unsigned char odo_type;				// ODO Meter type A, B, C
	////////////////////////////////////////////////////////////////////////

	unsigned char dtc_count;
	char dtc_code[MAX_DTC_CODE][8];		// �����ڵ� MAX 10

	unsigned char fault_count;
	char padding7;
	unsigned short fault_code[10];

	unsigned short DistanceToEmpty;

	// �߰��Ǵ� ������ ���� : 
	long run_stop_time;					// ������ ����(��) �ð�	����Ÿ��� 1�̻��� ������ �����ð� - add 2018.12.04

	UCHAR SubDeviceErr[4];
	UCHAR OnCnt;		//OBD FW�� On Cnt

	char ebp_version[2];
	// dummy
	char pading8[1];
} OBD_TRIP_MAP, * POBD_TRIP_MAP;

typedef enum _TELEPHONE_COMAPNY
{
	LGU_PLUS = 0,
	LGU_PLUS_SMS,
	TELENORE_1,
	TELENORE_2,
	KTT,
	SKT,
	TC_MAX
}TELEPHONE_COMAPNY;

typedef struct _JIG_HW_TEST_
{
	unsigned short bUDRCommOk : 1;
	unsigned short bOBDCommOk : 1;
	unsigned short bBLECommOk : 1;
	unsigned short bLTECommOk : 1;
	unsigned short bAccelerometerOk : 1;
	unsigned short bUDRGPSOk : 1;
	unsigned short bQGPSOk : 1;
	unsigned short bPwrDttOK : 1;	//pjs20230811
	unsigned short bUdrExistOK : 1;
	unsigned short bQRI_pinOk : 1;
	unsigned short bQApready_pinOk : 1; //pjs20230811
	unsigned short bInternalAntOK : 1;	//pjs20230811
	unsigned short bExteranlAntOK : 1;	//pjs20230811
	unsigned short bRelayNExtRsvINOK : 1;	//pjs20230811
	unsigned short bExtAccINOK : 1;	//pjs20230811
	unsigned short bRTC : 1;	//pjs20230811

	unsigned short sensor_T;	//pjs20230811
	unsigned char LTERSSI;		//pjs20230811
	unsigned char T_COMAPNY;	//pjs20230811

	char app_version[4];
	char obd_version[4];
	char db_version[8];
	char mdm_version[20];
	char imei_gsn[24];			// IMEI of module
	char iccid_sim[24]; 		// ICCID number of SIM card
	char device_SN[12];
	unsigned short crc16;

} JIG_HW_TEST, * pJIG_HW_TEST;
#define DEF_T_COMAPNY KTT

#define SSL_FILE_NAME_LEN_MAX 40
typedef struct { 		//USE_SSL
	unsigned char ssl_magic[4]; // 
	char ssl_CertificatCrt[SSL_FILE_NAME_LEN_MAX];	//28c8593ea2-certificate.pem.crt
	char ssl_PrivatKey[SSL_FILE_NAME_LEN_MAX];	//28c8593ea2-private.pem.key
	char ssl_CA[SSL_FILE_NAME_LEN_MAX];	//AmazonRootCA1.pem
}SSL_DATA;// 124 = 4 + 40 + 40 + 40 : CONFIG_ETC_LEN_MAX

typedef struct {
	// 0
	unsigned char b00_CurLatitude : 1;	// 1	DataField[0]:0	CurLatitude	Signed Long	4	[1/10000000]���� ��ġ ���� (WGS84��ǥü��)
	unsigned char b01_CurLongitude : 1;	// 2	DataField[0]:1	CurLongitude	Signed Long 	4	[1/10000000]���� ��ġ �浵 (WGS84��ǥü��,)
	unsigned char b02_GpsStatus : 1;		// 3	DataField[0]:2	GpsStatus		UCHAR		1	GPS ���� ����
	unsigned char b03_GpsHdop : 1;		// 4	DataField[0]:3	GpsHdop		UCHAR		1	[1/10]HDOP
	unsigned char b04_TotalDistance : 1;	// 5	DataField[0]:4	TotalDistance	ULONG		4	[0.1Km]�� ���� �Ÿ�
	unsigned char b05_TripDistance : 1;	// 6	DataField[0]:5	TripDistance	Float			4	[m]�õ� �� ����Ÿ�
	unsigned char b06_TripFuel : 1;		// 7	DataField[0]:6	TripFuel		Float			4	[ml]�õ� �� ����Ҹ�
	unsigned char b07_Speed : 1;//8	DataField[0]:7	Speed	UCHAR	1	[km/h]�ӵ�
	// 1 			
	unsigned char reserved1;
}BTM_GPSFIELD;// 2

typedef struct {
	// 0
	unsigned char b00_Speed : 1;			// 1		DataField[0]:0	Speed		UCHAR	1	[km/h]�ӵ�
	unsigned char b01_PRM : 1;			// 2		DataField[0]:1	PRM			USHORT	2	[rpm]
	unsigned char b02_BV : 1;			// 3		DataField[0]:2	BV			short	2	���͸� ���� ( V / 10 ��Ʈ)
	unsigned char b03_ECT : 1;			// 4		DataField[0]:3	ECT			UCHAR	1	�ð����µ� (V-40 ����)
	unsigned char b04_FuelRate : 1;		// 5		DataField[0]:4	FuelRate		UCHAR	1	[0.1ml/s] ����л���
	unsigned char b05_FuelRemain : 1;	// 6		DataField[0]:5	FuelRemain	USHORT	2	[L] �ܿ����ᷮ =  (value - 1) * 0.1 ,[kwh]EV SOC(���������) =  (value - 1) * 0.1,noData : 0xFFFF or 0x0000	 
	unsigned char b06_FuelLevel : 1;		// 7		DataField[0]:6	FuelLevel		UCHAR	1	�����ܷ� [%] = value - 1  	noData : 0xFF or 0x00
	unsigned char b07_AccPos : 1;		// 8		DataField[0]:7	AccPos		UCHAR	1	�Ǽ� ������[%] = value - 1 	noData : 0xFF or 0x00
	// 1 
	unsigned char b10_eng_Oil_T : 1;		// 9		DataField[1]:0	eng_Oil_T		UCHAR	1	�������� �µ�[��] = value - 40	noData : 0xFF or 0x00
	unsigned char b11_VBCharge : 1;		// 10	DataField[1]:1	VBCharge		UCHAR	1	�嵥�� ������[%] = value - 1 	noData : 0xFF or 0x00
	unsigned char b12_VBLife : 1;		// 11	DataField[1]:2	VBLife		UCHAR	1	�嵥�� �ܿ�����[%] = value - 1 	noData : 0xFF or 0x00
	unsigned char b13_Gear : 1;			// 12	DataField[1]:3	Gear			Char		1	0x0X : P , 0x5X : D  , 0x6X : N  , 0x7X : R    noData : 0xff
	unsigned char b14_Brake : 1;			// 13	DataField[1]:4	Brake		Char		1	0 : xx, 1:OFF, 2 : ON                        noData : 0xff
	unsigned char b15_mission : 1;		// 14	DataField[1]:5	mission		UCHAR	1	Mission Temp : value - 40 	          noData : 0xff
	unsigned char b16_Blinker : 1;		// 15	DataField[1]:6	Blinker		Char		1	Left :((blinker >> 4) & 0x0f), Right : (blinker & 0x0f) 0 : xx, 1:OFF, 2 : ON                        noData : 0xff[degree] 				noData : 0xffffffff
	unsigned char b17_Steer : 1;			// 16	DataField[1]:7	Steer			float		4	[degree] 				noData : 0xffffffff
	// 2
	unsigned char b20_DPF_T : 1;			// 17	DataField[2]:0	DPF_T		float		4	���ܿµ�				noData : 0xffffffff
	unsigned char b21_DPF_Dis : 1;		// 18	DataField[2]:1	DPF_Dis		float		4	[km]����İŸ�				noData : 0xffffffff
	unsigned char b22_DPF_StorageRate : 1;// 19	DataField[2]:2	DPF_StorageRate float	4	�ſ�������				noData : 0xffffffff
	unsigned char b23_DPF_StorageSize : 1;// 20	DataField[2]:3	DPF_StorageSize float	4	�ſ�������				noData : 0xffffffff
	unsigned char b24_DPF_State : 1;		// 21	DataField[2]:4	DPF_State		float		4	������� ( ( value > 0.5)? ��� �� : ����� �ƴ� ) noData : 0xffffffff
	unsigned char b25_EV_State : 1;		// 22	DataField[2]:5	EV_State		UCHAR	1	�������� (0:��,1:��,2:��)				
	unsigned char b26_SOH : 1;			// 23	DataField[2]:6	SOH			UCHAR	1	[%]SOH = value - 1 	             noData : 0xFF or 0x00
	unsigned char b27_run_time : 1;		// 24	DataField[2]:7	run_time		float		4	[sec] �õ��� ���� �ð� = value
	// 3
	unsigned char b30_distance : 1;		// 25	DataField[3]:0	distance		float		4	[m] �õ��� ���� �Ÿ� = value
	unsigned char b31_fuel : 1;			// 26	DataField[3]:1	fuel			float		4	[ml] �õ��� ���� �Һ� = value
	unsigned char b32_Idle_time : 1;		// 27	DataField[3]:2	Idle_time		USHORT	2	[s] ��ȸ�� �ð�
	unsigned char b33_tpmspFL : 1;		// 28	DataField[3]:3	tpmspFL		USHORT	2	[Kpa] TPMS���� �з� (���� ������ ����(1))		
	unsigned char b34_tpmspFR : 1;		// 29	DataField[3]:4 	tpmspFR		USHORT	2	[Kpa] TPMS�տ� �з� (���� ������ ����(1))				
	unsigned char b35_tpmspRL : 1;		// 30	DataField[3]:5	tpmspRL		USHORT	2	[Kpa] TPMS���� �з� (���� ������ ����(1))			
	unsigned char b36_tpmspRR : 1;		// 31	DataField[3]:6	tpmspRR		USHORT	2	[Kpa] TPMS�ڿ� �з� (���� ������ ����(1))			
	unsigned char b37 : 1;				// 32	DataField[3]:7				
	// 4
	unsigned char reserved4;
}BTM_DATAFIELD;

typedef struct {
	char config_magic[4];
	unsigned short OnDataGetTerm;		// ������ �����ֱ� V ��, 0 �̸� �������� ����
	unsigned short OnDataSendCnt;		// ������ �����ֱ� V ��, 0 �̸� �������� ����
	BTM_DATAFIELD OnDataField;		//�õ� On DataField 
	unsigned short OnGpsGetTerm;		// ������ �����ֱ� V ��, 0 �̸� �������� ����
	unsigned short OnGpsSendCnt;		// ������ �����ֱ� V ��, 0 �̸� �������� ����
	BTM_GPSFIELD OnGpsField;		//�õ� On DataField 
	unsigned short tummy;
	unsigned short KASendTerm; //KEEP_ALIVE_SEND_TM
	unsigned short KGSendTerm; //KEEP_ALIVE_SEND_TM
}DC_CONFIG_DATA; //14 = 2+2+4 +2+2+2

#define BLE_GAP_ADDR_LEN (6)
typedef struct _BLE_INFO
{
	unsigned char Addr[BLE_GAP_ADDR_LEN];
	unsigned char name[20];
}BLE_INFO; //26byte

typedef struct {
	char ble_magic[4];
	BLE_INFO info;
	BLE_INFO reseved[2];
}BLE_DATA; //14 = 2+2+4 +2+2+2
#pragma pack(pop)   // ���� ������ ���� ����(�⺻��)�� �ǵ���

#endif
