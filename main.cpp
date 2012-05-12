#include <QtGui/QApplication>
#include "mainwindow.h"

#ifdef Q_WS_MACX
#include <Processes.h>
#endif

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    a.setQuitOnLastWindowClosed(false);

#ifdef Q_WS_MACX
    ProcessSerialNumber psn;
    if (GetCurrentProcess(&psn) == noErr)
    {
        TransformProcessType(&psn, kProcessTransformToBackgroundApplication);
    }
#endif

    MainWindow w;

    return a.exec();
}
