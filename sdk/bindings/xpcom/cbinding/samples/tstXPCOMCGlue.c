/* $Revision: 66862 $ */
/** @file tstXPCOMCGlue.c
 * Demonstrator program to illustrate use of C bindings of Main API.
 *
 * Linux only at the moment due to shared library magic in the Makefile.
 */

/*
 * Copyright (C) 2009 Oracle Corporation
 *
 * This file is part of VirtualBox Open Source Edition (OSE), as
 * available from http://www.virtualbox.org. This file is free software;
 * you can redistribute it and/or modify it under the terms of the GNU
 * General Public License (GPL) as published by the Free Software
 * Foundation, in version 2 as it comes in the "COPYING" file of the
 * VirtualBox OSE distribution. VirtualBox OSE is distributed in the
 * hope that it will be useful, but WITHOUT ANY WARRANTY of any kind.
 */

/*******************************************************************************
*   Header Files                                                               *
*******************************************************************************/
#include "VBoxXPCOMCGlue.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static void listVMs(IVirtualBox *virtualBox, ISession *session);
static void startVM(IVirtualBox *virtualBox, ISession *session, PRUnichar *id);

/**
 * List the registered VMs.
 *
 * @param   virtualBox ptr to IVirtualBox object
 * @param   session    ptr to ISession object
 */
static void listVMs(IVirtualBox *virtualBox, ISession *session)
{
    nsresult rc;
    IMachine **machines = NULL;
    PRUint32 machineCnt = 0;
    PRUint32 i;
    unsigned start_id;

    /*
     * Get the list of all registered VMs.
     */

    rc = virtualBox->vtbl->GetMachines(virtualBox, &machineCnt, &machines);
    if (NS_FAILED(rc))
    {
        fprintf(stderr, "could not get list of machines, rc=%08x\n",
            (unsigned)rc);
        return;
    }

    if (machineCnt == 0)
    {
        printf("\tNo VMs\n");
        return;
    }

    printf("VM List:\n\n");

    /*
     * Iterate through the collection.
     */

    for (i = 0; i < machineCnt; ++i)
    {
        IMachine *machine      = machines[i];
        PRBool    isAccessible = PR_FALSE;

        printf("\tMachine #%u\n", (unsigned)i);

        if (!machine)
        {
            printf("\t(skipped, NULL)\n");
            continue;
        }

        machine->vtbl->GetAccessible(machine, &isAccessible);

        if (isAccessible)
        {
            PRUnichar *machineNameUtf16;
            char *machineName;

            machine->vtbl->GetName(machine, &machineNameUtf16);
            g_pVBoxFuncs->pfnUtf16ToUtf8(machineNameUtf16,&machineName);
            printf("\tName:        %s\n", machineName);

            g_pVBoxFuncs->pfnUtf8Free(machineName);
            g_pVBoxFuncs->pfnComUnallocMem(machineNameUtf16);
        }
        else
        {
            printf("\tName:        <inaccessible>\n");
        }


        {
            PRUnichar *uuidUtf16 = NULL;
            char      *uuidUtf8  = NULL;

            machine->vtbl->GetId(machine, &uuidUtf16);
            g_pVBoxFuncs->pfnUtf16ToUtf8(uuidUtf16, &uuidUtf8);
            printf("\tUUID:        %s\n", uuidUtf8);

            g_pVBoxFuncs->pfnUtf8Free(uuidUtf8);
            g_pVBoxFuncs->pfnUtf16Free(uuidUtf16);
        }

        if (isAccessible)
        {
            {
                PRUnichar *configFile;
                char      *configFile1 = calloc((size_t)64, (size_t)1);

                machine->vtbl->GetSettingsFilePath(machine, &configFile);
                g_pVBoxFuncs->pfnUtf16ToUtf8(configFile, &configFile1);
                printf("\tConfig file: %s\n", configFile1);

                free(configFile1);
                g_pVBoxFuncs->pfnComUnallocMem(configFile);
            }

            {
                PRUint32 memorySize;

                machine->vtbl->GetMemorySize(machine, &memorySize);
                printf("\tMemory size: %uMB\n", memorySize);
            }

            {
                PRUnichar *typeId;
                PRUnichar *osNameUtf16;
                char *osName;
                IGuestOSType *osType = NULL;

                machine->vtbl->GetOSTypeId(machine, &typeId);
                virtualBox->vtbl->GetGuestOSType(virtualBox, typeId, &osType);
                osType->vtbl->GetDescription(osType, &osNameUtf16);
                g_pVBoxFuncs->pfnUtf16ToUtf8(osNameUtf16,&osName);
                printf("\tGuest OS:    %s\n\n", osName);

                osType->vtbl->nsisupports.Release((void *)osType);
                g_pVBoxFuncs->pfnUtf8Free(osName);
                g_pVBoxFuncs->pfnComUnallocMem(osNameUtf16);
                g_pVBoxFuncs->pfnComUnallocMem(typeId);
            }
        }
    }

    /*
     * Let the user chose a machine to start.
     */

    printf("Type Machine# to start (0 - %u) or 'quit' to do nothing: ",
        (unsigned)(machineCnt - 1));
    fflush(stdout);

    if (scanf("%u", &start_id) == 1 && start_id < machineCnt)
    {
        IMachine *machine = machines[start_id];

        if (machine)
        {
            PRUnichar *uuidUtf16 = NULL;

            machine->vtbl->GetId(machine, &uuidUtf16);
            startVM(virtualBox, session, uuidUtf16);

            g_pVBoxFuncs->pfnUtf16Free(uuidUtf16);
        }
    }

    /*
     * Don't forget to release the objects in the array.
     */

    for (i = 0; i < machineCnt; ++i)
    {
        IMachine *machine = machines[i];

        if (machine)
        {
            machine->vtbl->nsisupports.Release((void *)machine);
        }
    }
}

