#ifndef __SESWRAPPER_SESTYPES_H__
#define __SESWRAPPER_SESTYPES_H__

#include "c_function.hpp"
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
  typedef C_Function<void, int /* Error Code */>::Pointer ErrorNotify;
  typedef C_Function<void, int /* Point Index */>::Pointer PointReady;
  typedef C_Function<>::Pointer RegionReady;
  typedef C_Function<void, double * /* Offset */, bool * /* Finished */>::Pointer OffsetReady;

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
    char ADCMode;
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
      UseRegionDetector = true;
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
    char ADCMode;
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
}

#endif

