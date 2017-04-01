/****************************************************************************
** Meta object code from reading C++ file 'myposwidget.h'
**
** Created: Sat Apr 1 14:01:58 2017
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "myposwidget.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'myposwidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.7.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_myPosWidget[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
      39,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      13,   12,   12,   12, 0x0a,
      31,   12,   12,   12, 0x0a,
      47,   12,   12,   12, 0x0a,
      65,   12,   12,   12, 0x0a,
      85,   12,   12,   12, 0x0a,
     108,   12,   12,   12, 0x0a,
     132,   12,   12,   12, 0x0a,
     158,   12,   12,   12, 0x0a,
     185,   12,   12,   12, 0x0a,
     212,   12,   12,   12, 0x0a,
     238,   12,   12,   12, 0x0a,
     266,   12,   12,   12, 0x0a,
     294,   12,   12,   12, 0x0a,
     325,  321,   12,   12, 0x0a,
     362,   12,   12,   12, 0x0a,
     381,   12,   12,   12, 0x0a,
     401,   12,   12,   12, 0x0a,
     420,   12,   12,   12, 0x0a,
     440,   12,   12,   12, 0x0a,
     467,   12,   12,   12, 0x0a,
     494,   12,   12,   12, 0x0a,
     523,   12,   12,   12, 0x0a,
     545,   12,   12,   12, 0x0a,
     583,  564,   12,   12, 0x0a,
     626,   12,   12,   12, 0x0a,
     652,   12,   12,   12, 0x0a,
     682,  673,   12,   12, 0x0a,
     716,   12,   12,   12, 0x0a,
     736,   12,   12,   12, 0x0a,
     766,  759,   12,   12, 0x0a,
     788,   12,   12,   12, 0x0a,
     814,   12,   12,   12, 0x0a,
     827,   12,   12,   12, 0x0a,
     842,   12,   12,   12, 0x0a,
     862,   12,   12,   12, 0x0a,
     884,   12,   12,   12, 0x0a,
     903,   12,   12,   12, 0x0a,
     925,   12,   12,   12, 0x0a,
     952,   12,   12,   12, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_myPosWidget[] = {
    "myPosWidget\0\0showTradeDialog()\0"
    "showPosWidget()\0processBeatInfo()\0"
    "processBeatAck_v1()\0processConsumeAck_v1()\0"
    "processWeixinScanInfo()\0"
    "processWeixinBeatAck_v1()\0"
    "processWeixinTradeAck_v1()\0"
    "processAlipayTradeAck_v1()\0"
    "processAlipayKeyInfo_v1()\0"
    "processAlipayBlackInfo_v1()\0"
    "processAlipayBlackInfo_v2()\0"
    "processAlipayCardType_v1()\0tag\0"
    "processUnionpayAck_v1(unionPayAck_t)\0"
    "processLongpress()\0processShortpress()\0"
    "processLeftpress()\0processRightpress()\0"
    "processCheckConfirmBusID()\0"
    "processCheckConfirmPosID()\0"
    "processCheckConfirmBusline()\0"
    "processShowPosParam()\0processDriveSign()\0"
    "strDriver,cardtype\0"
    "processDriveSign_v2(QString,unsigned char)\0"
    "processDriverSignAck_v1()\0"
    "processPosParamSet()\0posParam\0"
    "processPosParamSet_v2(posParam_t)\0"
    "processPosFtpInfo()\0processUpdataInfoAck()\0"
    "retAck\0processPscamAck(char)\0"
    "processStmUpdateSuccess()\0onTimerOut()\0"
    "failTimerOut()\0tradeScanTimerOut()\0"
    "tradeScanTimerOutV2()\0signScanTimerOut()\0"
    "onlineValidTimerOut()\0processUnionPayTradeInfo()\0"
    "processUnionPayTermMkInfo()\0"
};

const QMetaObject myPosWidget::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_myPosWidget,
      qt_meta_data_myPosWidget, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &myPosWidget::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *myPosWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *myPosWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_myPosWidget))
        return static_cast<void*>(const_cast< myPosWidget*>(this));
    return QWidget::qt_metacast(_clname);
}

int myPosWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: showTradeDialog(); break;
        case 1: showPosWidget(); break;
        case 2: processBeatInfo(); break;
        case 3: processBeatAck_v1(); break;
        case 4: processConsumeAck_v1(); break;
        case 5: processWeixinScanInfo(); break;
        case 6: processWeixinBeatAck_v1(); break;
        case 7: processWeixinTradeAck_v1(); break;
        case 8: processAlipayTradeAck_v1(); break;
        case 9: processAlipayKeyInfo_v1(); break;
        case 10: processAlipayBlackInfo_v1(); break;
        case 11: processAlipayBlackInfo_v2(); break;
        case 12: processAlipayCardType_v1(); break;
        case 13: processUnionpayAck_v1((*reinterpret_cast< unionPayAck_t(*)>(_a[1]))); break;
        case 14: processLongpress(); break;
        case 15: processShortpress(); break;
        case 16: processLeftpress(); break;
        case 17: processRightpress(); break;
        case 18: processCheckConfirmBusID(); break;
        case 19: processCheckConfirmPosID(); break;
        case 20: processCheckConfirmBusline(); break;
        case 21: processShowPosParam(); break;
        case 22: processDriveSign(); break;
        case 23: processDriveSign_v2((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< unsigned char(*)>(_a[2]))); break;
        case 24: processDriverSignAck_v1(); break;
        case 25: processPosParamSet(); break;
        case 26: processPosParamSet_v2((*reinterpret_cast< posParam_t(*)>(_a[1]))); break;
        case 27: processPosFtpInfo(); break;
        case 28: processUpdataInfoAck(); break;
        case 29: processPscamAck((*reinterpret_cast< char(*)>(_a[1]))); break;
        case 30: processStmUpdateSuccess(); break;
        case 31: onTimerOut(); break;
        case 32: failTimerOut(); break;
        case 33: tradeScanTimerOut(); break;
        case 34: tradeScanTimerOutV2(); break;
        case 35: signScanTimerOut(); break;
        case 36: onlineValidTimerOut(); break;
        case 37: processUnionPayTradeInfo(); break;
        case 38: processUnionPayTermMkInfo(); break;
        default: ;
        }
        _id -= 39;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
