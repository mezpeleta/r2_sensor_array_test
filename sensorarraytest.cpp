#include "sensorarraytest.h"
#include "ui_sensorarraytest.h"

USBDeviceList_t SensorArrayTest::_devList = USBDeviceList_t();
int SensorArrayTest::_colorCamCount = 0;
int SensorArrayTest::_monoCamCount = 0;
int SensorArrayTest::_realsenseCount = 0;
int SensorArrayTest::_cameraCount = 0;
QDateTime SensorArrayTest::_timeStampStart = QDateTime();
QDateTime SensorArrayTest::_timeStampEnd = QDateTime();
QString SensorArrayTest::_opID = "";
QString SensorArrayTest::_serialNumber = "";
QString SensorArrayTest::_header = "";
QStringList SensorArrayTest::_testData = QStringList();
QRegularExpression SensorArrayTest::_regExp = QRegularExpression();

SensorArrayTest::SensorArrayTest(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::SensorArrayTest)
{
    ui->setupUi(this);

    this->setWindowTitle(APPLICATION_NAME);

    connect(this, SIGNAL(requestPrepend(QString)), this, SLOT(prependText(QString)));
    connect(ui->btnStart, SIGNAL(clicked()), this, SLOT(btnStart_Clicked()));
    connect(ui->actionExit, SIGNAL(triggered()), this, SLOT(actionExit_Triggered()));
    connect(ui->actionAbout, SIGNAL(triggered()), this, SLOT(actionAbout_Triggered()));

    _indicatorMap =
    {
        {LABEL_TEXT_PASS, BannerStyles(LABEL_TEXT_PASS, STYLESHEET_BANNER_GREEN)},
        {LABEL_TEXT_FAIL, BannerStyles(LABEL_TEXT_FAIL, STYLESHEET_BANNER_RED)},
        {LABEL_TEXT_TESTING, BannerStyles(LABEL_TEXT_TESTING, STYLESHEET_BANNER_YELLOW)},
        {LABEL_TEXT_IDLE, BannerStyles(LABEL_TEXT_IDLE, STYLESHEET_BANNER_DEFAULT)},
        {LABEL_TEXT_INCOMPLETE, BannerStyles(LABEL_TEXT_INCOMPLETE, STYLESHEET_BANNER_BLUE)}
    };

    QDir logDir(LOG_FILE_PATH);

    if (!logDir.exists())
    {
        logDir.mkdir(LOG_FILE_PATH);
    }

    QSettings settings(APPLICATION_ORGANIZATION, APPLICATION_NAME);

    restoreGeometry(settings.value(TAG_WINDOW_GEOMETRY).toByteArray());
    restoreState(settings.value(TAG_WINDOW_STATE).toByteArray());
}

SensorArrayTest::~SensorArrayTest()
{
    delete ui;
}

void SensorArrayTest::enumerateECON()
{
    libusb_context *ctx = nullptr;
    libusb_device **devList;
    libusb_device_handle *devHandle = nullptr;
    size_t devCount = 0;
    int res = 0;
    libusb_device_descriptor devDescriptor;
    unsigned char string_buffer_product[4096];
    unsigned char string_buffer_serial[4096];

    emit requestPrepend("Enumerating uncliamed USB devices - E-Con Cameras");

    QCoreApplication::sendPostedEvents();
    QCoreApplication::processEvents();

    if (libusb_init(&ctx) >= 0)
    {
        devCount = static_cast<size_t>(libusb_get_device_list(ctx, &devList));

        for (int i = 0; i < static_cast<int>(devCount); i++)
        {
            res = libusb_get_device_descriptor(devList[i], &devDescriptor);

            if (res >= 0)
            {
                res = libusb_open(devList[i], &devHandle);

                if (res >= 0)
                {
                    res = libusb_get_string_descriptor_ascii(devHandle, devDescriptor.iProduct, string_buffer_product, sizeof(string_buffer_product));

                    if (res >= 0)
                    {
                        res = libusb_get_string_descriptor_ascii(devHandle, devDescriptor.iSerialNumber, string_buffer_serial, sizeof(string_buffer_serial));

                        if (res >= 0)
                        {
                            QString serial = QString(reinterpret_cast<const char*>(string_buffer_serial));
                            QString name = QString(reinterpret_cast<const char*>(string_buffer_product));

                            if (name.contains(ECON_MONO_DESCRIPTION, Qt::CaseInsensitive))
                            {
                                USBDevice dev;

                                dev.name = name;
                                dev.serial = serial;
                                dev.type = DeviceTypes::CAMERA;
                                dev.color = false;

                                _devList.append(dev);
                            }
                        }
                    }

                    libusb_close(devHandle);
                }
            }
        }
    }
}

