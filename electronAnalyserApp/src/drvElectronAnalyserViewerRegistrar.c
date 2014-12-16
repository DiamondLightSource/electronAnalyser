/** \file drvElectronAnalyserViewerRegistrar.c
 * \brief Register and Exporting the configuration functions to the ioc shell.
 *
 * This is the EPICS dependent code for the driver for a electronAnalyserViewer detector.
 * By making this separate file for the EPICS dependent code the driver itself
 * only needs libCom from EPICS for OS-independence.
 *
 */

#include <iocsh.h>
#include <drvSup.h>
#include <epicsExport.h>

#include "drvElectronAnalyserViewer.h"

/* Code for iocsh registration */
static const iocshArg electronAnalyserViewerConfigArg0 = {"portName", iocshArgString};
//static const iocshArg electronAnalyserViewerConfigArg1 = {"workingDir", iocshArgString};
//static const iocshArg electronAnalyserViewerConfigArg2 = {"instrumentFile", iocshArgString};
static const iocshArg electronAnalyserViewerConfigArg1 = {"max number of NDArray buffers", iocshArgInt};
static const iocshArg electronAnalyserViewerConfigArg2 = {"maxMemory", iocshArgInt};
static const iocshArg electronAnalyserViewerConfigArg3 = {"priority", iocshArgInt};
static const iocshArg electronAnalyserViewerConfigArg4 = {"stackSize", iocshArgInt};
/*static const iocshArg * const electronAnalyserViewerConfigArgs[] =  {&electronAnalyserViewerConfigArg0,
                                                          &electronAnalyserViewerConfigArg1,
                                                          &electronAnalyserViewerConfigArg2,
                                                          &electronAnalyserViewerConfigArg3,
                                                          &electronAnalyserViewerConfigArg4,
                                                          &electronAnalyserViewerConfigArg5,
                                                          &electronAnalyserViewerConfigArg6};*/
static const iocshArg * const electronAnalyserViewerConfigArgs[] =  {&electronAnalyserViewerConfigArg0,
                                                          &electronAnalyserViewerConfigArg1,
                                                          &electronAnalyserViewerConfigArg2,
                                                          &electronAnalyserViewerConfigArg3,
                                                          &electronAnalyserViewerConfigArg4};
static const iocshFuncDef configelectronAnalyserViewer = {"electronAnalyserViewerConfig", 5, electronAnalyserViewerConfigArgs};
static void configelectronAnalyserViewerCallFunc(const iocshArgBuf *args)
{
    //electronAnalyserViewerConfig(args[0].sval, args[1].sval, args[2].sval, args[3].ival, args[4].ival, args[5].ival, args[6].ival);
    electronAnalyserViewerConfig(args[0].sval, args[1].ival, args[2].ival, args[3].ival, args[4].ival);
}

static void electronAnalyserViewerRegister(void)
{

    iocshRegister(&configelectronAnalyserViewer, configelectronAnalyserViewerCallFunc);
}

epicsExportRegistrar(electronAnalyserViewerRegister);
