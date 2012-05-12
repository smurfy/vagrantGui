#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    settings = new QSettings("smurfy", "vagrantGui", this);

    vagrant = new VagrantInterface();
    virtualbox = new VirtualboxUnifiedInterface();

    if (virtualbox->initVirtualBox()) {
        virtualbox->refreshVirtualBoxMachines();
        vagrant->initVagrant(settings);
        updateStateTimer = new QTimer();
        connect(vagrant, SIGNAL(vagrantSystemsUpdated(QList<VagrantConfig>)), this, SLOT(onVagrantSystemUpdated(QList<VagrantConfig>)));
        connect(virtualbox, SIGNAL(virtualBoxMachineStateChanged(VirtualBoxMachine)), this, SLOT(onVirtualBoxMachineStateChanged(VirtualBoxMachine)));
        connect(updateStateTimer, SIGNAL(timeout()), this, SLOT(onUpdateTime()));

        trayIcon = new QSystemTrayIcon();
        trayIcon->setIcon(QIcon(":/icon.png"));
        trayIcon->setToolTip(tr("VagrantGui"));
        trayIcon->show();

        updateGui();
        showWelcome();
        updateStateTimer->start(5000);
    } else {
        QMessageBox::critical(this, tr("VagrantGui"), tr("An error occured while starting up. Is Virtualbox installed?"));
    }
}

void MainWindow::onUpdateTime()
{
    virtualbox->refreshVirtualBoxMachines();
}

void MainWindow::onVirtualBoxMachineStateChanged(VirtualBoxMachine machine)
{
    VagrantConfig vgcfg = vagrant->getByVirtualBoxId(machine.id);
    if (vgcfg.valid) {
        QString boxName = vgcfg.name;

        if (vgcfg.registeredVmIds.count() > 1) {
            foreach (VagrantVirtualBoxMachineInfo boxInfo, vgcfg.registeredVmIds) {
                if (boxInfo.id == machine.id) {
                    boxName.append(" - ");
                    boxName.append(boxInfo.name);
                    break;
                }
            }
        }

        if (machine.state == 5) {
            trayIcon->showMessage(tr("VagrantGui: ") + boxName, tr("Virtual machine is now running.") );
        } else if (machine.state == 1) {
            trayIcon->showMessage(tr("VagrantGui: ") + boxName, tr("Virtual machine halted.") );
        } else if (machine.state == 2) {
            trayIcon->showMessage(tr("VagrantGui: ") + boxName, tr("Virtual machine suspended.") );
        } else if (machine.state == 10) {
            trayIcon->showMessage(tr("VagrantGui: ") + boxName, tr("Virtual machine starting.") );
        } else {
            qDebug()<<machine.state;
        }
        updateGui();
    }
}

void MainWindow::onVagrantSystemUpdated(QList<VagrantConfig> systems)
{
    updateGui();
}

void MainWindow::onAddNewMachine()
{
    QString path = QFileDialog::getExistingDirectory();

    if (path.isEmpty()) {
        return;
    }

    QString vgcfgFile = path + "/.vagrant";
    if (QFile::exists(vgcfgFile)) {
        if (vagrant->addVagrantSystem(path)) {
            //
        } else {
            QMessageBox::warning(this, tr("VagrantGui"), tr("No active vms found in vagrant config file."));
        }
    } else {
        QMessageBox::warning(this, tr("VagrantGui"), tr("Can not detect a vagrant config file, make sure that you have at least started the vm once with 'vagrant up'"));
    }
}

void MainWindow::onRemoveMachine()
{
    QAction *mnuAction = (QAction *)sender();
    ActionVagrantVirtualBoxMachineInfo *actionItem = (ActionVagrantVirtualBoxMachineInfo *) mnuAction->data().value<void *>();
    vagrant->removeVagrantSystem(actionItem->path);
}

