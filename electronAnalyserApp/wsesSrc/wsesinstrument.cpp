#include "wsesinstrument.h"
#include "common.hpp"

#include <sstream>

using namespace SesNS;
using namespace CommonNS;

/*!
 * \brief Contains the C functions imported from the SESInstrument library.
 *
 * The list of functions is compatible with SESInstrument.dll version 1.2.5. The functions described in the
 * callbacks here are documented in more detail in the instrument-interface.txt available with the SES software.
 */

/*!
 * Creates a WSESInstrument instance.
 */
WSESInstrument::WSESInstrument()
{
}

/*!
 * Destroys the WSESInstrument object.
 */
WSESInstrument::~WSESInstrument()
{
}

/*!
 * Loads the SESInstrument library.
 *
 * \param[in] fileName The name of the SESInstrment library (usually <b>dll\\SESInstrument.dll</b>).
 *
 * \return Returns \c true if successful.
 */
bool WSESInstrument::load(const char *fileName)
{
  bool success = true;

  if (!isLoaded() && !WLibrary::load(fileName))
    return false;

  success = ::import(*this, "GDS_GetLastError", GDS_GetLastError);
  success = ::import(*this, "GDS_GetLastErrorString", GDS_GetLastErrorString) && success;
  success = ::import(*this, "GDS_Initialize", GDS_Initialize) && success;
  success = ::import(*this, "GDS_Finalize", GDS_Finalize) && success;
  success = ::import(*this, "GDS_LoadInstrument", GDS_LoadInstrument) && success;
  success = ::import(*this, "GDS_SaveInstrument", GDS_SaveInstrument) && success;
  success = ::import(*this, "GDS_NewInstrument", GDS_NewInstrument) && success;
  success = ::import(*this, "GDS_LoadRunVar", GDS_LoadRunVar) && success;
  success = ::import(*this, "GDS_SaveRunVar", GDS_SaveRunVar) && success;
  success = ::import(*this, "GDS_ResetInstrument", GDS_ResetInstrument) && success;
  success = ::import(*this, "GDS_ZeroSupplies", GDS_ZeroSupplies) && success;
  success = ::import(*this, "GDS_TestCommunication", GDS_TestCommunication) && success;
  success = ::import(*this, "GDS_GetOption", GDS_GetOption) && success;
  success = ::import(*this, "GDS_SetOption", GDS_SetOption) && success;
  success = ::import(*this, "GDS_GetInstrumentInfo", GDS_GetInstrumentInfo) && success;
  success = ::import(*this, "GDS_GetDetectorInfo", GDS_GetDetectorInfo) && success;
  ::import(*this, "GDS_HasSupplyLib", GDS_HasSupplyLib);
  ::import(*this, "GDS_HasDetectorLib", GDS_HasDetectorLib);
  ::import(*this, "GDS_HasSignalsLib", GDS_HasSignalsLib);
  success = ::import(*this, "GDS_GetElementSets", GDS_GetElementSets) && success;
  success = ::import(*this, "GDS_GetLensModes", GDS_GetLensModes) && success;
  success = ::import(*this, "GDS_GetPassEnergies", GDS_GetPassEnergies) && success;
  success = ::import(*this, "GDS_GetCurrElementSet", GDS_GetCurrElementSet) && success;
  success = ::import(*this, "GDS_GetCurrLensMode", GDS_GetCurrLensMode) && success;
  success = ::import(*this, "GDS_GetCurrPassEnergy", GDS_GetCurrPassEnergy) && success;
  success = ::import(*this, "GDS_GetCurrKineticEnergy", GDS_GetCurrKineticEnergy) && success;
  success = ::import(*this, "GDS_GetCurrExcitationEnergy", GDS_GetCurrExcitationEnergy) && success;
  success = ::import(*this, "GDS_GetCurrBindingEnergy", GDS_GetCurrBindingEnergy) && success;
  success = ::import(*this, "GDS_GetGlobalDetector", GDS_GetGlobalDetector) && success;
  success = ::import(*this, "GDS_GetElement", GDS_GetElement) && success;
  ::import(*this, "GDS_GetElements", GDS_GetElements);
  success = ::import(*this, "GDS_SetElementSet", GDS_SetElementSet) && success;
  success = ::import(*this, "GDS_SetLensMode", GDS_SetLensMode) && success;
  success = ::import(*this, "GDS_SetPassEnergy", GDS_SetPassEnergy) && success;
  success = ::import(*this, "GDS_SetKineticEnergy", GDS_SetKineticEnergy) && success;
  success = ::import(*this, "GDS_SetExcitationEnergy", GDS_SetExcitationEnergy) && success;
  success = ::import(*this, "GDS_SetBindingEnergy", GDS_SetBindingEnergy) && success;
  success = ::import(*this, "GDS_SetGlobalDetector", GDS_SetGlobalDetector) && success;
  success = ::import(*this, "GDS_SetElement", GDS_SetElement) && success;
  success = ::import(*this, "GDS_CheckRegion", GDS_CheckRegion) && success;
  success = ::import(*this, "GDS_Start", GDS_Start) && success;
  success = ::import(*this, "GDS_Stop", GDS_Stop) && success;
  success = ::import(*this, "GDS_GetStatus", GDS_GetStatus) && success;
  success = ::import(*this, "GDS_GetDrift", GDS_GetDrift) && success;
  success = ::import(*this, "GDS_CalibrateOffset", GDS_CalibrateOffset) && success;
  success = ::import(*this, "GDS_GetOffset", GDS_GetOffset) && success;
  ::import(*this, "GDS_UseDetector", GDS_UseDetector);
  ::import(*this, "GDS_UseSignals", GDS_UseSignals);
  ::import(*this, "GDS_GetCurrSpectrum", GDS_GetCurrSpectrum);
  ::import(*this, "GDS_GetCurrSignals", GDS_GetCurrSignals);
  success = ::import(*this, "GDS_InstallInstrument", GDS_InstallInstrument) && success;
  success = ::import(*this, "GDS_InstallSupplies", GDS_InstallSupplies) && success;
  success = ::import(*this, "GDS_InstallElements", GDS_InstallElements) && success;
  success = ::import(*this, "GDS_InstallLensModes", GDS_InstallLensModes) && success;
  success = ::import(*this, "GDS_SetupDetector", GDS_SetupDetector) && success;
  ::import(*this, "GDS_SetupSignals", GDS_SetupSignals);
  success = ::import(*this, "GDS_CalibrateVoltages", GDS_CalibrateVoltages) && success;
  success = ::import(*this, "GDS_CalibrateDetector", GDS_CalibrateDetector) && success;
  success = ::import(*this, "GDS_ControlSupplies", GDS_ControlSupplies) && success;
  success = ::import(*this, "GDS_SupplyInfo", GDS_SupplyInfo) && success;
  success = ::import(*this, "GDS_DetectorInfo", GDS_DetectorInfo) && success;
  ::import(*this, "GDS_GetRawImage", GDS_GetRawImage);
  success = ::import(*this, "GDS_InitAcquisition", GDS_InitAcquisition) && success;
  //success = ::import(*this, "GDS_StartAcquisition", GDS_StartAcquisition) && success;
  GDS_StartAcquisition = 0;
success = true;
  return success;
}

