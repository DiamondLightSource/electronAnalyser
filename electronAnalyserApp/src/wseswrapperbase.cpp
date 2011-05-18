#pragma warning(disable:4996)

#include "wseswrapperbase.h"
#include "wsesinstrument.h"
#include "werror.h"
#include "common.hpp"
#include "constants.h"

#include <windows.h>

#include <iostream>
#include <string>
#include <sstream>
#include <algorithm>
#include <direct.h>

#include <fstream>

using namespace SESWrapperNS;

/*! @class WSESWrapperBase
 * @brief This class collects all variables and objects for SESWrapper. 
 *
 * It serves as a container for property callbacks used when "setting" or "getting" values for the properties.
 */

/*!
 * Creates  a WSESWrapperBase instance. Also creates the hash table for the properties, where the property
 * names are linked to their corresponding callbacks through instances of the WVariable template class.
 *
 * @param[in] workingDir The current working directory.
 */
WSESWrapperBase::WSESWrapperBase(const char *workingDir)
: lib_(new WSesInstrument), instrumentLoaded_(false), workingDir_(workingDir), sesSpectrum_(0), sesSignals_(0),
  usingDetector_(true), usingSignals_(false), currentStep_(0), iteration_(0), startTime_(0),
  blockPointReady_(false), blockRegionReady_(false), resetDataBetweenIterations_(false)
{
  errors_ = WError::instance();

  *sesInstrumentInfo_.Model = 0;
  *sesInstrumentInfo_.SerialNo = 0;

  // Create property database
  properties_.insert(PropertyKeyValue("lib_description", Property(this, &WSESWrapperBase::readOnlyStub, &WSESWrapperBase::getLibDescription, Property::TYPE_STRING)));
  properties_.insert(PropertyKeyValue("lib_version", Property(this, &WSESWrapperBase::readOnlyStub, &WSESWrapperBase::getLibVersion, Property::TYPE_STRING)));
  properties_.insert(PropertyKeyValue("lib_error", Property(this, &WSESWrapperBase::readOnlyStub, &WSESWrapperBase::getLibError, Property::TYPE_STRING)));
  properties_.insert(PropertyKeyValue("lib_working_dir", Property(this, &WSESWrapperBase::setLibWorkingDir, &WSESWrapperBase::getLibWorkingDir, Property::TYPE_STRING)));
  properties_.insert(PropertyKeyValue("instrument_status", Property(this, &WSESWrapperBase::readOnlyStub, &WSESWrapperBase::getInstrumentStatus, Property::TYPE_INT32)));
  properties_.insert(PropertyKeyValue("always_delay_region", Property(this, &WSESWrapperBase::setAlwaysDelayRegion, &WSESWrapperBase::getAlwaysDelayRegion, Property::TYPE_BOOL)));
  properties_.insert(PropertyKeyValue("allow_io_with_detector", Property(this, &WSESWrapperBase::setAllowIOWithDetector, &WSESWrapperBase::getAllowIOWithDetector, Property::TYPE_BOOL)));
  properties_.insert(PropertyKeyValue("instrument_model", Property(this, &WSESWrapperBase::readOnlyStub, &WSESWrapperBase::getInstrumentModel, Property::TYPE_STRING)));
  properties_.insert(PropertyKeyValue("instrument_serial_no", Property(this, &WSESWrapperBase::readOnlyStub, &WSESWrapperBase::getInstrumentSerialNo, Property::TYPE_STRING)));
  properties_.insert(PropertyKeyValue("detector_info", Property(this, &WSESWrapperBase::readOnlyStub, &WSESWrapperBase::getDetectorInfo, Property::TYPE_DETECTORINFO)));
  properties_.insert(PropertyKeyValue("detector_region", Property(this, &WSESWrapperBase::setDetectorRegion, &WSESWrapperBase::getDetectorRegion, Property::TYPE_DETECTORREGION)));
  properties_.insert(PropertyKeyValue("element_set_count", Property(this, &WSESWrapperBase::readOnlyStub, &WSESWrapperBase::getElementSetCount, Property::TYPE_INT32)));
  properties_.insert(PropertyKeyValue("element_set", Property(this, &WSESWrapperBase::setElementSet, &WSESWrapperBase::getElementSet, Property::TYPE_STRING)));
  properties_.insert(PropertyKeyValue("lens_mode_count", Property(this, &WSESWrapperBase::readOnlyStub, &WSESWrapperBase::getLensModeCount, Property::TYPE_INT32)));
  properties_.insert(PropertyKeyValue("lens_mode", Property(this, &WSESWrapperBase::setLensMode, &WSESWrapperBase::getLensMode, Property::TYPE_STRING)));
  properties_.insert(PropertyKeyValue("pass_energy_count", Property(this, &WSESWrapperBase::readOnlyStub, &WSESWrapperBase::getPassEnergyCount, Property::TYPE_INT32)));
  properties_.insert(PropertyKeyValue("pass_energy", Property(this, &WSESWrapperBase::setPassEnergy, &WSESWrapperBase::getPassEnergy, Property::TYPE_DOUBLE)));

  properties_.insert(PropertyKeyValue("analyzer_region", Property(this, &WSESWrapperBase::setAnalyzerRegion, &WSESWrapperBase::getAnalyzerRegion, Property::TYPE_ANALYZERREGION)));
  properties_.insert(PropertyKeyValue("use_external_io", Property(this, &WSESWrapperBase::setUseExternalIO, &WSESWrapperBase::getUseExternalIO, Property::TYPE_BOOL)));
  properties_.insert(PropertyKeyValue("use_detector", Property(this, &WSESWrapperBase::setUseDetector, &WSESWrapperBase::getUseDetector, Property::TYPE_BOOL)));
  properties_.insert(PropertyKeyValue("region_name", Property(this, &WSESWrapperBase::setRegionName, &WSESWrapperBase::getRegionName, Property::TYPE_STRING)));
  properties_.insert(PropertyKeyValue("temp_file_name", Property(this, &WSESWrapperBase::setTempFileName, &WSESWrapperBase::getTempFileName, Property::TYPE_STRING)));
  properties_.insert(PropertyKeyValue("reset_data_between_iterations", Property(this, &WSESWrapperBase::setResetDataBetweenIterations, &WSESWrapperBase::getResetDataBetweenIterations, Property::TYPE_BOOL)));
}