void MainWindow::updateGui()
{
    QMenu *mnu = new QMenu();
    foreach(VagrantConfig vbcfg, vagrant->getVagrantSystems())
    {
        QMenu *vagrantMnu = mnu->addMenu(vbcfg.name);
        if (vbcfg.registeredVmIds.count() > 1) {
            VirtualBoxMachine somevboxmachine;
            bool hasOnline = false;
            bool hasOffline = false;
            foreach(VagrantVirtualBoxMachineInfo boxinfo, vbcfg.registeredVmIds)
            {
                foreach(VirtualBoxMachine vboxmachine, virtualbox->getVirtualMachines())
                {
                    if (vboxmachine.id == boxinfo.id) {
                        somevboxmachine = vboxmachine;
                        if (vboxmachine.state == 5) {
                            hasOnline = true;
                        } else {
                            hasOffline = true;
                        }
                        break;
                    }
                }
            }

            if (hasOffline) {
                addMnuAction(vagrantMnu, tr("up"), somevboxmachine, "vagrant up");
            }
            if (hasOnline) {
                addMnuAction(vagrantMnu, tr("halt"), somevboxmachine, "vagrant halt");
                addMnuAction(vagrantMnu, tr("reload"), somevboxmachine, "vagrant reload");
                addMnuAction(vagrantMnu, tr("suspend"), somevboxmachine, "vagrant suspend");
            }
            if (hasOffline || hasOnline) {
                vagrantMnu->addSeparator();
            }
        }
        foreach(VagrantVirtualBoxMachineInfo boxinfo, vbcfg.registeredVmIds)
        {
            foreach(VirtualBoxMachine vboxmachine, virtualbox->getVirtualMachines())
            {
                if (vboxmachine.id == boxinfo.id) {
                    QString prefix = "";
                    QString suffix = "";
                    if (vbcfg.registeredVmIds.count() > 1) {
                        prefix = boxinfo.name + ": ";
                        suffix = boxinfo.name;
                    }
                    if (vboxmachine.state == 5) {
                        addMnuAction(vagrantMnu, prefix + tr("ssh"), vboxmachine, "vagrant ssh " + suffix );
                        addMnuAction(vagrantMnu, prefix + tr("halt"), vboxmachine, "vagrant halt " + suffix);
                        addMnuAction(vagrantMnu, prefix + tr("reload"), vboxmachine, "vagrant reload " + suffix);
                        addMnuAction(vagrantMnu, prefix + tr("suspend"), vboxmachine, "vagrant suspend " + suffix);
                    } else if (vboxmachine.state == 2) {
                        addMnuAction(vagrantMnu, prefix + tr("resume"), vboxmachine, "vagrant resume " + suffix);
                    } else {
                        addMnuAction(vagrantMnu, prefix + tr("up"), vboxmachine, "vagrant up " + suffix);
                    }
                    break;
                }
            }
        }
    }

    mnu->addSeparator();

    QMenu *manageMnu = mnu->addMenu(tr("Settings"));
    QAction *manageAction = manageMnu->addAction(tr("Add new vagrant instance"));
    connect(manageAction, SIGNAL(triggered()), this, SLOT(onAddNewMachine()));

    if (vagrant->getVagrantSystems().count() > 0) {
        manageMnu->addSeparator();
        foreach(VagrantConfig vbcfg, vagrant->getVagrantSystems())
        {
            QAction *removeAction = manageMnu->addAction(vbcfg.name + ": " + tr("remove instance"));
            connect(removeAction, SIGNAL(triggered()), this, SLOT(onRemoveMachine()));
            ActionVagrantVirtualBoxMachineInfo *actionItem = new ActionVagrantVirtualBoxMachineInfo();
            actionItem->name = vbcfg.name;
            actionItem->path = vbcfg.vagrantPath;
            QVariant data = qVariantFromValue((void *) actionItem);
            removeAction->setData(data);
        }
    }

#ifdef Q_WS_WIN
        manageMnu->addSeparator();
        QAction *puttyAction = manageMnu->addAction(tr("Set putty path"));
        connect(puttyAction, SIGNAL(triggered()), this, SLOT(onSetPuttyPath()));
#endif

#ifdef Q_WS_MACX
        manageMnu->addSeparator();
        QMenu *terminalMnu = manageMnu->addMenu(tr("Terminal to use"));
        QActionGroup *actionGroup = new QActionGroup(this);
        actionGroup->setExclusive(true);
        QAction *defaultAction = terminalMnu->addAction(tr("Terminal"));
        connect(defaultAction, SIGNAL(triggered()), this, SLOT(onTerminalClicked()));
        defaultAction->setCheckable(true);
        defaultAction->setData(0);
        QAction *itermAction = terminalMnu->addAction(tr("iTerm"));
        connect(itermAction, SIGNAL(triggered()), this, SLOT(onTerminalClicked()));
        itermAction->setCheckable(true);
        itermAction->setData(1);

        actionGroup->addAction(defaultAction);
        actionGroup->addAction(itermAction);

        if (settings->value("terminal", 0) == 0) {
            defaultAction->setChecked(true);
        } else {
            itermAction->setChecked(true);
        }
#endif

    mnu->addSeparator();

    QAction *quitAction = mnu->addAction(tr("Quit"));
    connect(quitAction, SIGNAL(triggered()), this, SLOT(onQuit()));

    if (trayIcon->contextMenu()) {
        trayIcon->contextMenu()->clear();
    }

    trayIcon->setContextMenu(mnu);
}

