#!/dls_sw/tools/bin/dls-python2.7
from pkg_resources import require
require('cothread==2.9')
from cothread import *
from cothread.catools import *
nROIs = 8

class PixiumDetector:
    def __init__(self, pv_P, pv_R):
        self.pv_P = pv_P
        self.pv_R = pv_R
        self.portName = caget("%s%sPortName_RBV"%(self.pv_P,self.pv_R))
        self.sizeX = caget("%s%sSizeX_RBV"%(self.pv_P,self.pv_R))
        self.sizeY = caget("%s%sSizeY_RBV"%(self.pv_P,self.pv_R))
        #caput("%s%sDataType"%(self.pv_P,self.pv_R), 'UInt8')
        
    def caput(self, pv, value):
        fullpv = "%s%s%s"%(self.pv_P,self.pv_R, pv)
        print fullpv
        caput(fullpv, value)
        
    def setROI(self, offset, size):
        (minx, miny)=offset
        (sizex, sizey)=size
        data = [minx, miny, sizex, sizey]
        pvs = ["%s%s%s"%(self.pv_P,self.pv_R, pv) for pv in ["MinX", "MinY", "SizeX", "SizeY"]]
        caput( pvs, data )
        
    def setAcqPeriod(self, T):
        period = float(T)
        self.caput( "AcquirePeriod", period )
    
    def asynPortName(self):
        return str(self.portName)

class AdRoi:
    def __init__(self, pv_P, pv_R, nRois):
        self.pvbase = "%s%s"%(pv_P, pv_R)
        self.nRois = nRois
        self.pvrois = ["%s%d:"%(self.pvbase, n) for n in range(self.nRois)]
        
    def enable(self, enable=True):
        if enable:
            caput("%sEnableCallbacks"%(self.pvbase), 'Yes')
        else:
            caput("%sEnableCallbacks"%(self.pvbase), 'No') 

    def use(self, index=None, use='Yes'):
        if index == None:
            for pv in self.pvrois:
                caput("%sUse"%(pv), use)
        else:
            caput("%sUse"%(self.pvrois[index]), use)
            
    def datetype(self, index=None, dataType='UInt16'):
        if index == None:
            for pv in self.pvrois:
                caput("%sDataType"%(pv), dataType)
        else:
            caput("%sDataType"%(self.pvrois[index]), dataType)
        
    def statistics(self, index=None, compute='Yes'):
        if index == None:
            for pv in self.pvrois:
                caput("%sComputeStatistics"%(pv), compute)
        else:
            caput("%sComputeStatistics"%(self.pvrois[index]), compute)

    def histogram(self, index=None, compute='Yes'):
        if index == None:
            for pv in self.pvrois:
                caput("%sComputeHistogram"%(pv), compute)
        else:
            caput("%sComputeHistogram"%(self.pvrois[index]), compute)

    def setSlices( self, sliceSets ):
        for i, sliceSet in enumerate(sliceSets):
            pvs = ["%s%s"%(self.pvrois[i], field) for field in ["MinX", "MinY", "SizeX", "SizeY"]]
            caput( pvs, sliceSet )

class FileSave:
    def __init__(self, pv_P, pv_R):
        self.pvbase = "%s%s"%(pv_P, pv_R)
        
    def setFilePath(self, filepath):
        fullpv = "%sFilePath"%self.pvbase
        # change the string into a list of characters
        path = [ord(c) for c in filepath]
        caput(fullpv, path)

    def setFileName(self, filename):
        fullpv = "%sFileName"%self.pvbase
        # change the string into a list of characters
        path = [ord(c) for c in filename]
        caput(fullpv, path)
     
    def setFileTemplate(self, filetemplate):
        fullpv = "%sFileTemplate"%self.pvbase
        # change the string into a list of characters
        path = [ord(c) for c in filetemplate]
        caput(fullpv, path)
        caput("%sAutoIncrement"%self.pvbase, 'Yes')

    def setWriteMode(self, mode):
        fullpv = "%sFileWriteMode"%self.pvbase
        caput(fullpv, mode)
    
    def enablePlugin(self, enable=True):
        fullpv = "%sEnableCallbacks"%self.pvbase
        caput(fullpv, int(enable))
    
    def startCapture(self, start=True):
        fullpv = "%sAutoSave"%self.pvbase
        caput(fullpv, 'Yes')
        fullpv = "%sCapture"%self.pvbase
        caput(fullpv, int(start))

    def enableAutoSave(self):
        fullpv = "%sAutoSave"%self.pvbase
        caput(fullpv, 'Yes')
        
    def setNumCapture(self, nFrames):
        fullpv = "%sNumCapture"%self.pvbase
        caput(fullpv, int(nFrames))

def createRois( ccdsize, nSlices ):
    maxX, maxY = ccdsize
    sliceheight = int(maxY/nSlices)
    
    yStart = 0
    slices = []
    for i in range(nSlices):
        slices.append( (0, yStart, maxX, sliceheight) )
        yStart += sliceheight
    return slices
        
def main():
    basepv="BL12I-EA-DET-05:"
    pixium = PixiumDetector(basepv, "PIX:")
    #pixium.setROI( (0,0), (100,100) )
    pixium.setAcqPeriod( 0.0 )
    print pixium.asynPortName()
    
    rois = AdRoi(basepv, "ROI1:", nROIs)
    
    # Create 8 sets of ROIs 
    #myrois = [(0,0,100,100), (0,0,100,50), (0,50,100,100)]
    slices = createRois( (pixium.sizeX,pixium.sizeY), nROIs)
    print slices
    rois.setSlices( slices )
    for i in range(nROIs):
        rois.use(i)
        rois.datetype(i)
        rois.statistics(i)
    rois.enable()
    
    tiffs = []
    for i in range(nROIs):
        tiff = FileSave(basepv, "TIFF%d:"%i)
        tiff.setFilePath("""C:/pixiumdata/""")
        tiff.setFileName("tiff%d"%i)
        tiff.setFileTemplate("%s%s_%d.tif")
        tiff.setWriteMode('Stream')
        tiff.setNumCapture(50)
        tiff.enablePlugin()
        tiff.startCapture(True)
        tiffs.append(tiff)
    
    tiff = FileSave(basepv, "TIFF:")
    tiff.setFilePath("""C:/pixiumdata/""")
    tiff.setFileName("pix")
    tiff.setFileTemplate("%s%s_%d.tif")
    tiff.setWriteMode('Single')
    #tiff.setNumCapture(50)
    tiff.enablePlugin()
    #tiff.startCapture(True)
    tiff.enableAutoSave()
    
if __name__ == "__main__":
    main()