/*!
 * Destroys the WSESWrapperBase object.
 */
WSESWrapperBase::~WSESWrapperBase()
{
  delete lib_;
  errors_->release();
}

/*!
 * Getter for the @c lib_description property. If the @p value parameter is 0, @p size will be 
 * modified to return the required buffer length for the description.
 *
 * @param[in] index Not used.
 * @param[out] value A @c char* buffer to be filled with the description. Can be 0 (NULL).
 * @param[in,out] size If @p value is non-null, this parameter is assumed to contain the maximum number of elements in the
 *             buffer. After completion, @p size is always modified to contain the length of the resulting array.
 *
 * @return Always returns WError::ERR_OK.
 */
int WSESWrapperBase::getLibDescription(int index, void *value, int &size)
{
  char *strValue = reinterpret_cast<char *>(value);
  if (strValue != 0)
    strValue[std::string(SESWrapperNS::gLibraryName).copy(strValue, size)] = 0;
  size = strlen(SESWrapperNS::gLibraryName);
  return WError::ERR_OK;
}

/*!
 * Getter for the @c lib_version property. If the @p value parameter is 0, @p size will be
 * modified to return the required buffer length for the version.
 *
 * @param[in] index Not used.
 * @param[out] value A @c char* buffer to be filled with the version. The syntax is @code <major>.<minor>.<build> @endcode.
 *              Can be 0 (NULL).
 * @param[in,out] size If @p value is non-null, this parameter is assumed to contain the maximum number of elements in the
 *             buffer. After completion, @p size is always modified to contain the length of the resulting array.
 *
 * @return Always returns WError::ERR_OK.
 */
int WSESWrapperBase::getLibVersion(int index, void *value, int &size)
{
  char *strValue = reinterpret_cast<char *>(value);
  if (strValue != 0)
    strValue[std::string(SESWrapperNS::gVersion).copy(strValue, size)] = 0;
  size = strlen(SESWrapperNS::gVersion);
	return WError::ERR_OK;
}

/*!
 * Getter for the @c lib_error variable. If the @p value parameter is 0, @p size will be
 * modified to return the required buffer length for the error indicated by @p index.
 *
 * @param[in] index An error code, e.g. from a previously returned function. If @p index is not a valid error code,
 *              the error string is "Unknown Error".
 * @param[out] value A @c char* buffer to be filled with the error message connected to error code @p index. Can be 0 (NULL).
 * @param[in,out] size If @p value is non-null, this parameter is assumed to contain the maximum number of elements in the
 *             buffer. After completion, @p size is always modified to contain the length of the resulting array.
 * 
 * @return Always returns WError::ERR_OK;
 */
int WSESWrapperBase::getLibError(int index, void *value, int &size)
{
  char *strValue = reinterpret_cast<char *>(value);
  std::string message = errors_->message(index);

  if (strValue != 0)
    strValue[message.copy(strValue, size)] = 0;
  size = message.length();
	return WError::ERR_OK;
}

/*!
 * Getter for the @c lib_working_dir property. If the @p value parameter is 0, @p size will be
 * modified to return the required buffer length.
 *
 * @param[in] index Not used.
 * @param[out] value A @c char* buffer to be filled with the full path of the current working directory. Can be 0 (NULL).
 * @param[in,out] size If @p value is non-null, this parameter is assumed to contain the maximum number of elements in the
 *             buffer. After completion, @p size is always modified to contain the length of the resulting array.
 *
 * @return Always returns WError::ERR_OK.
 */
int WSESWrapperBase::getLibWorkingDir(int index, void *value, int &size)
{
  char *strValue = reinterpret_cast<char *>(value);
  if (strValue != 0)
    getcwd(strValue, size);
  else
  {
    strValue = getcwd(0, 0);
    size = strlen(strValue);
    free(strValue);
  }
	return WError::ERR_OK;
}

/*!
 * Getter for the @c instrument_status property. The @p value parameter must be a pointer to an 32-bit integer.
 *
 * @param[in] index Not used.
 * @param[out] value Pointer to a 32-bit integer. Possible values are given by SesNS::InstrumentStatus.
 * @param[in,out] size Not used.
 *
 * @return Always returns WError::ERR_OK.
 */
int WSESWrapperBase::getInstrumentStatus(int index, void *value, int &size)
{
  int *status = reinterpret_cast<int *>(value);
  if (lib_->isLoaded() && lib_->GDS_GetStatus != 0)
    lib_->GDS_GetStatus(status);
  else
    *status = SesNS::NonOperational;

  return WError::ERR_OK;
}

/*!
 * Getter for the @c always_delay_region property. The @p value parameter must be a pointer to a 1-byte boolean.
 * If @p value is 1 or @c true, there is a delay before starting a measurement.
 *
 * @param[in] index Not used.
 * @param[out] value Pointer to a 1-byte boolean.
 * @param[in,out] size Not used.
 *
 * @return If WSESWrapperMain::loadInstrument() has not been called, returns WError::ERR_NO_INSTRUMENT, otherwise WError::ERR_OK.
 */
int WSESWrapperBase::getAlwaysDelayRegion(int index, void *value, int &size)
{
  if (!instrumentLoaded_)
    return WError::ERR_NO_INSTRUMENT;

  bool *boolValue = reinterpret_cast<bool *>(value);
  lib_->GDS_GetOption(SesNS::AlwaysDelayRegion, boolValue);
  return WError::ERR_OK;
}

/*!
 * Getter for the @c allow_io_with_detector property. The @p value parameter must be a pointer to a 1-byte boolean.
 * If @p value is 1 or @c true, external I/O signalling and detector can be used simultaneously.
 *
 * @param[in] index Not used.
 * @param[out] value Pointer to a 1-byte boolean.
 * @param[in,out] size Not used.
 *
 * @return If WSESWrapperMain::loadInstrument() has not been called, returns WError::ERR_NO_INSTRUMENT, otherwise WError::ERR_OK.
 */
