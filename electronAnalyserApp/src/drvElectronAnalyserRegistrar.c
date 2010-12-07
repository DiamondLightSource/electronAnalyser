/** \file drvElectronAnalyserRegistrar.c
 * \brief Register and Exporting the configuration functions to the ioc shell.
 *
 * This is the EPICS dependent code for the driver for a electronAnalyser detector.
 * By making this separate file for the EPICS dependent code the driver itself
 * only needs libCom from EPICS for OS-independence.
 *
 * Author: Fajin Yuan
 *
 * Created:  11/02/2010
 */

#include <iocsh.h>
#include <drvSup.h>
#include <epicsExport.h>

#include "drvElectronAnalyser.h"

/* Code for iocsh registration */
static const iocshArg electronAnalyserConfigArg0 = {"IP Address", iocshArgString};
static const iocshArg electronAnalyserConfigArg1 = {"maxSizeX", iocshArgInt};
static const iocshArg electronAnalyserConfigArg2 = {"maxSizeY", iocshArgInt};
static const iocshArg electronAnalyserConfigArg3 = {"max number of NDArray buffers", iocshArgInt};
static const iocshArg electronAnalyserConfigArg4 = {"maxMemory", iocshArgInt};
static const iocshArg electronAnalyserConfigArg5 = {"priority", iocshArgInt};
static const iocshArg electronAnalyserConfigArg6 = {"stackSize", iocshArgInt};
static const iocshArg * const electronAnalyserConfigArgs[] =  {&electronAnalyserConfigArg0,
                                                          &electronAnalyserConfigArg1,
                                                          &electronAnalyserConfigArg2,
                                                          &electronAnalyserConfigArg3,
                                                          &electronAnalyserConfigArg4,
                                                          &electronAnalyserConfigArg5,
                                                          &electronAnalyserConfigArg6};
static const iocshFuncDef configElectronAnalyser = {"electronAnalyserConfig", 7, electronAnalyserConfigArgs};
static void configelectronAnalyserCallFunc(const iocshArgBuf *args)
{
    electronAnalyserConfig(args[0].sval, args[1].ival, args[2].ival, args[3].ival,
    		args[4].ival, args[5].ival, args[6].ival);
}

static void electronAnalyserRegister(void)
{

    iocshRegister(&configElectronAnalyser, configelectronAnalyserCallFunc);
}

epicsExportRegistrar(electronAnalyserRegister);
