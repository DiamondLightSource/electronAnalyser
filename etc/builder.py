
from iocbuilder import Device, AutoSubstitution, Architecture, SetSimulation
from iocbuilder.arginfo import *

from iocbuilder.modules.areaDetector import AreaDetector, _ADBase, _ADBaseTemplate, simDetector

class _electronAnalyser(AutoSubstitution):
    TemplateFile="electronAnalyser.template"
    SubstitutionOverwrites = [_ADBaseTemplate]

class electronAnalyser(_ADBase):
    '''Creates a electronAnalyser driver'''
    _SpecificTemplate = _electronAnalyser
    def __init__(self, BUFFERS = 50, MEMORY = -1, **args):
        # Init the superclass
        self.__super.__init__(**args)
        # Store the args
        self.__dict__.update(locals())
        print self._SpecificTemplate


    # __init__ arguments
    ArgInfo = _ADBase.ArgInfo + _electronAnalyser.ArgInfo + makeArgInfo(__init__,
#        XSIZE = Simple('X Size of image', int),
#        YSIZE = Simple('Y Size of image', int),
        BUFFERS = Simple('Maximum number of NDArray buffers to be created for '
            'plugin callbacks', int),
        MEMORY  = Simple('Max memory to allocate, should be maxw*maxh*nbuffer '
            'for driver and all attached plugins', int))

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
        print '# electronAnalyserConfig(portName, ' \
            'maxBuffers, maxMemory)'
        print 'electronAnalyserConfig("%(PORT)s", ' \
            '%(BUFFERS)d, %(MEMORY)d)' % self.__dict__



def electronAnalyser_sim(**kwargs):
    return simDetector(1024, 768, **kwargs)

SetSimulation(electronAnalyser, electronAnalyser_sim)



class _electronAnalyserViewer(AutoSubstitution):
    TemplateFile="electronAnalyserViewer.template"
    SubstitutionOverwrites = [_ADBaseTemplate]

class electronAnalyserViewer(_ADBase):
    '''Creates a electronAnalyserViewer driver'''
    _SpecificTemplate = _electronAnalyserViewer
    def __init__(self, BUFFERS = 50, MEMORY = -1, **args):
        # Init the superclass
        self.__super.__init__(**args)
        # Store the args
        self.__dict__.update(locals())
        print self._SpecificTemplate

    # __init__ arguments
    ArgInfo = _ADBase.ArgInfo + _electronAnalyserViewer.ArgInfo + makeArgInfo(__init__,
        BUFFERS = Simple('Maximum number of NDArray buffers to be created for '
            'plugin callbacks', int),
        MEMORY  = Simple('Max memory to allocate, should be maxw*maxh*nbuffer '
            'for driver and all attached plugins', int))

    LibFileList = ['electronAnalyserViewerSupport']
    DbdFileList = ['electronAnalyserViewerSupport']

    def Initialise(self):
        print '# electronAnalyserViewerConfig(portName, maxBuffers, maxMemory)'
        print 'electronAnalyserViewerConfig("%(PORT)s", %(BUFFERS)d, %(MEMORY)d)' % self.__dict__



