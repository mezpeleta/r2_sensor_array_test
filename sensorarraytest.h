#ifndef SENSORARRAYTEST_H
#define SENSORARRAYTEST_H

#define DEBUG false

#include <QMainWindow>
#include <QDebug>
#include <QFile>
#include <QInputDialog>
#include <QDateTime>
#include <QRegularExpression>
#include <QTextStream>
#include <QMap>
#include <QDir>
#include <QMessageBox>
#include <QSettings>
#include <QCloseEvent>

#include "defines.h"

#include "pylon/PylonIncludes.h"
#include "Intel/include/librealsense2/rs.hpp"
#include "libusb/libusb-MinGW-x64/include/libusb-1.0/libusb.h"

QT_BEGIN_NAMESPACE
namespace Ui
{
class SensorArrayTest;
}
QT_END_NAMESPACE

using namespace Pylon;

class BannerStyles
{
public:
    BannerStyles(QString label, QString style)
    {
        _label = label;
        _style = style;
    }

    BannerStyles()
    {
        _label = "";
        _style = "";
    }

    QString Label(){ return _label; }
    QString Style(){ return _style; }

private:
    QString _label;
    QString _style;
};

typedef  QMap<QString, BannerStyles> StatusMap;

typedef enum DeviceTypes
{
    REALSENSE,
    CAMERA,
} DeviceTypes_t;

typedef struct USBDevice
{
    QString name;
    QString serial;
    DeviceTypes_t type;
    bool color;
} USBDevice_t;

typedef QList<USBDevice_t> USBDeviceList_t;

typedef enum BannerStates
{
    IDLE,
    RUNNING,
    PASS,
    FAIL,
} BannerStates_t;

class SensorArrayTest : public QMainWindow
{
    Q_OBJECT
    Q_ENUM(DeviceTypes_t)

public:
    SensorArrayTest(QWidget *parent = nullptr);
    ~SensorArrayTest() override;

    void enumerateECON();
    void enumerateBasler();
    void enumerateIntel();

    bool evaluateDevices();
    void createTestLog(bool &result);

signals:
    void requestPrepend(QString text);

public slots:
    void prependText(QString text);
    void setStatusLabel(BannerStyles style);
    void btnStart_Clicked();
    void btnClearLog_Clicked();
    void actionExit_Triggered();
    void actionAbout_Triggered();
    void closeEvent(QCloseEvent *event) override;

private:
    Ui::SensorArrayTest *ui;

    QString getOperatorInput(QString title, QString prompt);
    QString makeHeader(bool result, QString serial, QString operID, QString startTimeStamp, QString endTimeStamp, QString appVersion = APPLICATION_VERSION);
    QString makeRow(QString number, QString topic, QString tIndex, QString test, QString lowLim, QString value, QString hiLim, QString unit, QString result);
    QString appendSeparator(uint length = LOG_SEPARATOR_LEN, QChar symbol = LOG_SEPARATOR);
    bool saveLog(QString logStr);
    bool validateSerial(QString serial);
    QString makeTestNameEntry(QString testName, int index);
    StatusMap _indicatorMap;
    bool _testRunning = false;

    static USBDeviceList_t _devList;
    static int _colorCamCount;
    static int _monoCamCount;
    static int _realsenseCount;
    static int _cameraCount;
    static QDateTime _timeStampStart;
    static QDateTime _timeStampEnd;
    static QString _opID;
    static QString _serialNumber;
    static QString _header;
    static QStringList _testData;
    static QRegularExpression _regExp;
};
#endif // SENSORARRAYTEST_H
