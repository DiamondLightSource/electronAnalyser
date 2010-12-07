/* electronAnalyser.cpp
 *
 * This is an implementation of EPICS areaDetector driver for VG Scienta electron analyser EW4000.
 *
 * It uses SESWrapper software to communicate with instrument firmware which in turn controls the electron analyser.
 *
 * Author:	Fajin Yuan
 * 			Diamond Light Source Ltd
 *
 * Created 06 Dec 2010
 *
 */

// Standard includes
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include "stdafx.h"
#include <atlstr.h>
#include <string>
#include <algorithm>
#include <math.h>
#include <stdlib.h>

// EPICS includes
#include <epicsString.h>
#include <epicsTime.h>
#include <epicsThread.h>
#include <epicsEvent.h>
#include <epicsMutex.h>
#include <epicsTimer.h>

// areaDetector includes
#include <ADDriver.h>

// include device-specific API
#include "wseswrappermain.h"
#include "werror.h"

#define MAX_MESSAGE_SIZE 256
#define MAX_FILENAME_LEN 256

const CString SES_ROOT = getenv("SES_ROOT");
#define AD_STATUS_EXTENSION_START_POINT ADStatusWaiting+1

/** Electron Analyser Trigger mode choices */
typedef enum
{
	TMInternal, TMExternal, TMAlignment
} electronAnalyserTriggerMode_t;
/** Enumeration for Electron analyser Lens Mode */
typedef enum
{
	LensModeFixed,
	LensModeSwept
} lensMode_t;

/** The String representation of lens mode.*/
static asynParamString_t lensModeString[] =
{
	{ LensModeFixed, "Fixed" },
	{ LensModeSwept, "Swept" },
};
/** PU Mode definition */

static const char *driverName = "electronAnalyser";

/**
 * Driver class for Pixium RF4343 Flat panel Detector System. This detector system consists of Dynamix board in
 * AcppBox for detector control and a Matrox Solios Frame Grabber in a Customer Windows PC for imaging acquisition.
 * AcppBox must be started before this class is called in EPICS IOC.
 */
class ElectronAnalyser: public ADDriver
{
public:
	ElectronAnalyser(const char *workingDir, int maxSizeX, int maxSizeY, int maxBuffers, size_t maxMemory,
			int priority, int stackSize);
	virtual ~ElectronAnalyser();
	virtual asynStatus writeInt32(asynUser *pasynUser, epicsInt32 value);
	virtual asynStatus writeFloat64(asynUser *pasynUser, epicsFloat64 value);
	virtual asynStatus writeOctet(asynUser *pasynUser, const char *value, size_t nChars, size_t *nActual);
	virtual asynStatus drvUserCreate(asynUser *pasynUser, const char *drvInfo, const char **pptypeName, size_t *psize);
	void report(FILE *fp, int details);
	epicsEventId startEventId; /**< Should be private but accessed from C, must be public */
	epicsEventId stopEventId; /**< Should be private but accessed from C, must be public */
	void electronAnalyserTask(); /**< This should be private but is called from C so must be public */

private:
	WSESWrapperMain *ses;
	asynStatus acquireData(void *pData);
	asynStatus setROI(int xmin, int ymin, int xsize, int ysize);
	asynStatus updateROI();
	asynStatus getDetectorTemperature(float * temperature);
	BOOL isCallReturnError(int & eaStatus, const char *functionName);

	WError *werror;

	float m_dTemperature;
};
/* end of Electron Analyser class description */

/** A bit of C glue to make the config function available in the startup script (ioc shell) */
extern "C" int electronAnalyserConfig(const char *workingDir, int maxSizeX, int maxSizeY, int maxBuffers, size_t maxMemory,
		int priority, int stackSize)
{
	new ElectronAnalyser(workingDir, maxSizeX, maxSizeY, maxBuffers, maxMemory, priority, stackSize);
	return asynSuccess;
}

static void electronAnalyserTaskC(void *drvPvt)
{
	ElectronAnalyser *pPvt = (ElectronAnalyser *) drvPvt;
	pPvt->electronAnalyserTask();
}

