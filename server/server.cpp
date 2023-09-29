#include "server.h"

#include <QCoreApplication>
#include <QFileInfo>
//#include <QtDebug>

#define DEVICE_SERVER_PATH "/data/lcoal/tmp/scrcpy-server.jar"
#define SOCKET_NAME "scrcpy"

server::server(QObject *parent)
    :QObject(parent)
{
    connect(&m_workProcess, &AdbProcess::adbProcessResult, this, &server::onWorkProcessResult);
}

/*
 * 1.将scrcpy push到手机上
* 2.反向连接端口
* 3.启动服务
* （如果失败都需请理前面的步骤）
*/

bool server::start(const QString &serial, quint16 localPort, quint16 maxSize, quint16 bitRate)
{
    m_serial = serial;
    m_localPort = localPort;
    m_maxSize = maxSize;
    m_bitRate = bitRate;

    m_serverStartStep = SSS_PUSH;

    return startServerByStep();
}

//根据adbprocess中信号进行跳转
void server::onWorkProcessResult(AdbProcess::ADB_PROCESS_RESULT processResult)
{
    if (m_serverStartStep != SSS_NULL) {
        switch (m_serverStartStep) {
        case SSS_PUSH:
            if (processResult == AdbProcess::AER_SUCCESS_EXEC) {
                m_serverStartStep = SSS_ENABLE_REVERSE;
                startServerByStep();
            } else {
                qCritical() << "adb push failed!";
                m_serverStartStep = SSS_NULL;
                emit serverStartResult(false);
            }
            break;
        case SSS_ENABLE_REVERSE:
            if (processResult == AdbProcess::AER_SUCCESS_EXEC) {
                m_serverStartStep = SSS_EXECUTE_SERVER;
                startServerByStep();
            } else {
                qCritical() << "adb reverse failed!";
                m_serverStartStep = SSS_NULL;
                //这里失败处理不同，因为上一步失败只需报错，这一步失败需要将上一步push的删除

                emit serverStartResult(false);
            }
            break;
        default:
            break;
        }
    }
}

//根据状态变量，设置一个状态机，然后一步步执行
bool server::startServerByStep()
{
    bool stepSuccess = false;

    if(m_serverStartStep != SSS_NULL) {
        switch (m_serverStartStep) {
        case SSS_PUSH:
            //使用封装好的adb进程，初始化一个adbprocess
            stepSuccess = pushServer();
            break;
        case SSS_ENABLE_REVERSE:
            stepSuccess = enableTunnelReverse();
            break;
        case SSS_EXECUTE_SERVER:

            break;
        case SSS_RUNNING:

            break;
        default:
            break;
        }
    }
    return stepSuccess;
}

bool server::pushServer()
{
    m_workProcess.push(m_serial, getServerPath(), DEVICE_SERVER_PATH);
    return true;
}

//这里不关心remove的处理结果，所以最好不适用m_workProcess处理流程
bool server::removeServer()
{
    AdbProcess * thisProcess = new AdbProcess();
    if (!thisProcess) {
        return false;
    }
    connect(thisProcess, &AdbProcess::adbProcessResult, this, [this](AdbProcess::ADB_PROCESS_RESULT processResult){
        if (processResult != AdbProcess::AER_SUCCESS_START) {
            //sender() 返回发送信号的对象的指针，调用deleteLater，
            sender()->deleteLater();
        }
    });
    thisProcess->removePath(m_serial, DEVICE_SERVER_PATH);
    return true;
}

bool server::enableTunnelReverse()
{
    m_workProcess.reverse(m_serial, SOCKET_NAME, m_localPort);
    return true;
}

QString server::getServerPath()
{
    if (m_serverPath.isEmpty()) {
        char* buf = nullptr;
        size_t sz = 0;
        _dupenv_s(&buf, &sz, "QTSCRCPY_SERVER_PATH");
        m_serverPath = QString::fromLocal8Bit(buf);
        QFileInfo fileInfo(m_serverPath);
        //
        /*
         * qDebug() << QDir::currentPath(); 当前工作目录
         * qDebug() << QCoreApplication::applicationDirPath(); 程序所在目录
        */
        if (m_serverPath.isEmpty() || !fileInfo.isFile()) //如果通过环境变量指定的adbPath不存在，那么就去当前程序所在目录
        {
           m_serverPath = QCoreApplication::applicationDirPath() + "\\scrcpy-server.jar";
        }
    }
    return m_serverPath;
}
