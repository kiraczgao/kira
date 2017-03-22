#ifndef CRC32_H
#define CRC32_H

#include <QFile>

//原算法信息
/*****************************************************
** Author       : gzshun
** Version      : 1.0
** Date         : 2011-12
** Description  : CRC32 Checking
** URL          : http://www.linuxidc.com/Linux/2011-12/49710.htm
******************************************************/

//改写
/*****************************************************
** Author       : chenx
** Version      : 2.0
** Date         : 2017-3
** Description  : CRC32 算法C语言实现C++封装，算法全部静态函数无需创建实例
**                考虑到跨平台因素，数据类型使用QT类型
******************************************************/

class CRC32
{
public:
    //初始化CRC表
    //static void init_crc_table();
    //计算buffer的crc校验函数
    static quint32 crc32(quint32 crc, quint8 *buffer, quint32 size);
    //计算大文件使用的CRC校验函数
    //chenx注：因为是QT，所以使用QFile，如果是标准C/C++，可以考虑换用FILE或者fstream来操作
    static qint32 calc_img_crc(QFile *in_file, quint32 crc, quint32 *img_crc);

private:
    //计算CRC中间值内部函数
    static quint32 crc32_priv(quint32 crc, quint8 *buffer, quint32 size);

private:
    //是否已初始化
    //static bool isInited;
    //CRC表
    static const quint32 crc_table[256];
    //本地缓存大小，用以大文件计算时使用
    static const quint32 BUFSIZE;
};

#endif // CRC32_H
