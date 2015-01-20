/* electronAnalyserViewer.cpp
 *
 * This is a driver for the network available image
 * present in the VG SES software from version 1.4 onwards.
 *
 * Author: gnx91527
 *
 * Created:  December 3, 2014
 */

#include <stddef.h>
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

#include <epicsTime.h>
#include <epicsThread.h>
#include <epicsEvent.h>
#include <epicsMutex.h>
#include <epicsString.h>
#include <epicsStdio.h>
#include <epicsMutex.h>
#include <cantProceed.h>
#include <iocsh.h>

#include "ADDriver.h"
#include <epicsExport.h>

#include "zmq.hpp"
#include "dgframe.pb.h"
#include <string>
#undef min
#undef max

#define SesVersionString "SES_VERSION"
#define SesConnectionString "SES_CONNECTION"

static const char *driverName = "electronAnalyserViewer";

/**
 * VG Scienta Electron Analyser Viewer driver.
 *
 * Uses zeromq and google protocol buffers to provide a live image
 * feed from the analyser camera into areaDetector.
 */
class ElectronAnalyserViewer : public ADDriver
{
  public:
    ElectronAnalyserViewer(const char *portName, int maxBuffers, size_t maxMemory, int priority, int stackSize);
    ~ElectronAnalyserViewer();
    void electronAnalyserViewerTask();
    asynStatus writeInt32(asynUser *pasynUser, epicsInt32 value);
    void report(FILE *fp, int details);

  protected:
    int SesVersion;
    #define FIRST_EEVIEWER_PARAM SesVersion
    int SesConnection;
    #define LAST_EEVIEWER_PARAM SesConnection

  private:
    epicsEventId startEventId;
    epicsEventId stopEventId;
    //WFrameLoader *framePtr;
    NDArray *pRaw;
};


/**
 * A bit of C glue to make the config function available in the startup script (ioc shell)
 */
extern "C" int electronAnalyserViewerConfig(const char *portName, int maxBuffers, size_t maxMemory, int priority, int stackSize)
{
  new ElectronAnalyserViewer(portName, maxBuffers, maxMemory, priority, stackSize);
  return asynSuccess;
}

/**
 * Make use of a c thread to call into the object and start the real
 * acquisition task.
 */
static void electronAnalyserViewerTaskC(void *drvPvt)
{
  ElectronAnalyserViewer *pPvt = (ElectronAnalyserViewer *)drvPvt;
  pPvt->electronAnalyserViewerTask();
}

/**
 * Number of asyn parameters (asyn commands) this driver supports
 */
#define NUM_EEVIEWER_PARAMS (&LAST_EEVIEWER_PARAM - &FIRST_EEVIEWER_PARAM + 1)

/**
 * ElectronAnalyserViewer destructor
 */
ElectronAnalyserViewer::~ElectronAnalyserViewer()
{
}

/**
 * ElectronAnalyserViewer constructor
 */
