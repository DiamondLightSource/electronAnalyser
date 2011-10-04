#pragma warning(disable:4996)

#include "wseswrappermain.h"
#include "wsesinstrument.h"
#include "constants.h"
#include "common.hpp"
#include "werror.h"

#include <direct.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <algorithm>

#include <ctime>

using namespace SESWrapperNS;
using namespace std;

WSESWrapperMain *WSESWrapperMain::this_ = 0;

/*!
 * Creates a WSESWrapperMain instance.
 *
 * @param[in] workingDir The current working directory
 */
WSESWrapperMain::WSESWrapperMain(const char *workingDir)
: WSESWrapperBase(workingDir), initialized_(false)
{
  this_ = this;
  // Create data parameter database
  dataParameters_.insert(DataParameterKeyValue("acq_channels", DataParameter(this, &WSESWrapperMain::readOnlyStub, &WSESWrapperMain::getAcqChannels, DataParameter::TYPE_INT32)));
  dataParameters_.insert(DataParameterKeyValue("acq_slices", DataParameter(this, &WSESWrapperMain::readOnlyStub, &WSESWrapperMain::getAcqSlices, DataParameter::TYPE_INT32)));
  dataParameters_.insert(DataParameterKeyValue("acq_iterations", DataParameter(this, &WSESWrapperMain::readOnlyStub, &WSESWrapperMain::getAcqIterations, DataParameter::TYPE_INT32)));
  dataParameters_.insert(DataParameterKeyValue("acq_intensity_unit", DataParameter(this, &WSESWrapperMain::readOnlyStub, &WSESWrapperMain::getAcqIntensityUnit, DataParameter::TYPE_STRING)));
  dataParameters_.insert(DataParameterKeyValue("acq_channel_unit", DataParameter(this, &WSESWrapperMain::readOnlyStub, &WSESWrapperMain::getAcqChannelUnit, DataParameter::TYPE_STRING)));
  dataParameters_.insert(DataParameterKeyValue("acq_slice_unit", DataParameter(this, &WSESWrapperMain::readOnlyStub, &WSESWrapperMain::getAcqSliceUnit, DataParameter::TYPE_STRING)));
  dataParameters_.insert(DataParameterKeyValue("acq_spectrum", DataParameter(this, &WSESWrapperMain::readOnlyStub, &WSESWrapperMain::getAcqSpectrum, DataParameter::TYPE_VECTOR_DOUBLE)));
  dataParameters_.insert(DataParameterKeyValue("acq_image", DataParameter(this, &WSESWrapperMain::readOnlyStub, &WSESWrapperMain::getAcqImage, DataParameter::TYPE_VECTOR_DOUBLE)));
  dataParameters_.insert(DataParameterKeyValue("acq_slice", DataParameter(this, &WSESWrapperMain::readOnlyStub, &WSESWrapperMain::getAcqSlice, DataParameter::TYPE_VECTOR_DOUBLE)));
  dataParameters_.insert(DataParameterKeyValue("acq_channel_scale", DataParameter(this, &WSESWrapperMain::readOnlyStub, &WSESWrapperMain::getAcqChannelScale, DataParameter::TYPE_VECTOR_DOUBLE)));
  dataParameters_.insert(DataParameterKeyValue("acq_slice_scale", DataParameter(this, &WSESWrapperMain::readOnlyStub, &WSESWrapperMain::getAcqSliceScale, DataParameter::TYPE_VECTOR_DOUBLE)));
  dataParameters_.insert(DataParameterKeyValue("acq_raw_image", DataParameter(this, &WSESWrapperMain::readOnlyStub, &WSESWrapperMain::getAcqRawImage, DataParameter::TYPE_VECTOR_INT32)));
  dataParameters_.insert(DataParameterKeyValue("acq_current_step", DataParameter(this, &WSESWrapperMain::readOnlyStub, &WSESWrapperMain::getAcqCurrentStep, DataParameter::TYPE_INT32)));
  dataParameters_.insert(DataParameterKeyValue("acq_elapsed_time", DataParameter(this, &WSESWrapperMain::readOnlyStub, &WSESWrapperMain::getAcqElapsedTime, DataParameter::TYPE_DOUBLE)));
  dataParameters_.insert(DataParameterKeyValue("acq_io_ports", DataParameter(this, &WSESWrapperMain::readOnlyStub, &WSESWrapperMain::getAcqIOPorts, DataParameter::TYPE_INT32)));
  dataParameters_.insert(DataParameterKeyValue("acq_io_size", DataParameter(this, &WSESWrapperMain::readOnlyStub, &WSESWrapperMain::getAcqIOSize, DataParameter::TYPE_INT32)));
  dataParameters_.insert(DataParameterKeyValue("acq_io_iterations", DataParameter(this, &WSESWrapperMain::readOnlyStub, &WSESWrapperMain::getAcqIterations, DataParameter::TYPE_INT32)));
  dataParameters_.insert(DataParameterKeyValue("acq_io_unit", DataParameter(this, &WSESWrapperMain::readOnlyStub, &WSESWrapperMain::getAcqIOUnit, DataParameter::TYPE_STRING)));
  dataParameters_.insert(DataParameterKeyValue("acq_io_scale", DataParameter(this, &WSESWrapperMain::readOnlyStub, &WSESWrapperMain::getAcqIOScale, DataParameter::TYPE_VECTOR_DOUBLE)));
  dataParameters_.insert(DataParameterKeyValue("acq_io_spectrum", DataParameter(this, &WSESWrapperMain::readOnlyStub, &WSESWrapperMain::getAcqIOSpectrum, DataParameter::TYPE_VECTOR_DOUBLE)));
  dataParameters_.insert(DataParameterKeyValue("acq_io_data", DataParameter(this, &WSESWrapperMain::readOnlyStub, &WSESWrapperMain::getAcqIOData, DataParameter::TYPE_VECTOR_DOUBLE)));
  dataParameters_.insert(DataParameterKeyValue("acq_io_port_name", DataParameter(this, &WSESWrapperMain::readOnlyStub, &WSESWrapperMain::getAcqIOPortName, DataParameter::TYPE_STRING)));

  if (!workingDir_.empty())
  {
    char *tmpDir = _getcwd(0, 0);
    _chdir(workingDir_.c_str());
    instrumentLoaded_ = lib_->load("dll\\SESInstrument.dll");
    _chdir(tmpDir);
    free(tmpDir);
  }
}

/*!
 * Destroys a WSESWrapperMain object. Closing the library usually triggers a call to this destructor.
 * It is recommended to call finalize() before closing, to ensure this is done in the correct
 * context. It has been noted that applications freeze sometimes if this has not been done.
 *
 * @see finalize()
 */
WSESWrapperMain::~WSESWrapperMain()
{
  if (lib_->isLoaded())
    lib_->unload();
}

/*!
 * This method can be used to get information about the initialization status of SESWrapper.
 *
 * @return <code>true</code> if the library is initialized.
 *
 * @see initialize()
 */
bool WSESWrapperMain::isInitialized()
{
  return initialized_;
}

