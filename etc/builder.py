
from iocbuilder import Device, AutoSubstitution, Architecture, SetSimulation
from iocbuilder.arginfo import *

from iocbuilder.modules.areaDetector import AreaDetector, _ADBase, _ADBaseTemplate, simDetector

class _electronAnalyser(AutoSubstitution):
    TemplateFile="electronAnalyser.template"
    SubstitutionOverwrites = [_ADBaseTemplate]

class electronAnalyser(_ADBase):
    '''Creates a electronAnalyser driver'''
    _SpecificTemplate = _electronAnalyser
    def __init__(self, WRAPPER_LOCATION, INSTRUMENT_FILE, XSIZE, YSIZE, BUFFERS = 50, MEMORY = -1, **args):
        # Init the superclass
        self.__super.__init__(**args)
        # Store the args
        self.__dict__.update(locals())
        print self._SpecificTemplate


    # __init__ arguments
    ArgInfo = _ADBase.ArgInfo + _electronAnalyser.ArgInfo + makeArgInfo(__init__,
        WRAPPER_LOCATION      = Simple('Location and version of SESWrapper', str),    
        INSTRUMENT_FILE   = Simple('Name of instrument file', str),
        XSIZE = Simple('X Size of image', int),
        YSIZE = Simple('Y Size of image', int),
        BUFFERS = Simple('Maximum number of NDArray buffers to be created for '
            'plugin callbacks', int),
        MEMORY  = Simple('Max memory to allocate, should be maxw*maxh*nbuffer '
            'for driver and all attached plugins', int))

    LibFileList = []
    DbdFileList = []
    if Architecture() == "win32-x86":
        # Device attributes
        LibFileList = ['electronAnalyserSupport']
        DbdFileList = ['electronAnalyserSupport']

    def Initialise(self):
        print '# electronAnalyserConfig(portName, SESWrapper Location & Version, Instrument File, XSize, YSize, ' \
            'maxBuffers, maxMemory)'
        print 'electronAnalyserConfig("%(PORT)s", "%(WRAPPER_LOCATION)s","%(INSTRUMENT_FILE)s", %(XSIZE)d, %(YSIZE)d, ' \
            '%(BUFFERS)d, %(MEMORY)d)' % self.__dict__



def electronAnalyser_sim(**kwargs):
    return simDetector(1024, 768, **kwargs)

SetSimulation(electronAnalyser, electronAnalyser_sim)