/**
 * Start a VM.
 *
 * @param   virtualBox ptr to IVirtualBox object
 * @param   session    ptr to ISession object
 * @param   id         identifies the machine to start
 */
static void startVM(IVirtualBox *virtualBox, ISession *session, PRUnichar *id)
{
    nsresult rc;
    IMachine  *machine    = NULL;
    IProgress *progress   = NULL;
    PRUnichar *env        = NULL;
    PRUnichar *sessionType;

    rc = virtualBox->vtbl->FindMachine(virtualBox, id, &machine);

    if (NS_FAILED(rc) || !machine)
    {
        fprintf(stderr, "Error: Couldn't get the machine handle.\n");
        return;
    }

    g_pVBoxFuncs->pfnUtf8ToUtf16("gui", &sessionType);

    rc = machine->vtbl->LaunchVMProcess(machine,
        session,
        sessionType,
        env,
        &progress
    );

    g_pVBoxFuncs->pfnUtf16Free(sessionType);

    if (NS_FAILED(rc))
    {
        fprintf(stderr, "Error: OpenRemoteSession failed.\n");
    }
    else
    {
        PRBool completed;
        PRInt32 resultCode;

        printf("Waiting for the remote session to open...\n");
        progress->vtbl->WaitForCompletion(progress, -1);

        rc = progress->vtbl->GetCompleted(progress, &completed);
        if (NS_FAILED(rc))
        {
            fprintf (stderr, "Error: GetCompleted status failed.\n");
        }

        progress->vtbl->GetResultCode(progress, &resultCode);
        if (NS_FAILED(resultCode))
        {
            IVirtualBoxErrorInfo *errorInfo;
            PRUnichar *textUtf16;
            char *text;

            progress->vtbl->GetErrorInfo(progress, &errorInfo);
            errorInfo->vtbl->GetText(errorInfo, &textUtf16);
            g_pVBoxFuncs->pfnUtf16ToUtf8(textUtf16, &text);
            printf("Error: %s\n", text);

            g_pVBoxFuncs->pfnComUnallocMem(textUtf16);
            g_pVBoxFuncs->pfnUtf8Free(text);
        }
        else
        {
            fprintf(stderr, "Remote session has been successfully opened.\n");
        }
        progress->vtbl->nsisupports.Release((void *)progress);
    }

    /* It's important to always release resources. */
    machine->vtbl->nsisupports.Release((void *)machine);
}

