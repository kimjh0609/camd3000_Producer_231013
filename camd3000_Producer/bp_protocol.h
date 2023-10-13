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

#define BP_PACKET_UPDATE_APP			0x01	//FTP 사용
#define BP_PACKET_UPDATE_HAPP			0x21 	// HTTP 사용 
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
	TCHAR szFilePath[4096];    // 파일의 경로 및 이름을 복사하기 위한 문자열
	int cur_no;
	int total_no;
	int file_size;
}FILE_INFO;


#pragma pack(push, 1)       // 1바이트 크기로 정렬
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
#pragma pack(pop)   // 정렬 설정을 이전 상태(기본값)로 되돌림

#define ATI_INFO_MAX 40
#define STATE_BUF_MAX 8
#define MAX_DTC_CODE        10
#define CONFIG_ETC_LEN_MAX 156

#pragma pack(push, 1)       // 1바이트 크기로 정렬
typedef struct _CONFIG_DAT_
{
	unsigned int magic;				// 0x5A5AA5A5
	char serial_no[32];			  // 7byte 사용 
	char host_ip[128];
	unsigned short port_no;
	char service_nm[32];
	unsigned char hwVER[2];
	unsigned char dbgFlag;
	unsigned char pad1[5];
	unsigned int service_iot_NO;
	char reserved[64];
	unsigned char device_SN[12];  // 필요시 제품 관리 번호로 사용 (KSTI)
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
	unsigned char cnum[20];		// 전화번호 of SIM card

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

	long igni_off_total_count;			// 총 Data
	long igni_on_total_count;			// 총 Data

	long acc_total_dist;				// [0.1Km]총 주행 거리

	char qm_vendor[8];
	char model[8];
	// Activation- 단말기 및 기타정보 - 시동 On 시 항상 전송
	// firmware version	펌웨어의 버전 정보. QM Revision(20), APP(4), OBD(4), DB(15)
	char qm_revision[20];				// quectel revision
	char app_version[4];				// Major 2Byte + Minor 2Byte 0xC000 0x0001

	char vehicle_id[17];				// 차량의 차대번호 정보 (Vehicle Identification Number)
	char padding1;

	unsigned short fuel_tank_capacity;	// 연료탱크 용량 - 0.1L

	char dev_imei[15];					// IMEI	단말기 통신모듈의 IMEI 정보 15자리의 수.
	char padding2;
	char phone_number[11];				// 단말기 통신모듈의 PHONE Number 11자리의 수.
	char padding3;

	///////////////////////////////////////////////////
	// 114 = 2+2+4+6+2+ 4 * 3 + 8 *2 + 20 + 4 + 17+1 + 2 + 15+1  + 11+1   index및 모뎀 및 버전 정보 
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
	char resereved[19];					// IMEI	단말기 통신모듈의 IMEI 정보 15자리의 수.
#else
	unsigned long trip_rtd_id;			// (0 ~ 999999) RTD를 생성할때마다 1씩 증가, Trip ID 증가시 마다 초기화
	unsigned short trip_rld_id;			// (1 ~ 60) RLD가 생성될때마다 1씩 증가, RTD 증가시 초기화
#endif

	volatile unsigned char ignition;	// 현재 시동 상태
	unsigned char state_H;				// 0x5F, 0x6F, 0x7F .... 0x50, 0x60, 0x70
	unsigned char state_L;				// reserver

	unsigned char igni_on_count;		//

	long igni_on_t;						// 시동 ON 시간 시동이 켜진 시간 - 초단위 시간으로 환산값

	long trip_start_t;					// Trip 시작 시간 - 초단위 시간으로 환산값  ==> 주행을 시작 했을 때  시간 ???
	long igni_off_t;					// Trip 종료 시간 - 초단위 시간으로 환산값

	long trip_run_time;					// 시동 후 운행 시간 (sec)
	float trip_run_dist; 				// 시동 후 운행 거리(실수) // [m] 0xFFFFFFFF(12:속도 센서 이상 )
	float trip_fuel;					//[ml] 시동 후 연료 소모량
	float	idle_time;			//[sec], 	시동  공회전 시간		

	unsigned char	AccPos; 		// Acc					// [%] -1		noData : 0x00
	unsigned char engine_oil_temp;		// 엔진오일 경고	엔진오일 온도값 DTC (standard PID로 가능하면 엔진오일 평균온도)	1B	BYTE	C	엔진온도 최고값 전송, +40 전송
	unsigned char ibs_info[2];			// IBS info	배터리 수명 및 잔여량 잔여 수명량 (0 ~ 100)%, 충전 잔여량 (0 ~ 100)% - 각 1Byte 씩

	unsigned char mission_oil_temp;		// 미션오일 온도	미션오일 온도값 DTC (standard PID로 가능하면 엔진오일 평균온도)	1B	BYTE	C	미션온도 최고값 전송. +40 전송
	char	FR_State;		// 연료 잔량 측정 상태

	unsigned char fuelFL;				// [%]탱크레벨 raw 데이타
	unsigned short batt_volt;


	long start_latitude;				// 주행 시작 위치 위도 long
	long start_longitude;				// 주행 시작 위치 경도 long

	long stop_latitude;					// 주행 종료 위치 위도 long
	long stop_longitude;				// 주행 종료 위치 경도 long

	unsigned short imu_warning_param1;	// 2019.03.26 008 version add
	unsigned short imu_warning_param2;	// 2019.03.26 008 version add
	unsigned short batt_warning_param;	// 2019.03.26 008 version add

	//unsigned char padding22[4];
	char obd_version[4];
	char tiObd_version[4];
	char db_version[4];


	char carCode[7];
	unsigned char padding33[1];

	unsigned short igni_on_fuel;		// 시동ON 연료탱크 잔여량	 시동ON 시점의 연료탱크 값	2B	USHORT	100ml	x10 전송
	IMU_DATA	igni_on_imu;			// 시동ON IMU 값	시동ON IMU 값	6B	USHORT * 3	각 0.01 Res	전면, 측면, 상하 각 2Byte - x10 전송

	unsigned short igni_off_fuel;		// 시동OFF 연료탱크 잔여량	시동ON시점의 연료탱크 값	2B	USHORT	100ml	x10 전송
	IMU_DATA 	igni_off_imu;		// 시동OFF IMU 값	시동ON IMU 값	6B	USHORT * 3	각 0.01 Res	전면, 측면, 상하 각 2Byte - x10 전송


	unsigned char coolant_temp;		// 냉각수 온도 	1B	BYTE	C	엔진온도 최고값 전송, +40 전송

	unsigned char device_tmp;			// CAR (Device) 온도 1 BYTE  C -40 C ~ , +40으로 전송
	char padding4;
	unsigned short tpms[4]; 			// 타이어 공기압   0 not support, val : 1 ~  xxx , unit =  KPA

	unsigned char warm_up_time;			// 웜업 시간	시동 ON후 주행거리가 0인 상태의 시간 1 BYTE 분
	long fuel_cut_time;					// 퓨얼컷 시간	엑셀 포지션값이 0인 상태에서 속도가 1이상인 시간 1BYTE 분
	char padding5;
	/////////////////////////////////////////////////////////////////////////
	unsigned short cur_rpm;				// Rpm 
	unsigned char cur_speed;			// Speed 
	unsigned char cur_break;			// 

	unsigned char safe_drive_data[12];	// 위험운전 이벤트	급가속/급감속	12B

	unsigned char accel_pos[60];		// 포지션	1초 단위	60B


	unsigned char odo_type;				// ODO Meter type A, B, C
	////////////////////////////////////////////////////////////////////////

	unsigned char dtc_count;
	char dtc_code[MAX_DTC_CODE][8];		// 고장코드 MAX 10

	unsigned char fault_count;
	char padding7;
	unsigned short fault_code[10];

	unsigned short DistanceToEmpty;

	// 추가되는 변수들 선언 : 
	long run_stop_time;					// 주행중 정지(차) 시간	주행거리가 1이상인 상태의 정차시간 - add 2018.12.04

	UCHAR SubDeviceErr[4];
	UCHAR OnCnt;		//OBD FW의 On Cnt

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
	unsigned char b00_CurLatitude : 1;	// 1	DataField[0]:0	CurLatitude	Signed Long	4	[1/10000000]현재 위치 위도 (WGS84좌표체계)
	unsigned char b01_CurLongitude : 1;	// 2	DataField[0]:1	CurLongitude	Signed Long 	4	[1/10000000]현재 위치 경도 (WGS84좌표체계,)
	unsigned char b02_GpsStatus : 1;		// 3	DataField[0]:2	GpsStatus		UCHAR		1	GPS 상태 정보
	unsigned char b03_GpsHdop : 1;		// 4	DataField[0]:3	GpsHdop		UCHAR		1	[1/10]HDOP
	unsigned char b04_TotalDistance : 1;	// 5	DataField[0]:4	TotalDistance	ULONG		4	[0.1Km]총 주행 거리
	unsigned char b05_TripDistance : 1;	// 6	DataField[0]:5	TripDistance	Float			4	[m]시동 후 주행거리
	unsigned char b06_TripFuel : 1;		// 7	DataField[0]:6	TripFuel		Float			4	[ml]시동 후 연료소모량
	unsigned char b07_Speed : 1;//8	DataField[0]:7	Speed	UCHAR	1	[km/h]속도
	// 1 			
	unsigned char reserved1;
}BTM_GPSFIELD;// 2

