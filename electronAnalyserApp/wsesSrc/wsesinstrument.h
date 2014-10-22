#ifndef __SESWRAPPER_WSESINSTRUMENT_H__
#define __SESWRAPPER_WSESINSTRUMENT_H__

#include "wlibrary.h"
#include "sestypes.h"

#include <string>
#include <vector>

class WSESInstrument : public WLibrary
{
public:
  WSESInstrument();
  ~WSESInstrument();
  
  bool load(const char *fileName);
  void unload();
  void resetFunctions();

  C_Function<int>::Pointer GDS_GetLastError; /*!< Retrieves the last reported error code */
  C_Function<const char *>::Pointer GDS_GetLastErrorString; /*!< Retrieves the las reported error string */
  C_Function<int, SesNS::ErrorNotify, void * /* HWND */>::Pointer GDS_Initialize; /*!< Initializes the library */
  C_Function<>::Pointer GDS_Finalize; /*!< Finalizes the library */
  C_Function<int, const char * /* File Name */>::Pointer GDS_LoadInstrument; /*!< Loads an instrument configuration */
  C_Function<int, const char * /* File Name */>::Pointer GDS_SaveInstrument; /*!< Saves the instrument configuration */
  C_Function<int>::Pointer GDS_NewInstrument; /*!< Creates a default instrument configuration */
  C_Function<int>::Pointer GDS_ResetInstrument; /*!< Resets the instrument */
  C_Function<int>::Pointer GDS_ZeroSupplies; /*!< Sets all voltage elements to a zero level */
  C_Function<int>::Pointer GDS_TestCommunication; /*!< Validates communication with the HW. */
  C_Function<int, int /* Option */, void * /* Value */>::Pointer GDS_GetOption; /*!< Retrieves an instrument option */
  C_Function<int, int /* Option */, const void * /* Value */>::Pointer GDS_SetOption; /*!< Sets an instrument option */
  C_Function<int, SesNS::WInstrumentInfo *>::Pointer GDS_GetInstrumentInfo; /*!< Retrieves instrument information */
  C_Function<int, SesNS::WDetectorInfo *>::Pointer GDS_GetDetectorInfo; /*!< Retrieves detector information */
  C_Function<bool>::Pointer GDS_HasSupplyLib; /*!< Checks for presence of a supply library */
  C_Function<bool>::Pointer GDS_HasDetectorLib; /*!< Checks for presence of a detector library */
  C_Function<bool>::Pointer GDS_HasSignalsLib; /*!< Checks for presence of a signal library */
  C_Function<int, char * /* Element Sets */, int * /* Buffer Length */>::Pointer GDS_GetElementSets; /*!< Retrieves the list of element sets, or the required size of the string buffer to store the element sets */
  C_Function<int, char * /* Element Names */, int * /* Buffer Length */>::Pointer GDS_GetElements; /*!< Retrievs a the list of element names, or the required size of the string buffer to store the element names */
  C_Function<int, char * /* Lens Modes */, int * /* Buffer Length */>::Pointer GDS_GetLensModes; /*!< Retrieves the list of lens modes, or the required size of the string buffer to store the lens modes */
  C_Function<int, const char * /* Lens Mode */, char * /* Pass Energies */, int * /* Buffer Length */>::Pointer GDS_GetPassEnergies; /*!< Retrieves the list of pass energies for the current lens mode, or the required size of the string buffer to store the pass energies */
  C_Function<int, char * /* Element Set */, int * /* Buffer Length */>::Pointer GDS_GetCurrElementSet; /*!< Retrieves the name of the currently selected element set */
  C_Function<int, char * /* Lens Mode */, int * /* Buffer Length */>::Pointer GDS_GetCurrLensMode; /*!< Retrieves the name of the currently selected lens mode */

  C_Function<int, double * /* Pass Energy */>::Pointer GDS_GetCurrPassEnergy; /*!< Retrieves the currently selected pass energy */
  C_Function<int, double * /* Kinetic Energy */>::Pointer GDS_GetCurrKineticEnergy; /*!< Retrieves the currently selected kinetic energy */
  C_Function<int, double * /* Excitation Energy */>::Pointer GDS_GetCurrExcitationEnergy; /*!< Retrieves the currently selected excitation energy */
  C_Function<int, double * /* Binding Energy */>::Pointer GDS_GetCurrBindingEnergy; /*!< Retrieves the currently selected binding energy */
  C_Function<int, SesNS::WDetector * /* Detecor Region */>::Pointer GDS_GetGlobalDetector; /*!< Retrieves the global detector region */
  
  C_Function<int, const char * /* Element Name */, double * /* Value */>::Pointer GDS_GetElement; /*!< Retrieves the voltage currently applied to an element */
  C_Function<int, const char * /* Element Set */>::Pointer GDS_SetElementSet; /*!< Selects element set */
  C_Function<int, const char * /* Lens Mode */>::Pointer GDS_SetLensMode; /*!< Selects lens mode */
  C_Function<int, double /* Pass Energy */>::Pointer GDS_SetPassEnergy; /*!< Selects pass energy. The lens mode and element set must be selected before this. */
  C_Function<int, double /* Kinetic Energy */>::Pointer GDS_SetKineticEnergy; /*!< Changes kinetic energy. */
  
