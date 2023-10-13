#ifndef __UART__
#define __UART__

typedef struct{
	BOOL bThreadEnable;
	BOOL bAutoConnectedReq;
	BOOL bHwtestReq;
	BOOL bCertificateCopyReq;
	int FOTAReq;
	int fileListReqTm;
	int nHwtestresultReq;
	int nHwTemptm;
	int LEDCheckReqTm;
	int testCompleteReqTm;
	TBYTE gRxBuff[4096*3];
	TCHAR gURxBuff[4096];
	DWORD dwRxByte;
	BOOL isOBDok;
}_stComm;

extern _stComm stComm;
extern HANDLE hCom;

void Wakeup_Idle(void);
BOOL  ComWirte(TCHAR *str, int length);
unsigned char Mkcomm_for_RFOutputchg(unsigned char nValue);
unsigned char Mkcomm_for_IDReq(void);
unsigned char Mkcomm_for_modechg(int nValue);
unsigned char Mkcomm_for_VerReq(void);
unsigned char Mkcomm_for_IDInit(void);
unsigned char Mkcomm_for_RFTest(void);
unsigned char Mkcomm_for_CARTypeReq(void);
#endif