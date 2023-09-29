#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include "../adb/adbprocess.h"

//adb shell /data/local/tmp/scrcpy-server.jar app_process / com.genymobile.scrcpy.Server maxsize bitrate false ""
//adb shell命令 启动这个jar文件中 斜杠后jar中这个类的main函数，启动scrcpy-server；
//后接参数，最大尺寸:0是默认，1080better、比特率8000000、是否正向连接（反向false）、是否对画面进行剪切（不剪切设为空）

class server : public QObject
{
    Q_OBJECT
public:
    enum SERVER_START_STEP{
        SSS_NULL,
        SSS_PUSH,
        SSS_ENABLE_REVERSE,
        SSS_EXECUTE_SERVER,
        SSS_RUNNING
    };

public:
    server(QObject *parent = nullptr);

    bool start(const QString & serial, quint16 localPort, quint16 maxSize, quint16 bitRate);

signals:
    void serverStartResult(bool success);

private slots:
    void onWorkProcessResult(AdbProcess::ADB_PROCESS_RESULT processResult);

private:
    bool startServerByStep();
    bool pushServer();
    bool removeServer();
    bool enableTunnelReverse();
    QString getServerPath();

private:
    QString m_serial = "";
    quint16 m_localPort = 0;
    quint16 m_maxSize = 0;
    quint16 m_bitRate = 0;

    SERVER_START_STEP m_serverStartStep = SSS_NULL;

    AdbProcess m_workProcess;

    QString m_serverPath;
};

#endif // SERVER_H
