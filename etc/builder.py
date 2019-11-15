
from iocbuilder import Device, AutoSubstitution, Architecture, SetSimulation
from iocbuilder.arginfo import *
from iocbuilder import iocwriter # For AddDbMakefileHooks
from iocbuilder.modules.asyn import Asyn, AsynPort, AsynIP

from iocbuilder.modules.ADCore import ADCore, ADBaseTemplate, makeTemplateInstance, includesTemplates

@includesTemplates(ADBaseTemplate)
class electronAnalyserTemplate(AutoSubstitution):
    TemplateFile="electronAnalyser.template"

class electronAnalyser(AsynPort):
    '''Creates a electronAnalyser driver'''
    Dependencies = (ADCore,)
    # This tells xmlbuilder to use PORT instead of name as the row ID
    UniqueName = "PORT"
    _SpecificTemplate = electronAnalyserTemplate
    def __init__(self, PORT, SES_BASE_DIR, SES_INSTRUMENT_FILE, BUFFERS = 50, MEMORY = -1, **args):
        # Init the superclass (AsynPort)
        self.__super.__init__(PORT)
        # Update the attributes of self from the commandline args
        self.__dict__.update(locals())
        # Make an instance of our template
        makeTemplateInstance(self._SpecificTemplate, locals(), args)
        iocwriter.AddDbMakefileHook(self.DbMakefileHook)

    # __init__ arguments
    ArgInfo = ADBaseTemplate.ArgInfo + _SpecificTemplate.ArgInfo + makeArgInfo(__init__,
        PORT = Simple('Port name for the detector', str),
        BUFFERS = Simple('Maximum number of NDArray buffers to be created for plugin callbacks', int),
        MEMORY = Simple('Max memory to allocate, should be maxw*maxh*nbuffer for driver and all attached plugins', int),
        SES_BASE_DIR = Simple('Path to SES base installation directory, e.g. C:/Projects/SES_1.5.0-r5_Win64/. Note the trailing / is required.'),
        SES_INSTRUMENT_FILE = Simple('Path to the SES instrument file.'))
                                                                            



    # Device attributes
    LibFileList = []
    DbdFileList = []
    SysLibFileList = []
    MakefileStringList = []
    if Architecture() == "win32-x86" or Architecture() == "windows-x64":
        # Device attributes
        LibFileList += ['electronAnalyserSupport', 'wses']
        SysLibFileList += ['nafxcw']
        DbdFileList += ['electronAnalyserSupport']
        MakefileStringList += ['%(ioc_name)s_LDFLAGS_WIN32 += /NOD:nafxcwd.lib',
                                 'BIN_INSTALLS_WIN32 += $(EPICS_BASE)/bin/$(EPICS_HOST_ARCH)/caRepeater.exe']

    def Initialise(self):
        print '''#Set SES_OVERRIDE_ENV to # to disable these parameters and load 
#SES settings from the environment (note this requires the IOC to be built 
#with the -b flag having been sent to iocbuilder, otherwise this macro will 
#already have been set to its default by msi).''' 
        print '$(SES_OVERRIDE_ENV="")epicsEnvSet "SES_BASE_DIR", "%(SES_BASE_DIR)s"' % self.__dict__
        print '$(SES_OVERRIDE_ENV="")epicsEnvSet "SES_INSTRUMENT_FILE", "%(SES_INSTRUMENT_FILE)s"' % self.__dict__
        print '$(SES_OVERRIDE_ENV="")epicsEnvSet "SES_INSTRUMENT_DLL", "dll/SESInstrument.dll"'
        print '# electronAnalyserConfig(portName, maxBuffers, maxMemory)'
        print 'electronAnalyserConfig("%(PORT)s", %(BUFFERS)d, %(MEMORY)d)' % self.__dict__

    def PostIocInitialise(self):
        print '# SESWrapper sets pwd to SES_BASE_DIR, and scans will fail to'
        print '# complete if this is not the case when they are run. We '
        print '# overwrite this with INSTALL, so set it back to SES_BASE_DIR '
        print '# here.'
        print 'cd "$(SES_BASE_DIR)"'
        

    def DbMakefileHook(self, makefile, ioc_name, db_filename, expanded_filename):
        makefile.AddLine("SYS_MSI_INCLUDES = -I$(ADCORE)/db")


#def electronAnalyser_sim(**kwargs):
#    return simDetector(1024, 768, **kwargs)
#
#SetSimulation(electronAnalyser, electronAnalyser_sim)



@includesTemplates(ADBaseTemplate)
class electronAnalyserViewerTemplate(AutoSubstitution):
    TemplateFile="electronAnalyserViewer.template"

class electronAnalyserViewer(AsynPort):
    '''Creates a electronAnalyserViewer driver'''
    Dependencies = (ADCore,)
    # This tells xmlbuilder to use PORT instead of name as the row ID
    UniqueName = "PORT"
    _SpecificTemplate = electronAnalyserViewerTemplate
    def __init__(self, PORT, BUFFERS = 50, MEMORY = -1, **args):
        # Init the superclass (AsynPort)
        self.__super.__init__(PORT)
        # Update the attributes of self from the commandline args
        self.__dict__.update(locals())
        # Make an instance of our template
        makeTemplateInstance(self._SpecificTemplate, locals(), args)

    # __init__ arguments
    ArgInfo = ADBaseTemplate.ArgInfo + _SpecificTemplate.ArgInfo + makeArgInfo(__init__,
        PORT = Simple('Port name for the detector', str),
        BUFFERS = Simple('Maximum number of NDArray buffers to be created for plugin callbacks', int),
        MEMORY = Simple('Max memory to allocate, should be maxw*maxh*nbuffer for driver and all attached plugins', int))

    LibFileList = ['electronAnalyserViewerSupport']
    DbdFileList = ['electronAnalyserViewerSupport']

    def Initialise(self):
        print '# electronAnalyserViewerConfig(portName, maxBuffers, maxMemory)'
        print 'electronAnalyserViewerConfig("%(PORT)s", %(BUFFERS)d, %(MEMORY)d)' % self.__dict__