/* Main - Start the ball rolling. */

int main(int argc, char **argv)
{
    IVirtualBox *vbox           = NULL;
    ISession   *session          = NULL;
    PRUint32    revision         = 0;
    PRUnichar  *versionUtf16     = NULL;
    PRUnichar  *homefolderUtf16  = NULL;
    nsresult    rc;     /* Result code of various function (method) calls. */

    printf("Starting Main\n");

    /*
     * VBoxComInitialize does all the necessary startup action and
     * provides us with pointers to vbox and session handles.
     * It should be matched by a call to VBoxComUninitialize(vbox)
     * when done.
     */

    if (VBoxCGlueInit() != 0)
    {
        fprintf(stderr, "%s: FATAL: VBoxCGlueInit failed: %s\n",
                argv[0], g_szVBoxErrMsg);
        return EXIT_FAILURE;
    }

    g_pVBoxFuncs->pfnComInitialize(IVIRTUALBOX_IID_STR, &vbox,
                                   ISESSION_IID_STR, &session);
    if (vbox == NULL)
    {
        fprintf(stderr, "%s: FATAL: could not get vbox handle\n", argv[0]);
        return EXIT_FAILURE;
    }
    if (session == NULL)
    {
        fprintf(stderr, "%s: FATAL: could not get session handle\n", argv[0]);
        return EXIT_FAILURE;
    }

    /*
     * Now ask for revision, version and home folder information of
     * this vbox. Were not using fancy macros here so it
     * remains easy to see how we access C++'s vtable.
     */

    printf("----------------------------------------------------\n");

    /* 1. Revision */

    rc = vbox->vtbl->GetRevision(vbox, &revision);
    if (NS_SUCCEEDED(rc))
    {
        printf("\tRevision: %u\n", revision);
    }
    else
    {
        fprintf(stderr, "%s: GetRevision() returned %08x\n",
            argv[0], (unsigned)rc);
    }

    /* 2. Version */

    rc = vbox->vtbl->GetVersion(vbox, &versionUtf16);
    if (NS_SUCCEEDED(rc))
    {
        char *version = NULL;
        g_pVBoxFuncs->pfnUtf16ToUtf8(versionUtf16, &version);
        printf("\tVersion: %s\n", version);
        g_pVBoxFuncs->pfnUtf8Free(version);
        g_pVBoxFuncs->pfnComUnallocMem(versionUtf16);
    }
    else
    {
        fprintf(stderr, "%s: GetVersion() returned %08x\n",
            argv[0], (unsigned)rc);
    }

    /* 3. Home Folder */

    rc = vbox->vtbl->GetHomeFolder(vbox, &homefolderUtf16);
    if (NS_SUCCEEDED(rc))
    {
        char *homefolder = NULL;
        g_pVBoxFuncs->pfnUtf16ToUtf8(homefolderUtf16, &homefolder);
        printf("\tHomeFolder: %s\n", homefolder);
        g_pVBoxFuncs->pfnUtf8Free(homefolder);
        g_pVBoxFuncs->pfnComUnallocMem(homefolderUtf16);
    }
    else
    {
        fprintf(stderr, "%s: GetHomeFolder() returned %08x\n",
            argv[0], (unsigned)rc);
    }

    listVMs(vbox, session);
    session->vtbl->UnlockMachine(session);

    printf("----------------------------------------------------\n");

    /*
     * Do as mom told us: always clean up after yourself.
     */

    g_pVBoxFuncs->pfnComUninitialize();
    VBoxCGlueTerm();
    printf("Finished Main\n");

    return 0;
}
/* vim: set ts=4 sw=4 et: */