ElectronAnalyserViewer::ElectronAnalyserViewer(const char *portName,
                                               int maxBuffers,
                                               size_t maxMemory,
                                               int priority,
                                               int stackSize) :
                        ADDriver(portName,
                                 1,
                                 NUM_EEVIEWER_PARAMS,
                                 maxBuffers,
                                 maxMemory,
                                 asynEnumMask | asynFloat64ArrayMask,
                                 asynEnumMask | asynFloat64ArrayMask, // No interfaces beyond those set in ADDriver.cpp
                                 ASYN_CANBLOCK,                       // CANBLOCK means separate thread for this driver
                                 1,
                                 priority,                            // Thread priority (0 = default)
                                 stackSize)                           // Stack size (0 = default)
{
  int status = asynSuccess;
  const char *functionName = "ElectronAnalyserViewer";

  // Create the epicsEvents for signalling to the Electron Analyser task when acquisition starts
  if (status == asynSuccess){
    this->startEventId = epicsEventCreate(epicsEventEmpty);
    if (!this->startEventId){
      asynPrint(this->pasynUserSelf, ASYN_TRACE_ERROR, "%s:%s epicsEventCreate failure for start event\n", driverName, functionName);
      status = asynError;
    }
  }

  // Create the epicsEvents for signalling to the Electron Analyser task when acquisition stops
  if (status == asynSuccess){
    this->stopEventId = epicsEventCreate(epicsEventEmpty);
    if (!this->stopEventId){
      asynPrint(this->pasynUserSelf, ASYN_TRACE_ERROR, "%s:%s: epicsEventCreate failure for stop event\n", driverName, functionName);
      status = asynError;
    }
  }

  if (status == asynSuccess){
    // Create version string
    status |= createParam(SesVersionString, asynParamOctet, &SesVersion);
    status |= createParam(SesConnectionString, asynParamOctet, &SesConnection);

    // Setup values for the collect panel
    status |= setDoubleParam(ADAcquireTime, 0.0);
    status |= setDoubleParam(ADAcquirePeriod, 0.0);
    status |= setIntegerParam(ADNumImages, 1);
    status |= setIntegerParam(ADNumExposures, 1);
    status |= setIntegerParam(ADImageMode, ADImageSingle);
    status |= setIntegerParam(ADTriggerMode, ADTriggerInternal);
    status |= setStringParam(ADManufacturer, "VG Scienta");
    status |= setStringParam(ADModel, "Live Viewer");
    //status |= setStringParam(ADStatusMessage, message);
    status |= setIntegerParam(NDAutoIncrement, 1);
  }

  if (status == asynSuccess){
    asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: Starting up polling task....\n", driverName, functionName);
    // Create the thread that updates the images
    status = (epicsThreadCreate("ElectronAnalyserTask",
                                epicsThreadPriorityMedium,
                                epicsThreadGetStackSize(epicsThreadStackMedium),
                                (EPICSTHREADFUNC)electronAnalyserViewerTaskC,
                                this) == NULL);
    if (status){
      asynPrint(this->pasynUserSelf, ASYN_TRACE_ERROR, "%s:%s: epicsTheadCreate failure for image task\n", driverName, functionName);
    }
  }
}

/**
 * Task to listen for images from the network viewer and process them for areaDetector.
 *
 *  This function runs within the thread created by the object.
 *  It is started in the class constructor and must not return until the IOC stops.
 */