typedef struct {
	// 0
	unsigned char b00_Speed : 1;			// 1		DataField[0]:0	Speed		UCHAR	1	[km/h]속도
	unsigned char b01_PRM : 1;			// 2		DataField[0]:1	PRM			USHORT	2	[rpm]
	unsigned char b02_BV : 1;			// 3		DataField[0]:2	BV			short	2	배터리 전압 ( V / 10 볼트)
	unsigned char b03_ECT : 1;			// 4		DataField[0]:3	ECT			UCHAR	1	냉각수온도 (V-40 도씨)
	unsigned char b04_FuelRate : 1;		// 5		DataField[0]:4	FuelRate		UCHAR	1	[0.1ml/s] 연료분사율
	unsigned char b05_FuelRemain : 1;	// 6		DataField[0]:5	FuelRemain	USHORT	2	[L] 잔여연료량 =  (value - 1) * 0.1 ,[kwh]EV SOC(전기차경우) =  (value - 1) * 0.1,noData : 0xFFFF or 0x0000	 
	unsigned char b06_FuelLevel : 1;		// 7		DataField[0]:6	FuelLevel		UCHAR	1	연료잔량 [%] = value - 1  	noData : 0xFF or 0x00
	unsigned char b07_AccPos : 1;		// 8		DataField[0]:7	AccPos		UCHAR	1	악셀 포지션[%] = value - 1 	noData : 0xFF or 0x00
	// 1 
	unsigned char b10_eng_Oil_T : 1;		// 9		DataField[1]:0	eng_Oil_T		UCHAR	1	엔진오일 온도[℃] = value - 40	noData : 0xFF or 0x00
	unsigned char b11_VBCharge : 1;		// 10	DataField[1]:1	VBCharge		UCHAR	1	밧데리 충전율[%] = value - 1 	noData : 0xFF or 0x00
	unsigned char b12_VBLife : 1;		// 11	DataField[1]:2	VBLife		UCHAR	1	밧데리 잔여수명[%] = value - 1 	noData : 0xFF or 0x00
	unsigned char b13_Gear : 1;			// 12	DataField[1]:3	Gear			Char		1	0x0X : P , 0x5X : D  , 0x6X : N  , 0x7X : R    noData : 0xff
	unsigned char b14_Brake : 1;			// 13	DataField[1]:4	Brake		Char		1	0 : xx, 1:OFF, 2 : ON                        noData : 0xff
	unsigned char b15_mission : 1;		// 14	DataField[1]:5	mission		UCHAR	1	Mission Temp : value - 40 	          noData : 0xff
	unsigned char b16_Blinker : 1;		// 15	DataField[1]:6	Blinker		Char		1	Left :((blinker >> 4) & 0x0f), Right : (blinker & 0x0f) 0 : xx, 1:OFF, 2 : ON                        noData : 0xff[degree] 				noData : 0xffffffff
	unsigned char b17_Steer : 1;			// 16	DataField[1]:7	Steer			float		4	[degree] 				noData : 0xffffffff
	// 2
	unsigned char b20_DPF_T : 1;			// 17	DataField[2]:0	DPF_T		float		4	전단온도				noData : 0xffffffff
	unsigned char b21_DPF_Dis : 1;		// 18	DataField[2]:1	DPF_Dis		float		4	[km]재생후거리				noData : 0xffffffff
	unsigned char b22_DPF_StorageRate : 1;// 19	DataField[2]:2	DPF_StorageRate float	4	매연축적률				noData : 0xffffffff
	unsigned char b23_DPF_StorageSize : 1;// 20	DataField[2]:3	DPF_StorageSize float	4	매연축적량				noData : 0xffffffff
	unsigned char b24_DPF_State : 1;		// 21	DataField[2]:4	DPF_State		float		4	재생상태 ( ( value > 0.5)? 재생 중 : 재생중 아님 ) noData : 0xffffffff
	unsigned char b25_EV_State : 1;		// 22	DataField[2]:5	EV_State		UCHAR	1	충전상태 (0:비,1:완,2:급)				
	unsigned char b26_SOH : 1;			// 23	DataField[2]:6	SOH			UCHAR	1	[%]SOH = value - 1 	             noData : 0xFF or 0x00
	unsigned char b27_run_time : 1;		// 24	DataField[2]:7	run_time		float		4	[sec] 시동후 운행 시간 = value
	// 3
	unsigned char b30_distance : 1;		// 25	DataField[3]:0	distance		float		4	[m] 시동후 운행 거리 = value
	unsigned char b31_fuel : 1;			// 26	DataField[3]:1	fuel			float		4	[ml] 시동후 운행 소비량 = value
	unsigned char b32_Idle_time : 1;		// 27	DataField[3]:2	Idle_time		USHORT	2	[s] 공회전 시간
	unsigned char b33_tpmspFL : 1;		// 28	DataField[3]:3	tpmspFL		USHORT	2	[Kpa] TPMS앞좌 압력 (직전 데이터 갱신(1))		
	unsigned char b34_tpmspFR : 1;		// 29	DataField[3]:4 	tpmspFR		USHORT	2	[Kpa] TPMS앞우 압력 (직전 데이터 갱신(1))				
	unsigned char b35_tpmspRL : 1;		// 30	DataField[3]:5	tpmspRL		USHORT	2	[Kpa] TPMS뒤좌 압력 (직전 데이터 갱신(1))			
	unsigned char b36_tpmspRR : 1;		// 31	DataField[3]:6	tpmspRR		USHORT	2	[Kpa] TPMS뒤우 압력 (직전 데이터 갱신(1))			
	unsigned char b37 : 1;				// 32	DataField[3]:7				
	// 4
	unsigned char reserved4;
}BTM_DATAFIELD;

typedef struct {
	char config_magic[4];
	unsigned short OnDataGetTerm;		// 주행중 수집주기 V 초, 0 이면 수집하지 않음
	unsigned short OnDataSendCnt;		// 주행중 전송주기 V 초, 0 이면 전송하지 않음
	BTM_DATAFIELD OnDataField;		//시동 On DataField 
	unsigned short OnGpsGetTerm;		// 주행중 수집주기 V 초, 0 이면 수집하지 않음
	unsigned short OnGpsSendCnt;		// 주행중 전송주기 V 초, 0 이면 전송하지 않음
	BTM_GPSFIELD OnGpsField;		//시동 On DataField 
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
#pragma pack(pop)   // 정렬 설정을 이전 상태(기본값)로 되돌림

#endif
