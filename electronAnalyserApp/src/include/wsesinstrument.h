#ifndef __SESWRAPPER_WSESINSTRUMENT_H__
#define __SESWRAPPER_WSESINSTRUMENT_H__

#include "wlibrary.h"
#include "sestypes.h"

#include <string>
#include <vector>

/*!
 * @brief Contains the C functions imported from the SESInstrument library.
 *
 * The list of functions is compatible with SESInstrument.dll version 1.2.5. The functions described in the
 * callbacks here are documented in more detail in the instrument-interface.txt available with the SES software.
 */
class WSesInstrument : public WLibrary
{
public:
  WSesInstrument();
  ~WSesInstrument();
  
  bool load(const char *fileName);
  void unload();

  SesNS::FnGDS_GetLastError GDS_GetLastError; /*!< Retrieves the last reported error code */
  SesNS::FnGDS_GetLastErrorString GDS_GetLastErrorString; /*!< Retrieves the las reported error string */
  SesNS::FnGDS_Initialize GDS_Initialize; /*!< Initializes the library */
  SesNS::FnGDS_Finalize GDS_Finalize; /*!< Finalizes the library */
  SesNS::FnGDS_LoadInstrument GDS_LoadInstrument; /*!< Loads an instrument configuration */
  SesNS::FnGDS_SaveInstrument GDS_SaveInstrument; /*!< Saves the instrument configuration */
  SesNS::FnGDS_NewInstrument GDS_NewInstrument; /*!< Creates a default instrument configuration */
  SesNS::FnGDS_LoadRunVar GDS_LoadRunVar; /*!< Load the RunVar.dat file */
  SesNS::FnGDS_SaveRunVar GDS_SaveRunVar; /*!< Save the RunVar.dat file */
  SesNS::FnGDS_ResetInstrument GDS_ResetInstrument; /*!< Resets the instrument */
  SesNS::FnGDS_ZeroSupplies GDS_ZeroSupplies; /*!< Sets all voltage elements to a zero level */
  SesNS::FnGDS_TestCommunication GDS_TestCommunication; /*!< Validates communication with the HW. */
  SesNS::FnGDS_GetOption GDS_GetOption; /*!< Retrieves an instrument option */
  SesNS::FnGDS_SetOption GDS_SetOption; /*!< Sets an instrument option */
  SesNS::FnGDS_GetInstrumentInfo GDS_GetInstrumentInfo; /*!< Retrieves instrument information */
  SesNS::FnGDS_GetDetectorInfo GDS_GetDetectorInfo; /*!< Retrieves detector information */
  SesNS::FnGDS_HasSupplyLib GDS_HasSupplyLib; /*!< Checks for presence of a supply library */
  SesNS::FnGDS_HasDetectorLib GDS_HasDetectorLib; /*!< Checks for presence of a detector library */
  SesNS::FnGDS_HasSignalsLib GDS_HasSignalsLib; /*!< Checks for presence of a signal library */
  SesNS::FnGDS_GetElementSets GDS_GetElementSets; /*!< Retrieves the list of element sets, or the required size of the string buffer to store the element sets */
  SesNS::FnGDS_GetLensModes GDS_GetLensModes; /*!< Retrieves the list of lens modes, or the required size of the string buffer to store the lens modes */
  SesNS::FnGDS_GetPassEnergies GDS_GetPassEnergies; /*!< Retrieves the list of pass energies for the current lens mode, or the required size of the string buffer to store the pass energies */
  SesNS::FnGDS_GetCurrElementSet GDS_GetCurrElementSet; /*!< Retrieves the name of the currently selected element set */
  SesNS::FnGDS_GetCurrLensMode GDS_GetCurrLensMode; /*!< Retrieves the name of the currently selected lens mode */
  SesNS::FnGDS_GetCurrPassEnergy GDS_GetCurrPassEnergy; /*!< Retrieves the currently selected pass energy */
  SesNS::FnGDS_GetCurrKineticEnergy GDS_GetCurrKineticEnergy; /*!< Retrieves the currently selected kinetic energy */
  SesNS::FnGDS_GetCurrExcitationEnergy GDS_GetCurrExcitationEnergy; /*!< Retrieves the currently selected excitation energy */
  SesNS::FnGDS_GetCurrBindingEnergy GDS_GetCurrBindingEnergy; /*!< Retrieves the currently selected binding energy */
  SesNS::FnGDS_GetGlobalDetector GDS_GetGlobalDetector; /*!< Retrieves the global detector region */
  SesNS::FnGDS_GetElement GDS_GetElement; /*!< Retrieves the voltage currently applied to an element */
  SesNS::FnGDS_SetElementSet GDS_SetElementSet; /*!< Selects element set */
  SesNS::FnGDS_SetLensMode GDS_SetLensMode; /*!< Selects lens mode */
  SesNS::FnGDS_SetPassEnergy GDS_SetPassEnergy; /*!< Selects pass energy. The lens mode and element set must be selected before this. */
  SesNS::FnGDS_SetKineticEnergy GDS_SetKineticEnergy; /*!< Changes kinetic energy. */
  SesNS::FnGDS_SetExcitationEnergy GDS_SetExcitationEnergy; /*!< Changes excitation energy */
  SesNS::FnGDS_SetBindingEnergy GDS_SetBindingEnergy; /*!< Changes binding energy */
  SesNS::FnGDS_SetGlobalDetector GDS_SetGlobalDetector; /*!< Modifies the global detector region */
  SesNS::FnGDS_SetElement GDS_SetElement; /*!< Changes element voltage */
  SesNS::FnGDS_CheckRegion GDS_CheckRegion; /*!< Validates a region */
  SesNS::FnGDS_Start GDS_Start; /*!< Starts an acquisition */
  SesNS::FnGDS_Stop GDS_Stop; /*!< Aborts the current acquisition */
  SesNS::FnGDS_GetStatus GDS_GetStatus; /*!< Retrieves the current status of the library */
  SesNS::FnGDS_GetDrift GDS_GetDrift; /*!< Retrieves the energy drift. Used in conjunction to a drift region */
  SesNS::FnGDS_CalibrateOffset GDS_CalibrateOffset; /*!< Starts an energy offset calibration */
  SesNS::FnGDS_GetOffset GDS_GetOffset; /*!< Retrieves an energy offset */
  SesNS::FnGDS_UseDetector GDS_UseDetector; /*!< Use the detector */
  SesNS::FnGDS_UseSignals GDS_UseSignals; /*!< Use signals */
  SesNS::FnGDS_GetCurrSpectrum GDS_GetCurrSpectrum; /*!< Retrieve the current spectrum */
  SesNS::FnGDS_GetCurrSignals GDS_GetCurrSignals; /*!< Retrieve the current signals */
  SesNS::FnGDS_InstallInstrument GDS_InstallInstrument; /*!< Opens an installation dialog */
  SesNS::FnGDS_InstallSupplies GDS_InstallSupplies; /*!< Opens a supply installation dialog */
  SesNS::FnGDS_InstallElements GDS_InstallElements; /*!< Opens an element installation dialog */
  SesNS::FnGDS_InstallLensModes GDS_InstallLensModes; /*!< Opens a lens mode installation dialog*/
  SesNS::FnGDS_SetupDetector GDS_SetupDetector; /*!< Opens the Global Detector Setup dialog */
  SesNS::FnGDS_SetupSignals GDS_SetupSignals; /*!< Opens a signals setup dialog */
  SesNS::FnGDS_CalibrateVoltages GDS_CalibrateVoltages; /*!< Opens the Voltage Calibration dialog */
  SesNS::FnGDS_CalibrateDetector GDS_CalibrateDetector; /*!< Opens the Detector Calibrartion dialog */
  SesNS::FnGDS_ControlSupplies GDS_ControlSupplies; /*!< Opens a supply control dialog */
  SesNS::FnGDS_SupplyInfo GDS_SupplyInfo; /*!< Opens a supply information dialog */
  SesNS::FnGDS_DetectorInfo GDS_DetectorInfo; /*!< Opens a detector info dialog */
  SesNS::FnGDS_GetRawImage GDS_GetRawImage; /*!< Retrieves a raw image from the detector */
  SesNS::FnGDS_InitAcquisition GDS_InitAcquisition; /*!< Initializing acquisition. Replaces Start(). Introduced in SESInstrument version 1.2.5-rc6 */
  SesNS::FnGDS_StartAcquisition GDS_StartAcquisition; /*!< Starts acquisition. Replaces Start(). Introduced in SESInstrument version 1.2.5-rc6. */
};

#endif

