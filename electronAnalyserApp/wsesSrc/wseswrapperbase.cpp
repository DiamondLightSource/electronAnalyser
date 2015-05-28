#pragma warning(disable:4996)

#include "wseswrapperbase.h"
#include "wsesinstrument.h"
#include "werror.h"
#include "common.hpp"
#include "constants.h"

#define NOMINMAX
#include <windows.h>

#include <iostream>
#include <string>
#include <sstream>
#include <algorithm>
#include <direct.h>

#include <fstream>

using namespace SESWrapperNS;

/*!
 * \class WSESWrapperBase
 * This class collects all variables and objects for SESWrapper. 
 *
 * It serves as a container for property callbacks used when "setting" or "getting" values for the \ref properties_sec properties.
 */

/*!
 * Creates  a WSESWrapperBase instance. Also creates the hash table for the \ref properties_sec, where the property
 * names are linked to their corresponding callbacks through instances of the WVariable template class.
 *
 * \param[in] workingDir The current working directory.
 */
WSESWrapperBase::WSESWrapperBase()
: lib_(new WSESInstrument), instrumentLoaded_(false),
  activeDetectors_(0x0001), iteration_(0), startTime_(0),
  blockPointReady_(false), blockRegionReady_(false), resetDataBetweenIterations_(false)
{ 
  errors_ = WError::instance();
  
  std::string baseDir = ::getenv("SES_BASE_DIR");
  if (baseDir.empty())
    instrumentLibraryName_ = "dll\\SESInstrument.dll";
  else
    instrumentLibraryName_ = baseDir + "\\dll\\SESInstrument.dll";

  *sesInstrumentInfo_.Model = 0;
  *sesInstrumentInfo_.SerialNo = 0;

  // Create property database
  properties_.insert(PropertyKeyValue("lib_description", Property(this, &WSESWrapperBase::readOnlyStub, &WSESWrapperBase::getLibDescription, Property::TYPE_STRING)));
  properties_.insert(PropertyKeyValue("lib_version", Property(this, &WSESWrapperBase::readOnlyStub, &WSESWrapperBase::getLibVersion, Property::TYPE_STRING)));
  properties_.insert(PropertyKeyValue("lib_error", Property(this, &WSESWrapperBase::readOnlyStub, &WSESWrapperBase::getLibError, Property::TYPE_STRING)));
  properties_.insert(PropertyKeyValue("lib_working_dir", Property(this, &WSESWrapperBase::setLibWorkingDir, &WSESWrapperBase::getLibWorkingDir, Property::TYPE_STRING)));
  properties_.insert(PropertyKeyValue("instrument_library", Property(this, &WSESWrapperBase::setInstrumentLibrary, &WSESWrapperBase::getInstrumentLibrary, Property::TYPE_STRING)));
  properties_.insert(PropertyKeyValue("instrument_status", Property(this, &WSESWrapperBase::readOnlyStub, &WSESWrapperBase::getInstrumentStatus, Property::TYPE_INT32)));
  properties_.insert(PropertyKeyValue("always_delay_region", Property(this, &WSESWrapperBase::setAlwaysDelayRegion, &WSESWrapperBase::getAlwaysDelayRegion, Property::TYPE_BOOL)));
  properties_.insert(PropertyKeyValue("allow_io_with_detector", Property(this, &WSESWrapperBase::setAllowIOWithDetector, &WSESWrapperBase::getAllowIOWithDetector, Property::TYPE_BOOL)));
  properties_.insert(PropertyKeyValue("instrument_model", Property(this, &WSESWrapperBase::readOnlyStub, &WSESWrapperBase::getInstrumentModel, Property::TYPE_STRING)));
  properties_.insert(PropertyKeyValue("instrument_serial_no", Property(this, &WSESWrapperBase::readOnlyStub, &WSESWrapperBase::getInstrumentSerialNo, Property::TYPE_STRING)));
  properties_.insert(PropertyKeyValue("detector_info", Property(this, &WSESWrapperBase::readOnlyStub, &WSESWrapperBase::getDetectorInfo, Property::TYPE_DETECTORINFO)));
  properties_.insert(PropertyKeyValue("detector_region", Property(this, &WSESWrapperBase::setDetectorRegion, &WSESWrapperBase::getDetectorRegion, Property::TYPE_DETECTORREGION)));
  properties_.insert(PropertyKeyValue("element_set_count", Property(this, &WSESWrapperBase::readOnlyStub, &WSESWrapperBase::getElementSetCount, Property::TYPE_INT32)));
  properties_.insert(PropertyKeyValue("element_set", Property(this, &WSESWrapperBase::setElementSet, &WSESWrapperBase::getElementSet, Property::TYPE_STRING)));
  properties_.insert(PropertyKeyValue("element_name_count", Property(this, &WSESWrapperBase::readOnlyStub, &WSESWrapperBase::getElementNameCount, Property::TYPE_INT32)));
  properties_.insert(PropertyKeyValue("element_name", Property(this, &WSESWrapperBase::readOnlyStub, &WSESWrapperBase::getElementName, Property::TYPE_STRING)));
  properties_.insert(PropertyKeyValue("lens_mode_count", Property(this, &WSESWrapperBase::readOnlyStub, &WSESWrapperBase::getLensModeCount, Property::TYPE_INT32)));
  properties_.insert(PropertyKeyValue("lens_mode", Property(this, &WSESWrapperBase::setLensMode, &WSESWrapperBase::getLensMode, Property::TYPE_STRING)));
  properties_.insert(PropertyKeyValue("pass_energy_count", Property(this, &WSESWrapperBase::readOnlyStub, &WSESWrapperBase::getPassEnergyCount, Property::TYPE_INT32)));
  properties_.insert(PropertyKeyValue("pass_energy", Property(this, &WSESWrapperBase::setPassEnergy, &WSESWrapperBase::getPassEnergy, Property::TYPE_DOUBLE)));

  properties_.insert(PropertyKeyValue("analyzer_region", Property(this, &WSESWrapperBase::setAnalyzerRegion, &WSESWrapperBase::getAnalyzerRegion, Property::TYPE_ANALYZERREGION)));
  properties_.insert(PropertyKeyValue("use_external_io", Property(this, &WSESWrapperBase::setUseExternalIO, &WSESWrapperBase::getUseExternalIO, Property::TYPE_BOOL)));
  properties_.insert(PropertyKeyValue("use_detector", Property(this, &WSESWrapperBase::setUseDetector, &WSESWrapperBase::getUseDetector, Property::TYPE_BOOL)));
  properties_.insert(PropertyKeyValue("use_spin", Property(this, &WSESWrapperBase::setUseSpin, &WSESWrapperBase::getUseSpin, Property::TYPE_BOOL)));
  properties_.insert(PropertyKeyValue("region_name", Property(this, &WSESWrapperBase::setRegionName, &WSESWrapperBase::getRegionName, Property::TYPE_STRING)));
  properties_.insert(PropertyKeyValue("temp_file_name", Property(this, &WSESWrapperBase::setTempFileName, &WSESWrapperBase::getTempFileName, Property::TYPE_STRING)));
  properties_.insert(PropertyKeyValue("reset_data_between_iterations", Property(this, &WSESWrapperBase::setResetDataBetweenIterations, &WSESWrapperBase::getResetDataBetweenIterations, Property::TYPE_BOOL)));
  properties_.insert(PropertyKeyValue("use_binding_energy", Property(this, &WSESWrapperBase::setUseBindingEnergy, &WSESWrapperBase::getUseBindingEnergy, Property::TYPE_BOOL)));
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
 * Getter for the \c instrument_library property. If the \p value parameter is 0, \p size will be 
 * modified to return the required buffer length for the description.
 *
 * \param[in] index Not used.
 * \param[out] value A \c char* buffer to be filled with the instrument library file name. Can be 0 (NULL).
 * \param[in,out] size If \p value is non-null, this parameter is assumed to contain the maximum number of elements in the
 *             buffer. After completion, \p size is always modified to contain the length of the resulting array.
 *
 * \return Always returns WError::ERR_OK.
 */
int WSESWrapperBase::getInstrumentLibrary(int index, void *value, int &size)
{
  char *strValue = reinterpret_cast<char *>(value);
  if (strValue != 0)
    strValue[std::string(SESWrapperNS::gLibraryName).copy(strValue, size)] = 0;
  size = strlen(SESWrapperNS::gLibraryName);
  return WError::ERR_OK;
}

/*!
 * Getter for the \c lib_description property. If the \p value parameter is 0, \p size will be 
 * modified to return the required buffer length for the description.
 *
 * \param[in] index Not used.
 * \param[out] value A \c char* buffer to be filled with the description. Can be 0 (NULL).
 * \param[in,out] size If \p value is non-null, this parameter is assumed to contain the maximum number of elements in the
 *             buffer. After completion, \p size is always modified to contain the length of the resulting array.
 *
 * \return Always returns WError::ERR_OK.
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
 * Getter for the \c lib_version property. If the \p value parameter is 0, \p size will be
 * modified to return the required buffer length for the version.
 *
 * \param[in] index Not used.
 * \param[out] value A \c char* buffer to be filled with the version. The syntax is \code <major>.<minor>.<build> \endcode.
 *              Can be 0 (NULL).
 * \param[in,out] size If \p value is non-null, this parameter is assumed to contain the maximum number of elements in the
 *             buffer. After completion, \p size is always modified to contain the length of the resulting array.
 *
 * \return Always returns WError::ERR_OK.
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
 * Getter for the \c lib_error variable. If the \p value parameter is 0, \p size will be
 * modified to return the required buffer length for the error indicated by \p index.
 *
 * \param[in] index An error code, e.g. from a previously returned function. If \p index is not a valid error code,
 *              the error string is "Unknown Error".
 * \param[out] value A \c char* buffer to be filled with the error message connected to error code \p index. Can be 0 (NULL).
 * \param[in,out] size If \p value is non-null, this parameter is assumed to contain the maximum number of elements in the
 *             buffer. After completion, \p size is always modified to contain the length of the resulting array.
 * 
 * \return Always returns WError::ERR_OK;
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
 * Getter for the \c lib_working_dir property. If the \p value parameter is 0, \p size will be
 * modified to return the required buffer length.
 *
 * \param[in] index Not used.
 * \param[out] value A \c char* buffer to be filled with the full path of the current working directory. Can be 0 (NULL).
 * \param[in,out] size If \p value is non-null, this parameter is assumed to contain the maximum number of elements in the
 *             buffer. After completion, \p size is always modified to contain the length of the resulting array.
 *
 * \return Always returns WError::ERR_OK.
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
 * Getter for the \c instrument_status property. The \p value parameter must be a pointer to an 32-bit integer.
 *
 * \param[in] index Not used.
 * \param[out] value Pointer to a 32-bit integer. Possible values are given by SesNS::InstrumentStatus.
 * \param[in,out] size Not used.
 *
 * \return Always returns WError::ERR_OK.
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
 * Getter for the \c always_delay_region property. The \p value parameter must be a pointer to a 1-byte boolean.
 * If \p value is 1 or \c true, there is a delay before starting a measurement.
 *
 * \param[in] index Not used.
 * \param[out] value Pointer to a 1-byte boolean.
 * \param[in,out] size Not used.
 *
 * \return If WSESWrapperMain::loadInstrument() has not been called, returns WError::ERR_NO_INSTRUMENT, otherwise WError::ERR_OK.
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
 * Getter for the \c allow_io_with_detector property. The \p value parameter must be a pointer to a 1-byte boolean.
 * If \p value is 1 or \c true, external I/O signalling and detector can be used simultaneously.
 *
 * \param[in] index Not used.
 * \param[out] value Pointer to a 1-byte boolean.
 * \param[in,out] size Not used.
 *
 * \return If WSESWrapperMain::loadInstrument() has not been called, returns WError::ERR_NO_INSTRUMENT, otherwise WError::ERR_OK.
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
 * Getter for the \c instrument_model property. If the \p value parameter is 0, \p size will be
 * modified to return the required buffer length. If WSESWrapperMain::loadInstrument() has not been called,
 * \p value is usually empty, but do not rely on it.
 *
 * \param[in] index Not used.
 * \param[out] value A \c char* buffer to be filled with the name of the instrument model. Can be 0 (NULL).
 * \param[in,out] size If \p value is non-null, this parameter is assumed to contain the maximum number of elements in the
 *             buffer. After completion, \p size is always modified to contain the length of the resulting array.
 *
 * \return Always returns WError::ERR_OK.
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
 * Getter for the \c instrument_serial_no property. If the \p value parameter is 0, \p size will be
 * modified to return the required buffer length. If WSESWrapperMain::loadInstrument() has not been called, \p value
 * is usually empty, but do not rely on it.
 *
 * \param[in] index Not used.
 * \param[out] value A \c char* buffer to be filled with the name of the instrument serial number. Can be 0 (NULL).
 * \param[in,out] size If \p value is non-null, this parameter is assumed to contain the maximum number of elements in the
 *             buffer. After completion, \p size is always modified to contain the length of the resulting array.
 *
 * \return Always returns WError::ERR_OK.
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
 * Getter for the \c detector_info property. 
 *
 * \param[in] index Not used.
 * \param[out] value A pointer to a SESWrapperNS::DetectorInfo structure.
 * \param[in,out] size Not used.
 *
 * \return WError::ERR_NO_INSTRUMENT if WSESWrapperMain::loadInstrument() has not
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
 * Getter for the \c detector_region property. 
 *
 * \param[in] index Not used.
 * \param[out] value A pointer to a SESWrapperNS::DetectorRegion structure.
 * \param[in,out] size Not used.
 *
 * \return WError::ERR_NO_INSTRUMENT if WSESWrapperMain::loadInstrument() has not been called,
 *         otherwise WError::ERR_OK.
 */
int WSESWrapperBase::getDetectorRegion(int index, void *value, int &size)
{
  if (!instrumentLoaded_)
    return WError::ERR_NO_INSTRUMENT;

  SESWrapperNS::DetectorRegion *detectorRegion = reinterpret_cast<DetectorRegion *>(value);

  if (detectorRegion != 0)
  {
    detectorRegion->adcMode_ = sesRegion_.ADCMode;
    detectorRegion->firstXChannel_ = sesRegion_.FirstXChannel;
    detectorRegion->firstYChannel_ = sesRegion_.FirstYChannel;
    detectorRegion->lastXChannel_ = sesRegion_.LastXChannel;
    detectorRegion->lastYChannel_ = sesRegion_.LastYChannel;
    detectorRegion->slices_ = sesRegion_.Slices;
  }

  return WError::ERR_OK;
}

/*!
 * Getter for the \c element_set_count property. The \p value parameter must be a pointer to a 32-bit integer.
 *
 * \param[in] index Not used.
 * \param[out] value A 32-bit integer that will contain the number of available element sets. This value can later be used
 *              as an index to the \c element_set_from_index property getter.
 * \param[in,out] size Not used.
 *
 * \return WError::ERR_NO_INSTRUMENT if WSESWrapperMain::loadInstrument() has not been called,
 *         otherwise WError::ERR_OK.
 *
 * \see WSESWrapperBase::getElementSetFromIndex()
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
 * Getter for the \c element_set property. If the \p value parameter is 0, \p size will be
 * modified to return the required buffer length for the requested element set.
 *
 * \param[in] index If set to -1, obtains the current element set. If 0 <= \p index < \c element_set_count, obtains the element set name for that index.
 * \param[out] value A \c char* buffer to be filled with an element set. Can be 0 (NULL).
 * \param[in,out] size If \p value is non-null, this parameter is assumed to contain the maximum number of elements in the
 *             buffer. After completion, \p size is always modified to contain the length of the resulting buffer.
 *
 * \return WError::ERR_NO_INSTRUMENT if WSESWrapperMain::loadInstrument() has not been called, WError::ERR_FAIL on
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
    size = elementSets_[index].length();
  }
  else
    errorCode = WError::ERR_INDEX;

  return errorCode;
}

/*!
 * Getter for the \c element_name_count property. The \p value parameter must be a pointer to a 32-bit integer.
 *
 * \param[in] index Not used.
 * \param[out] value A 32-bit integer that will contain the number of available element names. This value can later be used
 *              as an index to the \c element_name property getter.
 * \param[in,out] size Not used.
 *
 * \return WError::ERR_NO_INSTRUMENT if WSESWrapperMain::loadInstrument() has not been called,
 *         otherwise WError::ERR_OK.
 *
 * \see WSESWrapperBase::getElementName()
 */
int WSESWrapperBase::getElementNameCount(int index, void *value, int &size)
{
  if (!instrumentLoaded_)
    return WError::ERR_NO_INSTRUMENT;

  int *intValue = reinterpret_cast<int *>(value);

  if (intValue != 0)
    *intValue = elementNames_.size();

	return WError::ERR_OK;
}

/*!
 * Getter for the \c element_name property. If the \p value parameter is 0, \p size will be
 * modified to return the required buffer length for the requested element name.
 *
 * \param[in] index If 0 <= \p index < \c element_name_count, obtains the element name for that index.
 * \param[out] value A \c char* buffer to be filled with an element name. Can be 0 (NULL).
 * \param[in,out] size If \p value is non-null, this parameter is assumed to contain the maximum size of the
 *             buffer.
 *
 * \return WError::ERR_NO_INSTRUMENT if WSESWrapperMain::loadInstrument() has not been called, WError::ERR_FAIL on
 *         failure to report the element name, WError::ERR_INDEX if index is out-of-range, otherwise WError::ERR_OK.
 */
int WSESWrapperBase::getElementName(int index, void *value, int &size)
{
  if (!instrumentLoaded_)
    return WError::ERR_NO_INSTRUMENT;

  char *strValue = reinterpret_cast<char *>(value);
  int errorCode = WError::ERR_OK;

  if (index >= 0 && index < int(elementNames_.size()))
  {
    if (strValue != 0)
      strValue[elementNames_[index].copy(strValue, size)] = 0;
  }
  else
    errorCode = WError::ERR_INDEX;

  return errorCode;
}

/*!
 * Getter for the \c lens_mode_count property. The \p value parameter must be a pointer to a 32-bit integer.
 *
 * \param[in] index Not used.
 * \param[out] value A 32-bit integer that will contain the number of available lens modes. This value can later be used
 *              as an index to the \c lens_mode_from_index property getter.
 * \param[in,out] size Not used.
 *
 * \return WError::ERR_NO_INSTRUMENT if WSESWrapperMain::loadInstrument() has not been called,
 *         otherwise WError::ERR_OK.
 *
 * \see WSESWrapperBase::getLensModeFromIndex(), WSESWrapperBase::getLensMode()
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
 * Getter for the \c lens_mode property. If the \p value parameter is 0, \p size will be
 * modified to return the required buffer length for the requested lens mode.
 *
 * \param[in] index If set to -1, obtains the current lens mode. If 0 <= \p index < \c lens_mode_count, obtains the lens mode name for that index.
 * \param[out] value A \c char* buffer to be filled with the current lens mode. Can be 0 (NULL).
 * \param[in,out] size If \p value is 0, this parameter is modified with the required size of the buffer, otherwise it is
 *             used to determine the size of \c value.
 *
 * \return WError::ERR_NO_INSTRUMENT if WSESWrapperMain::loadInstrument() has not been called, WError::ERR_FAIL on
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
    size = lensModes_[index].length();
  }
  else
    errorCode = WError::ERR_INDEX;

  return errorCode;
}

/*!
 * Getter for the \c pass_energy_count property. The \p value parameter must be a pointer to a 32-bit integer.
 * \note The number of available pass energies is dependent on the current lens mode. If you change the lens
 *       mode, you need to update your internal list of pass energies, beginning with this function.
 *
 * \param[in] index Not used.
 * \param[out] value A 32-bit integer that will contain the number of available lens modes. This value can later be used
 *              as an index to the \c lens_mode_from_index property getter.
 * \param[in,out] size Not used.
 *
 * \return WError::ERR_NO_INSTRUMENT if WSESWrapperMain::loadInstrument() has not been called,
 *         otherwise WError::ERR_OK.
 *
 * \see WSESWrapperBase::getLensModeFromIndex(), WSESWrapperBase::getLensMode()
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
 * Getter for the \c pass_energy property. The \p value parameter must be a pointer to a \c double (8-byte floating
 * point).
 *
 * \param[in] index If set to -1, obtains the current pass energy. If 0 <= \p index < \c pass_energy_count, obtains the pass energy for that index.
 * \param[out] value A pointer to a \c double to be filled with the current pass energy.
 * \param[in,out] size Not used.
 *
 * \return WError::ERR_NO_INSTRUMENT if WSESWrapperMain::loadInstrument() has not been called, WError::ERR_FAIL on
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
 * Getter for the \c analyzer_region property. 
 *
 * \param[in] index Not used.
 * \param[out] value A pointer to a SESWrapperNS::AnalyzerRegion structure.
 * \param[in,out] size Not used.
 *
 * \return WError::ERR_NO_INSTRUMENT if WSESWrapperMain::loadInstrument() has not been called,
 *         otherwise WError::ERR_OK.
 */
int WSESWrapperBase::getAnalyzerRegion(int index, void *value, int &size)
{
  if (!instrumentLoaded_)
    return WError::ERR_NO_INSTRUMENT;

  SESWrapperNS::AnalyzerRegion *analyzerRegion = reinterpret_cast<SESWrapperNS::AnalyzerRegion *>(value);
  if (analyzerRegion != 0)
  {
    analyzerRegion->centerEnergy_ = sesRegion_.FixEnergy;
    analyzerRegion->dwellTime_ = sesRegion_.StepTime;
    analyzerRegion->energyStep_ = sesRegion_.EnergyStep;
    analyzerRegion->fixed_ = sesRegion_.Fixed;
    analyzerRegion->highEnergy_ = sesRegion_.HighEnergy;
    analyzerRegion->lowEnergy_ = sesRegion_.LowEnergy;
  }
  size = sizeof(SESWrapperNS::AnalyzerRegion);
  return WError::ERR_OK;
}

/*!
 * Getter for the \c use_external_io property. 
 *
 * \param[in] index Not used.
 * \param[out] value A pointer to a 1-byte boolean. If \c value is 0 (or \c false), no communication with the external
 *              I/O card is made. One example of an external I/O card is the DAQ cards from National Instruments.
 * \param[in,out] size Not used.
 *
 * \return WError::ERR_NO_INSTRUMENT if WSESWrapperMain::loadInstrument() has not been called,
 *         otherwise WError::ERR_OK.
 */
int WSESWrapperBase::getUseExternalIO(int index, void *value, int &size)
{
  if (!instrumentLoaded_)
    return WError::ERR_NO_INSTRUMENT;

  bool *boolValue = reinterpret_cast<bool *>(value);
  if (boolValue != 0)
    *boolValue = ((activeDetectors_ & 0x0002) != 0);
  return WError::ERR_OK;
}

/*!
 * Getter for the \c use_detector property. 
 *
 * \param[in] index Not used.
 * \param[out] value A pointer to a 1-byte boolean. If \c value is 0 (or \c false), no communication with the detector
 *              is made.
 * \param[in,out] size Not used.
 *
 * \return WError::ERR_NO_INSTRUMENT if WSESWrapperMain::loadInstrument() has not been called,
 *         otherwise WError::ERR_OK.
 */
int WSESWrapperBase::getUseDetector(int index, void *value, int &size)
{
  if (!instrumentLoaded_)
    return WError::ERR_NO_INSTRUMENT;

  bool *boolValue = reinterpret_cast<bool *>(value);
  if (boolValue != 0)
    *boolValue = ((activeDetectors_ & 0x0001) != 0);
  return WError::ERR_OK;
}

/*!
 * Getter for the \c region_name property. 
 *
 * \param[in] index Not used.
 * \param[out] value A \c char* buffer to be filled with the name of the region. Can be 0 (NULL).
 * \param[in,out] size If \p value is 0, this parameter is modified with the required length of the region name, otherwise
 *             it is used to determine the size of \c value.
 *
 * \return Always returns WError::ERR_OK.
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
 * Getter for the \c temp_file_name property.
 *
 * \param[in] index Not used.
 * \param[out] value A \c char* buffer to be filled with the name of the temporary working file created during acquisition.
 *              Can be 0 (NULL).
 * \param[in,out] size If \p value is 0, this parameter is modified with the required length of the file name, otherwise
 *             it is used to determine the size of \c value.
 *
 * \return Always returns WError::ERR_OK.
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
 * Getter for the \c reset_data_between_iterations property.
 *
 * \param[in] index Not used.
 * \param[out] value A pointer to a 1-byte boolean. If \c value is 0 (or \c false), data will be accumulated
 *                   between each call to startAcquisition unless initAcquisition is called. If it is 1 (or
 *                   \c true), all data is reset to 0 between the iterations, even if initAcquisition is not called.
 * \param[in,out] size Not used.
 *
 * \return Always returns WError::ERR_OK.
 */
int WSESWrapperBase::getResetDataBetweenIterations(int index, void *value, int &size)
{
  bool *boolValue = reinterpret_cast<bool *>(value);
  if (boolValue != 0)
    *boolValue = resetDataBetweenIterations_;
  return WError::ERR_OK;
}

/*!
 * Getter for the \c use_spin property. If there are no spin detectors installed, this function returns
 * WError::ERR_NOT_APPLICABLE and sets \p value to \c false.
 *
 * \param[in] index Specifies which spin detector being queried. The value ranges between 1 and the number of available
 *                  spin detectors. Which spin detector belongs to what index depends on the system configuration.
 * \param[out] value A pointer to a 1-byte boolean. This is set to \c true if the spin detector specified by \p index is
 *                   active.
 * \param[in,out] size Not used.
 *
 * \return WError::ERR_OK if a spin detector is found at index \p index.
 */
int WSESWrapperBase::getUseSpin(int index, void *value, int &size)
{
  if (!instrumentLoaded_)
    return WError::ERR_NO_INSTRUMENT;

  bool *boolValue = reinterpret_cast<bool *>(value);

  int count = 0;
  lib_->GDS_GetOption(SesNS::DetectorCount, &count);
  ++index;

  *boolValue = false;

  if (count < 3)
    return WError::ERR_NOT_APPLICABLE;

  if (index < 2 || (count > 2 && index >= count))
    return WError::ERR_INDEX;

  unsigned short activeDetector = 0;
  lib_->GDS_GetOption(SesNS::ActiveDetector, &activeDetector);
  
  return WError::ERR_OK;
}

/*!
 * Getter for the \c use_binding_energy property. 
 *
 * \param[in] index Not used.
 * \param[out] value A pointer to a 1-byte boolean. Set to \c true when the energies in the analyzer region are assumed to be given as binding energies.
 * \param[in,out] size Not used.
 *
 * \return Always returns WError::ERR_OK.
 */
int WSESWrapperBase::getUseBindingEnergy(int index, void *value, int &size)
{
  bool *boolValue = reinterpret_cast<bool *>(value);

  *boolValue = !sesRegion_.Kinetic;
  return WError::ERR_OK;
}

// Property setters

/*!
 * Stub for the read-only properties.
 *
 * \param[in] index Not used.
 * \param[in] value Not used.
 *
 * \return Always returns WError::ERR_READONLY to indicate that an attempt was made to modify a read-only property.
 */
int WSESWrapperBase::readOnlyStub(int index, const void *value)
{
  return WError::ERR_READONLY;
}

/*!
 * Setter for the \c lib_working_dir property. Changes the working directory for the library.
 *
 * \param[in] index Not used.
 * \param[in] value A null-terminated C string that contains the path to the new working directory.
 *
 * \return Always returns WError::ERR_OK.
 */
int WSESWrapperBase::setLibWorkingDir(int index, const void *value)
{
  const char *strValue = reinterpret_cast<const char *>(value);
  if (strValue != 0)
    _chdir(strValue);

	return WError::ERR_OK;
}

/*!
 * Setter for the \c instrument_library property. Always unloads the current library and opens the new one if it has been specified. A NULL string simply unloads the current library.
 * This function does not call GDS_Finalize() before unloading. That responsibility falls on the calling application.
 *
 * \param[in] index Not used.
 * \param[in] value A null-terminated C string that contains the name of the instrument library.
 *
 * \return Returns WError::ERR_OK if successfull, otherwise WError::ERR_LOAD_LIBRARY.
 */
int WSESWrapperBase::setInstrumentLibrary(int index, const void *value)
{
  const char *strValue = reinterpret_cast<const char *>(value);
  lib_->unload();
  if (strValue != 0)
  {
    instrumentLibraryName_ = strValue;
    if (!lib_->load(strValue))
      return WError::ERR_LOAD_LIBRARY;
  }
	return WError::ERR_OK;
}

/*!
 * Setter for the \c always_delay_region property. If \c true (1), a delay will be made before starting
 * the acquisition of a region.
 *
 * \param[in] index Not used.
 * \param[in] value A 1-byte boolean of value \c true (1) or \c false (0). 
 *
 * \return WError::ERR_NOT_INITIALIZED if the library has not been successfully initialized, otherwise WError::ERR_OK.
 */
int WSESWrapperBase::setAlwaysDelayRegion(int index, const void *value)
{
  const bool *boolValue = reinterpret_cast<const bool *>(value);
  if (lib_->GDS_SetOption == 0)
    return WError::ERR_NOT_INITIALIZED;
  lib_->GDS_SetOption(SesNS::AlwaysDelayRegion, boolValue);
	return WError::ERR_OK;
}

/*!
 * Setter for the \c allow_io_with_detector property. If \c true (1), external I/O communication will be
 * possible simultaneously with detector communication.
 *
 * \param[in] index Not used.
 * \param[in] value A 1-byte boolean of value \c true (1) or \c false (0). 
 *
 * \return WError::ERR_NOT_INITIALIZED if the library has not been successfully initialized, otherwise WError::ERR_OK.
 */
int WSESWrapperBase::setAllowIOWithDetector(int index, const void *value)
{
  const bool *boolValue = reinterpret_cast<const bool *>(value);
  if (lib_->GDS_SetOption == 0)
    return WError::ERR_NOT_INITIALIZED;
  lib_->GDS_SetOption(SesNS::AllowSignalsWithDetector, boolValue);
	return WError::ERR_OK;
}

/*!
 * Setter for the \c detector_region property. This will set up a new ROI (Region of Interest) for the acquisition.
 * The values in the detector region will be validated through a call to the GDS_CheckRegion() function.
 * 
 * \param[in] index Not used.
 * \param[in] value A pointer to a SESWrapperNS::DetectorRegion structure.
 *
 * \return Allways returns WError::ERR_OK.
 */
int WSESWrapperBase::setDetectorRegion(int index, const void *value)
{
  const DetectorRegion *detectorRegion = reinterpret_cast<const SESWrapperNS::DetectorRegion *>(value);
  if (detectorRegion != 0)
  {
    sesRegion_.ADCMode = detectorRegion->adcMode_;
    sesRegion_.FirstXChannel = detectorRegion->firstXChannel_;
    sesRegion_.LastXChannel = detectorRegion->lastXChannel_;
    sesRegion_.FirstYChannel = detectorRegion->firstYChannel_;
    sesRegion_.LastYChannel = detectorRegion->lastYChannel_;
    sesRegion_.Slices = detectorRegion->slices_;

    if (lib_->GDS_SetGlobalDetector != 0)
    {
      sesDetectorRegion_.ADCMask = sesRegion_.ADCMask;
      sesDetectorRegion_.ADCMode = detectorRegion->adcMode_;
      sesDetectorRegion_.DiscLvl = sesRegion_.DiscLvl;
      sesDetectorRegion_.FirstXChannel = detectorRegion->firstXChannel_;
      sesDetectorRegion_.LastXChannel = detectorRegion->lastXChannel_;
      sesDetectorRegion_.FirstYChannel = detectorRegion->firstYChannel_;
      sesDetectorRegion_.LastYChannel = detectorRegion->lastYChannel_;
      sesDetectorRegion_.Slices = detectorRegion->slices_;
      lib_->GDS_SetGlobalDetector(&sesDetectorRegion_);
    }
  }
  return WError::ERR_OK;
}

/*!
 * Setter for the \c element_set property.
 *
 * \param[in] index Must be set to -1. Values between 0 and \c element_set_count - 1 are index for future use.
 * \param[in] value A null-terminated string that specifies the name of the new element set.
 * 
 * \return WError::ERR_NO_INSTRUMENT if WSESWrapperMain::loadInstrument() has not been called,
 *         WError::ERR_INCORRECT_ELEMENT_SET if the given element set is not available, otherwise WError::ERR_OK.
 */
int WSESWrapperBase::setElementSet(int index, const void *value)
{
  if (!instrumentLoaded_)
    return WError::ERR_NO_INSTRUMENT;

  const char *strValue = reinterpret_cast<const char *>(value);
  return lib_->GDS_SetElementSet(strValue) == 0 ? WError::ERR_OK : WError::ERR_INCORRECT_ELEMENT_SET;
}

/*!
 * Setter for the \c element_set property. This function reloads the pass energy list when successful.
 * This requires updates for the pass energy lists for the calling application.
 *
 * \param[in] index  Must be set to -1. Values between 0 and \c element_set_count - 1 are index for future use.
 * \param[in] value A null-terminated string that specifies the name of the new lens mode.
 * 
 * \return WError::ERR_NO_INSTRUMENT if WSESWrapperMain::loadInstrument() has not been called,
 *         WError::ERR_INCORRECT_LENS_MODE if the given lens mode is not available, otherwise WError::ERR_OK.
 */
int WSESWrapperBase::setLensMode(int index, const void *value)
{
  if (!instrumentLoaded_)
    return WError::ERR_NO_INSTRUMENT;

  const std::string strValue = reinterpret_cast<const char *>(value);
  int errorCode = lib_->GDS_SetLensMode(strValue.c_str()) == 0 ? WError::ERR_OK : WError::ERR_INCORRECT_LENS_MODE;
  if (errorCode == WError::ERR_OK)
    sesRegion_.LensMode[strValue.copy(sesRegion_.LensMode, 32)] = 0;
	return errorCode;
}

/*!
 * Setter for the \c pass_energy property. If the pass energy to set is taken from a list of pass energies, make
 * sure to update that list after modifying the lens mode and before calling this function.
 *
 * \param[in] index  Must be set to -1. Values between 0 and \c element_set_count - 1 are index for future use.
 * \param[in] value A pointer to a \c double (8-byte floating point).
 * 
 * \return WError::ERR_NO_INSTRUMENT if WSESWrapperMain::loadInstrument() has not been called,
 *         WError::ERR_INCORRECT_PASS_ENERGY if the given pass energy is not available, otherwise WError::ERR_OK.
 */
int WSESWrapperBase::setPassEnergy(int index, const void *value)
{
  if (!instrumentLoaded_)
    return WError::ERR_NO_INSTRUMENT;

  double doubleValue = *reinterpret_cast<const double *>(value);
  int errorCode = lib_->GDS_SetPassEnergy(doubleValue) == 0 ? WError::ERR_OK : WError::ERR_INCORRECT_PASS_ENERGY;
  doubleValue = sesRegion_.PassEnergy;
  if (lib_->GDS_GetCurrPassEnergy(&doubleValue) == 0)
    sesRegion_.PassEnergy = doubleValue;

	return errorCode;
}

/*!
 * Setter for the \c analyzer_region property. The members in the WAnalyzerRegion structure are stored
 * in an internal object for later use in a call to WSESWrapperMain::initAcquisition(). There is no immediate
 * checking done of the validity of the given values. If such a check is required before starting acquisition,
 * the WSESWrapperMain::checkAnalyzerRegion() can be called.
 *
 * \param[in] index Not used.
 * \param[in] value A pointer to a WAnalyzerRegion structure.
 *
 * \return Always returns WError::ERR_OK.
 */
int WSESWrapperBase::setAnalyzerRegion(int index, const void *value)
{
  const WAnalyzerRegion *analyzerRegion = reinterpret_cast<const WAnalyzerRegion *>(value);
  if (analyzerRegion != 0)
  {
    sesRegion_.DriftRegion = false;
    sesRegion_.EnergyStep = analyzerRegion->energyStep_;
    sesRegion_.Fixed = analyzerRegion->fixed_;
    sesRegion_.FixEnergy = analyzerRegion->centerEnergy_;
    sesRegion_.Grating = 0;
    sesRegion_.HighEnergy = analyzerRegion->highEnergy_;
    sesRegion_.Illumination = 0;
    sesRegion_.LowEnergy = analyzerRegion->lowEnergy_;
    sesRegion_.Order = 0;
    sesRegion_.Slit = 0;
    sesRegion_.StepTime = analyzerRegion->dwellTime_;
    sesRegion_.UseRegionDetector = true;
  }
  return WError::ERR_OK;
}

/*!
 * Setter for the \c use_external_io property. Modify this to toggle the use of the external I/O card, if present.
 *
 * \param[in] index Not used.
 * \param[in] value A pointer to a 1-byte boolean.
 *
 * \return Always returns WError::ERR_OK.
 */
int WSESWrapperBase::setUseExternalIO(int index, const void *value)
{
  const bool *boolValue = reinterpret_cast<const bool *>(value);
  if (boolValue != 0)
    activeDetectors_ = *boolValue ? activeDetectors_ | 0x0002 : activeDetectors_ & ~0x0002;
  return WError::ERR_OK;
}

/*!
 * Setter for the \c use_spin property. If there are no spin detectors installed, this function returns
 * WError::ERR_NOT_APPLICABLE.
 *
 * \param[in] index Specifies which spin detector being queried. The value ranges between 1 and the number of available
 *                  spin detectors. Which spin detector belongs to what index depends on the system configuration.
 * \param[out] value A pointer to a 1-byte boolean.
 *
 * \return WError::ERR_OK if a spin detector is found at index \p index.
 */
int WSESWrapperBase::setUseSpin(int index, const void *value)
{
  if (!instrumentLoaded_)
    return WError::ERR_NO_INSTRUMENT;

  const bool *boolValue = reinterpret_cast<const bool *>(value);

  int count = 0;
  lib_->GDS_GetOption(SesNS::DetectorCount, &count);

  if (count < 3)
    return WError::ERR_NOT_APPLICABLE;

  if (index < 2 || (count > 2 && index >= count))
    return WError::ERR_INDEX;

  unsigned short activeDetector = 0;
  lib_->GDS_SetOption(SesNS::ActiveDetector, &activeDetector);
  
  return WError::ERR_OK;
}

/*!
 * Setter for the \c use_binding_energy property.
 * \warning If you set this to \c true, you must also set the excitation energy.
 *
 * \param[in] index Not used.
 * \param[in] value A pointer to a 1-byte boolean.
 *
 * \return Always returns WError::ERR_OK.
 */
int WSESWrapperBase::setUseBindingEnergy(int index, const void *value)
{
  const bool *boolValue = reinterpret_cast<const bool *>(value);
  if (boolValue != 0)
    sesRegion_.Kinetic = !*boolValue;
  return WError::ERR_OK;
}

/*!
 * Setter for the \c use_detector property. Modify this to toggle the use of the detector. Can be used in combination
 * with the \c use_external_io property.
 *
 * \param[in] index Not used.
 * \param[in] value A pointer to a 1-byte boolean.
 *
 * \return Always returns WError::ERR_OK.
 */
int WSESWrapperBase::setUseDetector(int index, const void *value)
{
  const bool *boolValue = reinterpret_cast<const bool *>(value);
  if (boolValue != 0)
    activeDetectors_ = *boolValue ? activeDetectors_ | 0x0001 : activeDetectors_ & ~0x0001;
  return WError::ERR_OK;
}

/*!
 * Setter for the \c region_name property. 
 *
 * \param[in] index Not used.
 * \param[in] value A null-terminated C string that specifies the region name. The name is limited to 32 characters,
 *              including the null character.
 *
 * \return Always returns WError::ERR_OK.
 */
int WSESWrapperBase::setRegionName(int index, const void *value)
{
  const std::string strValue = reinterpret_cast<const char *>(value);
  sesRegion_.Name[strValue.copy(sesRegion_.Name, 32)] = 0;
	return WError::ERR_OK;
}

/*!
 * Setter for the \c temp_file_name property. 
 *
 * \param[in] index Not used.
 * \param[in] value A null-terminated C string that specifies the name of the temporary file created during acquisition.
 *
 * \return Always returns WError::ERR_OK.
 */
int WSESWrapperBase::setTempFileName(int index, const void *value)
{
  tempFileName_ = reinterpret_cast<const char *>(value);
  return WError::ERR_OK;
}

/*!
 * Setter for the \c reset_data_between_iterations property.
 *
 * \param[in] index Not used.
 * \param[in] value A pointer to a 1-byte boolean. 
 *
 * \return Always returns WError::ERR_OK.
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
 * \return \c true if successful.
 */
bool WSESWrapperBase::loadElementSets()
{
  elementSets_.clear();

  if (!instrumentLoaded_)
    return false;

  char *buffer = 0;
  int bufferSize = 0;
  
  bool success = (lib_->GDS_GetElementSets(buffer, &bufferSize) == 0);
  if (success)
  {
    buffer = new char[bufferSize];
    success = (lib_->GDS_GetElementSets(buffer, &bufferSize) == 0);
    splitSESList(buffer, bufferSize, elementSets_);
    delete[] buffer;
  }

  return success;
}

/*!
 * Loads the lens modes. Called after loading a new instrument configuration.
 *
 * \return \c true if successful.
 */
bool WSESWrapperBase::loadLensModes()
{
  lensModes_.clear();

  if (!instrumentLoaded_)
    return false;

  char *buffer = 0;
  int bufferSize = 0;
  
  bool success = (lib_->GDS_GetLensModes(buffer, &bufferSize) == 0);
  if (success)
  {
    buffer = new char[bufferSize];
    success = (lib_->GDS_GetLensModes(buffer, &bufferSize) == 0);
    splitSESList(buffer, bufferSize, lensModes_);
    delete[] buffer;
  }

  return success;
}

/*!
 * Reloads the Pass energies. Called after loading a new instrument configuration.
 *
 * \return \c true if successful.
 */
bool WSESWrapperBase::loadPassEnergies(const std::string &lensMode, DoubleVector &result)
{
  passEnergies_.clear();

  if (!instrumentLoaded_)
    return false;

  char *buffer = 0;
  int bufferSize = 0;

  result.clear();
  bool success = (lib_->GDS_GetPassEnergies(lensMode.c_str(), buffer, &bufferSize) == 0);

  if (success)
  {
    buffer = new char[bufferSize];
    success = (lib_->GDS_GetPassEnergies(lensMode.c_str(), buffer, &bufferSize) == 0 && buffer[0] != 0);
    if (success)
    {
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
 * Reloads the Element names. Called after loading a new instrument configuration.
 *
 * \return \c true if successful.
 */
bool WSESWrapperBase::loadElementNames()
{
  elementNames_.clear();

  if (!instrumentLoaded_)
    return false;
  
  if (lib_->GDS_GetElements == 0)
    return true; // Optional function, should not generate errors.

  char *buffer = 0;
  int size = 0;
  int result = lib_->GDS_GetElements(0, &size);
  buffer = new char[size];
  result = lib_->GDS_GetElements(buffer, &size);
  splitSESList(buffer, size, elementNames_);
  delete[] buffer;
  return true;
}

/*!
 * Helper function to split strings obtained as lists from calls to SESInstrument functions.
 *
 * \param[in] buffer The null-terminated string containing the elements to be split. Each element is assumed to be wrapped
 *               between quotation marks ("), and separated with space (e.g. '"element 1" "element 2" "element 3"').
 * \param[in] bufferSize The maximum size of the buffer, used as an extra security measure in case the string lacks the null termination.
 * \param[out] result a string vector containing the split elements.
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
}
