#ifndef DATASTRUCTURE
#define DATASTRUCTURE

#define PACKED    __attribute__((__packed__))

#define WALLETTRADE     0x10
#define MONTHTRADE      0x11

#define MAX_DATETIMEDIFF        500.0
#define MAX_BEATTIMEDIFF        3.0

#define MAX_REPEATTIME          1000.0

#define MAX_DRIVER_REPEATTIME   5.0

#define MAX_ALIPAY_REPEATTIME   5.0

#define UTC_TIMEDIFF            8*3600

enum gprsState
{
    cmdATE,
    cmdCPIN,
    cmdCGREG,
    cmdCNMI,
    cmdCMGF,
    cmdCSMP,
    cmdCPMS,
    cmdMIPCALL,
    cmdMIPDIS,
    cmdMIPOPEN,
    cmdMIPCLOSE,
    cmdMIPSETS,
    cmdMIPDSETS,
    queryCGREG,
    finishINIT
};

enum uiState
{
    signalUI,
    buslineUI,
    busidUI,
    posidUI,
    blackclearUI
};

enum idLabelState
{
    idLabel_1,
    idLabel_2,
    idLabel_3,
    idLabel_4,
    idLabel_5,
    idLabel_6,
    idLabel_7,
    idLabel_8
};

enum lineTicketLabelState
{
    lineLabel_1,
    lineLabel_2,
    lineLabel_3,
    lineLabel_4,
    lineLabel_5,
    lineLabel_6,
    lineLabel_7,
    lineLabel_8,
    ticketLabel_1,
    ticketLabel_2,
    ticketLabel_3,
    ticketLabel_4,
    ticketLabel_5
};

struct myDateTime
{
    char year[4];
    char month[2];
    char day[2];
    char hour[2];
    char minitue[2];
    char second[2];
};

typedef struct dbConsumeInfo_s
{
    char deviceID[8];
    char userID[16];
    char tradeType;
    char citycode[6];
    char tradeTime[14];
    char ticketPrice[4];
    char amount[4];
    char longitude[8];
    char latitude[8];
} dbConsumeInfo_t;

typedef struct tradeBalance_s
{
    int wallet;
    int month;
    bool walletRepeat;
    bool monthRepeat;
}tradeBalance_t;

struct scanSerialInfo
{
    unsigned char data[70];
    unsigned char keybatch[8];
    unsigned char signlen[2];
    unsigned char sign[200];
};

struct scanTradeInfo
{
    unsigned char framehead[7];
    unsigned char account[16];
    unsigned char accountState;
    unsigned char citycode[6];
    unsigned char walletBalance[6];
    unsigned char monthBalance[6];
    unsigned char QRgenerationTime[14];
    unsigned char QRvalidTime[14];
};

typedef struct consumeInfo_s
{
    unsigned char deviceID[8];
    unsigned char userID[16];
    unsigned char tradeType;
    unsigned char citycode[6];
    unsigned char tradeTime[7];
    unsigned char ticketPrice[2];
    unsigned char amount[2];
    unsigned char longitude[8];
    unsigned char latitude[8];
} consumeInfo_t;

typedef struct consumeAckInfo_s
{
    unsigned char deviceID[8];
    unsigned char userID[16];
    unsigned char time[7];
}consumeAckInfo_t;

typedef struct beatInfo_s
{
    unsigned char deviceID[8];
}beatInfo_t;

typedef struct beatAckInfo_s
{
    unsigned char deviceID[8];
    unsigned char checkTime[7];
    unsigned char keyBatch[4];
}beatAckInfo_t;

// ΢������֧��
#define MAX_WEIXIN_DATALEN      128
typedef struct weixinScanInfo_s
{
    unsigned char scanData[MAX_WEIXIN_DATALEN];
    int scanLen;
}weixinScanInfo_t;

typedef struct weixinTradeInfo_s
{
    unsigned char deviceID[8];
    unsigned char citycode[6];
    unsigned char ticketPrice[2];
    unsigned char amount[2];
    unsigned char longitude[8];
    unsigned char latitude[8];
    unsigned char speed[2];
    unsigned char scanData[18];
}weixinTradeInfo_t;

typedef struct weixinTradeAck_s
{
    unsigned char tradeRet;
}weixinTradeAck_t;