/** Define Electron Analyser driver specific parameters */
typedef enum
{
	lensMode = ADLastStdParam,		/**< (asynInt32,    r/w) the lens mode selected*/
	acquisitionMode, 				/**< (asynInt32,    r/w) the acquisition mode selected*/
	ADLastDriverParam
} electronAnalyserParam_t;

/** Define a table that maps enum-string.
 * The string is used in the database to reference a certain parameter */
static asynParamString_t electronAnalyserParamString[] =
{
		{ lensMode, "LENS_MODE" },
		{ acquisitionMode, "ACQUISITION_MODE" },
};
// A little trick to get the number of parameters...
#define NUM_ELECTRONANALYSER_PARAMS (sizeof(electronAnalyserParamString)/sizeof(electronAnalyserParamString[0]))


ElectronAnalyser::~ElectronAnalyser()
{
}
ElectronAnalyser::ElectronAnalyser(const char *workingDir, int maxSizeX, int maxSizeY, int maxBuffers, size_t maxMemory,
		int priority, int stackSize) :
	ADDriver(portName, 1, ADLastDriverParam, maxBuffers, maxMemory, 0, 0, /* No interfaces beyond those set in ADDriver.cpp */
	ASYN_CANBLOCK, 1, //asynflags (CANBLOCK means separate thread for this driver)
			priority, stackSize) // thread priority and stack size (0=default)
{
	int status = asynSuccess;
	const char *functionName = "ElectronAnalyser";
	int eaStatus = WError::ERR_OK;
	char message[MAX_MESSAGE_SIZE];
	werror=WError::instance();
	// Create the epicsEvents for signaling to the pixium task when acquisition starts and stops
	this->startEventId = epicsEventCreate(epicsEventEmpty);
	if (!this->startEventId)
	{
		printf("%s:%s epicsEventCreate failure for start event\n", driverName,functionName);
		return;
	}
	this->stopEventId = epicsEventCreate(epicsEventEmpty);
	if (!this->stopEventId)
	{
		printf("%s:%s epicsEventCreate failure for stop event\n", driverName,functionName);
		return;
	}
	printf("%s:%s: Initialise SES library.\n", driverName, functionName);
	ses = new WSESWrapperMain(workingDir);
	eaStatus |= ses->setProperty("lib_working_dir", 0, workingDir);
	eaStatus |= ses->initialize(0);
	if (eaStatus != WError::ERR_OK) {
		printf("%s:%s: Initialising SES library failed: %s!\n", driverName, functionName, werror->message(eaStatus));
		asynPrint(this->pasynUserSelf, ASYN_TRACE_ERROR, "%s:%s: Initialising SES library failed: %s!\n", driverName, functionName, werror->message(eaStatus));
	}



	getDetectorTemperature(&m_dTemperature);

	/* Set some default values for parameters */
	status |= setStringParam(ADManufacturer, "VG Scienta");
	status |= setStringParam(ADModel, "EW4000");
	status |= setIntegerParam(ADMaxSizeX, maxSizeX);
	status |= setIntegerParam(ADMaxSizeY, maxSizeY);
	status |= setDoubleParam(ADTemperature, m_dTemperature);

	//status |= setIntegerParam(NDArraySize, 0);
	//status |= setIntegerParam(NDDataType,  NDInt32);
	status |= setIntegerParam(ADImageMode, ADImageSingle);
	status |= setIntegerParam(ADTriggerMode, TMInternal);
	status |= setIntegerParam(ADNumExposures, 1); // number of frames per image
	status |= setIntegerParam(ADNumImages, 1);
	status |= setDoubleParam(ADAcquireTime, 0.0);
	status |= setDoubleParam(ADAcquirePeriod, 0.0);
	status |= setStringParam(ADStatusMessage, message);
	status |= setIntegerParam(NDAutoIncrement, 1);

	if (status)
	{
		printf("%s:%s: unable to set detector parameters\n", driverName,functionName);
		return;
	}

	printf("  Starting up polling task...\n");
	/* Create the thread that updates the images */
	status = (epicsThreadCreate("ElectronAnalyserTask", epicsThreadPriorityMedium,
			epicsThreadGetStackSize(epicsThreadStackMedium),
			(EPICSTHREADFUNC) electronAnalyserTaskC, this) == NULL);
	if (status)
	{
		printf("%s:%s epicsThreadCreate failure for image task\n", driverName,functionName);
		return;
	}
}
/** Task to grab image off the Frame Grabber and send them up to areaDetector.
 *
 *  This function runs the polling thread.
 *  It is started in the class constructor and must not return until the IOC stops.
 *
 */