void ElectronAnalyserViewer::electronAnalyserViewerTask()
{
  int status = asynSuccess;
  const char *functionName = "electronAnalyserTask";
  int acquire;
  int numImages;
  int numImagesCounter;
  int imageCounter;
  int imageMode;
  int nbytes = 0;
  int arrayCallbacks;
  int waitMsec = 10;
  double acquireTime;
  double acquirePeriod;
  double delay;
  double elapsedTime;
  epicsTimeStamp startTime;
  epicsTimeStamp endTime;
  NDArray *pImage;
  size_t dims[2];
  NDDataType_t dataType;
  char connectionString[128];
  zmq::context_t *ctx;
  zmq::socket_t *frameSocket = 0;
  zmq::pollitem_t *items = 0;
  zmq::message_t msgHeader;
  zmq::message_t msgData;

  asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: Polling thread started\n", driverName, functionName);

  this->lock();
  while (1){
    getIntegerParam(ADAcquire, &acquire);
    // If we are not acquiring or encountered a problem then wait for a semaphore that is given when acquisition is started
    if (!acquire){
      // Check in case we were acquiring and have stopped, if so we need to clear up
      if (frameSocket != 0){
        frameSocket->setsockopt(ZMQ_UNSUBSCRIBE, "", 0);
        delete(frameSocket);
        frameSocket = 0;
      }
      // Only set the status message if we didn't encounter a problem last time, so we don't overwrite the error mesage
      if(!status){
        asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: Waiting for the acquire command\n", driverName, functionName);
        setStringParam(ADStatusMessage, "Waiting for the acquire command");
        setIntegerParam(ADStatus, ADStatusIdle);
      }
      // Reset the counters
      setIntegerParam(ADNumExposuresCounter, 0);
      setIntegerParam(ADNumImagesCounter, 0);
      callParamCallbacks();

      // Release the lock while we wait for an event that says acquire has started, then lock again
      this->unlock();
      asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: waiting for acquire to start\n", driverName, functionName);
      status = epicsEventWait(this->startEventId);
      this->lock();
      getIntegerParam(ADAcquire, &acquire);
      if (acquire){
        setStringParam(ADStatusMessage, "Waiting for header information");
        setIntegerParam(ADStatus, ADStatusInitializing);
        callParamCallbacks();
        // The acquisition has started, create the new zmq context
        ctx = new zmq::context_t;
        frameSocket = new zmq::socket_t(*ctx, ZMQ_SUB);
        // Readout the connection string
        getStringParam(SesConnection, sizeof(connectionString), connectionString);
        //std::string address = "tcp://172.23.12.96:55555";
        try
        {
          frameSocket->connect(connectionString);
          frameSocket->setsockopt(ZMQ_SUBSCRIBE, "", 0);
          items = (zmq::pollitem_t *)malloc(sizeof(zmq::pollitem_t));
          items[0].socket = *frameSocket;
          items[0].fd = 0;
          items[0].events = ZMQ_POLLIN;
          items[0].revents = 0;
          this->unlock();
          int p = zmq::poll(items, 1, 1000);
          this->lock();
          if (p > 0){
            if (items[0].revents == ZMQ_POLLIN){
              //printf("Received test frame for header information\n");
              if (frameSocket->recv(&msgHeader, ZMQ_RCVMORE) && frameSocket->recv(&msgData)){
                dgframe::Frame frame;
                frame.ParseFromArray(msgHeader.data(), msgHeader.size());
                //printf("  width:  %d\n", frame.width());
                //printf("  height: %d\n", frame.height());
                //printf("  length: %d\n", frame.length());
                //printf("  code:   %d\n", frame.code());
                dims[0] = frame.width();
                dims[1] = frame.height();
                nbytes = frame.length();
                setIntegerParam(ADMaxSizeX, frame.width());
                setIntegerParam(ADMaxSizeY, frame.height());
                setIntegerParam(ADMinX, 0);
                setIntegerParam(ADMinY, 0);
                setIntegerParam(ADSizeX, frame.width());
                setIntegerParam(ADSizeY, frame.height());
                setIntegerParam(NDArraySizeX, frame.width());
                setIntegerParam(NDArraySizeY, frame.height());
                setIntegerParam(NDArraySize, (frame.height()*frame.width()));
                callParamCallbacks();
              } else {
                acquire = 0;
                status = 1;
                setIntegerParam(ADAcquire, 0);
                setIntegerParam(ADStatus, ADStatusError);
                setStringParam(ADStatusMessage, "Error retrieving header information");
              }
            } else {
              acquire = 0;
              status = 1;
              setIntegerParam(ADAcquire, 0);
              setIntegerParam(ADStatus, ADStatusError);
              setStringParam(ADStatusMessage, "Error retrieving header information");
            }
          } else {
            acquire = 0;
            status = 1;
            setIntegerParam(ADAcquire, 0);
            setIntegerParam(ADStatus, ADStatusError);
            setStringParam(ADStatusMessage, "Error retrieving header information");
          }
        }  catch (zmq::error_t &e)
        {
          acquire = 0;
          status = 1;
          setIntegerParam(ADAcquire, 0);
          setIntegerParam(ADStatus, ADStatusError);
          setStringParam(ADStatusMessage, "Error retrieving header information");
        }
      }
    }
    callParamCallbacks();
    
    if (acquire){
      // We are acquiring.
      asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: We are acquiring\n", driverName, functionName);

      epicsTimeGetCurrent(&startTime);

      // Get the exposure parameters
      getDoubleParam(ADAcquireTime, &acquireTime);
      getDoubleParam(ADAcquirePeriod, &acquirePeriod);

      // Get the acquisition parameters
      getIntegerParam(ADNumImages, &numImages);

      setIntegerParam(ADStatus, ADStatusAcquire);
      setStringParam(ADStatusMessage, "Acquiring images");

      // Get data type
      getIntegerParam(NDDataType, (int *) &dataType);
      //asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: dims[0] = %d, dims[1] = %d, datatype = %d\n", driverName, functionName, dims[0], dims[1], dataType);

      // Allocate memory suitable for 2D data
      pImage = this->pNDArrayPool->alloc(2, dims, dataType, 0, NULL);

      // We release the mutex when acquire image, because this may take a long time and
      // we need to allow abort operations to get through
      this->unlock();
      asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: Collecting data from electron analyser....\n", driverName, functionName);
      //status = this->acquireData(pImage->pData, steps);

      try
      {
        int p = 0;
        while (p == 0){
          p = zmq::poll(items, 1, waitMsec);
        }
        if (p > 0){
          if (items[0].revents == ZMQ_POLLIN){
            frameSocket->recv(&msgHeader, ZMQ_RCVMORE);
            frameSocket->recv(&msgData);
          }
        }
        while (p > 0){
          //printf("Processing...\n");
          p = zmq::poll(items, 1, 0);
          if (p > 0){
            if (items[0].revents == ZMQ_POLLIN){
              frameSocket->recv(&msgHeader, ZMQ_RCVMORE);
              frameSocket->recv(&msgData);
            }
          }
        }
        dgframe::Frame frame;
        frame.ParseFromArray(msgHeader.data(), msgHeader.size());
        memcpy(pImage->pData, msgData.data(), frame.length());
      }  catch (zmq::error_t &e)
      {
        acquire = 0;
        status = 1;
        setIntegerParam(ADAcquire, 0);
        setIntegerParam(ADStatus, ADStatusError);
        setStringParam(ADStatusMessage, "Error during acquisition, aborted");
      }

      this->lock();
      // If there was an error jump to bottom of the loop
      if (status){
        // Find out why there was a problem acquiring data
        int ErrorStatus;
        getIntegerParam(ADStatus, &ErrorStatus);
        // User aborted acquisition
        if(ErrorStatus == ADStatusAborted){
          asynPrint(this->pasynUserSelf, ASYN_TRACE_ERROR, "%s:%s: Acquisition was aborted by the user\n",driverName, functionName);
          setStringParam(ADStatusMessage, "Acquisition was aborted by the user");
          acquire = 0;
          setIntegerParam(ADAcquire, acquire);
        } else {
          // The acquisition timed out
          asynPrint(this->pasynUserSelf, ASYN_TRACE_ERROR, "%s:%s: Acquisition timed out\n",driverName, functionName);
          setStringParam(ADStatusMessage,"Acquisition timed out");
          setIntegerParam(ADStatus, ADStatusError);
          // Reset both acquire and ADAcquire back to zero
          acquire = 0;
          setIntegerParam(ADAcquire, acquire);
          pImage->release();
          continue;
        }
      }

      asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: dims[0] = %d\n", driverName, functionName, (int)dims[0]);
      asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: dims[1] = %d\n", driverName, functionName, (int)dims[1]);
      asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: Number of bytes of NDArray = %d\n", driverName, functionName, nbytes);

      pImage->dims[0].size = dims[0];
      pImage->dims[1].size = dims[1];

      // Set a bit of areadetector image/frame statistics...
      getIntegerParam(ADNumImages, &numImages);
      getIntegerParam(ADImageMode, &imageMode);
      getIntegerParam(NDArrayCallbacks, &arrayCallbacks);
      getIntegerParam(NDArrayCounter, &imageCounter);
      getIntegerParam(ADNumImagesCounter, &numImagesCounter);
      imageCounter++;
      numImagesCounter++;
      setIntegerParam(NDArrayCounter, imageCounter);
      setIntegerParam(ADNumImagesCounter, numImagesCounter);

      pImage->uniqueId = imageCounter;
      pImage->timeStamp = startTime.secPastEpoch + startTime.nsec / 1.e9;

      // Get any attributes that have been defined for this driver
      this->getAttributes(pImage->pAttributeList);

      if (arrayCallbacks){
        // Must release the lock here, or we can get into a deadlock, because we can
        // block on the plugin lock, and the plugin can be calling us
        this->unlock();
        asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW,"%s:%s: calling NDArray callback\n", driverName, functionName);
        doCallbacksGenericPointer(pImage, NDArrayData, 0);
        this->lock();
      }

      // Free the image buffers
      pImage->release();

      // Check to see if acquisition is complete
      if ((imageMode == ADImageSingle) || ((imageMode == ADImageMultiple)&& (numImagesCounter >= numImages))){
        setIntegerParam(ADAcquire, 0);
        asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: acquisition completed\n", driverName, functionName);
      }
      // Call the callbacks to update any changes
      callParamCallbacks();
      getIntegerParam(ADAcquire, &acquire);

    }

    // If we are acquiring then sleep for the acquire period minus elapsed time.
    if (acquire){
      epicsTimeGetCurrent(&endTime);
      elapsedTime = epicsTimeDiffInSeconds(&endTime, &startTime);
      delay = acquirePeriod - elapsedTime;
      if (delay >= 0.0){
        asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW,"%s:%s: delay=%f\n",driverName, functionName, delay);
        // We set the status to indicate we are in the period delay
        setIntegerParam(ADStatus, ADStatusWaiting);
        callParamCallbacks();
        this->unlock();
        epicsEventWaitWithTimeout(this->stopEventId, delay);
        this->lock();
      }
    }
  }
}