typedef struct weixinBeatAck_s
{
    unsigned char time[7];
    unsigned char keyVer[8];
    unsigned char blackVer[8];
    // kira added 2017.3.6 
    unsigned char sysVersion[8];
    unsigned char stVersion[8];
    unsigned char cardTypeVersion[8];
    unsigned char weixinTime[7];
}weixinBeatAck_t;

typedef struct onlineTradeHeadInfo_s
{
    unsigned char deviceID[8];
    unsigned char citycode[6];
    unsigned char busID[8];
    unsigned char buslineID[8];
    unsigned char driverID[16];
    unsigned char ticketPrice[2];
    unsigned char amount[2];
    unsigned char serialNum[4];
    unsigned char validTime;
    unsigned char tradeTime[7];
    unsigned char longitude[8];
    unsigned char latitude[8];
}onlineTradeHeadInfo_t;

typedef struct onlineTradeAck_s
{
    unsigned char serialNum[4];
    unsigned char tradeRet;
}onlineTradeAck_t;

// Alipay
#define MAX_ALIPAY_DATALEN      1024
typedef struct alipayScanInfo_s
{
    char scanData[MAX_ALIPAY_DATALEN];
    int scanLen;
}alipayScanInfo_t;

typedef struct offlineHeadInfo_s
{
    char userIDLen[4];			//�û�ID���� memcpy(userIDLen, &len, 4);
    char offLineRecordLen[4];   //���߽�����Ϣ���� memcpy(userIDLen, &len, 4);
    char tradeRecord[4];
    char busline[8];			//������·�� ascii
    char busID[8];				//��������� ascii
    char posID[8];				//POS�豸�� ascii
    char cityID[6];             //POS���д��� ascii
    char tradeTime[14];			//����ʱ��YYYYMMDDHHMMSS
    char ticketPrice[4];		//Ʊ����Ϣ memcpy(ticketPrice, &price, 4);
    char longitude[8];			//������Ϣ-Ԥ��
    char latitude[8]; 			//γ����Ϣ-Ԥ��
    char driverID[16];          //˾��ID ascii
}offlineHeadInfo_t;

typedef struct alipayTradeInfo_s
{
    offlineHeadInfo_t offlineHeadInfo;
    char userID[17];
    char record[4096];
}alipayTradeInfo_t;

// kira added --- 2017.3.9
typedef struct offlineHeadInfoV2_s
{
    char userIDLen[4];			//�û�ID���� memcpy(userIDLen, &len, 4);
    char offLineRecordLen[4];   //���߽�����Ϣ���� memcpy(userIDLen, &len, 4);
    char cardNoLen[4];
    char cardDataLen[4];
    char cardTypeLen[4];
    char tradeRecord[4];
    char busline[8];			//������·�� ascii
    char busID[8];				//��������� ascii
    char posID[8];				//POS�豸�� ascii
    char cityID[6];             //POS���д��� ascii
    char tradeTime[14];			//����ʱ��YYYYMMDDHHMMSS
    char ticketPrice[4];		//Ʊ����Ϣ memcpy(ticketPrice, &price, 4);
    char longitude[12];			//������Ϣ-Ԥ��
    char latitude[12]; 			//γ����Ϣ-Ԥ��
    char driverID[16];          //˾��ID ascii
}offlineHeadInfoV2_t;

typedef struct alipayTradeInfoV2_s
{
    offlineHeadInfoV2_t offlineHeadInfo;
    char userID[17];
    char record[2048];
    char cardNo[32];
    char cardData[256];
    char cardType[16];
}alipayTradeInfoV2_t;

typedef struct alipayTradeInfoAck_s
{
    char tradeTime[14];			//����ʱ��YYYYMMDDHHMMSS
    char tradeRecord[4];
    char userID[17];
}alipayTradeInfoAck_t;

typedef struct alipayKey_s
{
    char keyOrder;
    char keyValue[66];
}alipayKey_t;

typedef struct alipayKeyInfo_s
{
    unsigned char pageNum;
    unsigned char currentNum;
    unsigned char num;
    alipayKey_t key[256];
}alipayKeyInfo_t;

typedef struct askAlipayKeyInfo_s
{
    unsigned char deviceID[8];
    unsigned char pageNum;          // ��Կ����ǰҳ��
}askAlipayKeyInfo_t;