void ElectronAnalyser::electronAnalyserTask()
{
	int status = asynSuccess;
	int acquire;
	int nbytes;
	int numImages, numImagesCounter, imageCounter, imageMode;
	int arrayCallbacks;
	double acquireTime, acquirePeriod, delay;
	epicsTimeStamp startTime, endTime;
	double elapsedTime;
	NDArray *pImage;
	int dims[2];
	NDDataType_t dataType;
	float temperature;
	const char *functionName = "electronAnalyserTask";

	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: thread started!\n", driverName, functionName);

	this->lock();
	while (1)
	{
		/* Is acquisition active? */
		getIntegerParam(ADAcquire, &acquire);

		/* If we are not acquiring then wait for a semaphore that is given when acquisition is started */
		if (!acquire)
		{
			setStringParam(ADStatusMessage, "Waiting for acquire command");
			setIntegerParam(ADStatus, ADStatusIdle);
			getDetectorTemperature(&temperature);
			setDoubleParam(ADTemperature, temperature);
			callParamCallbacks();
			/* Release the lock while we wait for an event that says acquire has started, then lock again */
			this->unlock();
			asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: waiting for acquire to start\n", driverName, functionName);
			status = epicsEventWait(this->startEventId);
			this->lock();
			getIntegerParam(ADAcquire, &acquire);
		}
		/* We are acquiring. */
		epicsTimeGetCurrent(&startTime);
		getDetectorTemperature(&temperature);
		setDoubleParam(ADTemperature, temperature);

		/* Get the exposure parameters */
		getDoubleParam(ADAcquireTime, &acquireTime);
		getDoubleParam(ADAcquirePeriod, &acquirePeriod);

		/* Get the acquisition parameters */
		//getIntegerParam(ADTriggerMode, &triggerMode);
		getIntegerParam(ADNumImages, &numImages);

		setIntegerParam(ADStatus, ADStatusAcquire);
		callParamCallbacks();

		/* get an image buffer from the pool */
		getIntegerParam(NDArraySizeX, &dims[0]);
		getIntegerParam(NDArraySizeY, &dims[1]);
		getIntegerParam(NDDataType, (int *) &dataType);
		pImage = this->pNDArrayPool->alloc(2, dims, dataType, 0, NULL);

        /* We release the mutex when acquire image, because this may take a long time and
         * we need to allow abort operations to get through */
		this->unlock();
		asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: collect data from electron analyser\n", driverName, functionName);
		status = this->acquireData(pImage->pData);
		this->lock();
		/* If there was an error jump to bottom of the loop */
		if (status)
		{
			asynPrint(this->pasynUserSelf, ASYN_TRACE_ERROR, "%s:%s: probelm in collecting data from electron analyser \n",driverName, functionName);
			setStringParam(ADStatusMessage,"Failed to collect data from electron analyser.");
			acquire=0;
			pImage->release();
			continue;
		}

		nbytes = (dims[0] * dims[1]) * sizeof(dataType);
		pImage->dims[0].size = dims[0];
		pImage->dims[1].size = dims[1];

		/* Set a bit of areadetector image/frame statistics... */
		getIntegerParam(ADNumImages, &numImages);
		getIntegerParam(NDArrayCounter, &imageCounter);
		getIntegerParam(ADNumImagesCounter, &numImagesCounter);
		getIntegerParam(ADImageMode, &imageMode);
		getIntegerParam(NDArrayCallbacks, &arrayCallbacks);
		numImagesCounter++;
		imageCounter++;
		setIntegerParam(ADNumImagesCounter, numImagesCounter);
		setIntegerParam(NDArrayCounter, imageCounter);
		setIntegerParam(NDArraySize, nbytes);

		pImage->uniqueId = imageCounter;
		pImage->timeStamp = startTime.secPastEpoch + startTime.nsec / 1.e9;

		/* Get any attributes that have been defined for this driver */
		this->getAttributes(pImage->pAttributeList);

		pImage->report(2); // print debugging info

		if (arrayCallbacks)
		{
			/* Must release the lock here, or we can get into a deadlock, because we can
			 * block on the plugin lock, and the plugin can be calling us */
			this->unlock();
			asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW,"%s:%s: calling NDArray callback\n", driverName, functionName);
			doCallbacksGenericPointer(pImage, NDArrayData, 0);
			this->lock();
		}
		/* Free the image buffer */
		pImage->release();

		/* check to see if acquisition is done */
		if ((imageMode == ADImageSingle) || ((imageMode == ADImageMultiple) && (numImagesCounter >= numImages)))
		{
			setIntegerParam(ADNumExposuresCounter, 0);
			setIntegerParam(ADNumImagesCounter, 0);
			setIntegerParam(ADAcquire, 0);
			asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: acquisition completed\n", driverName, functionName);
		}
		/* Call the callbacks to update any changes */
		callParamCallbacks();
		getIntegerParam(ADAcquire, &acquire);

		/* If we are acquiring then sleep for the acquire period minus elapsed time. */
		if (acquire)
		{
			epicsTimeGetCurrent(&endTime);
			elapsedTime = epicsTimeDiffInSeconds(&endTime, &startTime);
			delay = acquirePeriod - elapsedTime;
			if (delay >= 0.0)
			{
				asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW,"%s:%s: delay=%f\n",driverName, functionName, delay);
				/* We set the status to indicate we are in the period delay */
				setIntegerParam(ADStatus, ADStatusWaiting);
				callParamCallbacks();
				this->unlock();
				status = epicsEventWaitWithTimeout(this->stopEventId, delay);
				this->lock();
			}
		}
	}
}

