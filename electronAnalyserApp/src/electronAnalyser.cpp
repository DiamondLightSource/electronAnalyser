//================================================================================================
/* file:		electronAnalyser.cpp
 *
 * description:	Implementation of EPICS areaDetector driver for data acquisition using
 * 				VG Scienta Electron Analyser EW4000. It uses SESWrapper software to communicate
 * 				with instrument firmware which in turn controls the electron analyser.
 *
 * project:		ElectronAnalyser
 *
 * $Author:		Fajin Yuan $
 * 				Diamond Light Source Ltd
 *
 * $Revision:	1.0.0 $
 *
 * $Log:		electronAnalyser.cpp $
 *
 * Revision 1.0.0 2010/12/06 12:45:25 fajinyuan
 * initial creation of the project
 */

// Standard includes
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include "stdafx.h"
#include <atlstr.h>
#include <string>
#include <vector>
#include <list>
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

using std::string;

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
	LensModeFixed, LensModeSwept
} lensMode_t;

/** The String representation of lens mode.*/
static asynParamString_t lensModeString[] =
{
{ LensModeFixed, "Fixed" },
{ LensModeSwept, "Swept" }, };
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
    ElectronAnalyser(const char *workingDir, int maxSizeX, int maxSizeY, int maxBuffers, size_t maxMemory, int priority, int stackSize);
    virtual ~ElectronAnalyser();
    virtual asynStatus writeInt32(asynUser *pasynUser, epicsInt32 value);
    virtual asynStatus writeFloat64(asynUser *pasynUser, epicsFloat64 value);
    virtual asynStatus writeOctet(asynUser *pasynUser, const char *value, size_t nChars, size_t *nActual);
    virtual asynStatus drvUserCreate(asynUser *pasynUser, const char *drvInfo, const char **pptypeName, size_t *psize);
    void report(FILE *fp, int details);
    epicsEventId startEventId;
    epicsEventId stopEventId;
    void electronAnalyserTask();
private:
    WSESWrapperMain *ses;
    SESWrapperNS::WAnalyzerRegion analyzer;
    SESWrapperNS::WDetectorRegion detector;
    SESWrapperNS::WDetectorInfo info;
    asynStatus acquireData(void *pData);
    asynStatus setROI(int xmin, int ymin, int xsize, int ysize);
    asynStatus updateROI();
    asynStatus getDetectorTemperature(float *temperature);
    virtual void init_device();
    virtual void always_executed_hook();
    void delete_device();
    virtual void read_attr_hardware(std::vector<long > & attr_list);
    virtual asynStatus getKineticEnergy(double & kineticEnergy);
    virtual asynStatus setKineticEnergy(double kineticEnergy);
    virtual asynStatus getAnalyserMode(bool & mode);
    virtual asynStatus setAnalyserMode(bool mode);
    virtual asynStatus getEnergyScale(string & energyScale);
    virtual asynStatus setEnergyScale(string energyScale);
    virtual asynStatus getHighEnergy(double & highEnergy);
    virtual asynStatus setHighEnergy(double highEnergy);
    virtual asynStatus getLowEnergy(double & lowEnergy);
    virtual asynStatus setLowEnergy(double lowEnergy);
    virtual asynStatus getCenterEnergy(double & fixedEnergy);
    virtual asynStatus setCenterEnergy(double fixedEnergy);
    virtual asynStatus getEnergyStep(double & energyStep);
    virtual asynStatus setEnergyStep(double energyStep);
    virtual asynStatus getStepTime(int & stepTime);
    virtual asynStatus setStepTime(int stepTime);
    virtual asynStatus getLensMode(char * lensMode);
    virtual asynStatus setLensMode(char * lensMode);
    virtual asynStatus getPassEnergy(double & passEnergy);
    virtual asynStatus setPassEnergy(double passEnergy);
    virtual asynStatus getElementSet(char * passMode);
    virtual asynStatus setElementSet(char * passMode);
    virtual asynStatus getDetectorFirstXChannel(int & detectorFirstXChannel);
    virtual asynStatus setDetectorFirstXChannel(int detectorFirstXChannel);
    virtual asynStatus getDetectorLastXChannel(int & detectorLastXChannel);
    virtual asynStatus setDetectorLastXChannel(int detectorLastXChannel);
    virtual asynStatus getDetectorFirstYChannel(int & detectorFirstYChannel);
    virtual asynStatus setDetectorFirstYChannel(int detectorFirstYChannel);
    virtual asynStatus getDetectorLastYChannel(int & detectorLastYChannel);
    virtual asynStatus setDetectorLastYChannel(int detectorLastYChannel);
    virtual asynStatus getDetectorSlices(int & detectorSlices);
    virtual asynStatus setDetectorSlices(int detectorSlices);
    virtual asynStatus getADCMode(bool & adcMode);
    virtual asynStatus setADCMode(bool adcMode);
    virtual asynStatus getADCMask(char * adcMask);
    virtual asynStatus setADCMask(char * adcMask);
    virtual asynStatus getDiscriminatorLevel(int & discriminatorLevel);
    virtual asynStatus setDiscriminatorLevel(int discriminatorLevel);
    virtual asynStatus getChannels(int & channels);
    virtual asynStatus getSlices(int & slices);
    virtual asynStatus getSweeps(int & sweeps);
    virtual asynStatus getChannelScale(double * spectrum);
    virtual asynStatus getSliceScale(double * spectrum);
    virtual asynStatus getSpectrum(double * sumData);
    virtual asynStatus getData(double * data);
    virtual bool is_excitationEnergy_allowed();
    virtual bool is_mode_allowed();
    virtual bool is_energyScale_allowed();
    virtual bool is_highEnergy_allowed();
    virtual bool is_lowEnergy_allowed();
    virtual bool is_fixEnergy_allowed();
    virtual bool is_energyStep_allowed();
    virtual bool is_stepTime_allowed();
    virtual bool is_lensMode_allowed();
    virtual bool is_passEnergy_allowed();
    virtual bool is_passMode_allowed();
    virtual bool is_detectorFirstXChannel_allowed();
    virtual bool is_detectorLastXChannel_allowed();
    virtual bool is_detectorFirstYChannel_allowed();
    virtual bool is_detectorLastYChannel_allowed();
    virtual bool is_detectorSlices_allowed();
    virtual bool is_ADCMode_allowed();
    virtual bool is_ADCMask_allowed();
    virtual bool is_discriminatorLevel_allowed();
    virtual bool is_channels_allowed();
    virtual bool is_slices_allowed();
    virtual bool is_sweeps_allowed();
    virtual bool is_channelScale_allowed();
    virtual bool is_sliceScale_allowed();
    virtual bool is_sumData_allowed();
    virtual bool is_data_allowed();
    virtual bool is_Start_allowed();
    virtual bool is_Stop_allowed();
    virtual bool is_ResetInstrument_allowed();
    virtual bool is_ResetSupplies_allowed();
    virtual bool is_TestCommunication_allowed();
    virtual bool is_GetHardwareInfo_allowed();
    virtual bool is_GetLensModeList_allowed();
    virtual bool is_GetPassEnergyList_allowed();
    virtual bool is_GetPassModeList_allowed();
    void start();
    void stop();
    void reset_instrument();
    void reset_supplies();
    void test_communication();
    void get_hardware_info();
    std::vector<string> *get_lens_mode_list();
    std::vector<double> *get_pass_energy_list();
    std::vector<string> *get_pass_mode_list();
    void get_device_property();
    BOOL isCallReturnError(int & eaStatus, const char *functionName);
    bool isError(int & err, const char * functionName);
    WError *werror;
    string sesRootDirectory;
    string instrumentFilePath;

	float m_dTemperature;
};
/* end of Electron Analyser class description */