typedef struct alipayBlack_s
{
    int  blackOrder;
    char blackValue[17];
}PACKED alipayBlack_t;

typedef struct alipayBlackInfo_s
{
    int pageNum;
    int currentNum;
    unsigned char num;
    alipayBlack_t black[256];
}PACKED alipayBlackInfo_t;

typedef struct askAlipayBlackInfo_s
{
    unsigned char deviceID[8];
    unsigned char pageNum[4];       // ����������ǰҳ��
}askAlipayBlackInfo_t;

// drive
typedef struct driveSignScanInfo_s
{
    char head[9];
    char driveID[16];
    char signLen[2];
    char signInfo[510];
}driveSignScanInfo_t;

typedef struct driveSignRecordInfo_s
{
    driveSignScanInfo_t signInfo;
    int signInfoLen;
}driveSignRecordInfo_t;

typedef struct driveSignNetInfo_s
{
    char posID[8];
    char driveID[16];
    char signFlag;
    char signTime[14];
    char busline[8];
    char busID[8];
    char cityID[6];             //POS���д��� ascii
    char longitude[8];			//������Ϣ-Ԥ��
    char latitude[8]; 			//γ����Ϣ-Ԥ��
}driveSignNetInfo_t;

typedef struct driveSignNetAck_s
{
    char signTime[14];
    char driveID[16];
}driveSignNetAck_t;

// pos param
typedef struct posParam_s
{
    char posID[8];
    char buslineID[8];
    char busID[8];
    char cityID[6];
    char tickets[2];
    char time[14];
}posParam_t;

// version updata
typedef struct posVersionAskInfo_s
{
    char posID[8];
    char cityID[6];
    char version[8];
}posVersionAskInfo_t;

typedef struct posUpdataInfo_s
{
    char updataInfo[100];
    char infoLen;
}posUpdataInfo_t;

typedef struct unionPayInfo_s
{
    char busline[8];			//������·�� ascii
    char busID[8];				//��������� ascii
    char posID[8];				//POS�豸�� ascii
    char cityID[6];             //POS���д��� ascii
    char driverID[16];          //˾��ID ascii
    char tradeTime[14];			//����ʱ��YYYYMMDDHHMMSS
    char ticketPrice[4];		//Ʊ����Ϣ memcpy(ticketPrice, &price, 4);
    char longitude[12];			//������Ϣ-Ԥ��
    char latitude[12]; 			//γ����Ϣ-Ԥ��
    char cardNo[20];			//����(ASCII)
    char serialNo[3];			//��Ƭ���к�(HEX)
    char validDate[4];			//��Ч����(HEX)
    char tradeType;				//��������(0-��ǿ� 1-���ǿ�)
    char tradeNature;           //��������(0-��������/1-�ѻ�����)
    char settlementDate[8];		//��������YYYYMMDD
	char cardCenterTime[14];	//�ܿ������ڵ�����ʱ��(POS����������POS�������ڵصı�׼ʱ�� YYYYMMDDhhmmss)
	char terminalId[8];			//�ܿ����ն˱�ʶ��(�ն˱�� 8 ASCII)
	char merchantId[15];		//�ܿ�����ʶ��(�̻���� 15 ASCII)
	char currencyCode[3];		//���׻��Ҵ���(3 ASCII)
	char cardCenterSerialNo[12];//�����ο���(POS����ϵͳ��ˮ�� 12 ASCII)
	char retCode[2];			//Ӧ����(2 ASCII)
    char tradeAmount[4];        //hex
}unionPayInfo_t;

typedef struct unionPayAck_s
{
    char cardNo[20];
    char tradeTime[14];
}unionPayAck_t;

typedef struct updataInfo_s
{
    char posID[8];
    char cityID[6];
    char busID[8];
    char buslineID[8];
    char newVersion[8];
    char oldVersion[8];
    char time[14];
}updataInfo_t;

typedef struct updataInfoAck_s
{
    char ret;
    char version[8];
}updataInfpAck_t;

typedef struct pscamInfo_s
{
    int cardNoLen;
    int cardDataLen;
    char cardNo[32];
    char cardData[128];
}pscamInfo_t;
#endif // DATASTRUCTURE