/** Grab image off the Frame Grabber.
 * This function expects that the driver to locked already by the caller.
 *
 */
asynStatus ElectronAnalyser::acquireData(void *pData)
{
	asynStatus status = asynSuccess;
	const char *functionName = "acquireData";


	return status;
}

/** Called when asyn clients call pasynInt32->write().
 * Write integer value to the drivers parameter table.
 * \param pasynUser
 * \param value
 * \return asynStatus Either asynError or asynSuccess
 */
asynStatus ElectronAnalyser::writeInt32(asynUser *pasynUser, epicsInt32 value)
{
	int status = asynSuccess;
	int function = pasynUser->reason;
	const char *functionName = "writeInt32";
	// parameters for functions
	int xmin, ymin, xsize, ysize;
	int adstatus;

	status = setIntegerParam(function, value);
	getIntegerParam(ADStatus, &adstatus);

	switch (function)
	{
	case ADAcquire:
		if (value && (adstatus == ADStatusIdle))
		{
			/* Send an event to wake up the pixium task.  */
			epicsEventSignal(this->startEventId);
		}
		if (!value && (adstatus != ADStatusIdle))
		{
			/* Stop acquiring ( abort any hardware processings ) */
			epicsEventSignal(this->stopEventId);
		}
		break;
	case ADTriggerMode:
		break;
	case ADBinX:
		break;
	case ADBinY:
		break;
	case ADSizeX:
	case ADSizeY:
	case ADMinX:
	case ADMinY:
		//TODO hardware ROI settings
		status = getIntegerParam(ADSizeX, &xsize);
		status |= getIntegerParam(ADSizeY, &ysize);
		status |= getIntegerParam(ADMinX, &xmin);
		status |= getIntegerParam(ADMinY, &ymin);
		//this->setROI(xmin, ymin, xsize, ysize);
		break;
	default:
		/* If this is not a parameter we have handled call the base class */
		if (function < ADLastStdParam)
			status = ADDriver::writeInt32(pasynUser, value);
		break;
	}

	/* Do callbacks so higher layers see any changes */
	callParamCallbacks();

	if (status)
	{
		asynPrint(pasynUser, ASYN_TRACE_ERROR,
				"%s:%s: error, status=%d function=%d, value=%d\n",
				driverName, functionName, status, function, value);
		return asynError;
	}
	else
	{
		asynPrint(pasynUser, ASYN_TRACEIO_DRIVER,
				"%s:%s: function=%d, value=%d\n",
				driverName, functionName, function, value);
		return asynSuccess;
	}
	return asynSuccess;
}

