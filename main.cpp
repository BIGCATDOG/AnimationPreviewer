#include "mainwindow.h"

#include <QApplication>
#include <QDir>
#include <QLocale>
#include <QTranslator>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    //Get current system language
    auto systemLocale = QLocale::system();
    auto localLanguage = systemLocale.language();
     QTranslator translator;
    if (localLanguage == QLocale::Chinese) {
         auto current = QDir::currentPath();
        if (translator.load("ap_zh_CN.qm", qApp->applicationDirPath())) {

            QCoreApplication::installTranslator(&translator);
        }
    }
    MainWindow w;
    w.show();
    return a.exec();
}