void SensorArrayTest::enumerateBasler()
{
    PylonInitialize();

    CTlFactory &tlFactory = CTlFactory::GetInstance();
    DeviceInfoList baslerDevs;
    int baslerCount = tlFactory.EnumerateDevices(baslerDevs);

    emit requestPrepend("Enumerating Basler Devices");

    QCoreApplication::sendPostedEvents();
    QCoreApplication::processEvents();

    for (int i = 0; i < baslerCount; i++)
    {
        Pylon::CDeviceInfo devInfo = baslerDevs[static_cast<size_t>(i)];

        QString name = QString::fromStdString(devInfo.GetModelName().c_str());
        QString serial = QVariant::fromValue(QString::fromStdString(devInfo.GetSerialNumber().c_str())).toString();

        if (name.contains(BASLER_COLOR_DESCRIPTION, Qt::CaseInsensitive) || name.contains(BASLER_MONO_DESCRIPTION, Qt::CaseInsensitive))
        {
            USBDevice dev;

            dev.name = name;
            dev.serial = serial;
            dev.type = DeviceTypes::CAMERA;
            dev.color = name.contains(BASLER_COLOR_DESCRIPTION, Qt::CaseInsensitive) ? true : false;

            _devList.append(dev);
        }
    }

    PylonTerminate();
}

void SensorArrayTest::enumerateIntel()
{
    rs2::context ctx;
    rs2::device_list dev_list = ctx.query_devices();

    emit requestPrepend("Enumerating Realsense Devices");

    QCoreApplication::sendPostedEvents();
    QCoreApplication::processEvents();

    for (const auto&& dev : dev_list)
    {
        QString name = dev.get_info(RS2_CAMERA_INFO_NAME);
        QString serial = dev.get_info(RS2_CAMERA_INFO_SERIAL_NUMBER);

        if (name.contains(REALSENSE_DESCRIPTION, Qt::CaseInsensitive))
        {
            USBDevice dev;

            dev.name = name;
            dev.serial = serial;
            dev.type = DeviceTypes::REALSENSE;
            dev.color = true;

            _devList.append(dev);
        }
    }
}

bool SensorArrayTest::evaluateDevices()
{
    bool retVal = false;

    emit requestPrepend("Evaluating emunerated devices...");

    QCoreApplication::sendPostedEvents();
    QCoreApplication::processEvents();

    for (const auto &dev : qAsConst(_devList))
    {
        if (dev.type == DeviceTypes_t::CAMERA)
        {
            _cameraCount++;

            if (dev.color)
            {
                _colorCamCount++;
            }
            else
            {
                _monoCamCount++;
            }
        }
        else
        {
            _realsenseCount++;
        }

        retVal = true;
    }

    emit requestPrepend(QString("Total Devices: %1").arg(_cameraCount + _realsenseCount));
    emit requestPrepend(QString("Total Standard Cameras: %1").arg(_cameraCount));
    emit requestPrepend(QString("Total Realsense Cameras: %1").arg(_realsenseCount));
    emit requestPrepend(QString("Color Cameras: %1").arg(_colorCamCount));
    emit requestPrepend(QString("Monochrome Cameras: %1").arg(_monoCamCount));

    QCoreApplication::sendPostedEvents();
    QCoreApplication::processEvents();

    _timeStampEnd = QDateTime::currentDateTime();

    return retVal;
}