/** Write double value to the drivers parameter table.
 * \param pasynUser
 * \param value
 * \return asynStatus Either asynError or asynSuccess
 */
asynStatus ElectronAnalyser::writeFloat64(asynUser *pasynUser, epicsFloat64 value)
{
	int function = pasynUser->reason;
	asynStatus status = asynSuccess;
	const char *functionName = "writeFloat64";
	int adstatus;

	/* Set the parameter and readback in the parameter library.  This may be overwritten when we read back the
	 * status at the end, but that's OK */
	status = setDoubleParam(function, value);
	getIntegerParam(ADStatus, &adstatus);
	switch (function)
	{
	case ADGain:
		break;
	case ADAcquireTime:
		if (adstatus == ADStatusIdle)
		{
			setIntegerParam(ADStatus, ADStatusWaiting);
			callParamCallbacks();
			asynPrint(pasynUser, ASYN_TRACE_FLOW,"%s:%s: Set acquire tiem to %f seconds.\n", driverName, functionName, value);
			setIntegerParam(ADStatus, ADStatusIdle);
			callParamCallbacks();
		}
		break;
	case ADAcquirePeriod:
		if (adstatus == ADStatusIdle)
		{
			setIntegerParam(ADStatus, ADStatusWaiting);
			callParamCallbacks();
			asynPrint(pasynUser, ASYN_TRACE_FLOW,"%s:%s: Set acquire period to %f seconds.\n", driverName, functionName, value);
			setIntegerParam(ADStatus, ADStatusIdle);
			callParamCallbacks();
		}
		break;
	default:
		/* If this parameter belongs to a base class call its method */
		if (function < ADLastStdParam)
			status = ADDriver::writeFloat64(pasynUser, value);
		break;
	}
	/* Do callbacks so higher layers see any changes */
	callParamCallbacks();
	if (status)
		asynPrint(pasynUser, ASYN_TRACE_ERROR,
				"%s:%s error, status=%d function=%d, value=%f\n",
				driverName, functionName, status, function, value);
	else
		asynPrint(pasynUser, ASYN_TRACEIO_DRIVER,
				"%s:%s: function=%d, value=%f\n",
				driverName, functionName, function, value);
	return status;
}
/** Called when asyn clients call pasynOctet->write().
 * This function performs actions for some parameters, including ADFilePath, etc.
 * For all parameters it sets the value in the parameter library and calls any registered callbacks..
 * \param[in] pasynUser pasynUser structure that encodes the reason and address.
 * \param[in] value Address of the string to write.
 * \param[in] nChars Number of characters to write.
 * \param[out] nActual Number of characters actually written. */
asynStatus ElectronAnalyser::writeOctet(asynUser *pasynUser, const char *value,
		size_t nChars, size_t *nActual)
{
	int function = pasynUser->reason;
	asynStatus status = asynSuccess;
	const char *functionName = "writeOctet";

	/* Set the parameter in the parameter library. */
	status = (asynStatus) setStringParam(function, (char *) value);

	switch (function)
	{
	case acquisitionMode:
		break;
	default:
		/* If this parameter belongs to a base class call its method */
		if (function < ADLastStdParam)
			status = ADDriver::writeOctet(pasynUser, value, nChars, nActual);
		break;
	}

	/* Do callbacks so higher layers see any changes */
	status = (asynStatus) callParamCallbacks();

	if (status)
		epicsSnprintf(pasynUser->errorMessage, pasynUser->errorMessageSize,
				"%s:%s: status=%d, function=%d, value=%s", driverName,
				functionName, status, function, value);
	else
		asynPrint(pasynUser, ASYN_TRACEIO_DRIVER,
				"%s:%s: function=%d, value=%s\n",
				driverName, functionName, function, value);
	*nActual = nChars;
	return status;
}