/*!
 * Opens the SESInstrument.dll library and imports the functions of that library.
 *
 * When SESWrapper loads, it tries to load the SESInstrument library and run GDS_Initialize automatically.
 * If successful, initialize() should not be called. However, if the SESInstrument library was not found during
 * startup, the user needs to change the working directory to the SES software (using the @c lib_working_dir
 * property) and then call initialize(). In that case, the user assumes the responsibility for calling the
 * finalize() member function as well.
 *
 * @param[in,out] reserved This parameter is reserved for future use. It must be set to 0.
 *
 * @return WError::ERR_FAIL if initialization failed, otherwise WError::ERR_OK.
 *
 * @see finalize()
 */
int WSESWrapperMain::initialize(void *reserved)
{
  int errorCode = WError::ERR_OK;

  if (initialized_)
    return WError::ERR_OK;

  //instrumentLoaded_ = false;

  char *tmpDir = _getcwd(0, 0);
  _chdir(workingDir_.c_str());

  if (instrumentLoaded_ == false)
	errorCode = WError::ERR_FAIL;

  /*if (!lib_->load("dll\\SESInstrument.dll"))
    errorCode = WError::ERR_FAIL;*/

  if (errorCode == WError::ERR_OK && lib_->GDS_Initialize(errorNotify, reinterpret_cast<HWND>(reserved)) != 0)
    errorCode = WError::ERR_FAIL;

  _chdir(tmpDir);
  free(tmpDir);

  initialized_ = (errorCode == WError::ERR_OK);
  return errorCode;
}

/*!
 * Aborts any currently executing acquisition, and closes the SESInstrument.dll library.
 *
 * If initialization of SESWrapper was made by a call to initialize(), this member function must be called before
 * closing the library. If SESWrapper successfully made the initialization on startup, this member function does not
 * need to be called, as that is then done automatically when the library closes.
 *
 * @return Always returns WError::ERR_OK.
 *
 * @see initialize()
 */
int WSESWrapperMain::finalize()
{
  instrumentLoaded_ = false;
  initialized_ = false;
  
  if (lib_->isLoaded())
    lib_->GDS_Finalize();
  
  return WError::ERR_OK;
}

/*!
 * This is a generic function for obtaining the value of a property. The @p value parameter is
 * declared <code>void *</code> in order to allow any type of property to be accessed. The main
 * requirement for all types of properties is that the @p value parameter is a pointer to the object
 * containing the data of the property. All properties are defined with a getter and a setter in the WSESWrapperBase
 * class.
 *
 * @param[in] property A string specifying the property to be queried.
 * @param[in] index For those properties that are stored in arrays, this will indicate which array element
 *              is requested.
 * @param[out] value A pointer to a variable that will be given the requested property.
 * @param[in,out] size This parameter is used to get the size of @p value. In some cases (e.g. when the property
 *                     is an array or string) this parameter specifies the number of elements. When @p value
 *                     is 0 (NULL), @p size is modified to the required size of arrays/strings. When the property
 *                     is a structure or number, @p size if not modified, and @p value cannot be 0.
 *
 * @return WError::ERR_PARAMETER_NOT_FOUND if the @p property was not a valid variable name, other wise the
 *         return code is dependent on the type of variable.
 *
 * @see WSESWrapperBase,
 */
int WSESWrapperMain::getProperty(const char *property, int index, void *value, int &size)
{
  PropertyMap::iterator it = properties_.find(property);
  if (it == properties_.end())
    return WError::ERR_PARAMETER_NOT_FOUND;
  return it->second.get(index, value, size);
}

/*!
 * This is a convenience member function that can be used when the @p size parameter is not required.
 *
 * @see getProperty(const char *property, int index, void *value, int &size)
 */
int WSESWrapperMain::getProperty(const char *name, int index, void *value)
{
  int size = 0;
  return getProperty(name, index, value, size);
}

/*!
 * This is a generic function for modifying the value of a property. The @p value parameter is
 * declared <code>const void *</code> in order to allow any type of property to be accessed.
 *
 * @param[in] property A string specifying the property that will be modified.
 * @param[in] reserved This parameter is currently not used, but is reserved for future implementations.
 *                     Always set this to 0.
 * @param[in] value A pointer to the value of the property that will be modified.
 *
 * @return An error code, as defined by the corresponding setter. If the property is not found, a
 *         WError::ERR_PARAMETER_NOT_FOUND code is returned.
 *
 * @see WSESWrapperBase,
 */
int WSESWrapperMain::setProperty(const char *property, int reserved, const void *value)
{
  PropertyMap::iterator it = properties_.find(property);
  if (it == properties_.end())
    return WError::ERR_PARAMETER_NOT_FOUND;
  return it->second.set(reserved, value);
}

/*!
 * Validates the element set, lens mode, pass energy and kinetic energy (the kinetic energy is currently not checked and will thus always
 * result in success). This member function will return WError::ERR_OK if the supplied combination of @p elementSet, @p lensMode, @p passEnergy
 * and @p kineticEnergy is valid.
 *
 * @param[in] elementSet The element set to be validated.
 * @param[in] lensMode The lens mode to be validated.
 * @param[in] passEnergy The pass energy to be validated.
 * @param[in] kineticEnergy The kinetic energy to be validated. Currently not checked.
 *
 * @return WError::ERR_NO_INSTRUMENT if no instrument is loaded, WError::ERR_INCORRECT_ELEMENT_SET if the given element set is invalid,
 * WError::ERR_INCORRECT_LENS_MODE is the given lens mode is invalid, WError::ERR_INCORRECT_PASS_ENERGY if the given pass energy is not
 * available for the given lens mode. If the validation is successful, returns WError::ERR_OK.
 */
int WSESWrapperMain::validate(const char *elementSet, const char *lensMode, double passEnergy, double kineticEnergy)
{
  if (!instrumentLoaded_)
    return WError::ERR_NO_INSTRUMENT;

  int i = 0; 

  if (std::find(elementSets_.begin(), elementSets_.end(), elementSet) == elementSets_.end())
    return WError::ERR_INCORRECT_ELEMENT_SET;

  if (std::find(lensModes_.begin(), lensModes_.end(), lensMode) == lensModes_.end())
    return WError::ERR_INCORRECT_LENS_MODE;

  DoubleVector passEnergies;
  loadPassEnergies(lensMode, passEnergies);
  if (std::find(passEnergies.begin(), passEnergies.end(), passEnergy) == passEnergies.end())
    return WError::ERR_INCORRECT_PASS_ENERGY;

  return WError::ERR_OK;
}

// HW specific functions

/*!
 * Resets the instrument and puts it in a default state.
 *
 * @return WError::ERR_FAIL if not successful, otherwise WError::ERR_OK.
 */
int WSESWrapperMain::resetHW()
{
  stopAcquisition();
  return lib_->GDS_ResetInstrument() == 0 ? WError::ERR_OK : WError::ERR_FAIL;
}

/*!
 * Tests the hardware to check communication status.
 *
 * @return WError::ERR_FAIL if not successful, otherwise WError::ERR_OK.
 */