int WSESWrapperBase::getAllowIOWithDetector(int index, void *value, int &size)
{
  if (!instrumentLoaded_)
    return WError::ERR_NO_INSTRUMENT;

  bool *boolValue = reinterpret_cast<bool *>(value);
  lib_->GDS_GetOption(SesNS::AllowSignalsWithDetector, boolValue);
  return WError::ERR_OK;
}

/*!
 * Getter for the @c instrument_model property. If the @p value parameter is 0, @p size will be
 * modified to return the required buffer length. If WSESWrapperMain::loadInstrument() has not been called,
 * @p value is usually empty, but do not rely on it.
 *
 * @param[in] index Not used.
 * @param[out] value A @c char* buffer to be filled with the name of the instrument model. Can be 0 (NULL).
 * @param[in,out] size If @p value is non-null, this parameter is assumed to contain the maximum number of elements in the
 *             buffer. After completion, @p size is always modified to contain the length of the resulting array.
 *
 * @return Always returns WError::ERR_OK.
 */
int WSESWrapperBase::getInstrumentModel(int index, void *value, int &size)
{
  char *strValue = reinterpret_cast<char *>(value);
  if (strValue != 0)
    strValue[std::string(sesInstrumentInfo_.Model).copy(strValue, size)] = 0;
  size = strlen(sesInstrumentInfo_.Model);
	return WError::ERR_OK;
}

/*!
 * Getter for the @c instrument_serial_no property. If the @p value parameter is 0, @p size will be
 * modified to return the required buffer length. If WSESWrapperMain::loadInstrument() has not been called, @p value
 * is usually empty, but do not rely on it.
 *
 * @param[in] index Not used.
 * @param[out] value A @c char* buffer to be filled with the name of the instrument serial number. Can be 0 (NULL).
 * @param[in,out] size If @p value is non-null, this parameter is assumed to contain the maximum number of elements in the
 *             buffer. After completion, @p size is always modified to contain the length of the resulting array.
 *
 * @return Always returns WError::ERR_OK.
 */
int WSESWrapperBase::getInstrumentSerialNo(int index, void *value, int &size)
{
  char *strValue = reinterpret_cast<char *>(value);
  if (strValue != 0)
    strValue[std::string(sesInstrumentInfo_.SerialNo).copy(strValue, size)] = 0;
  size = strlen(sesInstrumentInfo_.SerialNo);
	return WError::ERR_OK;
}

/*!
 * Getter for the @c detector_info property. 
 *
 * @param[in] index Not used.
 * @param[out] value A pointer to a SESWrapperNS::DetectorInfo structure.
 * @param[in,out] size Not used.
 *
 * @return WError::ERR_NO_INSTRUMENT if WSESWrapperMain::loadInstrument() has not
 *         been called, otherwise WError::ERR_OK.
 */
int WSESWrapperBase::getDetectorInfo(int index, void *value, int &size)
{
  if (!instrumentLoaded_)
    return WError::ERR_NO_INSTRUMENT;

  SESWrapperNS::DetectorInfo *detectorInfo = reinterpret_cast<DetectorInfo *>(value);

  if (detectorInfo != 0)
  {
    SesNS::WDetectorInfo sesDetectorInfo;
    lib_->GDS_GetDetectorInfo(&sesDetectorInfo);
    detectorInfo->adcPresent_ = sesDetectorInfo.ADCPresent;
    detectorInfo->discPresent_ = sesDetectorInfo.DiscPresent;
    detectorInfo->frameRate_ = sesDetectorInfo.FramesPerSec;
    detectorInfo->maxChannels_ = sesDetectorInfo.MaxChannels;
    detectorInfo->maxSlices_ = sesDetectorInfo.MaxSlices;
    detectorInfo->timerControlled_ = sesDetectorInfo.TimerControlled;
    detectorInfo->xChannels_ = sesDetectorInfo.XChannels;
    detectorInfo->yChannels_ = sesDetectorInfo.YChannels;
  }

  return WError::ERR_OK;
}

/*!
 * Getter for the @c detector_region property. 
 *
 * @param[in] index Not used.
 * @param[out] value A pointer to a SESWrapperNS::DetectorRegion structure.
 * @param[in,out] size Not used.
 *
 * @return WError::ERR_NO_INSTRUMENT if WSesWrapperMain::loadInstrument() has not been called,
 *         otherwise WError::ERR_OK.
 */
int WSESWrapperBase::getDetectorRegion(int index, void *value, int &size)
{
  if (!instrumentLoaded_)
    return WError::ERR_NO_INSTRUMENT;

  SESWrapperNS::DetectorRegion *detectorRegion = reinterpret_cast<DetectorRegion *>(value);

  if (detectorRegion != 0)
  {
    SesNS::WDetector detector;
    lib_->GDS_GetGlobalDetector(&detector);
    detectorRegion->adcMode_ = detector.ADCMode;
    detectorRegion->firstXChannel_ = detector.FirstXChannel;
    detectorRegion->firstYChannel_ = detector.FirstYChannel;
    detectorRegion->lastXChannel_ = detector.LastXChannel;
    detectorRegion->lastYChannel_ = detector.LastYChannel;
    detectorRegion->slices_ = detector.Slices;
  }

  return WError::ERR_OK;
}

/*!
 * Getter for the @c element_set_count property. The @p value parameter must be a pointer to a 32-bit integer.
 *
 * @param[in] index Not used.
 * @param[out] value A 32-bit integer that will contain the number of available element sets. This value can later be used
 *              as an index to the @c element_set_from_index property getter.
 * @param[in,out] size Not used.
 *
 * @return WError::ERR_NO_INSTRUMENT if WSESWrapperMain::loadInstrument() has not been called,
 *         otherwise WError::ERR_OK.
 *
 * @see WSESWrapperBase::getElementSetFromIndex()
 */
int WSESWrapperBase::getElementSetCount(int index, void *value, int &size)
{
  if (!instrumentLoaded_)
    return WError::ERR_NO_INSTRUMENT;

  int *intValue = reinterpret_cast<int *>(value);

  if (intValue != 0)
    *intValue = elementSets_.size();

	return WError::ERR_OK;
}

