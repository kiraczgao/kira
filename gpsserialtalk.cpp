#include "gpsserialtalk.h"
#include "string"
#include "vector"
#include <QTimer>

gpsSerialTalk::gpsSerialTalk(QWidget *parent) : QWidget(parent)
{
    qDebug("kira --- Initial gps");
    memset(&gpsInfo,0,sizeof(gpsSerialInfo));
    pSerialPort = new boost::asio::serial_port(m_ios);
    pTimer = new boost::asio::deadline_timer(m_ios);
    m_bSucess = false;
    //New not success
    if ( !pSerialPort ){
        return;
    }
    if(pSerialPort->is_open()){
        m_bSucess = true;
        return;
    }
    //Open Serial port object
    pSerialPort->open("/dev/ttySP4",ec);
    if(ec){
        printf("qwy --- open ttySP4 error %s\n",ec.message().c_str());
        return;
    }
    //Set port argument
    pSerialPort->set_option(boost::asio::serial_port::baud_rate(9600),ec);
    pSerialPort->set_option(boost::asio::serial_port::flow_control(boost::asio::serial_port::flow_control::none),ec);
    pSerialPort->set_option(boost::asio::serial_port::parity(boost::asio::serial_port::parity::none),ec);
    pSerialPort->set_option(boost::asio::serial_port::stop_bits(boost::asio::serial_port::stop_bits::one),ec);
    pSerialPort->set_option(boost::asio::serial_port::character_size(8),ec);
    m_bSucess = true;

    m_bHaveData = true;
    boost::function<void()> f = boost::bind(&gpsSerialTalk::Run,this,(void*)0);
    m_thr = boost::thread(f);
}

void gpsSerialTalk::handle_read(char* buf,boost::system::error_code ec,std::size_t bytes_transferred)
{
    if (!ec){
        pTimer->cancel();
    }
    std::string lines = buf;
    std::vector<std::string> vec;
    boost::split(vec,lines,boost::is_any_of("\r\n"),boost::token_compress_on);
    for(int i=0;i<vec.size();++i)
    {
        gpsSerialInfo tag;
        memset(&tag,0,sizeof(gpsSerialInfo));
        if(GNRMCAnalysis(vec[i],tag))
        {
            m_bHaveData = true;
            QMutexLocker locker(&dataMutex);
            //copy tmpvalue
            memset(&gpsInfo,0,sizeof(gpsSerialInfo));
            memcpy(&gpsInfo,&tag,sizeof(gpsSerialInfo));
        }
    }
}

void gpsSerialTalk::Run(void* p)
{
    while(1)
    {
        m_bHaveData = false;
        read_from_serial();
        call_handle(300);

        if(m_bHaveData){
            boost::this_thread::sleep( boost::posix_time::seconds(3));
        }
    }
}

void gpsSerialTalk::getGpsInfo(gpsSerialInfo *pgpsInfo)
{
    QMutexLocker locker(&dataMutex);
    memcpy(pgpsInfo, &gpsInfo, sizeof(gpsSerialInfo));
}

//Write some data to port
void gpsSerialTalk::write_to_serial(const char* data, int isize)
{
    if(!m_bSucess) return;
    boost::asio::write(*pSerialPort,boost::asio::buffer(data,isize),ec);
}

//Read data from port which write data just now
void gpsSerialTalk::read_from_serial()
{
    memset(m_buf,0,1024);
    if(!m_bSucess) return;
    boost::asio::async_read(*pSerialPort,boost::asio::buffer(m_buf,1024),boost::bind(&gpsSerialTalk::handle_read,this,m_buf,_1,_2));
}

//To Call io_service::run function to call asyanc callback funciton
void gpsSerialTalk::call_handle(int timeout_millisec)
{
    if(!m_bSucess) return;
    //There can use deadline_timer to cancle serial_port read data
    pTimer->expires_from_now(boost::posix_time::millisec(timeout_millisec));
    pTimer->async_wait(boost::bind(&boost::asio::serial_port::cancel,boost::ref(*pSerialPort)));
    //Wait for call callback function
    m_ios.run();
    m_ios.reset();
}