  C_Function<int, double /* Excitation Energy */>::Pointer GDS_SetExcitationEnergy; /*!< Changes excitation energy */
  C_Function<int, double /* Binding Energy */>::Pointer GDS_SetBindingEnergy; /*!< Changes binding energy */
  C_Function<int, SesNS::WDetector *>::Pointer GDS_SetGlobalDetector; /*!< Modifies the global detector region */
  C_Function<int, const char * /* Element Name */, double /* Value */>::Pointer GDS_SetElement; /*!< Changes element voltage */
  C_Function<int, SesNS::WRegion *, int * /* Steps */, double * /* Time */, double * /* Energy Step */>::Pointer GDS_CheckRegion; /*!< Validates a region */
  C_Function<int, SesNS::WRegion *, SesNS::WSpectrum **, const char * /* Temporary File */, int /* Sweep */, SesNS::PointReady, SesNS::RegionReady>::Pointer GDS_Start; /*!< Starts an acquisition */
  
  C_Function<int>::Pointer GDS_Stop; /*!< Aborts the current acquisition */
  C_Function<int, int * /* Status */>::Pointer GDS_GetStatus; /*!< Retrieves the current status of the library */
  C_Function<int, double * /* Total Drift */, double * /* Delta-drift */>::Pointer GDS_GetDrift; /*!< Retrieves the energy drift. Used in conjunction to a drift region */
  C_Function<int, SesNS::WRegion *, SesNS::WSpectrum **, SesNS::OffsetReady, SesNS::RegionReady>::Pointer GDS_CalibrateOffset; /*!< Starts an energy offset calibration */
  C_Function<int, double * /* Offset */>::Pointer GDS_GetOffset; /*!< Retrieves an energy offset */
  C_Function<int, bool /* On */>::Pointer GDS_UseDetector; /*!< Use the detector */
  
  C_Function<int, bool /* On */>::Pointer GDS_UseSignals; /*!< Use signals */
  C_Function<int, SesNS::WSpectrum **>::Pointer GDS_GetCurrSpectrum; /*!< Retrieve the current spectrum */
  C_Function<int, SesNS::WSignals **>::Pointer GDS_GetCurrSignals; /*!< Retrieve the current signals */
  C_Function<int>::Pointer GDS_InstallInstrument; /*!< Opens an installation dialog */
  C_Function<int>::Pointer GDS_InstallSupplies; /*!< Opens a supply installation dialog */
  
  C_Function<int>::Pointer GDS_InstallElements; /*!< Opens an element installation dialog */
  C_Function<int>::Pointer GDS_InstallLensModes; /*!< Opens a lens mode installation dialog*/
  C_Function<int, SesNS::WDetector *>::Pointer GDS_SetupDetector; /*!< Opens the Global Detector Setup dialog */
  C_Function<int>::Pointer GDS_SetupSignals; /*!< Opens a signals setup dialog */
  C_Function<int>::Pointer GDS_CalibrateVoltages; /*!< Opens the Voltage Calibration dialog */
  C_Function<int>::Pointer GDS_CalibrateDetector; /*!< Opens the Detector Calibrartion dialog */
  
  C_Function<int>::Pointer GDS_ControlSupplies; /*!< Opens a supply control dialog */
  C_Function<int>::Pointer GDS_SupplyInfo; /*!< Opens a supply information dialog */
  C_Function<int>::Pointer GDS_DetectorInfo; /*!< Opens a detector info dialog */
  C_Function<int, unsigned char * /* Data */, int * /* Width */, int * /* Height */, int * /* Byte Size */>::Pointer GDS_GetRawImage; /*!< Retrieves a raw image from the detector */
  C_Function<int, SesNS::WRegion *, SesNS::WSpectrum **, SesNS::WSignals **, const char * /* Temporary File */, SesNS::PointReady, SesNS::RegionReady>::Pointer  GDS_InitAcquisition; /*!< Initializing acquisition. Replaces Start(). Introduced in SESInstrument version 1.2.5-rc6 */
  
  C_Function<int, int /* Sweep */>::Pointer GDS_StartAcquisition; /*!< Starts acquisition. Replaces Start(). Introduced in SESInstrument version 1.2.5-rc6. */
  C_Function<int, const char * /* Property */, void * /* Value Buffer */, int * /* Size */>::Pointer SC_GetProperty; /*!< Reads a property from  the instrument library */
  C_Function<int, const char * /* Property */, const void * /* Value Buffer */>::Pointer SC_SetProperty; /*!< Sets an instrument library property */
  C_Function<int, const char * /* Property */, const void * /* Value Buffer */, int /* Buffer Size */>::Pointer SC_SetPropertyEx; /*!< Sets an instrument library property. This function also contains the size (in bytes) of the data block. */
};

#endif