/*!
 * Getter for the @c element_set property. If the @p value parameter is 0, @p size will be
 * modified to return the required buffer length for the requested element set.
 *
 * @param[in] index If set to -1, obtains the current element set. If 0 <= @p index < @c element_set_count, obtains the element set name for that index.
 * @param[out] value A @c char* buffer to be filled with an element set. Can be 0 (NULL).
 * @param[in,out] size If @p value is non-null, this parameter is assumed to contain the maximum number of elements in the
 *             buffer. After completion, @p size is always modified to contain the length of the resulting buffer.
 *
 * @return WError::ERR_NO_INSTRUMENT if WSESWrapperMain::loadInstrument() has not been called, WError::ERR_FAIL on
 *         failure to report the current element set, WError::ERR_INDEX if index is out-of-range, otherwise WError::ERR_OK.
 */
int WSESWrapperBase::getElementSet(int index, void *value, int &size)
{
  if (!instrumentLoaded_)
    return WError::ERR_NO_INSTRUMENT;

  char *strValue = reinterpret_cast<char *>(value);
  int errorCode = WError::ERR_OK;

  if (index == -1)
  {
    if (lib_->GDS_GetCurrElementSet(strValue, &size) != 0)
	    errorCode =  WError::ERR_FAIL;
  }
  else if (index >= 0 && index < int(elementSets_.size()))
  {
    if (strValue != 0)
      strValue[elementSets_[index].copy(strValue, size)] = 0;
    size = strlen(strValue);
  }
  else
    errorCode = WError::ERR_INDEX;

  return errorCode;
}

/*!
 * Getter for the @c lens_mode_count property. The @p value parameter must be a pointer to a 32-bit integer.
 *
 * @param[in] index Not used.
 * @param[out] value A 32-bit integer that will contain the number of available lens modes. This value can later be used
 *              as an index to the @c lens_mode_from_index property getter.
 * @param[in,out] size Not used.
 *
 * @return WError::ERR_NO_INSTRUMENT if WSESWrapperMain::loadInstrument() has not been called,
 *         otherwise WError::ERR_OK.
 *
 * @see WSESWrapperBase::getLensModeFromIndex(), WSESWrapperBase::getLensMode()
 */
int WSESWrapperBase::getLensModeCount(int index, void *value, int &size)
{
  if (!instrumentLoaded_)
    return WError::ERR_NO_INSTRUMENT;

  int *intValue = reinterpret_cast<int *>(value);

  if (intValue != 0)
    *intValue = lensModes_.size();

	return WError::ERR_OK;
}

/*!
 * Getter for the @c lens_mode property. If the @p value parameter is 0, @p size will be
 * modified to return the required buffer length for the requested lens mode.
 *
 * @param[in] index If set to -1, obtains the current lens mode. If 0 <= @p index < @c lens_mode_count, obtains the lens mode name for that index.
 * @param[out] value A @c char* buffer to be filled with the current lens mode. Can be 0 (NULL).
 * @param[in,out] size If @p value is 0, this parameter is modified with the required size of the buffer, otherwise it is
 *             used to determine the size of @c value.
 *
 * @return WError::ERR_NO_INSTRUMENT if WSESWrapperMain::loadInstrument() has not been called, WError::ERR_FAIL on
 *         failure to report the current lens mode, WError::ERR_INDEX if index is out-of-range, otherwise WError::ERR_OK.
 */
int WSESWrapperBase::getLensMode(int index, void *value, int &size)
{
  if (!instrumentLoaded_)
    return WError::ERR_NO_INSTRUMENT;

  char *strValue = reinterpret_cast<char *>(value);
  int errorCode = WError::ERR_OK;

  if (index == -1)
  {
    if (lib_->GDS_GetCurrLensMode(strValue, &size) != 0)
	    errorCode =  WError::ERR_FAIL;
  }
  else if (index >= 0 && index < int(lensModes_.size()))
  {
    if (strValue != 0)
      strValue[lensModes_[index].copy(strValue, size)] = 0;
    size = strlen(strValue);
  }
  else
    errorCode = WError::ERR_INDEX;

  return errorCode;
}

/*!
 * Getter for the @c pass_energy_count property. The @p value parameter must be a pointer to a 32-bit integer.
 * @note The number of available pass energies is dependent on the current lens mode. If you change the lens
 *       mode, you need to update your internal list of pass energies, beginning with this function.
 *
 * @param[in] index Not used.
 * @param[out] value A 32-bit integer that will contain the number of available lens modes. This value can later be used
 *              as an index to the @c lens_mode_from_index property getter.
 * @param[in,out] size Not used.
 *
 * @return WError::ERR_NO_INSTRUMENT if WSESWrapperMain::loadInstrument() has not been called,
 *         otherwise WError::ERR_OK.
 *
 * @see WSESWrapperBase::getLensModeFromIndex(), WSESWrapperBase::getLensMode()
 */
int WSESWrapperBase::getPassEnergyCount(int index, void *value, int &size)
{
  if (!instrumentLoaded_)
    return WError::ERR_NO_INSTRUMENT;

  int *intValue = reinterpret_cast<int *>(value);
  int errorCode = WError::ERR_OK;

  if (intValue != 0)
    *intValue = passEnergies_.size();

	return WError::ERR_OK;
}

/*!
 * Getter for the @c pass_energy property. The @p value parameter must be a pointer to a @c double (8-byte floating
 * point).
 *
 * @param[in] index If set to -1, obtains the current pass energy. If 0 <= @p index < @c pass_energy_count, obtains the pass energy for that index.
 * @param[out] value A pointer to a @c double to be filled with the current pass energy.
 * @param[in,out] size Not used.
 *
 * @return WError::ERR_NO_INSTRUMENT if WSESWrapperMain::loadInstrument() has not been called, WError::ERR_FAIL on
 *         failure to report the current pass energy, WError::ERR_INDEX if index is out-of-range, otherwise WError::ERR_OK.
 */
int WSESWrapperBase::getPassEnergy(int index, void *value, int &size)
{
  if (!instrumentLoaded_)
    return WError::ERR_NO_INSTRUMENT;

  double *doubleValue = reinterpret_cast<double *>(value);
  int errorCode = WError::ERR_OK;

  if (index == -1)
  {
    if (lib_->GDS_GetCurrPassEnergy(doubleValue) != 0)
	    errorCode =  WError::ERR_FAIL;
  }
  else if (index >= 0 && index < int(passEnergies_.size()))
  {
    if (doubleValue != 0)
      *doubleValue = passEnergies_[index];
  }
  else
    errorCode = WError::ERR_INDEX;

  return errorCode;
}