double gpsSerialTalk::GPSTransforming(std::string _Value)
{
    double Ret = 0.0;
    std::vector<std::string> TempStr;
    boost::split(TempStr,_Value,boost::is_any_of("."),boost::token_compress_on);
    if(TempStr.size()<2)
        return (0.0);
    if(TempStr[0].length()<4 || TempStr[1].length()<5)
        return (0.0);
    std::string x = TempStr[0].substr(0, TempStr[0].length() - 2);
    std::string y = TempStr[0].substr(TempStr[0].length() - 2, 2);
    std::string z = TempStr[1].substr(0, 5);
    if(z.length()!=5){
        printf("%s %d\n",z.c_str(),z.length());
    }
    Ret = atof(x.c_str()) + atof(y.c_str()) / 60 + atof(z.c_str()) / 6000000;
    return Ret;
}

bool gpsSerialTalk::GNRMCAnalysis(std::string _RecString,gpsSerialInfo& gps)
{
    std::vector<std::string> vec;
    boost::split(vec,_RecString,boost::is_any_of(","),boost::token_compress_off);
    if(vec.size() < 10)
        return false;
    vec[0].erase(remove_if(vec[0].begin(), vec[0].end(), isspace), vec[0].end());
    if(vec[0].compare("$GNRMC")!=0)
        return false;
    //时间
    vec[1].erase(remove_if(vec[1].begin(), vec[1].end(), isspace), vec[1].end());
    //纬度
    vec[3].erase(remove_if(vec[3].begin(), vec[3].end(), isspace), vec[3].end());
    if(vec[3].empty())
        return false;
    double dlat = GPSTransforming(vec[3]);
    vec[4].erase(remove_if(vec[4].begin(), vec[4].end(), isspace), vec[4].end());
    sprintf((char*)gps.latitude,"%.5f%s",dlat,vec[4].c_str());
    //经度
    vec[5].erase(remove_if(vec[5].begin(), vec[5].end(), isspace), vec[5].end());
    if(vec[5].empty())
        return false;
    double dlon = GPSTransforming(vec[5]);
    vec[6].erase(remove_if(vec[6].begin(), vec[6].end(), isspace), vec[6].end());
    sprintf((char*)gps.longitude,"%.5f%s",dlon,vec[6].c_str());
    if(outofChina(dlat,dlon)){
        printf("qwy --- out of china\n");
        return false;
    }
    //速度
    vec[7].erase(remove_if(vec[7].begin(), vec[7].end(), isspace), vec[7].end());
    if(boost::all(vec[7],boost::is_digit()|| boost::is_any_of(".")))
    {
        double dspeed = atof(vec[7].c_str()) * 1.852;
        sprintf((char*)gps.speed,"%.2f",dspeed);
    }
    //方向
    vec[8].erase(remove_if(vec[8].begin(), vec[8].end(), isspace), vec[8].end());
    if(boost::all(vec[8],boost::is_digit()|| boost::is_any_of(".")))
    {
        double dangle = atof(vec[8].c_str());
        sprintf((char*)gps.direction,"%.1f",dangle);
    }
    //日期
    vec[9].erase(remove_if(vec[9].begin(), vec[9].end(), isspace), vec[9].end());
    //--------------------------------------------------------------------------
    if(vec[1].length()>=6 && vec[9].length()>=6)
    {
        if(boost::all(vec[1],boost::is_digit()|| boost::is_any_of(".")) && boost::all(vec[9],boost::is_digit()|| boost::is_any_of(".")))
        {
            std::string datetime = "20";
            datetime += vec[9].substr(4,2);
            datetime += vec[9].substr(2,2);
            datetime += vec[9].substr(0,2);
            datetime += "T";
            datetime += vec[1].substr(0,2);
            datetime += vec[1].substr(2,2);
            datetime += vec[1].substr(4,2);
            boost::posix_time::ptime p = boost::posix_time::from_iso_string(datetime);
            p += boost::posix_time::hours(8);
            std::string dt = boost::posix_time::to_simple_string(p);
            memcpy(gps.datetime,dt.c_str(),dt.length());
        }
    }
    //-------------------------------------------------------------------------
    return true;
}

//中国坐标内
bool gpsSerialTalk::outofChina(double lat, double lon)
{
    if (lon < 72.004 || lon > 137.8347)
        return true;
    if (lat < 0.8293 || lat > 55.8271)
        return true;
    return false;
}

