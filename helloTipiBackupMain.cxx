#include <QApplication>
#include <QTranslator>

#include <QDebug>

#include "helloTipiBackupMainWindow.H"

int main(int argn, char **argv)
{
    QApplication app(argn, argv);
    QTranslator qtTranslator;

    qDebug() << QLocale::system().name();

    if(QLocale::system().name().contains("en_US") == true) {
        qtTranslator.load("helloTipiBackupEN", ".");
        app.installTranslator(&qtTranslator);
    }

    helloTipiBackupMainWindow mw;
    mw.show();

    return app.exec();
}
