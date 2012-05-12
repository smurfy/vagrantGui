#ifndef VIRTUALBOXUNIFIEDINTERFACE_H
#define VIRTUALBOXUNIFIEDINTERFACE_H

#include <QObject>
#include <QtGui>

/* Includes for VBoxLibs */
#ifdef Q_WS_WIN
    #include "VirtualBox.h"
#else
    #include "nsIEventQueue.h"
    #include "sdk/bindings/xpcom/cbinding/VBoxXPCOMCGlue.h"
#endif

typedef struct {
    int state;
    QString name;
    QString id;
    int sshPort;
} VirtualBoxMachine;

class VirtualboxUnifiedInterface : public QObject
{
    Q_OBJECT
public:
    explicit VirtualboxUnifiedInterface(QObject *parent = 0);
    bool initVirtualBox();
    bool shutdownVirtualBox();
    void refreshVirtualBoxMachines();
    QString getVirtualBoxVersion();
    QList<VirtualBoxMachine> getVirtualMachines();

signals:
    void virtualBoxMachineStateChanged(VirtualBoxMachine machine);

public slots:

private:
    QList<VirtualBoxMachine> vboxmachines;
    IVirtualBox *virtualBox;
    ISession *session;

    int findSSHPort(IMachine *machine);
};

#endif // VIRTUALBOXUNIFIEDINTERFACE_H
