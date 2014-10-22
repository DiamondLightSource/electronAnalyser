#include "wsesinstrument.h"
#include "common.hpp"

#define NOMINMAX
#include <Windows.h>

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
  resetFunctions();
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
  if (!WLibrary::load(fileName))
    return false;

  resetFunctions();

  try
  {
    ::import(*this, "GDS_GetLastError", SESWrapperNS::FUNCTION_REQUIRED, GDS_GetLastError);
    ::import(*this, "GDS_GetLastErrorString", SESWrapperNS::FUNCTION_REQUIRED, GDS_GetLastErrorString);
    ::import(*this, "GDS_Initialize", SESWrapperNS::FUNCTION_REQUIRED, GDS_Initialize);
    ::import(*this, "GDS_Finalize", SESWrapperNS::FUNCTION_REQUIRED, GDS_Finalize);
    ::import(*this, "GDS_LoadInstrument", SESWrapperNS::FUNCTION_REQUIRED, GDS_LoadInstrument);
    ::import(*this, "GDS_SaveInstrument", SESWrapperNS::FUNCTION_REQUIRED, GDS_SaveInstrument);
    ::import(*this, "GDS_NewInstrument", SESWrapperNS::FUNCTION_REQUIRED, GDS_NewInstrument);
    ::import(*this, "GDS_ResetInstrument", SESWrapperNS::FUNCTION_REQUIRED, GDS_ResetInstrument);
    ::import(*this, "GDS_ZeroSupplies", SESWrapperNS::FUNCTION_REQUIRED, GDS_ZeroSupplies);
    ::import(*this, "GDS_TestCommunication", SESWrapperNS::FUNCTION_REQUIRED, GDS_TestCommunication);
    ::import(*this, "GDS_GetOption", SESWrapperNS::FUNCTION_REQUIRED, GDS_GetOption);
    ::import(*this, "GDS_SetOption", SESWrapperNS::FUNCTION_REQUIRED, GDS_SetOption);
    ::import(*this, "GDS_GetInstrumentInfo", SESWrapperNS::FUNCTION_REQUIRED, GDS_GetInstrumentInfo);
    ::import(*this, "GDS_GetDetectorInfo", SESWrapperNS::FUNCTION_REQUIRED, GDS_GetDetectorInfo);
    ::import(*this, "GDS_HasSupplyLib", SESWrapperNS::FUNCTION_OPTIONAL, GDS_HasSupplyLib);
    ::import(*this, "GDS_HasDetectorLib", SESWrapperNS::FUNCTION_OPTIONAL, GDS_HasDetectorLib);
    ::import(*this, "GDS_HasSignalsLib", SESWrapperNS::FUNCTION_OPTIONAL, GDS_HasSignalsLib);
    ::import(*this, "GDS_GetElementSets", SESWrapperNS::FUNCTION_REQUIRED, GDS_GetElementSets);
    ::import(*this, "GDS_GetLensModes", SESWrapperNS::FUNCTION_REQUIRED, GDS_GetLensModes);
    ::import(*this, "GDS_GetPassEnergies", SESWrapperNS::FUNCTION_REQUIRED, GDS_GetPassEnergies);
    ::import(*this, "GDS_GetCurrElementSet", SESWrapperNS::FUNCTION_REQUIRED, GDS_GetCurrElementSet);
    ::import(*this, "GDS_GetCurrLensMode", SESWrapperNS::FUNCTION_REQUIRED, GDS_GetCurrLensMode);
    ::import(*this, "GDS_GetCurrPassEnergy", SESWrapperNS::FUNCTION_REQUIRED, GDS_GetCurrPassEnergy);
    ::import(*this, "GDS_GetCurrKineticEnergy", SESWrapperNS::FUNCTION_REQUIRED, GDS_GetCurrKineticEnergy);
    ::import(*this, "GDS_GetCurrExcitationEnergy", SESWrapperNS::FUNCTION_REQUIRED, GDS_GetCurrExcitationEnergy);
    ::import(*this, "GDS_GetCurrBindingEnergy", SESWrapperNS::FUNCTION_REQUIRED, GDS_GetCurrBindingEnergy);
    ::import(*this, "GDS_GetGlobalDetector", SESWrapperNS::FUNCTION_REQUIRED, GDS_GetGlobalDetector);
    ::import(*this, "GDS_GetElement", SESWrapperNS::FUNCTION_REQUIRED, GDS_GetElement);
    ::import(*this, "GDS_GetElements", SESWrapperNS::FUNCTION_OPTIONAL, GDS_GetElements);
    ::import(*this, "GDS_SetElementSet", SESWrapperNS::FUNCTION_REQUIRED, GDS_SetElementSet);
    ::import(*this, "GDS_SetLensMode", SESWrapperNS::FUNCTION_REQUIRED, GDS_SetLensMode);
    ::import(*this, "GDS_SetPassEnergy", SESWrapperNS::FUNCTION_REQUIRED, GDS_SetPassEnergy);
    ::import(*this, "GDS_SetKineticEnergy", SESWrapperNS::FUNCTION_REQUIRED, GDS_SetKineticEnergy);
    ::import(*this, "GDS_SetExcitationEnergy", SESWrapperNS::FUNCTION_REQUIRED, GDS_SetExcitationEnergy);
    ::import(*this, "GDS_SetBindingEnergy", SESWrapperNS::FUNCTION_REQUIRED, GDS_SetBindingEnergy);
    ::import(*this, "GDS_SetGlobalDetector", SESWrapperNS::FUNCTION_REQUIRED, GDS_SetGlobalDetector);
    ::import(*this, "GDS_SetElement", SESWrapperNS::FUNCTION_REQUIRED, GDS_SetElement);
    ::import(*this, "GDS_CheckRegion", SESWrapperNS::FUNCTION_REQUIRED, GDS_CheckRegion);
    ::import(*this, "GDS_Start", SESWrapperNS::FUNCTION_REQUIRED, GDS_Start);
    ::import(*this, "GDS_Stop", SESWrapperNS::FUNCTION_REQUIRED, GDS_Stop);
    ::import(*this, "GDS_GetStatus", SESWrapperNS::FUNCTION_REQUIRED, GDS_GetStatus);
    ::import(*this, "GDS_GetDrift", SESWrapperNS::FUNCTION_REQUIRED, GDS_GetDrift);
    ::import(*this, "GDS_CalibrateOffset", SESWrapperNS::FUNCTION_REQUIRED, GDS_CalibrateOffset);
    ::import(*this, "GDS_GetOffset", SESWrapperNS::FUNCTION_REQUIRED, GDS_GetOffset);
    ::import(*this, "GDS_UseDetector", SESWrapperNS::FUNCTION_OPTIONAL, GDS_UseDetector);
    ::import(*this, "GDS_UseSignals", SESWrapperNS::FUNCTION_OPTIONAL, GDS_UseSignals);
    ::import(*this, "GDS_GetCurrSpectrum", SESWrapperNS::FUNCTION_OPTIONAL, GDS_GetCurrSpectrum);
    ::import(*this, "GDS_GetCurrSignals", SESWrapperNS::FUNCTION_OPTIONAL, GDS_GetCurrSignals);
    ::import(*this, "GDS_InstallInstrument", SESWrapperNS::FUNCTION_REQUIRED, GDS_InstallInstrument);
    ::import(*this, "GDS_InstallSupplies", SESWrapperNS::FUNCTION_REQUIRED, GDS_InstallSupplies);
    ::import(*this, "GDS_InstallElements", SESWrapperNS::FUNCTION_REQUIRED, GDS_InstallElements);
    ::import(*this, "GDS_InstallLensModes", SESWrapperNS::FUNCTION_REQUIRED, GDS_InstallLensModes);
    ::import(*this, "GDS_SetupDetector", SESWrapperNS::FUNCTION_REQUIRED, GDS_SetupDetector);
    ::import(*this, "GDS_SetupSignals", SESWrapperNS::FUNCTION_OPTIONAL, GDS_SetupSignals);
    ::import(*this, "GDS_CalibrateVoltages", SESWrapperNS::FUNCTION_REQUIRED, GDS_CalibrateVoltages);
    ::import(*this, "GDS_CalibrateDetector", SESWrapperNS::FUNCTION_REQUIRED, GDS_CalibrateDetector);
    ::import(*this, "GDS_ControlSupplies", SESWrapperNS::FUNCTION_REQUIRED, GDS_ControlSupplies);
    ::import(*this, "GDS_SupplyInfo", SESWrapperNS::FUNCTION_REQUIRED, GDS_SupplyInfo);
    ::import(*this, "GDS_DetectorInfo", SESWrapperNS::FUNCTION_REQUIRED, GDS_DetectorInfo);
    ::import(*this, "GDS_GetRawImage", SESWrapperNS::FUNCTION_OPTIONAL, GDS_GetRawImage);
    ::import(*this, "GDS_InitAcquisition", SESWrapperNS::FUNCTION_REQUIRED, GDS_InitAcquisition);
    ::import(*this, "GDS_StartAcquisition", SESWrapperNS::FUNCTION_REQUIRED, GDS_StartAcquisition);
    ::import(*this, "SC_GetProperty", SESWrapperNS::FUNCTION_OPTIONAL, SC_GetProperty);
    ::import(*this, "SC_SetProperty", SESWrapperNS::FUNCTION_OPTIONAL, SC_SetProperty);
    ::import(*this, "SC_SetPropertyEx", SESWrapperNS::FUNCTION_OPTIONAL, SC_SetProperty);
  }
  catch (WFunctionException &)
  {
    unload();
    return false;
  }

  return true;
}

/*!
 * Releases the SESInstrument library and resets all function addresses. To continue using this class,
 * a call to load() must be made.
 */
void WSESInstrument::unload()
{
  WLibrary::unload();
  resetFunctions();
}

void WSESInstrument::resetFunctions()
{
  GDS_GetLastError = 0;
  GDS_GetLastErrorString = 0;
  GDS_Initialize = 0;
  GDS_Finalize = 0;
  GDS_LoadInstrument = 0;
  GDS_SaveInstrument = 0;
  GDS_NewInstrument = 0;
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
  SC_GetProperty = 0;
  SC_SetProperty = 0;
  SC_SetPropertyEx = 0;
}