/** Report status of the driver for debugging/testing purpose. Can be invoked from ioc shell.
 * Prints details about the driver if details>0.
 * It then calls the ADDriver::report() method.
 * \param[in] fp File pointed passed by caller where the output is written to.
 * \param[in] details If >0 then driver details are printed.
 */
void ElectronAnalyser::report(FILE *fp, int details)
{
	fprintf(fp, "Pixium detector %s\n", this->portName);
	if (details > 0)
	{
		int nx, ny, dataType;
		getIntegerParam(ADSizeX, &nx);
		getIntegerParam(ADSizeY, &ny);
		getIntegerParam(NDDataType, &dataType);
		fprintf(fp, "  NX, NY:            %d  %d\n", nx, ny);
		fprintf(fp, "  Data type:         %d\n", dataType);
	}
	/* Invoke the base class method */
	ADDriver::report(fp, details);
}

/** Map the drivers enum parameters to their string representation in electronAnalyserParamString[].
 * This is a boilerplate function and not need to change it at all. */
asynStatus ElectronAnalyser::drvUserCreate(asynUser *pasynUser, const char *drvInfo,
		const char **pptypeName, size_t *psize)
{
	asynStatus status;
	//const char *functionName = "drvUserCreate";
	status = this->drvUserCreateParam(pasynUser, drvInfo, pptypeName, psize,
			electronAnalyserParamString, NUM_ELECTRONANALYSER_PARAMS);

	/* If not, then call the base class method, see if it is known there */
	if (status)
		status = ADDriver::drvUserCreate(pasynUser, drvInfo, pptypeName, psize);
	return (status);
}


/** \brief set a single ROI for the detector
 *
 * On success, update AD ROI parameters
 * \param[in] xmin
 * \param[in] ymin
 * \param[in] xsize
 * \param[in] ysize
 * \return asynStatus
 */
asynStatus ElectronAnalyser::setROI(int xmin, int ymin, int xsize, int ysize)
{

	int eaint = 0;
	asynStatus status = asynSuccess;
	const char * functionName = "setROI";
	//TODO call firmware to set ROI
	updateROI();
	return status;
}
/* \brief poll ROI from Detector and Update
 *
 */
asynStatus ElectronAnalyser::updateROI()
{
	int eaStatus = 0;
	int status = asynSuccess;
	const char * functionName = "updateROI";
	int minx,miny,sizex,sizey;
	//TODO get ROI from Analyser

	// set the outputs
	status = setIntegerParam(ADMinX, minx);
	status |= setIntegerParam(ADMinY, miny);
	status |= setIntegerParam(ADSizeX, sizex);
	status |= setIntegerParam(ADSizeY, sizey);
	status |= callParamCallbacks();
	if (status)
		return asynError;
	return asynSuccess;
}

asynStatus ElectronAnalyser::getDetectorTemperature(float * temperature)
{
	int eaStatus = 0;
	asynStatus status = asynSuccess;
	const char * functionName = "getDetectorTemperature";
	//TODO  get temperature from analyser
	*temperature = 20.0;
	return status;
}

/** check Electron Analyser API call return status.
 *
 * If the call return error status, it converts the status code to error string message, add to asynTrace and
 * sets the Area Detector's ADStatusMessage field, update any client. It also check if the error is Fatal Error.
 *
 * \param[in] pixiumStatus - the return status code
 * \param[in] functionName - the function name where Pixium API call is made.
 * \return true if error in return, false if PIX_NO_ERROR is returned.
 */
BOOL ElectronAnalyser::isCallReturnError(int & eaStatus, const char *functionName)
{
	if (eaStatus != WError::ERR_OK)
	{
		const char * errorMessage = (werror->message(eaStatus)).c_str();
		asynPrint(this->pasynUserSelf, ASYN_TRACE_ERROR,"%s:%s: %s\n", driverName, functionName, errorMessage);
		setStringParam(ADStatusMessage, errorMessage);
		callParamCallbacks();
		return true;
	}
	return false;
}
