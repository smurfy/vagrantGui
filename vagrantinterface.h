#ifndef VAGRANTINTERFACE_H
#define VAGRANTINTERFACE_H

#include <QObject>
#include <QScriptEngine>
#include <QScriptValueIterator>
#include <QSettings>
#include <QDir>
#include <QFile>
#include <QStringList>
#include <QtGui>

typedef struct {
    QString name;
    QString id;
} VagrantVirtualBoxMachineInfo;


typedef struct {
    QList<VagrantVirtualBoxMachineInfo> registeredVmIds;
    QString vagrantPath;
    QString name;
    bool valid;
} VagrantConfig;


class VagrantInterface : public QObject
{
    Q_OBJECT
public:
    explicit VagrantInterface(QObject *parent = 0);
    QList<VagrantConfig> getVagrantSystems();
    void initVagrant(QSettings *setting);
    void saveVagrant(QSettings *setting);
    bool addVagrantSystem(QString path);
    void removeVagrantSystem(QString path);
    VagrantConfig getByVirtualBoxId(QString id);
    void executeCommand(QString id, QString command, QString cmd);
    
signals:
    void vagrantSystemsUpdated(QList<VagrantConfig> systems);
    
public slots:

private:
    QList<VagrantConfig> vagrantConfigs;
    QList<VagrantVirtualBoxMachineInfo> parseVagrantCfg(QString file);
};

#endif // VAGRANTINTERFACE_H
