#define STDCALL __stdcall
#define _bool unsigned char

typedef char String[];
typedef String *PString;

typedef double VectorDouble[];
typedef Vector *PVectorDouble;

typedef int VectorInt32[];
typedef VectorInt32 *PVectorInt32;

typedef struct {
  _bool timerControlled_;
  int xChannels_;
  int yChannels_;
  int maxSlices_;
  int maxChannels_;
  int frameRate_;
  _bool adcPresent_;
  _bool discPresent_;
} DetectorInfo;
typedef DetectorInfo *PDetectorInfo;

typedef struct {
  int firstXChannel_;
  int lastXChannel_;
  int firstYChannel_;
  int lastYChannel_;
  int slices_;
  _bool adcMode_;
} DetectorRegion;
typedef DetectorRegion *PDetectorRegion;

typedef struct {
  _bool fixed_;
  double highEnergy_;
  double lowEnergy_;
  double centerEnergy_;
  double energyStep_;
  int dwellTime_;
} AnalyzerRegion;
typedef AnalyzerRegion *PAnalyzerRegion;