/**
 * Called when asyn clients call pasynInt32->write().
 * Write integer value to the drivers parameter table.
 * \param pasynUser
 * \param value
 * \return asynStatus Either asynError or asynSuccess
 */
asynStatus ElectronAnalyserViewer::writeInt32(asynUser *pasynUser, epicsInt32 value)
{
  int status = asynSuccess;
  int function = pasynUser->reason;
  const char *functionName = "writeInt32";
  int OldValue;
  int adstatus;

  getIntegerParam(function, &OldValue);
  this->lock();
  status = setIntegerParam(function, value);
  getIntegerParam(ADStatus, &adstatus);

  if (function == ADAcquire){
    if (value && (adstatus == ADStatusIdle || adstatus == ADStatusError || adstatus == ADStatusAborted)){
      // Send an event to wake up the electronAnalyserViewer task.
      epicsEventSignal(this->startEventId);
    }
    if (!value && (adstatus != ADStatusIdle)){
      // Stop acquiring
      epicsEventSignal(this->stopEventId);
    }
  }
  // Do callbacks so higher layers see any changes
  callParamCallbacks();
  this->unlock();
  if (status){
    asynPrint(pasynUser, ASYN_TRACE_ERROR,"%s:%s: error, status=%d function=%d, value=%d\n",driverName, functionName, status, function, value);
    return asynError;
  }
  asynPrint(pasynUser, ASYN_TRACEIO_DRIVER,"%s:%s: function=%d, value=%d\n",driverName, functionName, function, value);
  return asynSuccess;
}

/**
 * Report status of the driver for debugging/testing purpose. Can be invoked from ioc shell.
 * Prints details about the driver if details>0.
 * It then calls the ADDriver::report() method.
 * \param[in] fp File pointed passed by caller where the output is written to.
 * \param[in] details If >0 then driver details are printed.
 */
void ElectronAnalyserViewer::report(FILE *fp, int details)
{
  fprintf(fp, "ElectronAnalyserViewer detector %s\n", this->portName);
  if (details > 0){
    int nx, ny, dataType;
    getIntegerParam(ADSizeX, &nx);
    getIntegerParam(ADSizeY, &ny);
    getIntegerParam(NDDataType, &dataType);
    fprintf(fp, "  NX, NY:            %d  %d\n", nx, ny);
    fprintf(fp, "  Data type:         %d\n", dataType);
  }
  // Invoke the base class method
  ADDriver::report(fp, details);
}

