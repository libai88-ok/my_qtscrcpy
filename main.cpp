#include "dialog.h"
#include "qtenvironmentvariables.h"

#include <QApplication>
#include <QLocale>
#include <QTranslator>

int main(int argc, char *argv[])
{
    qputenv("QTSCRCPY_ADB_PATH", "..\\qtscrcpy\\third_party\\adb\\win\\adb.exe"); //添加环境变量，以动态的方式获取adb.exe的路径
    qputenv("QTSCRCPY_SERVER_PATH", "..\\qtscrcpy\\third_party\\scrcpy-server.jar");

    QApplication a(argc, argv);
    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "qtscrcpy_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            a.installTranslator(&translator);
            break;
        }
    }
    Dialog w;
    w.show();
    return a.exec();
}