int WSESWrapperMain::testHW()
{
  return lib_->GDS_TestCommunication() == 0 ? WError::ERR_OK : WError::ERR_FAIL;
}

// Analyzer specific members

/*!
 * Loads an instrument configuration file. The SESInstrument library must have been initialized
 * before calling this member function.
 *
 * @param[in] fileName The name of the configuration file. It is possible to use an absolute or relative path.
 *
 * @return WError::ERR_NOT_INITIALIZED if initialize() has not been called, WError::ERR_FAIL if
 *         the file could not be loaded, otherwise WError::ERR_OK.
 */
int WSESWrapperMain::loadInstrument(const char *fileName)
{
  instrumentLoaded_ = false;
  memset(&sesInstrumentInfo_, 0, sizeof(SesNS::WInstrumentInfo));
  if (!initialized_)
	  return WError::ERR_NOT_INITIALIZED;

  if (*fileName == 0)
      return WError::ERR_FAIL;

  lib_->GDS_GetDetectorInfo(&sesDetectorInfo_);
  int errorCode = WError::ERR_OK;
  if (lib_->GDS_LoadInstrument(fileName) == 0)
  {
	//asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: Instrument configuration file loaded successfully\n", driverName, functionName);
    lib_->GDS_GetInstrumentInfo(&sesInstrumentInfo_);
    int length = *sesInstrumentInfo_.Model;
    memmove(sesInstrumentInfo_.Model, sesInstrumentInfo_.Model + 1, length);
    sesInstrumentInfo_.Model[length] = 0;
    length = *sesInstrumentInfo_.SerialNo;
    memmove(sesInstrumentInfo_.SerialNo, sesInstrumentInfo_.SerialNo + 1, length);
    sesInstrumentInfo_.SerialNo[length] = 0;
    instrumentLoaded_ = true;
  }
  else
  {
    errorCode = WError::ERR_FAIL;
  }

  if (!loadElementSets() || !loadLensModes() || (lensModes_.size() > 0 && !loadPassEnergies(lensModes_[0], passEnergies_)))
  {
    errorCode = WError::ERR_FAIL;
  }

  return errorCode;
}

/*!
 * Sets all voltage elements to zero.
 *
 * @return WError::ERR_NO_INSTRUMENT if loadInstrument() has not been called, WError::ERR_FAIL
 *         if the voltages could not be set, otherwise WError::ERR_OK.
 */
int WSESWrapperMain::zeroSupplies()
{
  if (!instrumentLoaded_)
    return WError::ERR_NO_INSTRUMENT;
  
  return lib_->GDS_ZeroSupplies() == 0 ? WError::ERR_OK : WError::ERR_FAIL;
}

/*!
 * Reads the current kinetic energy from the SESInstrument library.
 *
 * @param[out] kineticEnergy Pointer to a double to be modified with the current kinetic energy.
 *
 * @return WError::ERR_NO_INSTRUMENT if loadInstrument() has not been called, WError::ERR_FAIL
 *         if the energy could not be read, otherwise WError::ERR_OK.
 */
int WSESWrapperMain::getKineticEnergy(double *kineticEnergy)
{
  if (!instrumentLoaded_)
    return WError::ERR_NO_INSTRUMENT;

  return lib_->GDS_GetCurrKineticEnergy(kineticEnergy) == 0 ? WError::ERR_OK : WError::ERR_FAIL;
}

/*!
 * Changes the kinetic energy. Use this member function when initAcquisition() and startAcquisition() is not
 * going to be called, e.g. when controlling the Scienta analyzer with an external, third-party detector to make
 * measurements.
 *
 * @param[in] kineticEnergy The kinetic energy (in eV) to be set.
 *
 * @return WError::ERR_NO_INSTRUMENT if loadInstrument() has not been called, WError::ERR_FAIL
 *         if the energy could not be read, otherwise WError::ERR_OK.
 */
int WSESWrapperMain::setKineticEnergy(const double kineticEnergy)
{
  if (!instrumentLoaded_)
    return WError::ERR_NO_INSTRUMENT;

  return lib_->GDS_SetKineticEnergy(kineticEnergy) == 0 ? WError::ERR_OK : WError::ERR_FAIL;
}

/*!
 * Reads the current voltage from analyzer element @p elementName.
 *
 * @param[in] elementName A string specifying the name of the element to be read.
 * @param[out] voltage A pointer to a variable that will receive the voltage (in V).
 *
 * @return WError::ERR_NO_INSTRUMENT if loadInstrument() has not been called, WError::ERR_FAIL
 *         if the element could not be read (e.g. the element @p elementName does not exist), otherwise
 *         WError::ERR_OK.
 */
int WSESWrapperMain::getElementVoltage(const char *elementName, double *voltage)
{
  if (!instrumentLoaded_)
    return WError::ERR_NO_INSTRUMENT;

  return lib_->GDS_GetElement(elementName, voltage) == 0 ? WError::ERR_OK : WError::ERR_FAIL;
}

/*!
 * Changes the voltage of analyzer element @p elementName.
 *
 * @param[in] elementName A string specifying the name of the element to be modified.
 * @param[in] voltage The new voltage (in V).
 *
 * @return WError::ERR_NO_INSTRUMENT if loadInstrument() has not been called, WError::ERR_FAIL
 *         if the element voltage could not be set (e.g. the element @p elementName does not exist), otherwise
 *         WError::ERR_OK.
 */
int WSESWrapperMain::setElementVoltage(const char *elementName, const double voltage)
{
  if (!instrumentLoaded_)
    return WError::ERR_NO_INSTRUMENT;

  return lib_->GDS_SetElement(elementName, voltage) == 0 ? WError::ERR_OK : WError::ERR_FAIL;
}

/*!
 * Validates the current analyzer region.
 *
 * In addition to validating the region, this function also returns the
 * number of steps (for swept mode acquisitions) that will be taken, the minimum dwell time per step (ms), and the smallest
 * step size (eV).
 *
 * @param[out] analyzerRegion A pointer to a @ref SESWrapperNS::AnalyzerRegion struct that will be filled with validated values.
 * @param[out] steps The number of steps that will be taken for a swept mode acquisition.
 * @param[out] time_ms The estimated acquisition time (in ms) for one iteration.
 * @param[out] minEnergyStep_eV The minimum step size (in eV) for swept mode acquisition.
 *
 * @return WError::ERR_NO_INSTRUMENT if loadInstrument() has not been called, WError::ERR_INCORRECT_ANALYZER_REGION
 *         if an error was detected in the analyzer region, otherwise WError::ERR_OK.
 */
int WSESWrapperMain::checkAnalyzerRegion(WAnalyzerRegion *analyzerRegion, int *steps, double *time_ms, double *minEnergyStep_eV)
{
  if (!instrumentLoaded_)
    return WError::ERR_NO_INSTRUMENT;
  int error = lib_->GDS_CheckRegion(&sesRegion_, steps, time_ms, minEnergyStep_eV);
  analyzerRegion->centerEnergy_ = sesRegion_.FixEnergy;
  analyzerRegion->dwellTime_ =  sesRegion_.StepTime;
  analyzerRegion->energyStep_ = sesRegion_.EnergyStep;
  analyzerRegion->fixed_ = sesRegion_.Fixed;
  analyzerRegion->highEnergy_ = sesRegion_.HighEnergy;
  analyzerRegion->lowEnergy_ = sesRegion_.LowEnergy;

  return error == 0 ? WError::ERR_OK : WError::ERR_INCORRECT_ANALYZER_REGION;
}