#ifdef Q_WS_MACX
void MainWindow::onTerminalClicked()
{
    QAction *mnuAction = (QAction *)sender();
    settings->setValue("terminal", mnuAction->data());
}
#endif

#ifdef Q_WS_WIN
void MainWindow::onSetPuttyPath()
{
    QString path = QFileDialog::getOpenFileName(this, tr("Select putty.exe"), "", "putty.exe");

    if (path.isEmpty()) {
        return;
    }
    settings->setValue("putty", path);
}
#endif

void MainWindow::addMnuAction(QMenu *menu, QString title, VirtualBoxMachine vboxmachine, QString command)
{
    QAction *mnuAction = menu->addAction(title);
    connect(mnuAction, SIGNAL(triggered()), this, SLOT(onActionTriggered()));
    ActionVagrantVirtualBoxMachineInfo *actionItem = new ActionVagrantVirtualBoxMachineInfo();
    actionItem->vboxmachine = vboxmachine;
    actionItem->command = command;
    QVariant data = qVariantFromValue((void *) actionItem);
    mnuAction->setData(data);
}

void MainWindow::onActionTriggered()
{
    QAction *mnuAction = (QAction *)sender();
    ActionVagrantVirtualBoxMachineInfo *actionItem = (ActionVagrantVirtualBoxMachineInfo *) mnuAction->data().value<void *>();

    QString cmd = "";
    QString command = actionItem->command;

#ifdef Q_WS_WIN
    //On Windows we launch Putty
    if (command.startsWith("vagrant ssh")) {
        QString puttyCmd = settings->value("putty").toString();
        if (puttyCmd.isEmpty() || !QFile::exists(puttyCmd)) {
            QMessageBox::warning(this, tr("VagrantGui"), tr("Please specify your putty path"));
            return;
        }
        command = "start " + settings->value("putty").toString() + " -ssh vagrant@127.0.0.1 " + QString::number(actionItem->vboxmachine.sshPort);
    }
#endif
#ifdef Q_WS_MACX
    if (settings->value("terminal", 0) == 0) {
        cmd = "open -a Terminal ";
    } else {
        cmd = "open -a iTerm ";
    }
#endif
    vagrant->executeCommand(actionItem->vboxmachine.id, command, cmd);
}

void MainWindow::onQuit()
{
    QApplication::setQuitOnLastWindowClosed(true);
    close();
}

void MainWindow::showWelcome()
{
    trayIcon->showMessage(tr("VagrantGui"), tr("Successfully started and running on Virtualbox ") + virtualbox->getVirtualBoxVersion());
}

MainWindow::~MainWindow()
{
    virtualbox->shutdownVirtualBox();
    vagrant->saveVagrant(settings);
    trayIcon->hide();
}
