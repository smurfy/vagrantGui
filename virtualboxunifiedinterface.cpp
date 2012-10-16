#include "virtualboxunifiedinterface.h"

VirtualboxUnifiedInterface::VirtualboxUnifiedInterface(QObject *parent) :
    QObject(parent)
{
}

/*
 * Initialises the VirtualBoxInterface
 *
 * Returns true on success or false on error.
 */
bool VirtualboxUnifiedInterface::initVirtualBox()
{
#ifdef Q_WS_WIN
    HRESULT rc;
    CoInitialize(NULL);

    rc = CoCreateInstance(CLSID_VirtualBox,       /* the VirtualBox base object */
                                  NULL,                   /* no aggregation */
                                  CLSCTX_LOCAL_SERVER,    /* the object lives in a server process on this machine */
                                  IID_IVirtualBox,        /* IID of the interface */
                                  (void**)&virtualBox);

    if (!SUCCEEDED(rc))
    {
        return false;
    }
    return true;
#else
    nsresult rc;

    if (VBoxCGlueInit() != 0)
    {
        return false;
    }

    g_pVBoxFuncs->pfnComInitialize(IVIRTUALBOX_IID_STR, &virtualBox, ISESSION_IID_STR, &session);
    if (virtualBox == NULL)
    {
       return false;
    }
    if (session == NULL)
    {
        return false;
    }

    return true;
#endif
}

/*
 * Releases the virtualbox interface
 *
 * Returns true
 */
bool VirtualboxUnifiedInterface::shutdownVirtualBox()
{
#ifdef Q_WS_WIN
    if (virtualBox) {
        virtualBox->Release();
    }
    CoUninitialize();
#else
    g_pVBoxFuncs->pfnComUninitialize();
    VBoxCGlueTerm();
#endif
    return true;
}

/*
 * Refreshes the internal list of virtual machines and emits signal if state changes
 *
 * returns nothing
 */
void VirtualboxUnifiedInterface::refreshVirtualBoxMachines()
{
    QList<VirtualBoxMachine> newVboxmachines;
    newVboxmachines.clear();
#ifdef Q_WS_WIN
    HRESULT rc;
    SAFEARRAY *machinesArray = NULL;

    rc = virtualBox->get_Machines(&machinesArray);
    if (SUCCEEDED(rc))
    {
        IMachine **machines;
        rc = SafeArrayAccessData (machinesArray, (void **) &machines);
        if (SUCCEEDED(rc))
        {
            for (ULONG i = 0; i < machinesArray->rgsabound[0].cElements; ++i)
            {
                VirtualBoxMachine vboxmachine;
                BSTR str;
                rc = machines[i]->get_Name(&str);
                if (SUCCEEDED(rc))
                {
                    QString name ((QChar*)str, SysStringLen(str));
                    vboxmachine.name = name.toAscii();
                    SysFreeString(str);
                }
                rc = machines[i]->get_Id(&str);
                if (SUCCEEDED(rc))
                {
                    QString id ((QChar*)str, SysStringLen(str));
                    vboxmachine.id = id.toAscii();
                    SysFreeString(str);
                }

                MachineState st;
                rc = machines[i]->get_State(&st);
                if (SUCCEEDED(rc))
                {
                    vboxmachine.state = (int)st;
                }

                vboxmachine.sshPort = findSSHPort(machines[i]);

                newVboxmachines.append(vboxmachine);
            }

            SafeArrayUnaccessData (machinesArray);
        }

        SafeArrayDestroy (machinesArray);
    }
#else
    nsresult rc;
    IMachine **machines = NULL;
    PRUint32 machineCnt = 0;
    PRUint32 i;

    rc = virtualBox->GetMachines(&machineCnt, &machines);
    if (NS_FAILED(rc))
    {
        return;
    }

    if (machineCnt == 0)
    {
        return;
    }

    for (i = 0; i < machineCnt; ++i)
    {
        VirtualBoxMachine vboxmachine;

        IMachine *machine      = machines[i];
        PRBool    isAccessible = PR_FALSE;
        if (!machine)
        {
            continue;
        }

        machine->GetAccessible(&isAccessible);

        if (isAccessible)
        {
            PRUnichar *machineNameUtf16;
            char *machineName;

            machine->GetName(&machineNameUtf16);
            g_pVBoxFuncs->pfnUtf16ToUtf8(machineNameUtf16,&machineName);

            vboxmachine.name = machineName;

            g_pVBoxFuncs->pfnUtf8Free(machineName);
            g_pVBoxFuncs->pfnComUnallocMem(machineNameUtf16);

            PRUnichar *uuidUtf16 = NULL;
            char      *uuidUtf8  = NULL;

            machine->GetId(&uuidUtf16);
            g_pVBoxFuncs->pfnUtf16ToUtf8(uuidUtf16, &uuidUtf8);

            vboxmachine.id = uuidUtf8;

            g_pVBoxFuncs->pfnUtf8Free(uuidUtf8);
            g_pVBoxFuncs->pfnUtf16Free(uuidUtf16);

            PRUint32 stateint32 = 0;

            machine->GetState(&stateint32);

            vboxmachine.sshPort = findSSHPort(machine);

            vboxmachine.state = stateint32;

            newVboxmachines.append(vboxmachine);
       }
    }
#endif
    QList<VirtualBoxMachine> oldVboxmachines = vboxmachines;
    vboxmachines = newVboxmachines;

    foreach(VirtualBoxMachine newvboxmachine, vboxmachines) {
        foreach(VirtualBoxMachine vboxmachine, oldVboxmachines) {
            if (vboxmachine.id == newvboxmachine.id) {
                if (vboxmachine.state != newvboxmachine.state) {
                    emit virtualBoxMachineStateChanged(newvboxmachine);
                }
                break;
            }
        }
    }
}