/*!
 * Getter for the @c analyzer_region property. 
 *
 * @param[in] index Not used.
 * @param[out] value A pointer to a SESWrapperNS::AnalyzerRegion structure.
 * @param[in,out] size Not used.
 *
 * @return WError::ERR_NO_INSTRUMENT if WSesWrapperMain::loadInstrument() has not been called,
 *         otherwise WError::ERR_OK.
 */
int WSESWrapperBase::getAnalyzerRegion(int index, void *value, int &size)
{
  if (!instrumentLoaded_)
    return WError::ERR_NO_INSTRUMENT;

  SESWrapperNS::AnalyzerRegion *analyzerRegion = reinterpret_cast<SESWrapperNS::AnalyzerRegion *>(value);
  if (analyzerRegion != 0)
  {
	printf("\n\n\n######## Setting analyzerRegion = sesRegion in wseswrapperbase.cpp ########\n");
	printf("\nsesRegion Energy Mode = %d\n", sesRegion_.Fixed);
	printf("sesRegion Energy Step = %f\n", sesRegion_.EnergyStep);
	printf("sesRegion Low Energy = %f\n", sesRegion_.LowEnergy);
	printf("sesRegion Centre Energy = %f\n", sesRegion_.FixEnergy);
	printf("sesRegion High Energy = %f\n", sesRegion_.HighEnergy);
	printf("sesRegion Dwell Time = %d\n\n\n", sesRegion_.StepTime);
	analyzerRegion->centerEnergy_ = sesRegion_.FixEnergy;
    analyzerRegion->dwellTime_ = sesRegion_.StepTime;
    analyzerRegion->energyStep_ = sesRegion_.EnergyStep;
    analyzerRegion->fixed_ = sesRegion_.Fixed;
    analyzerRegion->highEnergy_ = sesRegion_.HighEnergy;
    analyzerRegion->lowEnergy_ = sesRegion_.LowEnergy;
    printf("\nanalyzerRegion Energy Mode = %d\n", analyzerRegion->fixed_);
    printf("analyzerRegion Energy Step = %f\n", analyzerRegion->energyStep_);
    printf("analyzerRegion Low Energy = %f\n", analyzerRegion->lowEnergy_);
    printf("analyzerRegion Centre Energy = %f\n", analyzerRegion->centerEnergy_);
    printf("analyzerRegion High Energy = %f\n", analyzerRegion->highEnergy_);
    printf("analyzerRegion Dwell Time = %d\n\n", analyzerRegion->dwellTime_);
  }
  size = sizeof(SESWrapperNS::AnalyzerRegion);
  return WError::ERR_OK;
}

/*!
 * Getter for the @c use_external_io property. 
 *
 * @param[in] index Not used.
 * @param[out] value A pointer to a 1-byte boolean. If @c value is 0 (or @c false), no communication with the external
 *              I/O card is made. One example of an external I/O card is the DAQ cards from National Instruments.
 * @param[in,out] size Not used.
 *
 * @return WError::ERR_NO_INSTRUMENT if WSesWrapperMain::loadInstrument() has not been called,
 *         otherwise WError::ERR_OK.
 */
int WSESWrapperBase::getUseExternalIO(int index, void *value, int &size)
{
  if (!instrumentLoaded_)
    return WError::ERR_NO_INSTRUMENT;

  bool *boolValue = reinterpret_cast<bool *>(value);
  if (boolValue != 0)
    *boolValue = usingSignals_;
  return WError::ERR_OK;
}

/*!
 * Getter for the @c use_detector property. 
 *
 * @param[in] index Not used.
 * @param[out] value A pointer to a 1-byte boolean. If @c value is 0 (or @c false), no communication with the detector
 *              is made.
 * @param[in,out] size Not used.
 *
 * @return WError::ERR_NO_INSTRUMENT if WSesWrapperMain::loadInstrument() has not been called,
 *         otherwise WError::ERR_OK.
 */
int WSESWrapperBase::getUseDetector(int index, void *value, int &size)
{
  if (!instrumentLoaded_)
    return WError::ERR_NO_INSTRUMENT;

  bool *boolValue = reinterpret_cast<bool *>(value);
  if (boolValue != 0)
    *boolValue = usingDetector_;
  return WError::ERR_OK;
}

/*!
 * Getter for the @c region_name property. 
 *
 * @param[in] index Not used.
 * @param[out] value A @c char* buffer to be filled with the name of the region. Can be 0 (NULL).
 * @param[in,out] size If @p value is 0, this parameter is modified with the required length of the region name, otherwise
 *             it is used to determine the size of @c value.
 *
 * @return Always returns WError::ERR_OK.
 */
int WSESWrapperBase::getRegionName(int index, void *value, int &size)
{
  char *strValue = reinterpret_cast<char *>(value);
  if (strValue != 0)
    strValue[std::string(sesRegion_.Name).copy(strValue, size)] = 0;
  size = strlen(sesRegion_.Name);
	return WError::ERR_OK;
}

/*!
 * Getter for the @c temp_file_name property.
 *
 * @param[in] index Not used.
 * @param[out] value A @c char* buffer to be filled with the name of the temporary working file created during acquisition.
 *              Can be 0 (NULL).
 * @param[in,out] size If @p value is 0, this parameter is modified with the required length of the file name, otherwise
 *             it is used to determine the size of @c value.
 *
 * @return Always returns WError::ERR_OK.
 */
int WSESWrapperBase::getTempFileName(int index, void *value, int &size)
{
  char *strValue = reinterpret_cast<char *>(value);
  if (strValue != 0)
    strValue[tempFileName_.copy(strValue, size)] = 0;
  size = tempFileName_.length();
  return WError::ERR_OK;
}

