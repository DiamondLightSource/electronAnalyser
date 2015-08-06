
from iocbuilder import Device, AutoSubstitution, Architecture, SetSimulation
from iocbuilder.arginfo import *
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

    # Device attributes
    LibFileList = []
    DbdFileList = []
    SysLibFileList = []
    MakefileStringList = []
    if Architecture() == "win32-x86":
        # Device attributes
        LibFileList += ['electronAnalyserSupport', 'wses']
        SysLibFileList += ['nafxcw']
        DbdFileList += ['electronAnalyserSupport']
        MakefileStringList += ['%(ioc_name)s_LDFLAGS_WIN32 += /NOD:nafxcwd.lib',
                                 'BIN_INSTALLS_WIN32 += $(EPICS_BASE)/bin/$(EPICS_HOST_ARCH)/caRepeater.exe']

    def Initialise(self):
        print '# electronAnalyserConfig(portName, maxBuffers, maxMemory)'
        print 'electronAnalyserConfig("%(PORT)s", %(BUFFERS)d, %(MEMORY)d)' % self.__dict__



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