/*!
 * Initializes an acquisition.
 *
 * This function initializes an acquisition. It sets the iteration index to 0, thus creating a new
 * acquisition. The @p blockPointReady and @p blockRegionReady parameters specifies whether acquisition should
 * pause for each step or regin, respectively. 
 *
 * @param[in] blockPointReady If @c true, this parameter tells the acquisition thread to wait for confirmation
 *                        between each step taken in a swept mode acquisition.
 * @param[in] blockRegionReady If @c true, this parameter tells the acquisition thread to wait for confirmation
 *                         once the acquisition is finished.
 *
 * @return WError::ERR_NO_INSTRUMENT if loadInstrument() has not been called, WError::ERR_INCORRECT_ANALYZER_REGION
 *         if an error was detected in the analyzer region, otherwise WError::ERR_OK.
 */
int WSESWrapperMain::initAcquisition(const bool blockPointReady, const bool blockRegionReady)
{
  if (!instrumentLoaded_)
    return WError::ERR_NO_INSTRUMENT;

  int sesStatus = SesNS::NonOperational;
  if (lib_->GDS_GetStatus != 0)
    lib_->GDS_GetStatus(&sesStatus);

  if (sesStatus == SesNS::Running)
    return WError::ERR_FAIL;

  abortAcquisitionEvent_.reset();
  pointReadyEvent_.reset();
  regionReadyEvent_.reset();
  continueAcquisitionEvent_.reset();

  blockPointReady_ = blockPointReady;
  blockRegionReady_ = blockRegionReady;

  iteration_ = 0;
  sesSpectrum_ = 0;
  currentStep_ = 0;

  int result = 0;

  if (lib_->GDS_InitAcquisition != 0)
    result = lib_->GDS_InitAcquisition(&sesRegion_, &sesSpectrum_, &sesSignals_, tempFileName_.c_str(), WSESWrapperMain::pointReady, WSESWrapperMain::regionReady);

  startTime_ = clock();

  return result == 0 ? WError::ERR_OK : WError::ERR_FAIL;
}

/*!
 * Starts an acquisition.
 *
 * If WAnalyzerRegion::fixed_ is set to @c true (or 1), this will start one fixed mode acquisition. If set to
 * @c false (or 0), a swept mode acquisition is started. initAcquisition() must be called the first time an
 * acquisition will be made. After that, startAcquisition() can be called any number of times to accumulate
 * data to the same spectrum. All data is stored in the temporary file specified by the @c temp_file_name
 * property, which can later be used translate the results to e.g. an Igor format. Before calling this member
 * function, the @c analyzer_region and @c detector_region properties should be properly prepared with
 * correct region settings. It is possible to validate the analyzer settings by calling checkanalyzerRegion()
 * before calling this.
 * 
 * The function returns immediately to allow the caller to continue working. Use waitForPointReady()
 * and/or waitForRegionReady() if blocking is required.
 *
 * @return WError::ERR_NO_INSTRUMENT if loadInstrument() has not been called, WError::ERR_FAIL if the acquisition
 *         could not be started (e.g. another acquisition is already running), otherwise WError::ERR_OK.
 */
int WSESWrapperMain::startAcquisition()
{
  if (!instrumentLoaded_)
    return WError::ERR_NO_INSTRUMENT;
  //asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: Acquisition running....\n", driverName, functionName);
  int sesStatus = SesNS::NonOperational;
  lib_->GDS_GetStatus(&sesStatus);
  if (sesStatus == SesNS::Running)
    return WError::ERR_ACQUIRING;
  startTime_ = clock();
  abortAcquisitionEvent_.reset();
  pointReadyEvent_.reset();
  regionReadyEvent_.reset();
  currentStep_ = 0;

  if (lib_->GDS_UseDetector != 0)
    lib_->GDS_UseDetector(usingDetector_);
  if (lib_->GDS_UseSignals != 0)
    lib_->GDS_UseSignals(usingSignals_);

  int result = 0;
  
  if (resetDataBetweenIterations_)
    iteration_ = 1;
  else
    iteration_++;

  if (lib_->GDS_StartAcquisition)
  {
    result = lib_->GDS_StartAcquisition(iteration_);
  }
  else
  {
    result = lib_->GDS_Start(&sesRegion_, &sesSpectrum_, tempFileName_.c_str(), iteration_, WSESWrapperMain::pointReady, WSESWrapperMain::regionReady);
    if (lib_->GDS_GetCurrSignals != 0)
      lib_->GDS_GetCurrSignals(&sesSignals_);
  }
  if(result == 0)
  {
	  //asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: Acquisition completed successfully\n", driverName, functionName);
  }
  else
  {
	  ////asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: Acquisition failed\n", driverName, functionName);
  }
  return result == 0 ? WError::ERR_OK : WError::ERR_FAIL;
}

/*!
 * Aborts the current acquisition. Can be called at any time to stop an active acquisition and set the library to an
 * idle state.
 *
 * @return WError::ERR_NO_INSTRUMENT if loadInstrument() has not been called, WError::ERR_FAIL if
 *         the acquisition could not be stopped, otherwise WError::ERR_OK.
 */
int WSESWrapperMain::stopAcquisition()
{
  if (!lib_->isLoaded())
    return WError::ERR_NO_INSTRUMENT;

  abortAcquisitionEvent_.set();
  pointReadyEvent_.reset();
  regionReadyEvent_.reset();

  return lib_->GDS_Stop() == 0 ? WError::ERR_OK : WError::ERR_FAIL;
}

/*!
 * This is a generic function for obtaining the value of acquired data or related variables. The @p data parameter is
 * declared <code>void *</code> in order to allow any type of data to be accessed. 
 *
 * @param[in] variable The name of the variable to be queried.
 * @param[in] index Used to query a specific element from a variable that is stored as a vector.
 * @param[out] data Pointer to a variable that will be filled with the data specified by @c variable. This parameter
 *                  can be 0 (NULL). In that case, @p size can be used to obtain the required size of @p data if the
 *                  variable is a string or vector.
 * @param[in,out] size This parameter is used to get the size of @p data when the variable is a string or vector.
 *
 * @return WError::ERR_PARAMETER_NOT_FOUND if @p variable was not a valid variable name, otherwise the return code
 *         depends on the type of variable.
 */
int WSESWrapperMain::getAcquiredData(const char *variable, int index, void *data, int &size)
{
  DataParameterMap::iterator it = dataParameters_.find(variable);
  if (it == dataParameters_.end())
    return WError::ERR_PARAMETER_NOT_FOUND;

  return it->second.get(index, data, size);
}

