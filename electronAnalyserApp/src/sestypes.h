#ifndef __SESWRAPPER_SESTYPES_H__
#define __SESWRAPPER_SESTYPES_H__

#include "types.h"
#include <memory.h>

/*!
 * This is a namespace containing the original types and functions of the SESInstrument library.
 */
namespace SesNS
{
  /* Basic types */
  typedef char Char32[32];
  typedef double *Vector;
  typedef Vector *Matrix;

  /* C callbacks */
  typedef void (STDCALL *FnErrorNotify)(int ErrorCode);
  typedef void (STDCALL *FnPointReady)(int Index);
  typedef void (STDCALL *FnRegionReady)();
  typedef void (STDCALL *FnOffsetReady)(double *Offset, bool *Finished);

  /* Enumerations */
  enum InstrumentOptions
  {
    FermiEdge,
    SmoothPoints,
    AlwaysDelayRegion,
    ZeroVoltages,
    ShowProgress,
    DelayDrawing,
    ImageScaleFactor,
    AllowSignalsWithDetector
  };

  /*! @brief Enumeration of the possible states for the SESInstrument library */
  enum InstrumentStatus
  {
    Normal = 0, /*!< The library is functioning properly, and is currently idle. */
    Running = 1, /*!< An acquisition is currently running. */
    AcqError = 2, /*!< An acquisition was interrupted with an error. */
    NonOperational = 3, /*!< The library is not operational. Resetting may resolve the issue. */
    NotInitialized = 4 /*!< The SESInstrument library has not been initialized (the GDS_Initialize function has not been called). */
  };

  /* Structures */
  struct WInstrumentInfo
  {
    WInstrumentInfo()
    {
      memset(this, 0, sizeof(WInstrumentInfo));
    }
    char Model[256];
    char SerialNo[256];
  };

  struct WDetectorInfo
  {
    WDetectorInfo()
    {
      memset(this, 0, sizeof(WDetectorInfo));
    }
    char Name[256];
    bool TimerControlled;
    int XChannels;
    int YChannels;
    int MaxSlices;
    int MaxChannels;
    int FramesPerSec;
    bool ADCPresent;
    bool DiscPresent;
  };

  struct WDetector
  {
    WDetector()
    {
      memset(this, 0, sizeof(WDetector));
      ADCMode = true;
    }

    int FirstXChannel;
    int LastXChannel;
    int FirstYChannel;
    int LastYChannel;
    int Slices;
    bool ADCMode;
    int ADCMask;
    int DiscLvl;
  };

  struct WRegion
  {
    WRegion()
    {
      memset(this, 0, sizeof(WRegion));
      Kinetic = true;
      Fixed = true;
      UseRegionDetector = false;
      ADCMode = true;
    }
    Char32 Name;
    double ExcEnergy;
    bool Kinetic;
    bool Fixed;
    double HighEnergy;
    double LowEnergy;
    double FixEnergy;
    double EnergyStep;
    int StepTime;
    bool UseRegionDetector;
    //Detector DetectorRegion;
    int FirstXChannel;
    int LastXChannel;
    int FirstYChannel;
    int LastYChannel;
    int Slices;
    bool ADCMode;
    int ADCMask;
    int DiscLvl;
    // End of Detector DetectorRegion
    Char32 LensMode;
    double PassEnergy;
    bool DriftRegion;
    int Grating;
    int Order;
    double Illumination;
    double Slit;
  };

  struct WSpectrum
  {
    WSpectrum()
    {
      memset(this, 0, sizeof(WSpectrum));
    }

    int Channels;
    int Slices;
    int Sweeps;
    Char32 CountUnit;
    Char32 ChannelUnit;
    Char32 SliceUnit;
    Vector ChannelScale;
    Vector SliceScale;
    Matrix Data;
    Vector SumData;
  };

  struct WSignals
  {
    WSignals()
    {
      memset(this, 0, sizeof(WSignals));
    }

    int Count;
    int Steps;
    int Sweeps;
    Char32 StepsUnit;
    Vector StepsScale;
    Char32 *Names;
    Matrix Data;
  };