/*!
 * Getter for the @c reset_data_between_iterations property.
 *
 * @param[in] index Not used.
 * @param[out] value A pointer to a 1-byte boolean. If @c value is 0 (or @c false), data will be accumulated
 *                   between each call to startAcquisition unless initAcquisition is called. If it is 1 (or
 *                   @c true), all data is reset to 0 between the iterations, even if initAcquisition is not called.
 * @param[in,out] size Not used.
 *
 * @return Always returns WError::ERR_OK.
 */
int WSESWrapperBase::getResetDataBetweenIterations(int index, void *value, int &size)
{
  bool *boolValue = reinterpret_cast<bool *>(value);
  if (boolValue != 0)
    *boolValue = resetDataBetweenIterations_;
  return WError::ERR_OK;
}

// Property setters

/*!
 * Stub for the read-only properties.
 *
 * @param[in] index Not used.
 * @param[in] value Not used.
 *
 * @return Always returns WError::ERR_READONLY to indicate that an attempt was made to modify a read-only property.
 */
int WSESWrapperBase::readOnlyStub(int index, const void *value)
{
  return WError::ERR_READONLY;
}

/*!
 * Setter for the @c lib_working_dir property. Changes the working directory for the library.
 *
 * @param[in] index Not used.
 * @param[in] value A null-terminated C string that contains the path to the new working directory.
 *
 * @return Always returns WError::ERR_OK.
 */
int WSESWrapperBase::setLibWorkingDir(int index, const void *value)
{
  const char *strValue = reinterpret_cast<const char *>(value);
  printf("setting working dir = %s\n", value);
  if (strValue != 0)
  {
    _chdir(strValue);
    char *buffer = getcwd(0, 0);
    workingDir_ = buffer;
    free(buffer);
  }
	return WError::ERR_OK;
}

/*!
 * Setter for the @c always_delay_region property. If @c true (1), a delay will be made before starting
 * the acquisition of a region.
 *
 * @param[in] index Not used.
 * @param[in] value A 1-byte boolean of value @c true (1) or @c false (0). 
 *
 * @return WError::ERR_NOT_INITIALIZED if the library has not been successfully initialized, otherwise WError::ERR_OK.
 */
int WSESWrapperBase::setAlwaysDelayRegion(int index, const void *value)
{
  const bool *boolValue = reinterpret_cast<const bool *>(value);
  if (!lib_->GDS_SetOption == 0)
    return WError::ERR_NOT_INITIALIZED;
  lib_->GDS_SetOption(SesNS::AlwaysDelayRegion, boolValue);
	return WError::ERR_OK;
}

/*!
 * Setter for the @c allow_io_with_detector property. If @c true (1), external I/O communication will be
 * possible simultaneously with detector communication.
 *
 * @param[in] index Not used.
 * @param[in] value A 1-byte boolean of value @c true (1) or @c false (0). 
 *
 * @return WError::ERR_NOT_INITIALIZED if the library has not been successfully initialized, otherwise WError::ERR_OK.
 */
int WSESWrapperBase::setAllowIOWithDetector(int index, const void *value)
{
  const bool *boolValue = reinterpret_cast<const bool *>(value);
  if (!lib_->GDS_SetOption == 0)
    return WError::ERR_NOT_INITIALIZED;
  lib_->GDS_SetOption(SesNS::AllowSignalsWithDetector, boolValue);
	return WError::ERR_OK;
}

/*!
 * Setter for the @c detector_region property. This will set up a new ROI (Region of Interest) for the acquisition.
 * The values in the detector region will be validated through a call to the GDS_CheckRegion() function.
 * 
 * @param[in] index Not used.
 * @param[in] value A pointer to a SESWrapperNS::DetectorRegion structure.
 *
 * @return WError::ERR_NO_INSTRUMENT if WSESWrapper::loadInstrument() has not been called, WError::ERR_INCORRECT_DETECTOR_REGION
 *         is an error was detected in the detector region, otherwise WError::ERR_OK.
 */
int WSESWrapperBase::setDetectorRegion(int index, const void *value)
{
  if (!instrumentLoaded_)
    return WError::ERR_NO_INSTRUMENT;

  int status = SesNS::NonOperational;
  lib_->GDS_GetStatus(&status);
  if (status == SesNS::Running)
    return WError::ERR_ACQUIRING;

  const DetectorRegion *detectorRegion = reinterpret_cast<const SESWrapperNS::DetectorRegion *>(value);
  SesNS::WRegion region;
  char buffer[256];
  int size = 256;
  double passEnergy = 0;
  lib_->GDS_GetCurrLensMode(buffer, &size);
  lib_->GDS_GetCurrPassEnergy(&passEnergy);
  memmove(&region, &sesRegion_, sizeof(SesNS::WRegion));
  std::copy(buffer, buffer + strlen(buffer), region.LensMode);
  region.PassEnergy = passEnergy;
  region.ADCMask = 0;
  region.ADCMode = detectorRegion->adcMode_;
  region.DiscLvl = 0;
  region.FirstXChannel = detectorRegion->firstXChannel_;
  region.FirstYChannel = detectorRegion->firstYChannel_;
  region.LastXChannel = detectorRegion->lastXChannel_;
  region.LastYChannel = detectorRegion->lastYChannel_;
  region.Slices = detectorRegion->slices_;
  region.UseRegionDetector = true;
  int tmp1 = 0;
  double tmp2 = 0;
  double tmp3 = 0;
  lib_->GDS_CheckRegion(&region, &tmp1, &tmp2, &tmp3);

  SesNS::WDetector detector;
  detector.ADCMask = region.ADCMask;
  detector.ADCMode = region.ADCMode;
  detector.DiscLvl = region.DiscLvl;
  detector.FirstXChannel = region.FirstXChannel;
  detector.FirstYChannel = region.FirstYChannel;
  detector.LastXChannel = region.LastXChannel;
  detector.LastYChannel = region.LastYChannel;
  detector.Slices = region.Slices;
  return lib_->GDS_SetGlobalDetector(&detector) == 0 ? WError::ERR_OK : WError::ERR_INCORRECT_DETECTOR_REGION;
}