/*!
 * Blocks execution of the callers thread until the current point has been completed during a swept mode acquisition.
 * If @p timeout_ms is set to -1, there is no time-out. Do not use -1 when running fixed mode acquisitions, as there
 * is no step taken in those cases, so there is no event that can unlock this.
 * The WError::ERR_TIMEOUT is a warning code (error code > 0) which can be used to separate behavior from
 * more serious errors.
 *
 * @param[in] timeout_ms The number of milliseconds to wait until time-out.
 *
 * @return WError::ERR_NO_INSTRUMENT if loadInstrument() has not been called.
 *         Returns immediately with a WError::ERR_OK if there is no acquisition running.
 *         WError::ERR_OK is returned when the measured point has been completed. WError::ERR_TIMEOUT is returned if
 *         the function times out before comletion of the point.
 *
 * @see continueAcquisition(), waitForPointReady(),
 *      initAcquisition(), startAcquisition(), stopAcquisition()
 */
int WSESWrapperMain::waitForPointReady(int timeout_ms)
{
  if (!instrumentLoaded_)
    return WError::ERR_NO_INSTRUMENT;

  int sesStatus = SesNS::NonOperational;
  lib_->GDS_GetStatus(&sesStatus);

  if (sesStatus != SesNS::Running)
    return WError::ERR_OK;

  const HANDLE handles[] = {pointReadyEvent_.handle(), abortAcquisitionEvent_.handle()};
  DWORD result = WaitForMultipleObjects(2, handles, FALSE, timeout_ms);
  return result == WAIT_TIMEOUT ? WError::ERR_TIMEOUT : WError::ERR_OK;
}

/*!
 * Blocks execution of the callers thread until the current region has been completed.
 * If @p timeout_ms is set to -1, there is no time-out. This can be used for both fixed and swept mode acquisitions.
 * The WError::ERR_TIMEOUT is a warning code (error code > 0) which can be used to separate behaviour from
 * more serious errors.
 *
 * @param[in] timeout_ms The number of milliseconds to wait until time-out.
 *
 * @return WError::ERR_NO_INSTRUMENT if loadInstrument() has not been called.
 *         Returns immediately with a WError::ERR_OK if there is no acquisition running.
 *         WError::ERR_OK is returned when the measured region has been completed. WError::ERR_TIMEOUT is returned if
 *         the function times out before completion of the point.
 *
 * @see continueAcquisition(), waitForPointReady(),
 *      initAcquisition(), startAcquisition(), stopAcquisition()
 */
int WSESWrapperMain::waitForRegionReady(int timeout_ms)
{
  if (!instrumentLoaded_)
    return WError::ERR_NO_INSTRUMENT;

  int sesStatus = SesNS::NonOperational;
  lib_->GDS_GetStatus(&sesStatus);

  if (sesStatus != SesNS::Running)
    return WError::ERR_OK;

  const HANDLE handles[] = {regionReadyEvent_.handle(), abortAcquisitionEvent_.handle()};
  DWORD result = WaitForMultipleObjects(2, handles, FALSE, timeout_ms);
  return result == WAIT_TIMEOUT ? WError::ERR_TIMEOUT : WError::ERR_OK;
}

/*!
 * Releases the acquisition thread after a blocked point or region. Blocking points or regions is done by
 * setting @p blockPointReady or @p blockRegionReady to @c true in the initAcquisition() member
 * function call.

 * @see waitForPointReady(), waitForRegionReady()
 *      initAcquisition(), startAcquisition(), stopAcquisition()
 */
int WSESWrapperMain::continueAcquisition()
{
  /* Line below added to correct pointReady 'always ready' error */
  pointReadyEvent_.reset();
  continueAcquisitionEvent_.set();
  return WError::ERR_OK;
}

/*!
 * Queries the type of a property or data parameter by searching for @p name.
 *
 * @return One of the types defined in WVariable.
 */
int WSESWrapperMain::parameterType(const char *name)
{
  PropertyMap::iterator p = properties_.find(name);
  if (p != properties_.end())
    return p->second.valueType();

  DataParameterMap::iterator dp = dataParameters_.find(name);
  if (dp != dataParameters_.end())
    return dp->second.valueType();

  return -1;
}

// Private WSESWrapperMain members

/*!
 * Callback function that is called by the SESInstrument library when an error has occured.
 * It opens a log file for reading and adds the error message obtained from SESInstrument.
 *
 * @param[in] errorCode The error code detected.
 */
void STDCALL WSESWrapperMain::errorNotify(int errorCode)
{
  char *buffer = this_->lib_->GDS_GetLastErrorString();
  std::ofstream oFile("seswrapper.log", std::ios::out | std::ios::app);
  time_t t = time(0);
  std::string tStr = ctime(&t);
  int pos = tStr.find('\n');
  if (pos != tStr.npos)
    tStr = tStr.substr(0, pos);
  oFile << "[" << tStr << "] SesInstrument Error " << errorCode << ": " << buffer << std::endl;
}

/*!
 * Callback function that is called by SESInstrument each time a step has been taken during a swept
 * mode acquisition. If @p blockPointReady is set to @c true, this function will block execution of the
 * current thread until continueAcquisition() or stopAcquisition() has been called.
 *
 * @param[in] index The index of the just finished step. This value can be negative.
 *
 * @see initAcquisition()
 */
void STDCALL WSESWrapperMain::pointReady(int index)
{
  if (this_ == 0)
    return;

  this_->pointReadyEvent_.set();
  this_->currentStep_++;

  if (this_->blockPointReady_)
  {
    HANDLE handles[] = {this_->continueAcquisitionEvent_.handle(), this_->abortAcquisitionEvent_.handle()};
    WaitForMultipleObjects(2, handles, FALSE, INFINITE);
    /* Line below added to correct pointReady 'always ready' error */
    this_->continueAcquisitionEvent_.reset();
  }
}

/*!
 * Callback function that is called by SESInstrument when an acquisition has been completed. If
 * @p blockRegionReady is set to @c true, this function will block execution of the current thread until
 * continueAcquisition() or stopAcquisition() has been called.
 *
 * @see initAcquisition()
 */
void STDCALL WSESWrapperMain::regionReady()
{
  if (this_ == 0)
    return;

  int t = clock() - this_->startTime_;

  this_->regionReadyEvent_.set();
  if (this_->blockRegionReady_)
  {
    HANDLE handles[] = {this_->continueAcquisitionEvent_.handle(), this_->abortAcquisitionEvent_.handle()};
    WaitForMultipleObjects(2, handles, FALSE, INFINITE);
    this_->continueAcquisitionEvent_.reset();
    this_->abortAcquisitionEvent_.reset();
  }
}

// Acquired data getters

/*!
 * Getter for the @c acq_channels variable. If no acquisition has been performed, the number of channels is 0.
 *
 * @param[in] index Not used.
 * @param[out] data A pointer to a 32-bit integer that will be modified with the number of energy channels in the acquired
 *             spectrum.
 * @param[in,out] size Not used.
 *
 * @return Always returns WError::ERR_OK.
 */
int WSESWrapperMain::getAcqChannels(int index, void *data, int &size)
{
  int *intData = reinterpret_cast<int *>(data);
  *intData = sesSpectrum_ != 0 ? sesSpectrum_->Channels : 0;
	return WError::ERR_OK;
}