void SensorArrayTest::createTestLog(bool &result)
{
    int grpCount = 0;
    uint testCount = 0;
    QString header;
    QStringList testResults;
    int totalDevCount = _cameraCount + _realsenseCount;
    QString resStr = "";
    bool totalResult = true;
    bool colorOK = false;
    bool monoOK = false;
    double duration = static_cast<double>(_timeStampStart.msecsTo(_timeStampEnd))/1000.0;
    bool tmpRes = false;

    _testData.clear();
    _header.clear();

    testResults << makeRow("Num", "Test Group", "Index", "Test Name", "LL", "Value", "HL", "Unit", "Result");
    testResults << appendSeparator();
    testResults << makeRow(QString::number(++grpCount), "Device Counts", "", "", "", "", "", "", "");

    tmpRes = ((totalDevCount >= TOTAL_DEVICES_LOW_LIM) && (totalDevCount <= TOTAL_DEVICES_HIGH_LIM));
    totalResult &= tmpRes;
    resStr = tmpRes ? "PASS" : "FAIL";
    testResults << makeRow("", "", QString::number(++testCount), TEST_NAME_TOTAL_DEVICES, QString::number(TOTAL_DEVICES_LOW_LIM),
                           QString::number(totalDevCount), QString::number(TOTAL_DEVICES_HIGH_LIM), "EA", resStr);

    tmpRes = (_realsenseCount == REALSENSE_COUNT_LOW_LIM);
    totalResult &= tmpRes;
    resStr = tmpRes ? "PASS" : "FAIL";
    testResults << makeRow("", "", QString::number(++testCount), TEST_NAME_REALSENSE_COUNT, QString::number(REALSENSE_COUNT_LOW_LIM),
                           QString::number(_realsenseCount), QString::number(REALSENSE_COUNT_HIGH_LIM), "EA", resStr);

    tmpRes = ((_cameraCount >= CAMERA_COUNT_LOW_LIM) && (_cameraCount >= CAMERA_COUNT_HIGH_LIM));
    totalResult &= tmpRes;
    resStr = tmpRes ? "PASS" : "FAIL";
    testResults << makeRow("", "", QString::number(++testCount), TEST_NAME_CAMERA_COUNT, QString::number(CAMERA_COUNT_LOW_LIM),
                           QString::number(_cameraCount), QString::number(CAMERA_COUNT_HIGH_LIM), "EA", resStr);

    testResults << appendSeparator();
    testResults << makeRow(QString::number(++grpCount), "Sensor Configuration", "", "", "", "", "", "", "");

    // Reset the test index for the next topic
    testCount = 0;

    // Center cam is color, side cams are either... Min 1 color cam if 2x mono - these params are tied together for config
    if ((_colorCamCount == REQUIRED_COLOR_CAMERA_COUNT_MIN) && (_monoCamCount == REQUIRED_MONO_CAMERA_MAX))
    {
        colorOK = true;
        monoOK = true;
    }
    else if ((_colorCamCount == REQUIRED_COLOR_CAMERA_COUNT_MAX) && (_monoCamCount == REQUIRED_MONO_CAMERA_MIN))
    {
        colorOK = true;
        monoOK = true;
    }
    else if (_colorCamCount == REQUIRED_COLOR_CAMERA_COUNT_MIN)
    {
        if (_monoCamCount < REQUIRED_MONO_CAMERA_MAX)
        {
            colorOK = true;
            monoOK = false;
        }
    }
    else if (_monoCamCount == REQUIRED_MONO_CAMERA_MAX)
    {
        if (_colorCamCount < REQUIRED_COLOR_CAMERA_COUNT_MIN)
        {
            colorOK = false;
            monoOK = true;
        }
    }

    tmpRes = monoOK;
    totalResult &= tmpRes;
    resStr = tmpRes ? "PASS" : "FAIL";
    testResults << makeRow("", "", QString::number(++testCount), TEST_NAME_MONO_CAM_COUNT, MONO_CAM_COUNT_LOW_LIM,
                           QString::number(_monoCamCount), MONO_CAM_COUNT_HIGH_LIM, "EA", resStr);

    tmpRes = colorOK;
    totalResult &= tmpRes;
    resStr = tmpRes ? "PASS" : "FAIL";
    testResults << makeRow("", "", QString::number(++testCount), TEST_NAME_COLOR_CAM_COUNT, QString::number(COLOR_CAM_COUNT_LOW_LIM),
                           QString::number(_colorCamCount), QString::number(COLOR_CAM_COUNT_HIGH_LIM), "EA", resStr);

    tmpRes = monoOK && colorOK;
    totalResult &= tmpRes;
    resStr = tmpRes ? "PASS" : "FAIL";
    testResults << makeRow("", "", QString::number(++testCount), TEST_SENSOR_CONFIGURATION, QString::number(ARRAY_CONFIG_LOW_LIM),
                           (monoOK && colorOK) ? "1" : "-1", QString::number(ARRAY_CONFIG_HIGH_LIM), "EA", resStr);

    testResults << appendSeparator();

    testResults << makeRow(QString::number(++grpCount), "Total Test Time", "", "", "", QString::number(duration), "", "sec", "");

    testResults << appendSeparator();

    result = totalResult;

    _header = makeHeader(result, _serialNumber, _opID, _timeStampStart.toString(LOG_TIMESTAMP_FORMAT), _timeStampEnd.toString(LOG_TIMESTAMP_FORMAT), APPLICATION_VERSION);

    _testData = testResults;
}

