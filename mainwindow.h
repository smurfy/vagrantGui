#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QObject>
#include <QSystemTrayIcon>
#include <QMessageBox>
#include <QMenu>
#include <QProcess>
#include <QFileDialog>
#include <QtGui>
#include <QSettings>
#include "vagrantinterface.h"
#include "virtualboxunifiedinterface.h"

typedef struct {
    VirtualBoxMachine vboxmachine;
    QString command;
    QString name;
    QString path;
} ActionVagrantVirtualBoxMachineInfo;

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void onQuit();
    void onVagrantSystemUpdated(QList<VagrantConfig> systems);
    void onVirtualBoxMachineStateChanged(VirtualBoxMachine machine);
    void onUpdateTime();
    void onActionTriggered();
    void onAddNewMachine();
    void onRemoveMachine();
#ifdef Q_WS_WIN
    void onSetPuttyPath();
#endif
#if defined(Q_WS_MACX) || defined(RT_OS_LINUX)
    void onTerminalClicked();
#endif
    
private:
    VagrantInterface *vagrant;
    VirtualboxUnifiedInterface *virtualbox;
    QSystemTrayIcon *trayIcon;
    QSettings *settings;
    QTimer *updateStateTimer;

    void showWelcome();
    void updateGui();
    void setupRestOfUi();
    void addMnuAction(QMenu *menu, QString title, VirtualBoxMachine boxinfo, QString command);
};

#endif // MAINWINDOW_H
