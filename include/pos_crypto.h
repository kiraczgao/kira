#ifndef __POS_CRYPTO_H__
#define __POS_CRYPTO_H__

#ifdef __cplusplus
extern "C" {
#endif

#define SUCCESS					 1
#define MALFORMED_QRCODE		-1
#define QRCODE_INFO_EXPIRED 	-2
#define QRCODE_KEY_EXPIRED  	-3
#define POS_PARAM_ERROR			-4
#define QUOTA_EXCEEDED			-5
#define NO_ENOUGH_MEMORY   		-6
#define SYSTEM_ERROR 			-7
#define CARDTYPE_UNSUPPORTED 	-8
#define NOT_INITIALIZED			-9
#define ILLEGAL_PARAM			-10
#define PROTO_UNSUPPORTED 		-11
#define QRCODE_DUPLICATED		-12

#define ANT_BUS_PAY "ANT00001"

/**
*  验证二维码的请求
*  参数：qrcode 格式的二维码信息
*  参数：qrcode_len 二维码长度
*  参数：master_pub_key hex格式的支付宝公钥
		例："028C452939E766BB17DE9EAE0390F7896545D40C565859C9FBFB1B424974915D86"
*  参数：pos_param json形式的pos信息 长度不能大于2048
		ex：{
				"pos_id":"sh001",
				"type":"SINGLE",
				"subject":"杭州公交190路",
				"record_id":"123456"
			}
*  参数：amount_cent 交易金额 如果无交易金额（如两次过闸机才算交易的情况）就传入0
*/
typedef struct _verify_request{
    const unsigned char* qrcode;
    int qrcode_len;
    const char* master_pub_key;
    const char* pos_param;
	int amount_cent;
}VERIFY_REQUEST;

/**
 * 验证二维码的响应信息
 * @param: uid 存放输出uid的buffer
 * @param: uid_len 输出的uid的buffer长度 不能小于17
 * @param: record 存放输出record的buffer 返还的record是标准字符串信息
 * @param: record_len 存record的buffer的长度 不能小于2048
 */
typedef struct _verify_response{
	char* uid;
	int uid_len;
	char* record;
	int record_len;
}VERIFY_RESPONSE;

/**
*  验证二维码的请求 版本二
*  参数：qrcode 格式的二维码信息
*  参数：qrcode_len 二维码长度
*  参数：pos_param json形式的pos信息 长度不能大于1024
		ex：{
				"pos_id":"sh001",
				"type":"SINGLE",
				"subject":"杭州公交190路",
				"record_id":"123456"
			}
*  参数：amount_cent 交易金额 如果无交易金额（如两次过闸机才算交易的情况）就传入0
*/
typedef struct _verify_request_v2{
    const unsigned char* qrcode;
    int qrcode_len;
    const char* pos_param;
	int amount_cent;
}VERIFY_REQUEST_V2;

/**
 * 验证二维码的响应信息 版本二
 * @param: uid 存放输出uid的buffer
 * @param: uid_len 输出的uid的buffer长度 不能小于17
 * @param: record 存放输出record的buffer 返还的record是标准字符串信息
 * @param: record_len 存record的buffer的长度 不能小于2048
 * @param: card_no 存放输出卡号的buffer
 * @param: card_no_len 存card_no的buffer长度 不能小于17
 * @param: card_data 存放输出卡数据的buffer
 * @param: card_data_len 存card_data的buffer长度 不能小于65
 * @param: card_type 存放输出卡类型的buffer
 * @param: card_type_len 存card_type的buffer长度 不能小于9
 */
typedef struct _verify_response_v2{
	char* uid;
	int uid_len;
	char* record;
	int record_len;
	char* card_no;
	int card_no_len;
	unsigned char* card_data;
	int card_data_len;
	char* card_type;
	int card_type_len;
}VERIFY_RESPONSE_V2;

/**
 * 初始化验证二维码
 * 录入支付宝公钥以及支持的卡类型
 * @param: key_list_json 从支付宝开放网关拉取的秘钥信息，以json形式组织成字符串传入
 *   ex:
 		[
			{"key_id":0,"public_key":"02170D3C441AF17AE1010A4095B974BF1FE1EA48FCD65BE060A5AD577ABB885088"},
		    {"key_id":1,"public_key":"03410D92CDAB5BC9349731136619A93FC3225DE6B235E839F6BEB41A77B79A0424"},
		    ...
	    ]
 * @param: card_type_list 可支持的卡类型列表
 * @param:
 * 	ex:
		[
			"HZ000001",
			"HZ000002",
			"WH000001"
		]
 */
int init_pos_verify(const char* key_list, const char* card_type_list);

/**
 * 验证二维码信息 版本二
 * @param：request 验证请求
 * @return: 成功返回 1 失败见错误码
 *
 * */
int verify_qrcode_v2(VERIFY_REQUEST_V2* request_v2, 
					VERIFY_RESPONSE_V2* response_v2);

/**
 * 获取二维码中指定的密钥id
 * @param: qrcode 二进制的二维码信息
 * @return: 成功返回大于等于0的密钥ID 失败见错误码
 *
 * */	
int get_key_id(	const unsigned char* qrcode);

/**
 * 验证二维码信息
 * @param：request 验证请求
 * @return: 成功返回 1 失败见错误码
 *
 * */
int verify_qrcode(VERIFY_REQUEST* request,
				VERIFY_RESPONSE* response);

/**
 * 获取当前so库版本号
 * @return: 返回当前so库版本号
 */
char* get_version();

#ifdef __cplusplus
}
#endif
#endif