/*!
 * Getter for the @c acq_slices variable. If no acquisition has been performed, the number of slices is 0.
 *
 * @param[in] index Not used.
 * @param[out] data A pointer to a 32-bit integer that will be modified with the number of slices in the acquired spectrum.
 * @param[in,out] size Not used.
 *
 * @return Always returns WError::ERR_OK.
 */
int WSESWrapperMain::getAcqSlices(int index, void *data, int &size)
{
  int *intData = reinterpret_cast<int *>(data);
  *intData = sesSpectrum_ != 0 ? sesSpectrum_->Slices : 0;
	return WError::ERR_OK;
}

/*!
 * Getter for the @c acq_iterations variable. The number of iterations is incremented by 1 when startAcquisition() is
 * called. The counter is reset when initAcquisition() is called. If no acquisition has been performed, the number of
 * iterations is 0.
 *
 * @param[in] index Not used.
 * @param[out] data A pointer to a 32-bit integer that will be modified with the number of iterations that have elapsed since
 *             the last call of initAcquisition().
 * @param[in,out] size Not used.
 *
 * @return Always returns WError::ERR_OK.
 */
int WSESWrapperMain::getAcqIterations(int index, void *data, int &size)
{
  int *intData = reinterpret_cast<int *>(data);
  *intData = sesSpectrum_ != 0 ? sesSpectrum_->Sweeps : 0;
	return WError::ERR_OK;
}

/*!
 * Getter for the @c acq_intensity_unit variable. The intensity unit is a string of up to 32 characters (including
 * the null termination) defining the unit of the intensity axis in the acquired spectrum. 
 *
 * @param[in] index Not used.
 * @param[out] data A @c char* buffer to be filled with the intensity unit. Can be 0 (NULL).
 * @param[in,out] size If @p data is non-null, this parameter is assumed to contain the maximum number of elements in the
 *             buffer. After completion, @p size is always modified to contain the length of the resulting array.
 *
 * @return WError::ERR_FAIL if no acquisition has been performed, otherwise WError::ERR_OK.
 */
int WSESWrapperMain::getAcqIntensityUnit(int index, void *data, int &size)
{
  if (sesSpectrum_ == 0)
    return WError::ERR_FAIL;

  char *strData = reinterpret_cast<char *>(data);
  if (strData != 0)
    strData[std::string(sesSpectrum_->CountUnit).copy(strData, size)] = 0;
  size = sizeof(sesSpectrum_->CountUnit);
	return WError::ERR_OK;
}

/*!
 * Getter for the @c acq_channel_unit variable. The channel unit is a string of up to 32 characters (including
 * the null termination) defining the unit of the energy axis in the acquired spectrum. 
 *
 * @param[in] index Not used.
 * @param[out] data A @c char* buffer to be filled with the channel unit. Can be 0 (NULL).
 * @param[in,out] size If @p data is non-null, this parameter is assumed to contain the maximum number of elements in the
 *             buffer. After completion, @p size is always modified to contain the length of the resulting array.
 *
 * @return WError::ERR_FAIL if no acquisition has been performed, otherwise WError::ERR_OK.
 */
int WSESWrapperMain::getAcqChannelUnit(int index, void *data, int &size)
{
  if (sesSpectrum_ == 0)
    return WError::ERR_FAIL;

  char *strData = reinterpret_cast<char *>(data);
  if (strData != 0)
    strData[std::string(sesSpectrum_->ChannelUnit).copy(strData, size)] = 0;
  size = sizeof(sesSpectrum_->ChannelUnit);
	return WError::ERR_OK;
}

/*!
 * Getter for the @c acq_slice_unit variable. The slice unit is a string of up to 32 characters (including
 * the null termination) defining the unit of the Y axis in the acquired spectrum image. 
 *
 * @param[in] index Not used.
 * @param[out] data A @c char* buffer to be filled with the channel unit. Can be 0 (NULL).
 * @param[in,out] size If @p data is non-null, this parameter is assumed to contain the maximum number of elements in the
 *             buffer. After completion, @p size is always modified to contain the length of the resulting array.
 *
 * @return WError::ERR_FAIL if no acquisition has been performed, otherwise WError::ERR_OK.
 */
int WSESWrapperMain::getAcqSliceUnit(int index, void *data, int &size)
{
  if (sesSpectrum_ == 0)
    return WError::ERR_FAIL;

  char *strData = reinterpret_cast<char *>(data);
  if (strData != 0)
    strData[std::string(sesSpectrum_->SliceUnit).copy(strData, size)] = 0;
  size = sizeof(sesSpectrum_->SliceUnit);
	return WError::ERR_OK;
}

/*!
 * Getter for the @c acq_spectrum variable. The spectrum is a vector of doubles (8-byte floating point values)
 * with the integrated intensities of all slices.
 *
 * @param[in] index Not used.
 * @param[out] data An array of doubles that will be filled with the integrated spectrum. Can be 0 (NULL).
 * @param[in,out] size If @p data is non-null, this parameter is assumed to contain the maximum number of elements in the
 *             buffer. After completion, @p size is always modified to contain the length of the resulting array.
 *
 * @return WError::ERR_FAIL if no acquisition has been performed, otherwise WError::ERR_OK.
 */
int WSESWrapperMain::getAcqSpectrum(int index, void *data, int &size)
{
  if (sesSpectrum_ == 0)
    return WError::ERR_FAIL;

  if (data != 0)
    memmove(data, sesSpectrum_->SumData, sesSpectrum_->Channels * sizeof(double));
  size = sesSpectrum_->Channels;
	return WError::ERR_OK;
}

/*!
 * Getter for the @c acq_image variable.
 *
 * @param[in] index Not used.
 * @param[out] data An array of doubles that will be filled with the acquired image. Can be 0 (NULL).
 * @param[in,out] size If @p data is non-null, this parameter is assumed to contain the maximum number of elements in the
 *             buffer. After completion, @p size is always modified to contain the length of the resulting array.
 */
int WSESWrapperMain::getAcqImage(int index, void *data, int &size)
{
  if (sesSpectrum_ == 0)
    return WError::ERR_FAIL;

  double *doubleData = reinterpret_cast<double *>(data);
  int sliceSize = sesSpectrum_->Channels * sizeof(double);
  if (doubleData != 0)
  {
    for (int slice = 0; slice < sesSpectrum_->Slices; slice++, doubleData += sesSpectrum_->Channels)
      memmove(doubleData, sesSpectrum_->Data[slice], sliceSize);
  }
  size = sesSpectrum_->Channels * sesSpectrum_->Slices;
	return WError::ERR_OK;
}

/*!
 * Getter for the @c acq_slice variable. This function can be called to obtain one slice from the spectrum image.
 *
 * @param[in] index Specifies which slice to extract from the spectrum. Must be between 0 and the number of slices - 1.
 * @param[out] data An array of doubles that will be filled with the elements of the slice. Can be 0 (NULL).
 * @param[in,out] size If @p data is non-null, this parameter is assumed to contain the maximum number of elements in the
 *             buffer. After completion, @p size is always modified to contain the length of the resulting array.
 *
 * @return WError::ERR_FAIL if no acquisition has been performed, WError::ERR_INDEX if @p index is out-of-bounds,
 *         otherwise WError::ERR_OK.
 */
