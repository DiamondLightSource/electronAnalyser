#ifndef __SESWRAPPER_SESTYPES_H__
#define __SESWRAPPER_SESTYPES_H__

#include "types.h"
#include <memory.h>

/*! \namespace SesNS
 * This is a namespace containing the original types and functions of the SESInstrument library.
 */
namespace SesNS
{
  /* Basic types */
  typedef char Char32[32];
  typedef double *Vector;
  typedef Vector *Matrix;

  /* C callbacks */
  typedef void (__stdcall *FnErrorNotify)(int ErrorCode);
  typedef void (__stdcall *FnPointReady)(int Index);
  typedef void (__stdcall *FnRegionReady)();
  typedef void (__stdcall *FnOffsetReady)(double *Offset, bool *Finished);

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
    AllowSignalsWithDetector,
    ErrorLevel,
    AdjustRegionForSignalsWithDetector,
    DetectorCount,
    DetectorNamesSize,
    DetectorNames,
    ActiveDetector
  };

  /*! \brief Enumeration of the possible states for the SESInstrument library */
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

  typedef void (__stdcall *VoidNoArg)();
  typedef int (__stdcall *IntNoArg)();
  typedef const char *(__stdcall *StringNoArg)();
  typedef bool (__stdcall *BoolNoArg)();

  typedef int (__stdcall *FnGDS_Initialize)(FnErrorNotify errorNotify, void *mainWindow);
  typedef int (__stdcall *FnGDS_LoadInstrument)(const char *fileName);
  typedef int (__stdcall *FnGDS_SaveInstrument)(const char *fileName);
  typedef int (__stdcall *FnGDS_LoadRunVar)(const char *fileName);
  typedef int (__stdcall *FnGDS_SaveRunVar)(const char *fileName);
  typedef int (__stdcall *FnGDS_GetOption)(int Option, void *value);
  typedef int (__stdcall *FnGDS_SetOption)(int Option, const void *value);
  typedef int (__stdcall *FnGDS_GetInstrumentInfo)(const WInstrumentInfo *instrumentInfo);
  typedef int (__stdcall *FnGDS_GetDetectorInfo)(const WDetectorInfo *detectorInfo);
  typedef int (__stdcall *FnGDS_GetElementSets)(const char *elementSets, int *len);
  typedef int (__stdcall *FnGDS_GetElements)(const char *elementNames, int *len);
  typedef int (__stdcall *FnGDS_GetLensModes)(const char *lensModes, int *len);
  typedef int (__stdcall *FnGDS_GetPassEnergies)(const char *lensMode, char *passEnergies, int *len);
  typedef int (__stdcall *FnGDS_GetCurrElementSet)(char *elementSet, int *len);
  typedef int (__stdcall *FnGDS_GetCurrLensMode)(char *lensMode, int *len);
  typedef int (__stdcall *FnGDS_GetCurrPassEnergy)(double *passEnergy);
  typedef int (__stdcall *FnGDS_GetCurrKineticEnergy)(double *kineticEnergy);
  typedef int (__stdcall *FnGDS_GetCurrExcitationEnergy)(double *excitationEnergy);
  typedef int (__stdcall *FnGDS_GetCurrBindingEnergy)(double *bindingEnergy);
  typedef int (__stdcall *FnGDS_GetGlobalDetector)(WDetector *detectorRegion);
  typedef int (__stdcall *FnGDS_GetElement)(const char *elementName, double *voltage);
  typedef int (__stdcall *FnGDS_SetElementSet)(const char *elementSet);
  typedef int (__stdcall *FnGDS_SetLensMode)(const char *lensMode);
  typedef int (__stdcall *FnGDS_SetPassEnergy)(double passEnergy);
  typedef int (__stdcall *FnGDS_SetKineticEnergy)(double kineticEnergy);
  typedef int (__stdcall *FnGDS_SetExcitationEnergy)(double excitationEnergy);
  typedef int (__stdcall *FnGDS_SetBindingEnergy)(double bindingEnergy);
  typedef int (__stdcall *FnGDS_SetGlobalDetector)(WDetector *detectorRegion);
  typedef int (__stdcall *FnGDS_SetElement)(const char *elementName, double voltage);
  typedef int (__stdcall *FnGDS_CheckRegion)(WRegion *region, int *steps, double *time, double *energyStep);
  typedef int (__stdcall *FnGDS_Start)(WRegion *region, WSpectrum **spectrum, const char *tempFile,
                        int sweepNumber, FnPointReady pointReady, FnRegionReady regionReady);
  typedef int (__stdcall *FnGDS_GetStatus)(int *status);
  typedef int (__stdcall *FnGDS_GetDrift)(double *totalDrift, double *deltaDrift);
  typedef int (__stdcall *FnGDS_CalibrateOffset)(WRegion *region, WSpectrum **spectrum,
                                  FnOffsetReady offsetReady, FnRegionReady regionReady);
  typedef int (__stdcall *FnGDS_GetOffset)(double *offset);
  typedef int (__stdcall *FnGDS_UseDetector)(bool useDetector);
  typedef int (__stdcall *FnGDS_UseSignals)(bool useSignals);
  typedef int (__stdcall *FnGDS_GetCurrSpectrum)(WSpectrum **spectrum);
  typedef int (__stdcall *FnGDS_GetCurrSignals)(WSignals **signals);
  typedef int (__stdcall *FnGDS_SetupDetector)(WDetector *detectorRegion);
  typedef int (__stdcall *FnGDS_GetRawImage)(unsigned char *data, int *width, int *height, int *byteSize);
  typedef int (__stdcall *FnGDS_InitAcquisition)(WRegion *region, WSpectrum **spectrum, WSignals **signals, const char *tempFile, FnPointReady pointReady, FnRegionReady regionReady);
  typedef int (__stdcall *FnGDS_StartAcquisition)(int sweepNumber);
}

#endif

