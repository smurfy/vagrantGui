#include "vagrantinterface.h"

VagrantInterface::VagrantInterface(QObject *parent) :
    QObject(parent)
{
}

/*
 * Returns the list of vagrant systems
 */
QList<VagrantConfig> VagrantInterface::getVagrantSystems()
{
    return vagrantConfigs;
}

/*
 * Loads list of vagrant configs
 */
void VagrantInterface::initVagrant(QSettings *setting)
{
    setting->beginGroup("vagrantPaths");
    QStringList keys = setting->allKeys();
    foreach (QString key, keys) {
        addVagrantSystem(setting->value(key).toString());
    }
    setting->endGroup();
}

/*
 * Saves the vagrant settings
 */
void VagrantInterface::saveVagrant(QSettings *setting)
{
    setting->beginGroup("vagrantPaths");
    foreach(VagrantConfig vgcfg, vagrantConfigs) {
        setting->setValue(vgcfg.name, vgcfg.vagrantPath);
    }
    setting->endGroup();
}

/*
 * Returns Vagrant Config by Virtualbox Id
 */
VagrantConfig VagrantInterface::getByVirtualBoxId(QString id)
{
    foreach(VagrantConfig vgcfg, vagrantConfigs) {
        foreach (VagrantVirtualBoxMachineInfo boxInfo, vgcfg.registeredVmIds) {
            if (boxInfo.id == id) {
                return vgcfg;
            }
        }
    }

    VagrantConfig tmp;
    tmp.valid = false;
    return tmp;
}

/*
 * Executes vagrant command
 */
void VagrantInterface::executeCommand(QString id, QString command, QString cmd)
{
    VagrantConfig vgcfg = getByVirtualBoxId(id);

#ifdef Q_WS_WIN
    QString fileheader = "@echo off\n";
    fileheader.append(vgcfg.vagrantPath.at(0));
    fileheader.append(":\n");
    QString tmpFile = QDir::tempPath() + "/vagrantGui.bat";
#else
    QString fileheader = "";
    QString tmpFile =  QDir::tempPath() + "/vagrantGui.sh";
#endif
    QFile file(tmpFile);
    file.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream out(&file);
    out << fileheader;
    out << "cd " + vgcfg.vagrantPath << "\n";
    out << command << "\n";
    file.close();
    file.setPermissions(QFile::ExeUser | QFile::ReadUser | QFile::WriteUser);

    QProcess *process = new QProcess();
    process->startDetached(cmd + QDir::toNativeSeparators(tmpFile));
}

/*
 * Adds a new vagrand config
 *
 * Returns true on success
 */
bool VagrantInterface::addVagrantSystem(QString path)
{
    foreach(VagrantConfig vgcfg, vagrantConfigs) {
        //Already found
        if (vgcfg.vagrantPath == path) {
            return true;
        }
    }
    QString vgcfgFile = path + "/.vagrant";
    if (QFile::exists(vgcfgFile)) {
        QList<VagrantVirtualBoxMachineInfo> vmids = parseVagrantCfg(vgcfgFile);
        if (vmids.count() > 0) {
            QStringList tmp = QDir::fromNativeSeparators(path).split("/");
            VagrantConfig vgcfg;
            vgcfg.registeredVmIds = vmids;
            vgcfg.vagrantPath = path;
            vgcfg.name = tmp[tmp.count()-1];
            vgcfg.valid = true;
            vagrantConfigs.append(vgcfg);
            emit vagrantSystemsUpdated(vagrantConfigs);
            return true;
        }
    }
    return false;
}

/*
 * Removes a config from the array
 */
void VagrantInterface::removeVagrantSystem(QString path)
{
    for(int index = 0; index < vagrantConfigs.size(); ++index) {
       VagrantConfig vgcfg = vagrantConfigs[index];
        if (vgcfg.vagrantPath == path) {
            vagrantConfigs.removeAt(index);
            emit vagrantSystemsUpdated(vagrantConfigs);
            break;
        }
    }
}

/*
 * Parses a json config
 */
QList<VagrantVirtualBoxMachineInfo> VagrantInterface::parseVagrantCfg(QString file)
{
    QList<VagrantVirtualBoxMachineInfo> list;
    QFile *data = new QFile(file);
    data->open(QIODevice::ReadOnly);

    QScriptValue sc;
    QScriptEngine engine;
    sc = engine.evaluate("(" + QString(data->readAll()) + ")");
    data->close();

    if (sc.property("active").isObject())
    {
        QScriptValueIterator it(sc.property("active"));
        while (it.hasNext()) {
            it.next();
            VagrantVirtualBoxMachineInfo entry;
            entry.name = it.name();
            entry.id = it.value().toString();
            list.append(entry);
        }
    }
    return list;
}