/*
 * Gets ssh port from machine
 * returns the portnumber as integer
 */
int VirtualboxUnifiedInterface::findSSHPort(IMachine *machine)
{
    int port = -1;

    //ssh port only needed on windows for putty support so we dont
    //use it on other systems
 #ifdef Q_WS_WIN
    INetworkAdapter *adapter;
    INATEngine *natDriver;
    machine->GetNetworkAdapter(0, &adapter);
    adapter->get_NATEngine(&natDriver);

    SAFEARRAY *redirectsArray = NULL;
    natDriver->get_Redirects(&redirectsArray);

    BSTR HUGEP *redirects;
    SafeArrayAccessData (redirectsArray,  (void HUGEP* FAR*)&redirects);
    for (ULONG i = 0; i < redirectsArray->rgsabound[0].cElements; ++i) {
        BSTR innerstr = redirects[i];
        QString data ((QChar*)innerstr, SysStringLen(innerstr));
        QStringList redirect = data.split(",");
        if (redirect[0] == "ssh") {
            port = redirect[3].toInt();
        }
        SysFreeString(innerstr);
    }
    SafeArrayDestroy (redirectsArray);
#endif
    return port;
}

/*
 * Returns virtualbox version as QString
 */
QString VirtualboxUnifiedInterface::getVirtualBoxVersion()
{
#ifdef Q_WS_WIN
    BSTR str;
    virtualBox->get_Version(&str);
    QString version ((QChar*)str, SysStringLen(str));
    SysFreeString(str);
#else
    PRUnichar  *versionUtf16     = NULL;
    nsresult rc;
    rc = virtualBox->GetVersion(&versionUtf16);
    QString version;
    if (NS_SUCCEEDED(rc))
    {
        char *tmpversion = NULL;
        g_pVBoxFuncs->pfnUtf16ToUtf8(versionUtf16, &tmpversion);
        version = tmpversion;
        g_pVBoxFuncs->pfnUtf8Free(tmpversion);
        g_pVBoxFuncs->pfnComUnallocMem(versionUtf16);
    }
#endif
    return version;
}

/*
 * Returns QList of all vbox machines
 */
QList<VirtualBoxMachine> VirtualboxUnifiedInterface::getVirtualMachines()
{
    return vboxmachines;
}