  typedef int (STDCALL * FnGDS_GetLastError)();
  typedef char *(STDCALL * FnGDS_GetLastErrorString)();
  typedef int (STDCALL * FnGDS_Initialize)(FnErrorNotify errorNotify, void *mainWindow);
  typedef void (STDCALL * FnGDS_Finalize)();
  typedef int (STDCALL * FnGDS_LoadInstrument)(const char *fileName);
  typedef int (STDCALL * FnGDS_SaveInstrument)(const char *fileName);
  typedef int (STDCALL * FnGDS_NewInstrument)();
  typedef int (STDCALL * FnGDS_LoadRunVar)(const char *fileName);
  typedef int (STDCALL * FnGDS_SaveRunVar)(const char *fileName);
  typedef int (STDCALL * FnGDS_ResetInstrument)();
  typedef int (STDCALL * FnGDS_ZeroSupplies)();
  typedef int (STDCALL * FnGDS_TestCommunication)();
  typedef int (STDCALL * FnGDS_GetOption)(int Option, void *value);
  typedef int (STDCALL * FnGDS_SetOption)(int Option, const void *value);
  typedef int (STDCALL * FnGDS_GetInstrumentInfo)(const WInstrumentInfo *instrumentInfo);
  typedef int (STDCALL * FnGDS_GetDetectorInfo)(const WDetectorInfo *detectorInfo);
  typedef bool (STDCALL * FnGDS_HasSupplyLib)();
  typedef bool (STDCALL * FnGDS_HasDetectorLib)();
  typedef bool (STDCALL * FnGDS_HasSignalsLib)();
  typedef int (STDCALL * FnGDS_GetElementSets)(const char *elementSets, int *len);
  typedef int (STDCALL * FnGDS_GetLensModes)(const char *lensModes, int *len);
  typedef int (STDCALL * FnGDS_GetPassEnergies)(const char *lensMode, char *passEnergies, int *len);
  typedef int (STDCALL * FnGDS_GetCurrElementSet)(char *elementSet, int *len);
  typedef int (STDCALL * FnGDS_GetCurrLensMode)(char *lensMode, int *len);
  typedef int (STDCALL * FnGDS_GetCurrPassEnergy)(double *passEnergy);
  typedef int (STDCALL * FnGDS_GetCurrKineticEnergy)(double *kineticEnergy);
  typedef int (STDCALL * FnGDS_GetCurrExcitationEnergy)(double *excitationEnergy);
  typedef int (STDCALL * FnGDS_GetCurrBindingEnergy)(double *bindingEnergy);
  typedef int (STDCALL * FnGDS_GetGlobalDetector)(WDetector *detectorRegion);
  typedef int (STDCALL * FnGDS_GetElement)(const char *elementName, double *voltage);
  typedef int (STDCALL * FnGDS_SetElementSet)(const char *elementSet);
  typedef int (STDCALL * FnGDS_SetLensMode)(const char *lensMode);
  typedef int (STDCALL * FnGDS_SetPassEnergy)(double passEnergy);
  typedef int (STDCALL * FnGDS_SetKineticEnergy)(double kineticEnergy);
  typedef int (STDCALL * FnGDS_SetExcitationEnergy)(double excitationEnergy);
  typedef int (STDCALL * FnGDS_SetBindingEnergy)(double bindingEnergy);
  typedef int (STDCALL * FnGDS_SetGlobalDetector)(WDetector *detectorRegion);
  typedef int (STDCALL * FnGDS_SetElement)(const char *elementName, double voltage);
  typedef bool (STDCALL * FnGDS_CheckRegion)(WRegion *region, int *steps, double *time, double *energyStep);
  typedef int (STDCALL * FnGDS_Start)(WRegion *region, WSpectrum **spectrum, const char *tempFile,
                        int sweepNumber, FnPointReady pointReady, FnRegionReady regionReady);
  typedef int (STDCALL * FnGDS_Stop)();
  typedef int (STDCALL * FnGDS_GetStatus)(int *status);
  typedef int (STDCALL * FnGDS_GetDrift)(double *totalDrift, double *deltaDrift);
  typedef int (STDCALL * FnGDS_CalibrateOffset)(WRegion *region, WSpectrum **spectrum,
                                  FnOffsetReady offsetReady, FnRegionReady regionReady);
  typedef int (STDCALL * FnGDS_GetOffset)(double *offset);
  typedef int (STDCALL * FnGDS_UseDetector)(bool useDetector);
  typedef int (STDCALL * FnGDS_UseSignals)(bool useSignals);
  typedef int (STDCALL * FnGDS_GetCurrSpectrum)(WSpectrum **spectrum);
  typedef int (STDCALL * FnGDS_GetCurrSignals)(WSignals **signals);
  typedef int (STDCALL * FnGDS_InstallInstrument)();
  typedef int (STDCALL * FnGDS_InstallSupplies)();
  typedef int (STDCALL * FnGDS_InstallElements)();
  typedef int (STDCALL * FnGDS_InstallLensModes)();
  typedef int (STDCALL * FnGDS_SetupDetector)(WDetector *detectorRegion);
  typedef int (STDCALL * FnGDS_SetupSignals)();
  typedef int (STDCALL * FnGDS_CalibrateVoltages)();
  typedef int (STDCALL * FnGDS_CalibrateDetector)();
  typedef int (STDCALL * FnGDS_ControlSupplies)();
  typedef int (STDCALL * FnGDS_SupplyInfo)();
  typedef int (STDCALL * FnGDS_DetectorInfo)();
  typedef int (STDCALL * FnGDS_GetRawImage)(unsigned char *data, int *width, int *height, int *byteSize);
  typedef int (STDCALL * FnGDS_InitAcquisition)(WRegion *region, WSpectrum **spectrum, WSignals **signals, const char *tempFile, FnPointReady pointReady, FnRegionReady regionReady);
  typedef int (STDCALL * FnGDS_StartAcquisition)(int sweepNumber);
}

#endif

