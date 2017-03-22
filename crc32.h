#ifndef CRC32_H
#define CRC32_H

#include <QFile>

//ԭ�㷨��Ϣ
/*****************************************************
** Author       : gzshun
** Version      : 1.0
** Date         : 2011-12
** Description  : CRC32 Checking
** URL          : http://www.linuxidc.com/Linux/2011-12/49710.htm
******************************************************/

//��д
/*****************************************************
** Author       : chenx
** Version      : 2.0
** Date         : 2017-3
** Description  : CRC32 �㷨C����ʵ��C++��װ���㷨ȫ����̬�������贴��ʵ��
**                ���ǵ���ƽ̨���أ���������ʹ��QT����
******************************************************/

class CRC32
{
public:
    //��ʼ��CRC��
    //static void init_crc_table();
    //����buffer��crcУ�麯��
    static quint32 crc32(quint32 crc, quint8 *buffer, quint32 size);
    //������ļ�ʹ�õ�CRCУ�麯��
    //chenxע����Ϊ��QT������ʹ��QFile������Ǳ�׼C/C++�����Կ��ǻ���FILE����fstream������
    static qint32 calc_img_crc(QFile *in_file, quint32 crc, quint32 *img_crc);

private:
    //����CRC�м�ֵ�ڲ�����
    static quint32 crc32_priv(quint32 crc, quint8 *buffer, quint32 size);

private:
    //�Ƿ��ѳ�ʼ��
    //static bool isInited;
    //CRC��
    static const quint32 crc_table[256];
    //���ػ����С�����Դ��ļ�����ʱʹ��
    static const quint32 BUFSIZE;
};

#endif // CRC32_H
