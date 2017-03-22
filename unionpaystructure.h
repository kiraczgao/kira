#ifndef UNIONPAYSTRUCTURE_H
#define UNIONPAYSTRUCTURE_H

#define PACKED    __attribute__((__packed__))

#define ENCRYPT     0
#define DECRYPT     1

//#define false       0
//#define true        1

#define MAX_BUFF_LENGTH     256
#define MAX_SIZE            1024

/*flag*/
#define		F_FIX_LEN		0	/*(LENgth fix ��ָ��ISO_8583�еĳ��Ⱦ�������ĳ���)*/
#define		F_LLVAR_LEN	    1	/*(LENgth 00~99)*/
#define		F_LLLVAR_LEN    2	/*(LENgth 00~999)*/
/*type*/
#define     T_BIT         0   /* ���ݵĶ����Ʊ�ʾ��������ֱ�ʾλ��bit���ĸ��� */
#define     T_BYTE        1   /* ���ڱ�ʾ�䳤�Ķ���������������ֱ�ʾ������������ռ�ֽڣ�Byte���ĸ��� */
#define     T_NUM         2   /* ��ֵ��0��9���ҿ�����λ��Ч����ǰ���㡣����ʾ����ҽ������Ҷ�λΪ�ǡ��� */
#define     T_P           3   /* ����ַ�����ո� */
#define     T_S           4   /* ������� */
#define     T_AN          5   /* ��ĸ�������ַ����󿿣��ұ߶���λ��ո� */
#define     T_AS          6   /* ��ĸ�������ַ����󿿣��ұ߶���λ��ո� */
#define     T_CN          7   /* ѹ�������룬��BCD�� */
#define     T_NS          8   /* ���ֺ������ַ����󿿣��ұ߶���λ��0 */
#define     T_ANS         9   /* ��ĸ�����ֺ������ַ����󿿣��ұ߶���λ��ո� */
#define     T_ANSB        10  /* ��ĸ�����֡������ַ��Ͷ����������󿿣��ұ߶���λ��ո� */
#define     T_XN          11  /* ������ţ�����Ϊ��C�������Ϊ��D��������������һ�������ͽ������Ԫ���� */
#define     T_Z           12  /* ISO 4909��ISO 7813�ж���Ĵſ��ڶ������ŵ��Ĵ��뼯������������ʼ������������LRC�� */
#define     T_BCD         13  /* ����PBOC2.0 ����� IC������*/

#define BE_TO_LE16(X) ((uint16_t)((((uint16_t)(X) & (uint16_t)0x00ffU) << 8) | (((uint16_t)(X) & (uint16_t)0xff00U) >> 8)))

//�ն�����Կ
//#define TERM_MK "\xEC\xFB\x01\x46\x83\xCE\x52\xC8\xD5\xD6\x6D\xC7\x1F\xF4\x34\xC4"

//�ж�CupPoSTerm�����Ƿ�ִ�гɹ�
#define ASSERT_FAIL(e) \
    if (e != SLSY_TRANS_SUCCESS)\
    {\
        return e;\
    }

//�ɹ�
#define SLSY_TRANS_SUCCESS				0x00

//ʧ��
#define SLSY_TRANS_ERROR				0x01

//����Ӧ
#define SLSY_TRANS_NOACK				0xFF

//ԭ���ײ�����
#define SLSY_TRANS_SRC_TRANS_NO_EXSIT	0x02

typedef struct CUP8583_MSG_Field
{
    int StateFlag;
    int Length;
    unsigned char Buf[MAX_BUFF_LENGTH];
}CUP8583_MSG_Field;

typedef struct CUP8583_MSGpkg
{
    struct CUP8583_MSG_Field Field[128];   //������
}CUP8583_MSG;

typedef struct CUP8583_DEF
{
    int				Length;   /* data element max length */
    unsigned char   Type;     /* bit0,bit1���� , bit2:Ϊ0ʱ�����,Ϊ1ʱ�Ҷ��� , bit3:Ϊ0ʱBCD��,Ϊ1ʱASC�� , type:0,1,2,3���� */
    unsigned char   Flag;     /* length field length: 0--F_FIX_LEN�� 1--F_LLVAR_LEN�� 2--F_F_LLLVAR_LEN�� */
}CUP8583_DEF;

typedef  struct {
   unsigned char length[2];
   unsigned char cmd[2];
   unsigned char mainkeytype;
   unsigned char keyindex[2];
   unsigned char MAK_length;
   unsigned char MAK_mathod;
   unsigned char MAK_KEY[8];
   unsigned char MAK_INI[8];
   unsigned char Datalen[2];
   unsigned char Datacalc[254];
}*CALCMACMSG;

typedef  struct {
   unsigned short length;
   unsigned char cmd;
   unsigned char keyindex[2];
   unsigned char MAK_KEY[16];
   unsigned char MAK_INI[8];
   unsigned char ENCRYPINT;
   unsigned char MAK_mathod;
   unsigned char Datalen[2];
   unsigned char Datacalc[254];
}*CALC_CHECK;

typedef struct  tag_cardreceiveA2 {
    unsigned char tradetype;//���ÿ�/�����
    char cardno[20];
    char citiao2data[38];
    char citiao2data_l;
    char serialno[3];//��Ƭ���к�
    int trademoney;
    unsigned char data55[140];
    char len55;
    char validdate[4];
}PACKED cardreceiveA2;

#endif // UNIONPAYSTRUCTURE_H
