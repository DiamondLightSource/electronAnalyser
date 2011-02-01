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
static const iocshArg electronAnalyserConfigArg0 = {"portName", iocshArgString};
static const iocshArg electronAnalyserConfigArg1 = {"workingDir", iocshArgString};
static const iocshArg electronAnalyserConfigArg2 = {"instrumentFile", iocshArgString};
static const iocshArg electronAnalyserConfigArg3 = {"maxSizeX", iocshArgInt};
static const iocshArg electronAnalyserConfigArg4 = {"maxSizeY", iocshArgInt};
static const iocshArg electronAnalyserConfigArg5 = {"max number of NDArray buffers", iocshArgInt};
static const iocshArg electronAnalyserConfigArg6 = {"maxMemory", iocshArgInt};
static const iocshArg electronAnalyserConfigArg7 = {"priority", iocshArgInt};
static const iocshArg electronAnalyserConfigArg8 = {"stackSize", iocshArgInt};
static const iocshArg * const electronAnalyserConfigArgs[] =  {&electronAnalyserConfigArg0,
                                                          &electronAnalyserConfigArg1,
                                                          &electronAnalyserConfigArg2,
                                                          &electronAnalyserConfigArg3,
                                                          &electronAnalyserConfigArg4,
                                                          &electronAnalyserConfigArg5,
                                                          &electronAnalyserConfigArg6,
                                                          &electronAnalyserConfigArg7,
                                                          &electronAnalyserConfigArg8};
static const iocshFuncDef configElectronAnalyser = {"electronAnalyserConfig", 9, electronAnalyserConfigArgs};
static void configElectronAnalyserCallFunc(const iocshArgBuf *args)
{
    electronAnalyserConfig(args[0].sval, args[1].sval, args[2].sval, args[3].ival,
    		args[4].ival, args[5].ival, args[6].ival, args[7].ival,args[8].ival);
}

static void electronAnalyserRegister(void)
{

    iocshRegister(&configElectronAnalyser, configElectronAnalyserCallFunc);
}

epicsExportRegistrar(electronAnalyserRegister);
