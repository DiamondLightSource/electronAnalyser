#ifndef __SESWRAPPER_WSESWRAPPERMAIN_H__
#define __SESWRAPPER_WSESWRAPPERMAIN_H__

#ifndef DOXYGEN_SHOULD_SKIP_THIS
#include "wseswrapperbase.h"
#include "wevent.h"
#endif /* DOXYGEN_SHOULD_SKIP_THIS */

class WSESWrapperMain : public WSESWrapperBase
{
  WSESWrapperMain();
  ~WSESWrapperMain();

public:
  typedef WVariable<WSESWrapperMain> DataParameter;
  typedef std::pair<std::string, DataParameter> DataParameterKeyValue;
  typedef std::map<std::string, DataParameter> DataParameterMap;

  static WSESWrapperMain *instance();
  void release();
  int references() const;

  bool isInitialized();
  int initialize(void *);
  int finalize();
  int getProperty(const char *name, int index, void *value, int &size);
  int getProperty(const char *name, int index, void *value);
  int setProperty(const char *name, int index, const void *value);
  int validate(const char *elementSet, const char *lensMode, double passEnergy, double kineticEnergy);
  int resetHW();
  int testHW();
  int loadInstrument(const char *fileName);
  int zeroSupplies();
  int getBindingEnergy(double *bindingEnergy);
  int setBindingEnergy(const double bindingEnergy);
  int getKineticEnergy(double *kineticEnergy);
  int setKineticEnergy(const double kineticEnergy);
  int getExcitationEnergy(double *excitationEnergy);
  int setExcitationEnergy(const double excitationEnergy);
  int getElementVoltage(const char *elementName, double *voltage);
  int setElementVoltage(const char *element, const double voltage);
  int checkAnalyzerRegion(SESWrapperNS::AnalyzerRegion *analyzerRegion, int *steps, double *time_ms, double *minEnergyStep_eV);
  int initAcquisition(const bool blockPointReady, const bool blockRegionReady);
  int startAcquisition();
  int stopAcquisition();
  int getAcquiredData(const char *name, int index, void *data, int &size);
  int waitForPointReady(int timeout_ms);
  int waitForRegionReady(int timeout_ms);
  int continueAcquisition();

  int parameterType(const char *parameter);

  int getAcqChannels(int index, void *data, int &size);
  int getAcqSlices(int index, void *data, int &size);
  int getAcqIterations(int index, void *data, int &size);
  int getAcqIntensityUnit(int index, void *data, int &size);
  int getAcqChannelUnit(int index, void *data, int &size);
  int getAcqSliceUnit(int index, void *data, int &size);
  int getAcqSpectrum(int index, void *data, int &size);
  int getAcqImage(int index, void *data, int &size);
  int getAcqSlice(int index, void *data, int &size);
  int getAcqChannelScale(int index, void *data, int &size);
  int getAcqSliceScale(int index, void *data, int &size);
  int getAcqRawImage(int index, void *data, int &size);
  int getAcqCurrentStep(int index, void *data, int &size);
  int getAcqElapsedTime(int index, void *data, int &size);
  int getAcqIOPorts(int index, void *data, int &size);
  int getAcqIOSize(int index, void *data, int &size);
  int getAcqIOIterations(int index, void *data, int &size);
  int getAcqIOUnit(int index, void *data, int &size);
  int getAcqIOScale(int index, void *data, int &size);
  int getAcqIOSpectrum(int index, void *data, int &size);
  int getAcqIOData(int index, void *data, int &size);
  int getAcqIOPortName(int index, void *data, int &size);
  int getAcqCurrentPoint(int index, void *value, int &size);
  int getAcqPointIntensity(int index, void *value, int &size);
  int getAcqChannelIntensity(int index, void *value, int &size);

private:
  static void __stdcall errorNotify(int errorCode);
  static void __stdcall pointReady(int point);
  static void __stdcall regionReady();
  void errorNotifyHandler(int errorCode);
  void pointReadyHandler(int point);
  void regionReadyHandler();
  bool readSpectrumObject();
  bool readSignalsObject();

  static WSESWrapperMain *this_;
  static int references_;
  bool initialized_;
  int currentStep_;
  int currentPoint_;
  SesNS::WSpectrum *sesSpectrum_;
  SesNS::WSignals *sesSignals_;

  DataParameterMap dataParameters_;

  WEvent pointReadyEvent_; 
  WEvent regionReadyEvent_; 
  WEvent continueAcquisitionEvent_; 
  WEvent abortAcquisitionEvent_;
};

#endif