/*!
 * Releases the SESInstrument library and resets all function addresses. To continue using this class,
 * a call to load() must be made.
 */
void WSESInstrument::unload()
{
  WLibrary::unload();

  GDS_GetLastError = 0;
  GDS_GetLastErrorString = 0;
  GDS_Initialize = 0;
  GDS_Finalize = 0;
  GDS_LoadInstrument = 0;
  GDS_SaveInstrument = 0;
  GDS_NewInstrument = 0;
  GDS_LoadRunVar = 0;
  GDS_SaveRunVar = 0;
  GDS_ResetInstrument = 0;
  GDS_ZeroSupplies = 0;
  GDS_TestCommunication = 0;
  GDS_GetOption = 0;
  GDS_SetOption = 0;
  GDS_GetInstrumentInfo = 0;
  GDS_GetDetectorInfo = 0;
  GDS_HasSupplyLib = 0;
  GDS_HasDetectorLib = 0;
  GDS_HasSignalsLib = 0;
  GDS_GetElements = 0;
  GDS_GetElementSets = 0;
  GDS_GetLensModes = 0;
  GDS_GetPassEnergies = 0;
  GDS_GetCurrElementSet = 0;
  GDS_GetCurrLensMode = 0;
  GDS_GetCurrPassEnergy = 0;
  GDS_GetCurrKineticEnergy = 0;
  GDS_GetCurrExcitationEnergy = 0;
  GDS_GetCurrBindingEnergy = 0;
  GDS_GetGlobalDetector = 0;
  GDS_GetElement = 0;
  GDS_SetElementSet = 0;
  GDS_SetLensMode = 0;
  GDS_SetPassEnergy = 0;
  GDS_SetKineticEnergy = 0;
  GDS_SetExcitationEnergy = 0;
  GDS_SetBindingEnergy = 0;
  GDS_SetGlobalDetector = 0;
  GDS_SetElement = 0;
  GDS_CheckRegion = 0;
  GDS_Start = 0;
  GDS_Stop = 0;
  GDS_GetStatus = 0;
  GDS_GetDrift = 0;
  GDS_CalibrateOffset = 0;
  GDS_GetOffset = 0;
  GDS_UseDetector = 0;
  GDS_UseSignals = 0;
  GDS_GetCurrSpectrum = 0;
  GDS_GetCurrSignals = 0;
  GDS_InstallInstrument = 0;
  GDS_InstallSupplies = 0;
  GDS_InstallElements = 0;
  GDS_InstallLensModes = 0;
  GDS_SetupDetector = 0;
  GDS_SetupSignals = 0;
  GDS_CalibrateVoltages = 0;
  GDS_CalibrateDetector = 0;
  GDS_ControlSupplies = 0;
  GDS_SupplyInfo = 0;
  GDS_DetectorInfo = 0;
  GDS_GetRawImage = 0;
  GDS_InitAcquisition = 0;
  GDS_StartAcquisition = 0;
}