/*!
 * Setter for the @c element_set property.
 *
 * @param[in] index Must be set to -1. Values between 0 and @c element_set_count - 1 are index for future use.
 * @param[in] value A null-terminated string that specifies the name of the new element set.
 * 
 * @return WError::ERR_NO_INSTRUMENT if WSESWrapperMain::loadInstrument() has not been called,
 *         WError::ERR_INCORRECT_ELEMENT_SET if the given element set is not available, otherwise WError::ERR_OK.
 */
int WSESWrapperBase::setElementSet(int index, const void *value)
{
  if (!instrumentLoaded_)
    return WError::ERR_NO_INSTRUMENT;

  if (index != -1)
    return WError::ERR_NOT_APPLICABLE;

  const char *strValue = reinterpret_cast<const char *>(value);
  return lib_->GDS_SetElementSet(strValue) == 0 ? WError::ERR_OK : WError::ERR_INCORRECT_ELEMENT_SET;
}

/*!
 * Setter for the @c element_set property. This function reloads the pass energy list when successful.
 * This requires updates for the pass energy lists for the calling application.
 *
 * @param[in] index  Must be set to -1. Values between 0 and @c element_set_count - 1 are index for future use.
 * @param[in] value A null-terminated string that specifies the name of the new lens mode.
 * 
 * @return WError::ERR_NO_INSTRUMENT if WSESWrapperMain::loadInstrument() has not been called,
 *         WError::ERR_INCORRECT_LENS_MODE if the given lens mode is not available, otherwise WError::ERR_OK.
 */
int WSESWrapperBase::setLensMode(int index, const void *value)
{
  if (!instrumentLoaded_)
    return WError::ERR_NO_INSTRUMENT;

  if (index != -1)
    return WError::ERR_NOT_APPLICABLE;

  const std::string strValue = reinterpret_cast<const char *>(value);
  int errorCode = lib_->GDS_SetLensMode(strValue.c_str()) == 0 ? WError::ERR_OK : WError::ERR_INCORRECT_LENS_MODE;
  if (errorCode == WError::ERR_OK)
    sesRegion_.LensMode[strValue.copy(sesRegion_.LensMode, 32)] = 0;
	return errorCode;
}

/*!
 * Setter for the @c pass_energy property. If the pass energy to set is taken from a list of pass energies, make
 * sure to update that list after modifying the lens mode and before calling this function.
 *
 * @param[in] index  Must be set to -1. Values between 0 and @c element_set_count - 1 are index for future use.
 * @param[in] value A pointer to a @c double (8-byte floating point).
 * 
 * @return WError::ERR_NO_INSTRUMENT if WSESWrapperMain::loadInstrument() has not been called,
 *         WError::ERR_INCORRECT_PASS_ENERGY if the given pass energy is not available, otherwise WError::ERR_OK.
 */
int WSESWrapperBase::setPassEnergy(int index, const void *value)
{
  if (!instrumentLoaded_)
    return WError::ERR_NO_INSTRUMENT;

  if (index != -1)
    return WError::ERR_NOT_APPLICABLE;

  double doubleValue = *reinterpret_cast<const double *>(value);
  int errorCode = lib_->GDS_SetPassEnergy(doubleValue) == 0 ? WError::ERR_OK : WError::ERR_INCORRECT_PASS_ENERGY;
  doubleValue = sesRegion_.PassEnergy;
  if (lib_->GDS_GetCurrPassEnergy(&doubleValue) == 0)
    sesRegion_.PassEnergy = doubleValue;

	return errorCode;
}

/*!
 * Setter for the @c analyzer_region property. The members in the WAnalyzerRegion structure are stored
 * in an internal object for later use in a call to WSESWrapperMain::initAcquisition(). There is no immediate
 * checking done of the validity of the given values. If such a check is required before starting acquisition,
 * the WSESWrapperMain::checkAnalyzerRegion() can be called.
 *
 * @param[in] index Not used.
 * @param[in] value A pointer to a WAnalyzerRegion structure.
 *
 * @return Always returns WError::ERR_OK.
 */
int WSESWrapperBase::setAnalyzerRegion(int index, const void *value)
{
  const WAnalyzerRegion *analyzerRegion = reinterpret_cast<const WAnalyzerRegion *>(value);
  if (analyzerRegion != 0)
  {
	printf("\n\n\n######## Setting sesRegion = analyzerRegion in wseswrapperbase.cpp ########\n");
	printf("\nanalyzerRegion Energy Mode = %d\n", analyzerRegion->fixed_);
	printf("analyzerRegion Energy Step = %f\n", analyzerRegion->energyStep_);
	printf("analyzerRegion Low Energy = %f\n", analyzerRegion->lowEnergy_);
	printf("analyzerRegion Centre Energy = %f\n", analyzerRegion->centerEnergy_);
	printf("analyzerRegion High Energy = %f\n", analyzerRegion->highEnergy_);
	printf("analyzerRegion Dwell Time = %d\n\n", analyzerRegion->dwellTime_);
	sesRegion_.DriftRegion = false;
    sesRegion_.EnergyStep = analyzerRegion->energyStep_; //0.2
    sesRegion_.ExcEnergy = 0;
    sesRegion_.Fixed = analyzerRegion->fixed_;
    sesRegion_.FixEnergy = analyzerRegion->centerEnergy_;
    sesRegion_.Grating = 0;
    sesRegion_.HighEnergy = analyzerRegion->highEnergy_;
    sesRegion_.Illumination = 0;
    sesRegion_.Kinetic = true;
    sesRegion_.LowEnergy = analyzerRegion->lowEnergy_;
    sesRegion_.Order = 0;
    sesRegion_.Slit = 0;
    sesRegion_.StepTime = analyzerRegion->dwellTime_; //0.1
    sesRegion_.UseRegionDetector = false;
    printf("\n\nsesRegion Energy Mode = %d\n", sesRegion_.Fixed);
    printf("sesRegion Energy Step = %f\n", sesRegion_.EnergyStep);
    printf("sesRegion Low Energy = %f\n", sesRegion_.LowEnergy);
    printf("sesRegion Centre Energy = %f\n", sesRegion_.FixEnergy);
    printf("sesRegion High Energy = %f\n", sesRegion_.HighEnergy);
    printf("sesRegion Dwell Time = %d\n\n\n", sesRegion_.StepTime);
  }
  return WError::ERR_OK;
}