int WSESWrapperMain::getAcqSlice(int index, void *data, int &size)
{
  if (sesSpectrum_ == 0)
    return WError::ERR_FAIL;

  if (index < 0 || index >= sesSpectrum_->Slices)
    return WError::ERR_INDEX;

  if (data != 0)
    memmove(data, sesSpectrum_->Data[index], sesSpectrum_->Channels * sizeof(double));

  size = sesSpectrum_->Channels;
  return WError::ERR_OK;
}

/*!
 * Getter for the @c acq_channel_scale variable. Defines an array of doubles where each element
 * corresponds to an energy channel. The scale is always in kinetic energy.
 *
 * @param[in] index Not used.
 * @param[out] data An array of doubles that will be filled with the channel scale. Can be 0 (NULL).
 * @param[in,out] size If @p data is non-null, this parameter is assumed to contain the maximum number of elements in the
 *             buffer. After completion, @p size is always modified to contain the length of the resulting array.
 *
 * @return WError::ERR_FAIL if no acquisition has been performed, otherwise WError::ERR_OK.
 */
int WSESWrapperMain::getAcqChannelScale(int index, void *data, int &size)
{
  if (sesSpectrum_ == 0)
    return WError::ERR_FAIL;

  if (data != 0)
    memmove(data, sesSpectrum_->ChannelScale, sesSpectrum_->Channels * sizeof(double));
  size = sesSpectrum_->Channels;
  return WError::ERR_OK;
}

/*!
 * Getter for the @c acq_slice_scale variable. Defines an array of doubles where each element
 * corresponds to one position in the Y axis.
 *
 * @param[in] index Not used.
 * @param[out] data An array of doubles that will be filled with the Y axis scale (the slices). Can be 0 (NULL).
 * @param[in,out] size If @p data is non-null, this parameter is assumed to contain the maximum number of elements in the
 *             buffer. After completion, @p size is always modified to contain the length of the resulting array.
 *
 * @return WError::ERR_FAIL if no acquisition has been performed, otherwise WError::ERR_OK.
 */
int WSESWrapperMain::getAcqSliceScale(int index, void *data, int &size)
{
  if (sesSpectrum_ == 0)
    return WError::ERR_FAIL;

  if (data != 0)
    memmove(data, sesSpectrum_->SliceScale, sesSpectrum_->Slices * sizeof(double));
  size = sesSpectrum_->Slices;
  return WError::ERR_OK;
}

/*!
 * Getter for the @c acq_raw_image variable. Defines a byte array that will span the data in one frame taken
 * from the detector. If the detector is not based on frames/images, this variable will not be available.
 *
 * One frame usually has a size of xChannels * yChannels * byteSize, where xChannels and yChannels can be obtained
 * from the @c detector_info property. The byteSize variable is the number of bytes used per element in the image,
 * which is 1 for 8-bit images, or 2 for 16-bit images. To get the byteSize, call this function with @p data set to
 * 0 (NULL) and then divide the @p size parameter obtained with xChannels * yChannels.
 *
 * @param[in] index Not used.
 * @param[out] data An array of bytes (unsigned char*) that will be filled with a snapshot of the detector image.
 *             Can be 0 (NULL).
 * @param[in,out] size If @p data is non-null, this parameter is assumed to contain the maximum number of elements in the
 *             buffer. After completion, @p size is always modified to contain the length of the resulting array
 *             (xChannels * yChannels * byteSize).
 *
 * @return WError::ERR_NOT_INITIALIZED if the wrapped library is not properly initialized, WError::ERR_FAIL if the
 *         raw image could not be obtained (possibly because the detector does not support 2D-frames), otherwise
 *         WError::ERR_OK on success.
 *
 * @see WSESWrapperBase::getDetectorInfo()
 */
int WSESWrapperMain::getAcqRawImage(int index, void *data, int &size)
{
  unsigned char *uCharData = reinterpret_cast<unsigned char *>(data);
  int errorCode = lib_->GDS_GetRawImage != 0 ? WError::ERR_OK : WError::ERR_NOT_INITIALIZED;
  int width = sesDetectorInfo_.XChannels;
  int height = sesDetectorInfo_.YChannels;
  int byteSize = 1;
  
  if (errorCode == WError::ERR_OK)
  {
    errorCode = lib_->GDS_GetRawImage(uCharData, &width, &height, &byteSize) == 0 ? WError::ERR_OK : WError::ERR_FAIL;
    size = width * height * byteSize;
  }
	return errorCode;
}

/*!
 * Getter for the @c acq_current_step variable. Use this to obtain the current step in a swept acquisition.
 *
 * @param[in] index Not used.
 * @param[out] data A pointer to a 32-bit integer that will be set to the current step.
 * @param[in,out] size Not used.
 *
 * @return WError::ERR_FAIL if no acquisition has been performed, otherwise WError::ERR_OK.
 */
int WSESWrapperMain::getAcqCurrentStep(int index, void *data, int &size)
{
  if (sesSpectrum_ == 0)
    return WError::ERR_FAIL;

  int *intData = reinterpret_cast<int *>(data);
  *intData = currentStep_;
	return WError::ERR_OK;
}

/*!
 * Getter for the @c acq_elapsed_time variable. This can be used to evaluate how many milliseconds have passed
 * since the last call of startAcquisition().
 *
 * @param[in] index Not used.
 * @param[out] data A pointer to a 32-bit unsigned integer that will be modified to the number of millisecondes elapsed
 *             since the last call of startAcquisition().
 * @param[in,out] size Not used.
 *
 * @return Always returns WError::ERR_OK.
 */
int WSESWrapperMain::getAcqElapsedTime(int index, void *data, int &size)
{
  unsigned int *intData = reinterpret_cast<unsigned int *>(data);
  *intData = clock() - startTime_;
	return WError::ERR_OK;
}

/*!
 * Getter for the @c acq_io_ports variable. Use this to obtain the number of External I/O-ports.
 *
 * @param[in] index Not used.
 * @param[out] data A pointer to a 32-bit integer that will be modified with the number of External I/O-ports.
 * @param[in,out] size Not used.
 * 
 * @return WError::ERR_FAIL if External I/O is not available or if no acquisition has been initialized, otherwise
 *         WError::ERR_OK.
 */
int WSESWrapperMain::getAcqIOPorts(int index, void *data, int &size)
{
  if (sesSignals_ == 0)
    return WError::ERR_FAIL;

  int *intData = reinterpret_cast<int *>(data);
  *intData = sesSignals_->Count;
	return WError::ERR_OK;
}

