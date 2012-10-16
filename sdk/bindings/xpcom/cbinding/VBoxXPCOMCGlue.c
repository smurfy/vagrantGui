/* $Revision: 66985 $ */
/** @file
 * Glue code for dynamically linking to VBoxXPCOMC.
 */

/*
 * Copyright (C) 2008-2010 Oracle Corporation
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

/*******************************************************************************
*   Header Files                                                               *
*******************************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdint.h>
#include <dlfcn.h>

#include "VBoxXPCOMCGlue.h"


/*******************************************************************************
*   Defined Constants And Macros                                               *
*******************************************************************************/
#if defined(__linux__) || defined(__linux_gnu__) || defined(__sun__) || defined(__FreeBSD__)
# define DYNLIB_NAME    "VBoxXPCOMC.so"
#elif defined(__APPLE__)
# define DYNLIB_NAME    "VBoxXPCOMC.dylib"
#elif defined(_MSC_VER) || defined(__OS2__)
# define DYNLIB_NAME    "VBoxXPCOMC.dll"
#else
# error "Port me"
#endif


/*******************************************************************************
*   Global Variables                                                           *
*******************************************************************************/
/** The dlopen handle for VBoxXPCOMC. */
void *g_hVBoxXPCOMC = NULL;
/** The last load error. */
char g_szVBoxErrMsg[256];
/** Pointer to the VBoxXPCOMC function table. */
PCVBOXXPCOM g_pVBoxFuncs = NULL;
/** Pointer to VBoxGetXPCOMCFunctions for the loaded VBoxXPCOMC so/dylib/dll. */
PFNVBOXGETXPCOMCFUNCTIONS g_pfnGetFunctions = NULL;


/**
 * Wrapper for setting g_szVBoxErrMsg. Can be an empty stub.
 *
 * @param   fAlways         When 0 the g_szVBoxErrMsg is only set if empty.
 * @param   pszFormat       The format string.
 * @param   ...             The arguments.
 */
static void setErrMsg(int fAlways, const char *pszFormat, ...)
{
    if (    fAlways
        ||  !g_szVBoxErrMsg[0])
    {
        va_list va;
        va_start(va, pszFormat);
        vsnprintf(g_szVBoxErrMsg, sizeof(g_szVBoxErrMsg), pszFormat, va);
        va_end(va);
    }
}


/**
 * Try load VBoxXPCOMC.so/dylib/dll from the specified location and resolve all
 * the symbols we need.
 *
 * @returns 0 on success, -1 on failure.
 * @param   pszHome         The director where to try load VBoxXPCOMC from. Can
 *                          be NULL.
 * @param   fSetAppHome     Whether to set the VBOX_APP_HOME env.var. or not
 *                          (boolean).
 */
static int tryLoadOne(const char *pszHome, int fSetAppHome)
{
    size_t      cchHome = pszHome ? strlen(pszHome) : 0;
    size_t      cbBufNeeded;
    char        szName[4096];
    int         rc = -1;

    /*
     * Construct the full name.
     */
    cbBufNeeded = cchHome + sizeof("/" DYNLIB_NAME);
    if (cbBufNeeded > sizeof(szName))
    {
        setErrMsg(1, "path buffer too small: %u bytes needed",
                  (unsigned)cbBufNeeded);
        return -1;
    }
    if (cchHome)
    {
        memcpy(szName, pszHome, cchHome);
        szName[cchHome] = '/';
        cchHome++;
    }
    memcpy(&szName[cchHome], DYNLIB_NAME, sizeof(DYNLIB_NAME));

    /*
     * Try load it by that name, setting the VBOX_APP_HOME first (for now).
     * Then resolve and call the function table getter.
     */
    if (fSetAppHome)
    {
        if (pszHome)
            setenv("VBOX_APP_HOME", pszHome, 1 /* always override */);
        else
            unsetenv("VBOX_APP_HOME");
    }
    g_hVBoxXPCOMC = dlopen(szName, RTLD_NOW | RTLD_LOCAL);
    if (g_hVBoxXPCOMC)
    {
        PFNVBOXGETXPCOMCFUNCTIONS pfnGetFunctions;
        pfnGetFunctions = (PFNVBOXGETXPCOMCFUNCTIONS)(uintptr_t)
            dlsym(g_hVBoxXPCOMC, VBOX_GET_XPCOMC_FUNCTIONS_SYMBOL_NAME);
        if (pfnGetFunctions)
        {
            g_pVBoxFuncs = pfnGetFunctions(VBOX_XPCOMC_VERSION);
            if (g_pVBoxFuncs)
            {
                g_pfnGetFunctions = pfnGetFunctions;
                return 0;
            }

            /* bail out */
            setErrMsg(1, "%.80s: pfnGetFunctions(%#x) failed",
                      szName, VBOX_XPCOMC_VERSION);
        }
        else
            setErrMsg(1, "dlsym(%.80s/%.32s): %.128s",
                      szName, VBOX_GET_XPCOMC_FUNCTIONS_SYMBOL_NAME, dlerror());
        dlclose(g_hVBoxXPCOMC);
        g_hVBoxXPCOMC = NULL;
    }
    else
        setErrMsg(0, "dlopen(%.80s): %.160s", szName, dlerror());
    return rc;
}


/**
 * Tries to locate and load VBoxXPCOMC.so/dylib/dll, resolving all the related
 * function pointers.
 *
 * @returns 0 on success, -1 on failure.
 *
 * @remark  This should be considered moved into a separate glue library since
 *          its its going to be pretty much the same for any user of VBoxXPCOMC
 *          and it will just cause trouble to have duplicate versions of this
 *          source code all around the place.
 */
int VBoxCGlueInit(void)
{
    /*
     * If the user specifies the location, try only that.
     */
    const char *pszHome = getenv("VBOX_APP_HOME");
    if (pszHome)
        return tryLoadOne(pszHome, 0);

    /*
     * Try the known standard locations.
     */
    g_szVBoxErrMsg[0] = '\0';
#if defined(__gnu__linux__) || defined(__linux__)
    if (tryLoadOne("/opt/VirtualBox", 1) == 0)
        return 0;
    if (tryLoadOne("/usr/lib/virtualbox", 1) == 0)
        return 0;
#elif defined(__sun__)
    if (tryLoadOne("/opt/VirtualBox/amd64", 1) == 0)
        return 0;
    if (tryLoadOne("/opt/VirtualBox/i386", 1) == 0)
        return 0;
#elif defined(__APPLE__)
    if (tryLoadOne("/Application/VirtualBox.app/Contents/MacOS", 1) == 0)
        return 0;
#elif defined(__FreeBSD__)
    if (tryLoadOne("/usr/local/lib/virtualbox", 1) == 0)
        return 0;
#elif defined(__OS2__)
    if (tryLoadOne("C:/Apps/VirtualBox", 1) == 0)
        return 0;
#else
# error "port me"
#endif

    /*
     * Finally try the dynamic linker search path.
     */
    if (tryLoadOne(NULL, 1) == 0)
        return 0;

    /* No luck, return failure. */
    return -1;
}


/**
 * Terminate the C glue library.
 */
void VBoxCGlueTerm(void)
{
    if (g_hVBoxXPCOMC)
    {
#if 0 /* VBoxRT.so doesn't like being reloaded. See @bugref{3725}. */
        dlclose(g_hVBoxXPCOMC);
#endif
        g_hVBoxXPCOMC = NULL;
    }
    g_pVBoxFuncs = NULL;
    g_pfnGetFunctions = NULL;
    memset(g_szVBoxErrMsg, 0, sizeof(g_szVBoxErrMsg));
}

