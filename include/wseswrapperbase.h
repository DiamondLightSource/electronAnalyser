#ifndef __SESWRAPPER_WSESWRAPPERBASE_H__
#define __SESWRAPPER_WSESWRAPPERBASE_H__

#include "wvariable.hpp"
#include "sestypes.h"

#include <map>
#include <vector>
#include <string>

class WSesInstrument;
class WError;

class WSESWrapperBase
{
public:
  typedef WVariable<WSESWrapperBase> Property;
  typedef std::pair<std::string, Property> PropertyKeyValue;
  typedef std::map<std::string, Property> PropertyMap;
  typedef std::vector<std::string> NameVector;
  typedef std::vector<double> DoubleVector;

  WSESWrapperBase(const char *workingDir);
  ~WSESWrapperBase();

protected:
  // Property getters
  int getLibDescription(int index, void *value, int &size);
  int getLibVersion(int index, void *value, int &size);
  int getLibError(int index, void *value, int &size);
  int getLibWorkingDir(int index, void *value, int &size);
  int getInstrumentStatus(int index, void *value, int &size);
  int getAlwaysDelayRegion(int index, void *value, int &size);
  int getAllowIOWithDetector(int index, void *value, int &size);
  int getInstrumentModel(int index, void *value, int &size);
  int getInstrumentSerialNo(int index, void *value, int &size);
  int getDetectorInfo(int index, void *value, int &size);
  int getDetectorRegion(int index, void *value, int &size);
  int getElementSetCount(int index, void *value, int &size);
  int getElementSet(int index, void *value, int &size);
  int getLensModeCount(int index, void *value, int &size);
  int getLensMode(int index, void *value, int &size);
  int getPassEnergyCount(int index, void *value, int &size);
  int getPassEnergy(int index, void *value, int &size);
  int getAnalyzerRegion(int index, void *value, int &size);
  int getUseExternalIO(int index, void *value, int &size);
  int getUseDetector(int index, void *value, int &size);
  int getRegionName(int index, void *value, int &size);
  int getTempFileName(int index, void *value, int &size);
  int getResetDataBetweenIterations(int index, void *value, int &size);

  // Property setters
  int readOnlyStub(int index, const void *value);
  int setLibWorkingDir(int index, const void *value);
  int setAlwaysDelayRegion(int index, const void *value);
  int setAllowIOWithDetector(int index, const void *value);
  int setDetectorRegion(int index, const void *value);
  int setElementSet(int index, const void *value);
  int setLensMode(int index, const void *value);
  int setPassEnergy(int index, const void *value);
  int setAnalyzerRegion(int index, const void *value);
  int setUseExternalIO(int index, const void *value);
  int setUseDetector(int index, const void *value);
  int setRegionName(int index, const void *value);
  int setTempFileName(int index, const void *value);
  int setResetDataBetweenIterations(int index, const void *value);

  bool loadElementSets();
  bool loadLensModes();
  bool loadPassEnergies(const std::string &lensMode, DoubleVector &result);

  void splitSESList(const char *buffer, int bufferSize, NameVector &result);

  PropertyMap properties_;

  WSesInstrument *lib_;

  bool instrumentLoaded_;
  std::string workingDir_;
  SesNS::WInstrumentInfo sesInstrumentInfo_;
  SesNS::WDetectorInfo sesDetectorInfo_;
  SesNS::WDetector sesDetectorRegion_;
  SesNS::WRegion sesRegion_;
  SesNS::WSpectrum *sesSpectrum_;
  SesNS::WSignals *sesSignals_;
  std::string tempFileName_;
  bool usingDetector_;
  bool usingSignals_;

  NameVector elementSets_;
  NameVector lensModes_;
  DoubleVector passEnergies_;
  unsigned int startTime_;

  bool blockPointReady_;
  bool blockRegionReady_;
  bool resetDataBetweenIterations_;
  int currentStep_;
  int iteration_;
  WError *errors_;
};

#endif