void SensorArrayTest::prependText(QString text)
{
    QString curText = ui->txtLog->toPlainText();

    if ((curText.length() + text.length() >= LOG_MAX_LEN))
    {
        curText = curText.left(curText.length() + text.length() - LOG_TRIM_LENGTH);
    }

    curText = text + "\n" + curText;

    ui->txtLog->setPlainText(curText);
}

void SensorArrayTest::setStatusLabel(BannerStyles style)
{
    ui->lblTestStatus->setText(style.Label());
    ui->lblTestStatus->setStyleSheet(style.Style());
}

void SensorArrayTest::btnStart_Clicked()
{
    QString postToWindow;

    _timeStampStart = QDateTime::currentDateTime();
    _timeStampEnd = QDateTime();
    _devList.clear();
    _colorCamCount = 0;
    _monoCamCount = 0;
    _realsenseCount = 0;
    _cameraCount = 0;
    _opID = "";
    _serialNumber = "";
    _testData.clear();

    setStatusLabel(_indicatorMap[LABEL_TEXT_TESTING]);

    _testRunning = true;

    _serialNumber = getOperatorInput("Serial Number", "Please scan the assembly\'s Serial Number").toUpper();

    if (!_serialNumber.isEmpty() && validateSerial(_serialNumber))
    {
        _opID = getOperatorInput("Operator ID", "Please scan or enter your Operator ID").toUpper();

        if (!_opID.isEmpty())
        {
            bool result = false;

            enumerateECON();
            enumerateBasler();
            enumerateIntel();

            result = evaluateDevices();

            createTestLog(result);

            postToWindow = _header;

            for (const auto &row : qAsConst(_testData))
            {
                postToWindow += "\n" + row;
            }

            emit requestPrepend(postToWindow);

            if (saveLog(postToWindow))
            {
                if (result)
                {
                    setStatusLabel(_indicatorMap[LABEL_TEXT_PASS]);
                }
                else
                {
                    setStatusLabel(_indicatorMap[LABEL_TEXT_FAIL]);
                }
            }
            else
            {
                emit requestPrepend("Unexpected Error - Failed to save test log");

                setStatusLabel(_indicatorMap[LABEL_TEXT_INCOMPLETE]);
            }
        }
        else
        {
            emit requestPrepend("Invalid Operator ID or user cancelled");

            setStatusLabel(_indicatorMap[LABEL_TEXT_INCOMPLETE]);
        }
    }
    else
    {
        emit requestPrepend("Invalid Serial Number or user cancelled");

        setStatusLabel(_indicatorMap[LABEL_TEXT_INCOMPLETE]);
    }

    _testRunning = false;
}