/*!
 * Getter for the @c acq_io_size variable. This is used with the External I/O mechanizm, where the data is collected
 * from a number of ports from e.g. a DAQ card. This variable show the size of one spectrum.
 *
 * @param[in] index Not used.
 * @param[out] data A pointer to a 32-bit integer that will be modified with the number of channels in one External I/O spectrum.
 * @param[in,out] size Not used.
 *
 * @return WError::ERR_FAIL if External I/O is not available or if no acquisition has been initialized, otherwise
 *         WError::ERR_OK.
 */
int WSESWrapperMain::getAcqIOSize(int index, void *data, int &size)
{
  if (sesSignals_ == 0)
    return WError::ERR_FAIL;

  int *intData = reinterpret_cast<int *>(data);
  *intData = sesSignals_->Steps;
	return WError::ERR_OK;
}

/*!
 * Getter for the @c acq_io_iterations variable. Used with External I/O measurements to query the number of iterations
 * that have elapsed since the last call of initAcquisition().
 *
 * @param[in] index Not used.
 * @param[out] data A pointer to a 32-bit integer the will be modified to show the number of iterations.
 * @param[in,out] size Not used.
 *
 * @return WError::ERR_FAIL if External I/O is not available or if no acquisition has been initialized, otherwise
 *         WError::ERR_OK.
 */
int WSESWrapperMain::getAcqIOIterations(int index, void *data, int &size)
{
  if (sesSignals_ == 0)
    return WError::ERR_FAIL;

  int *intData = reinterpret_cast<int *>(data);
  *intData = sesSignals_->Sweeps;
	return WError::ERR_OK;
}

/*!
 * Getter for the @c acq_io_unit variable. The External I/O unit is a string of up to 32 characters (including
 * the null termination) defining the unit of the External I/O spectrum. 
 *
 * @param[in] index Not used.
 * @param[out] data A @c char* buffer to be filled with the External I/O unit. Can be 0 (NULL).
 * @param[in,out] size If @p data is non-null, this parameter is assumed to contain the maximum number of elements in the
 *             buffer. After completion, @p size is always modified to contain the length of the resulting array.
 *
 * @return WError::ERR_FAIL if External I/O is not available or if no acquisition has been initialized, otherwise
 *         WError::ERR_OK.
 */
int WSESWrapperMain::getAcqIOUnit(int index, void *data, int &size)
{
  if (sesSignals_ == 0)
    return WError::ERR_FAIL;

  if (data != 0)
    memmove(data, sesSignals_->StepsUnit, sizeof(sesSignals_->StepsUnit));
  size = strlen(sesSignals_->StepsUnit);
	return WError::ERR_OK;
}

/*!
 * Getter for the @c acq_io_scale variable. Defines an array of doubles where each element
 * corresponds to an External I/O step.
 *
 * @param[in] index Not used.
 * @param[out] data An array of doubles that will be filled with the External I/O scale. Can be 0 (NULL).
 * @param[in,out] size If @p data is non-null, this parameter is assumed to contain the maximum number of elements in the
 *             buffer. After completion, @p size is always modified to contain the length of the resulting array.
 *
 * @return WError::ERR_FAIL if External I/O is not available or if no acquisition has been initialized, otherwise
 *         WError::ERR_OK.
 */
int WSESWrapperMain::getAcqIOScale(int index, void *data, int &size)
{
  if (sesSignals_ == 0)
    return WError::ERR_FAIL;
  
  if (data != 0)
    memmove(data, sesSignals_->StepsScale, sesSignals_->Steps * sizeof(double));
  size = sesSignals_->Steps;
	return WError::ERR_OK;
}

/*!
 * Getter for the @c acq_io_spectrum variable. Extracts one spectrum from one of the External I/O ports. To extract 
 * all ports at once, use the @c acq_io_data variable instead.
 *
 * @param[in] index The index of the port to be queried. Must be between 0 and @c acq_io_ports - 1.
 * @param[out] data An array of doubles that will be filled with the data acquired from External I/O port @c index. Can be 0 (NULL).
 * @param[in,out] size If @p data is non-null, this parameter is assumed to contain the maximum number of elements in the
 *             buffer. After completion, @p size is always modified to contain the length of the resulting array.
 *
 * @return WError::ERR_FAIL if External I/O is not available or if no acquisition has been initialized,
           WError::ERR_INDEX if @p index is out-of-bounds, otherwise
 *         WError::ERR_OK.
 */
int WSESWrapperMain::getAcqIOSpectrum(int index, void *data, int &size)
{
  if (sesSignals_ == 0)
    return WError::ERR_FAIL;

  if (index < 0 || index >= sesSignals_->Count)
    return WError::ERR_INDEX;

  if (data != 0)
    memmove(data, sesSignals_->Data[index], sesSignals_->Steps * sizeof(double));
  size = sesSignals_->Steps;
	return WError::ERR_OK;
}

/*!
 * Getter for the @c acq_io_data variable. Extracts all External I/O data acquired.
 *
 * @param[in] index Not used.
 * @param[out] data An array of doubles that will be filled with the spectra acquired from the External I/O ports. Can be 0 (NULL).
 * @param[in,out] size If @p data is non-null, this parameter is assumed to contain the maximum number of elements in the
 *             buffer. After completion, @p size is always modified to contain the length of the resulting array
 *             (@c acq_io_ports * @c acq_io_size).
 *
 * @return WError::ERR_FAIL if External I/O is not available or if no acquisition has been initialized, otherwise
 *         WError::ERR_OK.
 */
int WSESWrapperMain::getAcqIOData(int index, void *data, int &size)
{
  if (sesSignals_ == 0)
    return WError::ERR_FAIL;

  double *doubleData = reinterpret_cast<double *>(data);
  int channelSize = sesSignals_->Steps * sizeof(double);
  if (doubleData != 0)
  {
    for (int channel = 0; channel < sesSignals_->Count; channel++, doubleData += channelSize)
      memmove(doubleData, sesSignals_->Data[channel], channelSize);
  }
  size = sesSignals_->Count * sesSignals_->Steps;
  return WError::ERR_OK;
}

/*!
 * Getter for the @c acq_io_port_name variable. Obtains the name of one port in the External I/O port list.
 *
 * @param[in] index The index of the port that will be queried. Must be between 0 and @c acq_io_ports.
 * @param[out] data A @c char* buffer to be filled with the name of the External I/O port specified by @c index. Can be 0 (NULL).
 * @param[in,out] size If @p data is non-null, this parameter is assumed to contain the maximum number of elements in the
 *             buffer. After completion, @p size is always modified to contain the length of the resulting array.
 *
 * @return WError::ERR_FAIL if External I/O is not available or if no acquisition has been initialized,
           WError::ERR_INDEX if @p index is out-of-bounds, otherwise
 *         WError::ERR_OK.
 */
int WSESWrapperMain::getAcqIOPortName(int index, void *data, int &size)
{
  if (sesSignals_ == 0)
    return WError::ERR_FAIL;
  if (data != 0)
  {
    if (index < 0 || index >= sizeof(SesNS::Char32))
      return WError::ERR_INDEX;
    memmove(data, sesSignals_->Names[index], min(size, sizeof(SesNS::Char32)));
  }
  else
    size = strlen(sesSignals_->Names[index]);
	return WError::ERR_OK;
}