/** A bit of C glue to make the config function available in the startup script (ioc shell) */
extern "C" int electronAnalyserConfig(const char *workingDir, int maxSizeX,
		int maxSizeY, int maxBuffers, size_t maxMemory, int priority,
		int stackSize)
{
	new ElectronAnalyser(workingDir, maxSizeX, maxSizeY, maxBuffers, maxMemory,
			priority, stackSize);
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
	lensMode = ADLastStdParam, /**< (asynInt32,    r/w) the lens mode selected*/
	acquisitionMode, /**< (asynInt32,    r/w) the acquisition mode selected*/
	ADLastDriverParam
} electronAnalyserParam_t;

/** Define a table that maps enum-string.
 * The string is used in the database to reference a certain parameter */
static asynParamString_t electronAnalyserParamString[] =
{
{ lensMode, "LENS_MODE" },
{ acquisitionMode, "ACQUISITION_MODE" }, };
// A little trick to get the number of parameters...
#define NUM_ELECTRONANALYSER_PARAMS (sizeof(electronAnalyserParamString)/sizeof(electronAnalyserParamString[0]))

ElectronAnalyser::~ElectronAnalyser()
{
}
ElectronAnalyser::ElectronAnalyser(const char *workingDir, int maxSizeX,
		int maxSizeY, int maxBuffers, size_t maxMemory, int priority,
		int stackSize) :
	ADDriver(portName, 1, ADLastDriverParam, maxBuffers, maxMemory, 0, 0, /* No interfaces beyond those set in ADDriver.cpp */
	ASYN_CANBLOCK, 1, //asynflags (CANBLOCK means separate thread for this driver)
			priority, stackSize) // thread priority and stack size (0=default)
{
	int status = asynSuccess;
	const char *functionName = "ElectronAnalyser";
	int eaStatus = WError::ERR_OK;
	char message[MAX_MESSAGE_SIZE];
	werror = WError::instance();
	// Create the epicsEvents for signaling to the Electron Analyser task when acquisition starts and stops
	this->startEventId = epicsEventCreate(epicsEventEmpty);
	if (!this->startEventId)
	{
		printf("%s:%s epicsEventCreate failure for start event\n", driverName,
				functionName);
		return;
	}
	this->stopEventId = epicsEventCreate(epicsEventEmpty);
	if (!this->stopEventId)
	{
		printf("%s:%s epicsEventCreate failure for stop event\n", driverName,
				functionName);
		return;
	}
	printf("%s:%s: Initialise SES library.\n", driverName, functionName);
	ses = new WSESWrapperMain(workingDir);
	eaStatus |= ses->setProperty("lib_working_dir", 0, workingDir);
	eaStatus |= ses->initialize(0);
	if (eaStatus != WError::ERR_OK)
	{
		printf("%s:%s: Initialising SES library failed: %s!\n", driverName,
				functionName, werror->message(eaStatus));
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
		printf("%s:%s: unable to set detector parameters\n", driverName,
				functionName);
		return;
	}

	printf("  Starting up polling task...\n");
	/* Create the thread that updates the images */
	status = (epicsThreadCreate("ElectronAnalyserTask",
			epicsThreadPriorityMedium, epicsThreadGetStackSize(
					epicsThreadStackMedium),
			(EPICSTHREADFUNC) electronAnalyserTaskC, this) == NULL);
	if (status)
	{
		printf("%s:%s epicsThreadCreate failure for image task\n", driverName,
				functionName);
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
			setStringParam(ADStatusMessage,
					"Failed to collect data from electron analyser.");
			acquire = 0;
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
		if ((imageMode == ADImageSingle) || ((imageMode == ADImageMultiple)
				&& (numImagesCounter >= numImages)))
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
asynStatus ElectronAnalyser::writeFloat64(asynUser *pasynUser,
		epicsFloat64 value)
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
asynStatus ElectronAnalyser::drvUserCreate(asynUser *pasynUser,
		const char *drvInfo, const char **pptypeName, size_t *psize)
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
	int minx, miny, sizex, sizey;
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

/**
 * @brief check if call to instrument API return error.
 *
 * If the call returns error status, it converts the status code to error message, add to Asyn log and
 * sets the Area Detector's ADStatusMessage field, update any client.
 *
 * \param[in] err - the return status code
 * \param[in] functionName - the function name where Pixium API call is made.
 * \return true if error in call return, false if err==0.
 */
bool ElectronAnalyser::isError(int & err, const char * functionName)
{
    if(err!=0){
		string msg = WError::instance()->message(err);
		this->setStringParam(ADStatusMessage,msg.c_str());
		callParamCallbacks();
		asynPrint(this->pasynUserSelf, ASYN_TRACE_ERROR, "%s:%s: %s", driverName, functionName, msg.c_str());
		return true;
	}
    return false;
}

/*!
 * device descruction.
 *
 * This method will abort any currently executing acquisition, and close the SESInstrument.dll library.
 * It will be called at device destruction or at init command.
 */
void ElectronAnalyser::delete_device()
{
	//	Delete device's allocated object
	if (ses)
	{
		ses->finalize();
		delete ses;
	}
	// delete all cached variables
	if (!sesRootDirectory.empty())
	{
		sesRootDirectory.clear();
	}
	if (!instrumentFilePath.empty())
	{
		instrumentFilePath.clear();
	}
}

/**
 * create and initialise the device annd instrument software library. Must be called in constructor.
 */
void ElectronAnalyser::init_device()
{
	const char * functionName = "init_device()";
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: create device", driverName, functionName);
	// Initialise variables to default values
	get_device_property();

	// Get connection to the SES wrapper
	ses = new WSESWrapperMain(sesRootDirectory.c_str());
	int err = ses->initialize(0);
	if (err)
	{
		this->setIntegerParam(ADStatus, ADStatusError);
		this->setStringParam(ADStatusMessage,"SES library initialisation failed");
		asynPrint(this->pasynUserSelf, ASYN_TRACE_ERROR, "%s:%s: SES library initialisation failed", driverName, functionName);
	}
	else
	{
		err = ses->loadInstrument(instrumentFilePath.c_str());
		if (err)
		{
			this->setIntegerParam(ADStatus, ADStatusError);
			char * message = new char[MAX_MESSAGE_SIZE];
			epicsSnprintf(message, sizeof(message), "LoadInstrument file : %s failed.", instrumentFilePath);
			this->setStringParam(ADStatusMessage, message);
			asynPrint(this->pasynUserSelf, ASYN_TRACE_ERROR, "%s:%s: LoadInstrument file : %s failed ", driverName, functionName, instrumentFilePath);
		}
		else
		{
			if (ses->isInitialized())
			{
				this->setIntegerParam(ADStatus, ADStatusIdle);
				this->setStringParam(ADStatusMessage,"SES library initialisation completed.");
				asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: completed.", driverName, functionName);
			}
			else
			{
				this->setIntegerParam(ADStatus, ADStatusError);
				this->setStringParam(ADStatusMessage,"SES initialisation failed");
				asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: SES nitialisation failed", driverName, functionName);
			}
		}
	}
	callParamCallbacks();
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: - out", driverName, functionName);
}

//+----------------------------------------------------------------------------
//
// method : 		ElectronAnalyser::get_device_property()
//
// description : 	Read the device properties from database.
//
//-----------------------------------------------------------------------------
void ElectronAnalyser::get_device_property()
{
	//	Initialize your default values here
	sesRootDirectory = getenv("SES_ROOT");
	instrumentFilePath = sesRootDirectory.append("\\data\\instrument.dat");
}
//+----------------------------------------------------------------------------
//
// method : 		ElectronAnalyser::always_executed_hook()
//
// description : 	method always executed before any command is executed
//
//-----------------------------------------------------------------------------
void ElectronAnalyser::always_executed_hook()
{
	const char * functionName = "always_executed_hook()";
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: entering...", driverName, functionName);
	int status=0;
	int err = ses->getProperty("instrument_status", 0, &status);
	if(err!=0)
	{
		string msg = WError::instance()->message(err);
		this->setIntegerParam(ADStatus, ADStatusError);
		this->setStringParam(ADStatusMessage, msg.c_str());
		asynPrint(this->pasynUserSelf, ASYN_TRACE_ERROR, "%s:%s: SES driver failure: %s", driverName, functionName, msg.c_str());
	}
	switch(status)
	{
	case SesNS::Normal:
		this->setIntegerParam(ADStatus, ADStatusIdle);
		this->setStringParam(ADStatusMessage,"Analyser READY.");
		asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: Analyser READY.", driverName, functionName);
		break;
	case  SesNS::Running:
		this->setIntegerParam(ADStatus, ADStatusAcquire);
		this->setStringParam(ADStatusMessage,"Analyser BUSY.");
		asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: Analyser BUSY.", driverName, functionName);
		break;
	case  SesNS::AcqError :
		this->setIntegerParam(ADStatus, ADStatusError);
		this->setStringParam(ADStatusMessage,"Acquisition was interrupted with an error.");
		asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: Acquisition was interrupted with an error.", driverName, functionName);
		break;
	case SesNS::NonOperational:
		this->setIntegerParam(ADStatus, ADStatusError);
		this->setStringParam(ADStatusMessage,"The library is not operational. Resetting may resolve the issue.");
		asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: The library is not operational. Resetting may resolve the issue.", driverName, functionName);
		break;
	case SesNS::NotInitialized :
		this->setIntegerParam(ADStatus, ADStatusError);
		this->setStringParam(ADStatusMessage,"The SESInstrument library has not been initialized (the GDS_Initialize function has not been called).");
		asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: The SESInstrument library has not been initialized (the GDS_Initialize function has not been called).", driverName, functionName);
		break;
	}

	int steps = 0;
	int dummy = 0;
	ses->getAcquiredData("acq_current_step", 0, &steps,dummy);

	int channels = 0;
	ses->getAcquiredData("acq_channels", 0, &channels,dummy);
	if(steps>channels){
		steps = channels;
	}
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: Current step: %d", driverName, functionName, steps);
	this->callParamCallbacks();
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: exit.", driverName, functionName);
}
//+----------------------------------------------------------------------------
//
// method : 		ElectronAnalyser::read_attr_hardware
//
// description : 	Hardware acquisition for attributes.
//
//-----------------------------------------------------------------------------
void ElectronAnalyser::read_attr_hardware(std::vector<long> &attr_list)
{
	const char * functionName = "read_attr_hardware(vector<long> &attr_list)";
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: entering...", driverName, functionName);
	//	Add your own code here
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: exit.", driverName, functionName);
}

/*!
 * Reads the current kinetic energy from the SESInstrument library.
 *
 * @param[out] kineticEnergy Pointer to a double to be modified with the current kinetic energy.
 * @return asynError if kinetic energy can not be read, otherwise asynSuccess
 */
asynStatus ElectronAnalyser::getKineticEnergy(double &kineticEnergy)
{
	const char * functionName = "read_excitationEnergy(double &kineticEnergy)";
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: entering...", driverName, functionName);
	int err = ses->getKineticEnergy(&kineticEnergy);
    if(isError(err, functionName)) return asynError;
    asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: exit.", driverName, functionName);
	return asynSuccess;
}

/*!
 * Changes the kinetic energy. Use this member function when initAcquisition() and startAcquisition() is not
 * going to be called, e.g. when controlling the Scienta analyzer with an external, third-party detector to make
 * measurements.
 *
 * @param[in] kineticEnergy The kinetic energy (in eV) to be set.
 * @return asynError if kinetic energy can not be read, otherwise asynSuccess
 */
asynStatus ElectronAnalyser::setKineticEnergy(double kineticEnergy)
{
	const char * functionName = "setKineticEnergy(double kineticEnergy)";
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: entering...", driverName, functionName);
	int err =  ses->setKineticEnergy(kineticEnergy);
    if(isError(err, functionName)) return asynError;
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: exit.", driverName, functionName);
	return asynSuccess;
}
/**
 * gets the analyser mode in which the region is measured. fixed (@c true) or swept (@c false) mode.
 * This is a one-byte boolean. The value can be set to 1 (for @c true) or 0 (for @c false).
 * @param [out] mode - fixed (@c true) or swept (@c false) mode.
 * @return always asynSuccess
 */
asynStatus ElectronAnalyser::getAnalyserMode(bool &mode)
{
	const char * functionName = "getAnalyserMode(bool &mode)";
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: entering...", driverName, functionName);
	if(analyzer.fixed_)
	{
		mode= true;
	}
	else
	{
		mode = false;
	}
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: exit.", driverName, functionName);
	return asynSuccess;
}
/**
 * sets the analyser mode so that the region will be measured in fixed (@c true) or swept (@c false) mode.
 * This is a one-byte boolean. The value can be set to 1 (for @c true) or 0 (for @c false).
 * @param [in] mode - fixed (@c true) or swept (@c false)
 * @return always asynSuccess
 */
asynStatus ElectronAnalyser::setAnalyserMode(bool mode)
{
	const char * functionName = "setAnalyserMode(bool mode)";
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: entering...", driverName, functionName);
	if(mode)
	{
		analyzer.fixed_ = true;
	}
	else
	{
		analyzer.fixed_ = false;
	}
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: exit.", driverName, functionName);
	return asynSuccess;
}
/**
 * gets energy scale from hardware
 * @param energyScale
 * @return always asynSuccess
 */
asynStatus ElectronAnalyser::getEnergyScale(string &energyScale)
{
	const char * functionName = "setMode(string mode)";
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: entering...", driverName, functionName);
	//FIXME: waiting for availabilty in wrapper see MANTIS 15320
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: Not implemented within the GammaData wrapper.", driverName, functionName);

	/*string	attr_energyScale;
	if(){
	attr_energyScale = "Kinetic";
	}
	else{
	attr_energyScale = "Binding";
	}

	attr.set_value(&attr_energyScale);*/
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: exit.", driverName, functionName);
	return asynSuccess;
}
/**
 * sets the energy scale to hardware.
 *
 * @param [in] energyScale - Kinetic or Binding
 * @return always asynSuccess
 */
asynStatus ElectronAnalyser::setEnergyScale(string energyScale)
{
	const char * functionName = "setEnergyScale(string energyScale)";
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: entering...", driverName, functionName);
	//FIXME: waiting for availabilty in wrapper see MANTIS 15320
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: Not implemented within the GammaData wrapper.", driverName, functionName);
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: exit.", driverName, functionName);
	return asynSuccess;
}
/**
 * gets the high-end kinetic energy (eV) for swept mode acquisition.
 * @param [out] highEnergy
 * @return always asynSuccess
 */
asynStatus ElectronAnalyser::getHighEnergy(double &highEnergy)
{
	const char * functionName = "getHighEnergy(double &highEnergy)";
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: entering...", driverName, functionName);
	highEnergy = analyzer.highEnergy_;
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: exit.", driverName, functionName);
	return asynSuccess;
}

/**
 * sets the high-end kinetic energy (eV) for swept mode acquisition.
 *
 * @param [in] highEnergy
 * @return always asynSuccess
 */
asynStatus ElectronAnalyser::setHighEnergy(double highEnergy)
{
	const char * functionName = "setHighEnergy(double highEnergy)";
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: entering...", driverName, functionName);
	analyzer.highEnergy_ = highEnergy;
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: exit.", driverName, functionName);
	return asynSuccess;
}
/**
 * gets the low-end kinetic energy (eV) for swept mode acquisition.
 * @param [out] lowEnergy
 * @return always asynSuccess
 */
asynStatus ElectronAnalyser::getLowEnergy(double &lowEnergy)
{
	const char * functionName = "getLowEnergy(double &lowEnergy)";
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: entering...", driverName, functionName);
	lowEnergy = analyzer.lowEnergy_;
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: exit.", driverName, functionName);
	return asynSuccess;
}
/**
 * sets the low-end kinetic energy (eV) for swept mode acquisition.
 * @param [in] lowEnergy
 * @return always asynSuccess
 */
asynStatus ElectronAnalyser::setLowEnergy(double lowEnergy)
{
	const char * functionName = "setLowEnergy(double lowEnergy)";
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: entering...", driverName, functionName);
	analyzer.lowEnergy_ = lowEnergy;
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: exit.", driverName, functionName);
	return asynSuccess;
}
/**
 * gets the center energy (eV) for fixed mode acquisition
 * (the low and high end energies is calculated from this value and the current pass energy).
 * @param [out] centreEnergy
 * @return always asynSuccess
 */
asynStatus ElectronAnalyser::getCenterEnergy(double &centreEnergy)
{
	const char * functionName = "getCenterEnergy(double &centreEnergy)";
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: entering...", driverName, functionName);
	centreEnergy = analyzer.centerEnergy_;
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: exit.", driverName, functionName);
	return asynSuccess;
}
/**
 * sets the center energy (eV) for fixed mode acquisition
 * (the low and high end energies is calculated from this value and the current pass energy).
 * @param [in] centreEnergy
 * @return always asynSuccess
 */
asynStatus ElectronAnalyser::setCenterEnergy(double centreEnergy)
{
	const char * functionName = "setCenterEnergy(double centreEnergy)";
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: entering...", driverName, functionName);
	analyzer.centerEnergy_ = centreEnergy;
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: exit.", driverName, functionName);
	return asynSuccess;
}
/**
 * gets the energy step size (eV) for swept mode acquisition.
 * @param [out] energyStep
 * @return always asynSuccess
 */
asynStatus ElectronAnalyser::getEnergyStep(double &energyStep)
{
	const char * functionName = "getEnergyStep(double &energyStep)";
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: entering...", driverName, functionName);
	energyStep = analyzer.energyStep_;
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: exit.", driverName, functionName);
	return asynSuccess;
}
/**
 * sets the energy step size (eV) for swept mode acquisition.
 * @param [in] energyStep
 * @return always asynSuccess
 */
asynStatus ElectronAnalyser::setEnergyStep(double energyStep)
{
	const char * functionName = "setEnergyStep(double energyStep)";
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: entering...", driverName, functionName);
	analyzer.energyStep_ = energyStep;
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: exit.", driverName, functionName);
	return asynSuccess;
}
/**
 * gets the dwell time (ms) for fixed or swept mode acquisition.
 * @param [out] stepTime
 * @return always asynSuccess
 */
asynStatus ElectronAnalyser::getStepTime(int &stepTime)
{
	const char * functionName = "getStepTime(int &stepTime)";
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: entering...", driverName, functionName);
	stepTime = analyzer.dwellTime_;
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: exit.", driverName, functionName);
	return asynSuccess;
}
/**
 * sets the dwell time (ms) for fixed or swept mode acquisition.
 * @param [in] stepTime
 * @return always asynSuccess
 */
asynStatus ElectronAnalyser::setStepTime(int stepTime)
{
	const char * functionName = "setStepTime(int stepTime)";
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: entering...", driverName, functionName);
	analyzer.dwellTime_ = stepTime;
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: exit.", driverName, functionName);
	return asynSuccess;
}
/**
 * gets the current lens mode value.
 * @param [out] lensMode
 * @return asynError if lens mode can not be read, otherwise asynSuccess
 */
asynStatus ElectronAnalyser::getLensMode(char * lensMode)
{
	const char * functionName = "getLensMode(char * lensMode)";
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: entering...", driverName, functionName);
	char* myLensMode =0;
	int size =0;
	int err = ses->getProperty("lens_mode", 0, myLensMode, size);
	if(isError(err, functionName)){
		return asynError;
	}
	myLensMode = new char[size];
	err = ses->getProperty("lens_mode", 0, myLensMode, size);
	if(isError(err, functionName)){
		delete [] myLensMode;
		return asynError;
	}
	strncpy(lensMode ,myLensMode, size);
	delete [] myLensMode;
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: exit.", driverName, functionName);
	return asynSuccess;
}
/**
 * sets the lens mode for next acquisition.
 * @param [in] lensMode
 * @return asynError if change lens mode fails, otherwise asynSuccess
 */
asynStatus ElectronAnalyser::setLensMode(char * lensMode)
{
	const char * functionName = "setLensMode(char * lensMode)";
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: entering...", driverName, functionName);
	int err = ses->setProperty("lens_mode", 0, lensMode);
	if(isError(err, functionName)){
		return asynError;
	}
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: exit.", driverName, functionName);
	return asynSuccess;
}
/**
 * gets the current pass energy value.
 * @param [out] passEnergy
 * @return asynError if pass energy can not be read, otherwise asynSuccess
 */
asynStatus ElectronAnalyser::getPassEnergy(double &passEnergy)
{
	const char * functionName = "getPassEnergy(double &passEnergy)";
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: entering...", driverName, functionName);
	int err = ses->getProperty("pass_energy", 0, &passEnergy);
	if(isError(err, functionName)){
		return asynError;
	}
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: exit.", driverName, functionName);
	return asynSuccess;
}
/**
 * sets the pass energy for next acquisition.
 * @param [in] passEnergy
 * @return asynError if changing pass energy fails, otherwise asynSuccess
 */
asynStatus ElectronAnalyser::setPassEnergy(double passEnergy)
{
	const char * functionName = "setPassEnergy(double passEnergy)";
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: entering...", driverName, functionName);
	int err = ses->setProperty("pass_energy", 0,&passEnergy);
	if(isError(err, functionName)){
		return asynError;
	}
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: exit.", driverName, functionName);
	return asynSuccess;
}
/**
 * gets the current element set or pass mode.
 * @param [out] elementSet
 * @return asynError if element set can not be read, otherwise asynSuccess
 */
asynStatus ElectronAnalyser::getElementSet(char * elementSet)
{
	const char * functionName = "getElementSet(char * elementSet)";
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: entering...", driverName, functionName);
	char* passMode =0;
	int size =0;
	int err = ses->getProperty("element_set", 0, passMode, size);
	if(isError(err, functionName)){
		return asynError;
	}
	passMode = new char[size];
	err = ses->getProperty("element_set", 0, passMode, size);
	if(isError(err, functionName)){
		delete [] passMode;
		return asynError;
	}
	strncpy(elementSet ,passMode, size);
	delete [] passMode;
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: exit.", driverName, functionName);
	return asynSuccess;
}
/**
 * sets the element set for next acquisition.
 * @param elementSet
 * @return asynError if changing element set fails, otherwise asynSuccess
 */
asynStatus ElectronAnalyser::setElementSet(char * elementSet)
{
	const char * functionName = "setElementSet(char * elementSet)";
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: entering...", driverName, functionName);
	int err = ses->setProperty("element_set", 0, elementSet);
	if(isError(err, functionName)){
		return asynError;
	}
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: exit.", driverName, functionName);
	return asynSuccess;
}
/**
 * gets the first X channel to be used on the detector (range: <code>[0...@ref WDetectorInfo::xChannels_-1]</code>. )
 * @param [out] dfx
 * @return always asynSuccess
 */
asynStatus ElectronAnalyser::getDetectorFirstXChannel(int &dfx)
{
	const char * functionName = "getDetectorFirstXChannel(int &dfx)";
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: entering...", driverName, functionName);
	dfx = detector.firstXChannel_;
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: exit.", driverName, functionName);
	return asynSuccess;
}
/**
 * sets the first X channel to be used on the detector (range: <code>[0...@ref WDetectorInfo::xChannels_-1]</code>. )
 * @param [in]
 * @return always asynSuccess
 */
asynStatus ElectronAnalyser::setDetectorFirstXChannel(int dfx)
{
	const char * functionName = "setDetectorFirstXChannel(int dfx)";
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: entering...", driverName, functionName);
	detector.firstXChannel_ = dfx;
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: exit.", driverName, functionName);
	return asynSuccess;
}
/**
 * gets the last X channel to be used on the detector.
 * @param [out] dlx
 * @return always asynSuccess
 */
asynStatus ElectronAnalyser::getDetectorLastXChannel(int &dlx)
{
	const char * functionName = "getDetectorLastXChannel(int &dlx)";
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: entering...", driverName, functionName);
	dlx = detector.lastXChannel_;
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: exit.", driverName, functionName);
	return asynSuccess;
}
/**
 * sets the last X channel to be used on the detector.
 * @param [in] dlx
 * @return always asynSuccess
 */
asynStatus ElectronAnalyser::setDetectorLastXChannel(int dlx)
{
	const char * functionName = "setDetectorLastXChannel(int dlx)";
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: entering...", driverName, functionName);
	detector.lastXChannel_ = dlx;
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: exit.", driverName, functionName);
	return asynSuccess;
}
/**
 * gets the first Y channel to be used on the detector (range: <code>[0...@ref WDetectorInfo::yChannels_-1]</code>.
 * @param [out] dfy
 * @return always asynSuccess
 */
asynStatus ElectronAnalyser::getDetectorFirstYChannel(int &dfy)
{
	const char * functionName = "getDetectorFirstYChannel(int &dfy)";
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: entering...", driverName, functionName);
	dfy = detector.firstYChannel_;
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: exit.", driverName, functionName);
	return asynSuccess;
}
/**
 * sets the first Y channel to be used on the detector (range: <code>[0...@ref WDetectorInfo::yChannels_-1]</code>.
 * @param [in] dfy
 * @return always asynSuccess
 */
asynStatus ElectronAnalyser::setDetectorFirstYChannel(int dfy)
{
	const char * functionName = "setDetectorFirstYChannel(int dfy)";
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: entering...", driverName, functionName);
	detector.firstYChannel_ = dfy;
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: exit.", driverName, functionName);
	return asynSuccess;
}
/**
 * gets the last Y channel to be used on the detector.
 * @param [out] dly
 * @return always asynSuccess
 */
asynStatus ElectronAnalyser::getDetectorLastYChannel(int &dly)
{
	const char * functionName = "getDetectorLastYChannel(int &dly)";
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: entering...", driverName, functionName);
	dly = detector.lastYChannel_;
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: exit.", driverName, functionName);
	return asynSuccess;
}
/**
 * sets the last Y channel to be used on the detector.
 * @param [in] dly
 * @return always asynSuccess
 */
asynStatus ElectronAnalyser::setDetectorLastYChannel(int dly)
{
	const char * functionName = "setDetectorLastYChannel(int dly)";
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: entering...", driverName, functionName);
	detector.lastYChannel_ = dly;
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: exit.", driverName, functionName);
	return asynSuccess;
}
/**
 * gets the current number of Y channels (slices).
 * @param [out] detectorSlices
 * @return always asynSuccess
 */
asynStatus ElectronAnalyser::getDetectorSlices(int &detectorSlices)
{
	const char * functionName = "getDetectorSlices(int &detectorSlices)";
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: entering...", driverName, functionName);
	detectorSlices = detector.slices_;
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: exit.", driverName, functionName);
	return asynSuccess;
}
/**
 * sets the current number of Y channels (slices).
 * @param [in] detectorSlices
 * @return always asynSuccess
 */
asynStatus ElectronAnalyser::setDetectorSlices(int detectorSlices)
{
	const char * functionName = "setDetectorSlices(int detectorSlices)";
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: entering...", driverName, functionName);
	detector.slices_ = detectorSlices;
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: exit.", driverName, functionName);
	return asynSuccess;
}
/**
 * gets the detector running mode: ADC mode (@c true), or Pulse Counting mode (@c false).
 * @param [out] mode
 * @return always asynSuccess
 */
asynStatus ElectronAnalyser::getADCMode(bool &mode)
{
	const char * functionName = "getADCMode(bool &mode)";
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: entering...", driverName, functionName);
	if(detector.adcMode_)
	{
		mode= true;
	}
	else
	{
		mode = false;
	}
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: exit.", driverName, functionName);
	return asynSuccess;
}
/**
 * Specifies whether the detector is running in ADC mode (@c true), or Pulse Counting mode (@c false).
 * @param [in] mode
 * @return always asynSuccess
 */
asynStatus ElectronAnalyser::setADCMode(bool mode)
{
	const char * functionName = "setADCMode(bool mode)";
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: entering...", driverName, functionName);
	if(mode)
	{
		detector.adcMode_ = true;
	}
	else
	{
		detector.adcMode_ = false;
	}
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: exit.", driverName, functionName);
	return asynSuccess;
}
asynStatus ElectronAnalyser::getADCMask(char * mask)
{
	const char * functionName = "getADCMask(char * mask)";
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: entering...", driverName, functionName);
	//FIXME: waiting for availabilty in wrapper see MANTIS 15320
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: Not implemented within the GammaData wrapper.", driverName, functionName);
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: exit.", driverName, functionName);
	return asynSuccess;
}
asynStatus ElectronAnalyser::setADCMask(char * mask)
{
	const char * functionName = "setADCMask(char * mask)";
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: entering...", driverName, functionName);
	//FIXME: waiting for availabilty in wrapper see MANTIS 15320
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: Not implemented within the GammaData wrapper.", driverName, functionName);
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: exit.", driverName, functionName);
	return asynSuccess;
}
asynStatus ElectronAnalyser::getDiscriminatorLevel(int &level)
{
	const char * functionName = "getDiscriminatorLevel(int &level)";
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: entering...", driverName, functionName);
	//FIXME: waiting for availabilty in wrapper see MANTIS 15320
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: Not implemented within the GammaData wrapper.", driverName, functionName);
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: exit.", driverName, functionName);
	return asynSuccess;
}
asynStatus ElectronAnalyser::setDiscriminatorLevel(int level)
{
	const char * functionName = "setDiscriminatorLevel(int level)";
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: entering...", driverName, functionName);
	//FIXME: waiting for availabilty in wrapper see MANTIS 15320
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: Not implemented within the GammaData wrapper.", driverName, functionName);
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: exit.", driverName, functionName);
	return asynSuccess;
}
/**
 * geta the number of channels in acquired data.
 * @param [out] channels
 * @return asynError if acq_channels can not be read, otherwise asynSuccess
 */
asynStatus ElectronAnalyser::getChannels(int & channels)
{
	const char * functionName = "getChannels(int & channels)";
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: entering...", driverName, functionName);
	int dummy = 0;
	int err = ses->getAcquiredData("acq_channels", 0, &channels,dummy);
	if (isError(err, functionName)) {
		return asynError;
	}
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: exit.", driverName, functionName);
	return asynSuccess;
}
/**
 * gets the number of slices in acquired data.
 * @param [out] slices
 * @return asynError if acq_slices can not be read, otherwise asynSuccess
 */
asynStatus ElectronAnalyser::getSlices(int &slices)
{
	const char * functionName = "getSlices(int &slices)";
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: entering...", driverName, functionName);
	int dummy = 0;
	int err = ses->getAcquiredData("acq_slices", 0, &slices,dummy);
	if (isError(err, functionName)) {
		return asynError;
	}
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: exit.", driverName, functionName);
	return asynSuccess;
}
/**
 * gets the number of calls to startAcquisition() iterations that have passed since last call to initAcquisition().
 * @param [out] sweeps
 * @return asynError if acq_iteractions can not be read, otherwise asynSuccess
 */
asynStatus ElectronAnalyser::getSweeps(int &sweeps)
{
	const char * functionName = "getSweeps(int &sweeps)";
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: entering...", driverName, functionName);
	int dummy = 0;
	int err = ses->getAcquiredData("acq_iterations", 0, &sweeps,dummy);
	if (isError(err, functionName)) {
		return asynError;
	}
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: exit.", driverName, functionName);
	return asynSuccess;
}
/**
 * gets the channel scale
 * @param [out] channelScale - channel scale array
 * @return
 */
asynStatus ElectronAnalyser::getChannelScale(double * spectrum)
{
	const char * functionName = "getChannelScale(double * spectrum)";
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: entering...", driverName, functionName);
	int channels = 0;
	int dummy = 0;
	int err = ses->getAcquiredData("acq_channels", 0, &channels,dummy);
	if (isError(err, functionName)) {
		return asynError;
	}
	err = ses->getAcquiredData("acq_channel_scale", 0, spectrum, channels);
	if (isError(err, functionName)) {
		return asynError;
	}
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: exit.", driverName, functionName);
	return asynSuccess;
}
/**
 * gets the slice scale.
 * @param [out] sliceScale - slice scale array
 * @return
 */
asynStatus ElectronAnalyser::getSliceScale(double * spectrum)
{
	const char * functionName = "getSliceScale(double * spectrum)";
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: entering...", driverName, functionName);
	int dummy = 0;
	int slices = 0;
	int err = ses->getAcquiredData("acq_slices", 0, &slices,dummy);
	if (isError(err, functionName)) {
		return asynError;
	}

	err = ses->getAcquiredData("acq_slice_scale", 0, spectrum, slices);
	if (isError(err, functionName)) {
		return asynError;
	}

	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: exit.", driverName, functionName);
	return asynSuccess;
}
/**
 * gets the integrated spectrum.
 * @param [out] sumData - the integrated spectrum
 * @return
 */
asynStatus ElectronAnalyser::getSpectrum(double * sumData)
{
	const char * functionName = "getSpectrum(double * sumData)";
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: entering...", driverName, functionName);
	int channels = 0;
	int dummy = 0;
	int err = ses->getAcquiredData("acq_channels", 0, &channels,dummy);
	if (isError(err, functionName)) {
		return asynError;
	}

	err = ses->getAcquiredData("acq_spectrum", 0, sumData, channels);
	if (isError(err, functionName)) {
		return asynError;
	}

	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: exit.", driverName, functionName);
	return asynSuccess;
}
/**
 * gets the 2D matrix of acquired data  - image
 * @param [out] data -image
 * @return
 */
asynStatus ElectronAnalyser::getData(double * data)
{
	const char * functionName = "getData(double * data)";
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: entering...", driverName, functionName);
	int channels = 0;
	int dummy = 0;
	int err = ses->getAcquiredData("acq_channels", 0, &channels,dummy);
	if (isError(err, functionName)) {
		return asynError;
	}
	int slices = 0;
	err = ses->getAcquiredData("acq_slices", 0, &slices,dummy);
	if (isError(err, functionName)) {
		return asynError;
	}
	int size = slices*channels;
	err = ses->getAcquiredData("acq_image", 0, data, size);
	if (isError(err, functionName)) {
		return asynError;
	}
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: exit.", driverName, functionName);
	return asynSuccess;
}