void SensorArrayTest::actionExit_Triggered()
{
    close();
}

void SensorArrayTest::actionAbout_Triggered()
{
    QMessageBox msg;
    QString aboutStr = "Test Application Name: " + QString(APPLICATION_NAME) + "\n";

    aboutStr += "Test Application Version: " + QString(APPLICATION_VERSION);

    msg.setWindowTitle("About");
    msg.setText(aboutStr);

    msg.exec();
}

void SensorArrayTest::closeEvent(QCloseEvent *event)
{
    if (!_testRunning)
    {
        QSettings settings(APPLICATION_ORGANIZATION, APPLICATION_NAME);

        settings.setValue(TAG_WINDOW_GEOMETRY, saveGeometry());
        settings.setValue(TAG_WINDOW_STATE, saveState());

        event->accept();
    }
    else
    {
        QMessageBox::warning(this, "TESTING", "Cannot exit while the test is running!");

        event->ignore();
    }
}

QString SensorArrayTest::getOperatorInput(QString title, QString prompt)
{
    QString response = "";
    bool okCancel = true;

    while (response.isEmpty() && okCancel)
    {
        response = QInputDialog::getText(this, title, prompt, QLineEdit::EchoMode::Normal, "", &okCancel);
    }

    return response;
}

QString SensorArrayTest::makeHeader(bool result, QString serial, QString operID, QString startTimeStamp, QString endTimeStamp, QString appVersion)
{
    QString header = "";

    header += appendSeparator();
    header += "Serial Number: " + serial + "\n";
    header += "Operator Identifier: " + operID + "\n";
    header += "Test Application Version: " + QString(appVersion) + "\n";
    header += "Test Start Time: " + startTimeStamp + "\n";
    header += "Test End Time: " + endTimeStamp + "\n";
    header += appendSeparator();
    header += QString("Test Result: ") + ((result) ? "PASS" : "FAIL") + "\n";
    header += appendSeparator();

    return header;
}

QString SensorArrayTest::makeRow(QString number, QString topic, QString tIndex, QString test, QString lowLim, QString value, QString hiLim, QString unit, QString result)
{
    QString row = number.rightJustified(4,' ');

    row += topic.rightJustified(25, ' ');
    row += tIndex.rightJustified(6, ' ');
    row += test.rightJustified(20, ' ');
    row += lowLim.rightJustified(4, ' ');
    row += value.rightJustified(6, ' ');
    row += hiLim.rightJustified(4, ' ');
    row += unit.rightJustified(5, ' ');
    row += result.rightJustified(7, ' ');

    return row;
}

QString SensorArrayTest::appendSeparator(uint length, QChar symbol)
{
    return QString(static_cast<int>(length), symbol) + "\n";
}

bool SensorArrayTest::saveLog(QString logStr)
{
    bool retVal = false;
    QFile fh(QString(LOG_FILE_PATH) + "\\" + QString(LOG_FILE_NAME).arg(_serialNumber, _timeStampStart.toString(LOG_TIMESTAMP_FORMAT)));
    QTextStream tStream(&fh);

    if (fh.open(QIODevice::ReadWrite | QIODevice::Text))
    {
        tStream << logStr;

        fh.close();

        retVal = true;
    }

    return retVal;
}

bool SensorArrayTest::validateSerial(QString serial)
{
    bool retVal = false;
    QRegularExpressionMatch match;

#if DEBUG
    Q_UNUSED(serial)

    retVal = true;
#else
    _regExp.setPattern(SERIAL_REG_EXPR);
    match = _regExp.match(serial);

    if (match.hasMatch())
    {
        retVal = true;
    }
#endif

    return retVal;
}

QString SensorArrayTest::makeTestNameEntry(QString testName, int index)
{
    return QString("%1").arg(QString::number(index)).rightJustified(3, ' ') + QString(" ") + testName;
}