/*!
 * Setter for the @c use_external_io property. Modify this to toggle the use of the external I/O card, if present.
 *
 * @param[in] index Not used.
 * @param[in] value A pointer to a 1-byte boolean.
 *
 * @return Always returns WError::ERR_OK.
 */
int WSESWrapperBase::setUseExternalIO(int index, const void *value)
{
  const bool *boolValue = reinterpret_cast<const bool *>(value);
  if (boolValue != 0)
    usingSignals_ = *boolValue;
  return WError::ERR_OK;
}

/*!
 * Setter for the @c use_detector property. Modify this to toggle the use of the detector. Can be used in combination
 * with the @c use_external_io property.
 *
 * @param[in] index Not used.
 * @param[in] value A pointer to a 1-byte boolean.
 *
 * @return Always returns WError::ERR_OK.
 */
int WSESWrapperBase::setUseDetector(int index, const void *value)
{
  const bool *boolValue = reinterpret_cast<const bool *>(value);
  if (boolValue != 0)
    usingDetector_ = *boolValue;
  return WError::ERR_OK;
}

/*!
 * Setter for the @c region_name property. 
 *
 * @param[in] index Not used.
 * @param[in] value A null-terminated C string that specifies the region name. The name is limited to 32 characters,
 *              including the null character.
 *
 * @return Always returns WError::ERR_OK.
 */
int WSESWrapperBase::setRegionName(int index, const void *value)
{
  const std::string strValue = reinterpret_cast<const char *>(value);
  sesRegion_.Name[strValue.copy(sesRegion_.Name, 32)] = 0;
	return WError::ERR_OK;
}

/*!
 * Setter for the @c temp_file_name property. 
 *
 * @param[in] index Not used.
 * @param[in] value A null-terminated C string that specifies the name of the temporary file created during acquisition.
 *
 * @return Always returns WError::ERR_OK.
 */
int WSESWrapperBase::setTempFileName(int index, const void *value)
{
  tempFileName_ = reinterpret_cast<const char *>(value);
  return WError::ERR_OK;
}

/*!
 * Setter for the @c reset_data_between_iterations property.
 *
 * @param[in] index Not used.
 * @param[in] value A pointer to a 1-byte boolean. 
 *
 * @return Always returns WError::ERR_OK.
 */
int WSESWrapperBase::setResetDataBetweenIterations(int index, const void *value)
{
  const bool *boolValue = reinterpret_cast<const bool *>(value);
  if (boolValue != 0)
    resetDataBetweenIterations_ = *boolValue;
  return WError::ERR_OK;
}

/*!
 * Reloads the elemet sets. Called after loading a new instrument configuration.
 *
 * @return @c true if successful.
 */
bool WSESWrapperBase::loadElementSets()
{
  elementSets_.clear();

  /*if (!instrumentLoaded_)
    return false;*/

  char *buffer = 0;
  int bufferSize = 0;
  
  bool success = false;
  if (lib_->GDS_GetElementSets(buffer, &bufferSize) == 0)
  {
    buffer = new char[bufferSize];
    if (lib_->GDS_GetElementSets(buffer, &bufferSize) == 0)
      success = true;

    splitSESList(buffer, bufferSize, elementSets_);
    delete[] buffer;
  }

  return success;
}

/*!
 * Loads the lens modes. Called after loading a new instrument configuration.
 *
 * @return @c true if successful.
 */
bool WSESWrapperBase::loadLensModes()
{
  lensModes_.clear();

  /*if (!instrumentLoaded_)
    return false;*/

  char *buffer = 0;
  int bufferSize = 0;
  
  bool success = false;
  if (lib_->GDS_GetLensModes(buffer, &bufferSize) == 0)
  {
    buffer = new char[bufferSize];
    if (lib_->GDS_GetLensModes(buffer, &bufferSize) == 0)
      success = true;

    splitSESList(buffer, bufferSize, lensModes_);
    delete[] buffer;
  }

  return success;
}

/*!
 * Reloads the Pass energies. Called after loading a new instrument configuration.
 *
 * @return @c true if successful.
 */
bool WSESWrapperBase::loadPassEnergies(const std::string &lensMode, DoubleVector &result)
{
  passEnergies_.clear();

  /*if (!instrumentLoaded_)
    return false;*/

  char *buffer = 0;
  int bufferSize = 0;
  bool success = false;

  result.clear();
  if (lib_->GDS_GetPassEnergies(lensMode.c_str(), buffer, &bufferSize) == 0)
  {
    buffer = new char[bufferSize];
    if (lib_->GDS_GetPassEnergies(lensMode.c_str(), buffer, &bufferSize) == 0 && buffer[0] != 0)
    {
      success = true;

      NameVector strResult;
      splitSESList(buffer, bufferSize, strResult);
      for (int i = 0; i < int(strResult.size()); i++)
        result.push_back(atof(strResult[i].c_str()));
    }
    delete[] buffer;
  }

  return success;
}

/*!
 * Helper function to split strings obtained as lists from calls to SESInstrument functions.
 *
 * @param[in] buffer The null-terminated string containing the elements to be split. Each element is assumed to be wrapped
 *               between quotation marks ("), and separated with space (e.g. '"element 1" "element 2" "element 3"').
 * @param[in] bufferSize The maximum size of the buffer, used as an extra security measure in case the string lacks the null termination.
 * @param[out] result a string vector containing the split elements.
 */
void WSESWrapperBase::splitSESList(const char *buffer, int bufferSize, NameVector &result)
{
  std::istringstream istr(buffer);
  while (istr.good())
  {
    std::string item;
    if (istr.peek() == '"')
    {
      istr.get();
      std::getline(istr, item, '"');
      result.push_back(item);
    }
    istr.get();
    istr >> std::ws;
  }

  //std::ostringstream ostr;

  //const char *p = buffer + 1;
  //for (;bufferSize > 0 && *p != 0;)
  //{
  //  for (; *p != '"' && bufferSize > 0; *p++, bufferSize--)
  //    ostr << *p;
  //  ostr << std::ends;
  //  result.push_back(ostr.str());
  //  p += 3;
  //  bufferSize -= 3;
  //  ostr.seekp(0);
  //  ostr.clear();
  //}
}
