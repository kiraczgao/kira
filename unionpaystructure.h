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
#define		F_FIX_LEN		0	/*(LENgth fix 是指由ISO_8583中的长度决定该域的长度)*/
#define		F_LLVAR_LEN	    1	/*(LENgth 00~99)*/
#define		F_LLLVAR_LEN    2	/*(LENgth 00~999)*/
/*type*/
#define     T_BIT         0   /* 数据的二进制表示，后跟数字表示位（bit）的个数 */
#define     T_BYTE        1   /* 用于表示变长的二进制数，后跟数字表示二进制数据所占字节（Byte）的个数 */
#define     T_NUM         2   /* 数值，0至9，右靠，首位有效数字前填零。若表示人民币金额，则最右二位为角、分 */
#define     T_P           3   /* 填充字符，如空格 */
#define     T_S           4   /* 特殊符号 */
#define     T_AN          5   /* 字母和数字字符，左靠，右边多余位填空格 */
#define     T_AS          6   /* 字母和特殊字符，左靠，右边多余位填空格 */
#define     T_CN          7   /* 压缩数字码，即BCD码 */
#define     T_NS          8   /* 数字和特殊字符，左靠，右边多余位填0 */
#define     T_ANS         9   /* 字母、数字和特殊字符，左靠，右边多余位填空格 */
#define     T_ANSB        10  /* 字母、数字、特殊字符和二进制数，左靠，右边多余位填空格 */
#define     T_XN          11  /* 借贷符号，贷记为“C”，借记为“D”，并且总是与一个数字型金额数据元相连 */
#define     T_Z           12  /* ISO 4909和ISO 7813中定义的磁卡第二、三磁道的代码集，但不包含起始符、结束符和LRC符 */
#define     T_BCD         13  /* 例如PBOC2.0 借贷记 IC卡数据*/

#define BE_TO_LE16(X) ((uint16_t)((((uint16_t)(X) & (uint16_t)0x00ffU) << 8) | (((uint16_t)(X) & (uint16_t)0xff00U) >> 8)))

//终端主密钥
//#define TERM_MK "\xEC\xFB\x01\x46\x83\xCE\x52\xC8\xD5\xD6\x6D\xC7\x1F\xF4\x34\xC4"

//判断CupPoSTerm函数是否执行成功
#define ASSERT_FAIL(e) \
    if (e != SLSY_TRANS_SUCCESS)\
    {\
        return e;\
    }

//成功
#define SLSY_TRANS_SUCCESS				0x00

//失败
#define SLSY_TRANS_ERROR				0x01

//无响应
#define SLSY_TRANS_NOACK				0xFF

//原交易不存在
#define SLSY_TRANS_SRC_TRANS_NO_EXSIT	0x02

typedef struct CUP8583_MSG_Field
{
    int StateFlag;
    int Length;
    unsigned char Buf[MAX_BUFF_LENGTH];
}CUP8583_MSG_Field;

typedef struct CUP8583_MSGpkg
{
    struct CUP8583_MSG_Field Field[128];   //数据域
}CUP8583_MSG;

typedef struct CUP8583_DEF
{
    int				Length;   /* data element max length */
    unsigned char   Type;     /* bit0,bit1暂留 , bit2:为0时左对齐,为1时右对齐 , bit3:为0时BCD码,为1时ASC码 , type:0,1,2,3三种 */
    unsigned char   Flag;     /* length field length: 0--F_FIX_LEN型 1--F_LLVAR_LEN型 2--F_F_LLLVAR_LEN型 */
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
    unsigned char tradetype;//信用卡/借贷款
    char cardno[20];
    char citiao2data[38];
    char citiao2data_l;
    char serialno[3];//卡片序列号
    int trademoney;
    unsigned char data55[140];
    char len55;
    char validdate[4];
}PACKED cardreceiveA2;

#endif // UNIONPAYSTRUCTURE_H
