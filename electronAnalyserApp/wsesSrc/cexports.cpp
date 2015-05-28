#include "wseswrappermain.h"
#include "types.h"
#include "sestypes.h"
#include "werror.h"

#include <list>
#include <string>
#include <sstream>

#define NOMINMAX
#define _WIN32_WINNT 0x0502
#include <windows.h>

namespace
{
  WSESWrapperMain *gMain = 0;
  int gAttachedThreads = 0;
}

BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID)
{
  BOOL result = TRUE;
  switch( dwReason ) 
  { 
  case DLL_PROCESS_ATTACH:
    {
      std::string baseDir;
      baseDir.resize(1024);
      ::GetModuleFileName(hInstance, const_cast<char *>(baseDir.c_str()), baseDir.size());
      int pos = baseDir.find_last_of('/');
      if (pos == baseDir.npos)
        pos = baseDir.find_last_of('\\');
      baseDir = baseDir.substr(0, pos);
      baseDir.append("\\..\\");
      ::_putenv_s("SES_BASE_DIR", baseDir.c_str());
      gMain = WSESWrapperMain::instance();
      break;
    }
  case DLL_THREAD_ATTACH:
    {
      gAttachedThreads++;
      break;
    }
  case DLL_THREAD_DETACH:
    {
      gAttachedThreads--;
      break;
    }

  case DLL_PROCESS_DETACH:
    {
      if (gMain != 0)
        gMain->release();
      gMain = 0;
      break;
    }
  }
  return result;
}

#ifdef __cplusplus
extern "C"
{
#endif

// Standard Library functions

/*!
 * \defgroup exported_functions Exported Functions
 * \brief This is a list of C functions exported from SESWrapper. All functions wrap calls
 * to a global instance of the WSESWrapperMain class.
 * \{
 */


/*!
 * Exported function that wraps the WSESWrapperMain::initialize() member function.
 */
int __stdcall WRP_Initialize(void *reserved)
{
  return gMain->initialize(reserved);
}

/*!
 * Exported function that wraps the WSESWrapperMain::finalize() member function.
 */
int __stdcall WRP_Finalize()
{
  return gMain->finalize();
}

// Property functions

/*!
 * Exported function that wraps the WSESWrapperMain::getProperty() member funciton. Does not verify the
 * type of \p value.
 */
int __stdcall WRP_GetProperty(const char *property, int index, void *value, int *size)
{
  return gMain->getProperty(property, index, value, *size);
}

/*!
 * Exported function that wraps the WSESWrapperMain::getProperty() member function for properties
 * of type TYPE_BOOL.
 */
int __stdcall WRP_GetPropertyBool(const char *property, int index, bool *value, int *size)
{
  int error = WError::ERR_NOT_APPLICABLE;
  if (gMain->parameterType(property) == WSESWrapperMain::Property::TYPE_BOOL)
    error = gMain->getProperty(property, index, value, *size);
  return error;
}

/*!
 * Exported function that wraps the WSESWrapperMain::getProperty() member function for properties
 * of type TYPE_INT32.
 */
int __stdcall WRP_GetPropertyInteger(const char *property, int index, int *value, int *size)
{
  int error = WError::ERR_NOT_APPLICABLE;
  if (gMain->parameterType(property) == WSESWrapperMain::Property::TYPE_INT32)
    error = gMain->getProperty(property, index, value, *size);
  return error;
}

/*!
 * Exported function that wraps the WSESWrapperMain::getProperty() member function for properties
 * of type TYPE_DOUBLE.
 */
int __stdcall WRP_GetPropertyDouble(const char *property, int index, double *value, int *size)
{
  int error = WError::ERR_NOT_APPLICABLE;
  if (gMain->parameterType(property) == WSESWrapperMain::Property::TYPE_DOUBLE)
    error = gMain->getProperty(property, index, value, *size);
  return error;
}

/*!
 * Exported function that wraps the WSESWrapperMain::getProperty() member function for properties
 * of type TYPE_STRING.
 */
int __stdcall WRP_GetPropertyString(const char *property, int index, char *value, int *size)
{
  int error = WError::ERR_NOT_APPLICABLE;
  if (gMain->parameterType(property) == WSESWrapperMain::Property::TYPE_STRING)
    error = gMain->getProperty(property, index, value, *size);
  return error;
}

/*!
 * Exported function that wraps the WSESWrapperMain::getProperty() member function for the \c detector_info property.
 */
int __stdcall WRP_GetDetectorInfo(SESWrapperNS::PDetectorInfo detectorInfo)
{
  int size = sizeof(SESWrapperNS::DetectorInfo);
  return gMain->getProperty("detector_info", 0, detectorInfo, size);
}

/*!
 * Exported function that wraps the WSESWrapperMain::getProperty() member function for the \c detector_region property.
 */
int __stdcall WRP_GetDetectorRegion(SESWrapperNS::PDetectorRegion detectorRegion)
{
  int size = sizeof(SESWrapperNS::DetectorRegion);
  return gMain->getProperty("detector_region", 0, detectorRegion, size);
}

/*!
 * Exported function that wraps the WSESWrapperMain::getProperty() member function for the \c analyzer_region property.
 */
int __stdcall WRP_GetAnalyzerRegion(SESWrapperNS::PAnalyzerRegion analyzerRegion)
{
  int size = sizeof(SESWrapperNS::AnalyzerRegion);
  return gMain->getProperty("analyzer_region", 0, analyzerRegion, size);
}

/*!
 * Exported function that wraps the WSESWrapperMain::setProperty() member function. Does not verify the
 * type of \p value.
 */
int __stdcall WRP_SetProperty(const char *property, int size, const void *value)
{
  return gMain->setProperty(property, size, value);
}

/*!
 * Exported function that wraps the WSESWrapperMain::setProperty() member function for properties of type
 * TYPE_BOOL.
 */
int __stdcall WRP_SetPropertyBool(const char *property, int /* size */, const bool *value)
{
  int error = WError::ERR_NOT_APPLICABLE;
  if (gMain->parameterType(property) == WSESWrapperMain::Property::TYPE_BOOL)
    error = gMain->setProperty(property, sizeof(bool), value);
  return error;
}

/*!
 * Exported function that wraps the WSESWrapperMain::setProperty() member function for properties of type
 * TYPE_INT32.
 */
int __stdcall WRP_SetPropertyInteger(const char *property, int /* size */, const int *value)
{
  int error = WError::ERR_NOT_APPLICABLE;
  if (gMain->parameterType(property) == WSESWrapperMain::Property::TYPE_INT32)
    error = gMain->setProperty(property, sizeof(int), value);
  return error;
}

/*!
 * Exported function that wraps the WSESWrapperMain::setProperty() member function for properties of type
 * TYPE_DOUBLE.
 */
int __stdcall WRP_SetPropertyDouble(const char *property, int /* size  */, const double *value)
{
  int error = WError::ERR_NOT_APPLICABLE;
  if (gMain->parameterType(property) == WSESWrapperMain::Property::TYPE_DOUBLE)
    error = gMain->setProperty(property, sizeof(double), value);
  return error;
}

/*!
 * Exported function that wraps the WSESWrapperMain::setProperty() member function for properties of type
 * TYPE_STRING.
 */
int __stdcall WRP_SetPropertyString(const char *property, int /* size */, const char *value)
{
  int error = WError::ERR_NOT_APPLICABLE;
  if (gMain->parameterType(property) == WSESWrapperMain::Property::TYPE_STRING)
    error = gMain->setProperty(property, strlen(value), value);
  return error;
}

/*!
 * Exported function that wraps the WSESWrapperMain::setProperty() member function for the \c detector_region
 * property.
 */
int __stdcall WRP_SetDetectorRegion(SESWrapperNS::PDetectorRegion detectorRegion)
{
  return gMain->setProperty("detector_region", sizeof(SESWrapperNS::WDetectorRegion), detectorRegion);
}

/*!
 * Exported function that wraps the WSESWrapperMain::setProperty() member function for the \c analyzer_region
 * property.
 */
int __stdcall WRP_SetAnalyzerRegion(SESWrapperNS::PAnalyzerRegion analyzerRegion)
{
  return gMain->setProperty("analyzer_region", sizeof(SESWrapperNS::WAnalyzerRegion), analyzerRegion);
}

/*!
 * Exported function that wraps the WSESWrapperMain::validate() member function.
 */
int __stdcall WRP_Validate(const char *elementSet, const char *lensMode, double passEnergy, double kineticEnergy)
{
  return gMain->validate(elementSet, lensMode, passEnergy, kineticEnergy);
}

// HW specific functions

/*!
 * Exported function that wraps the WSESWrapperMain::resetHW() member function.
 */
int __stdcall WRP_ResetHW()
{
  return gMain->resetHW();
}

/*!
 * Exported function that wraps the WSESWrapperMain::testHW() member function.
 */
int __stdcall WRP_TestHW()
{
  return gMain->testHW();
}

// Analyzer specific members

/*!
 * Exported function that wraps the WSESWrapperMain::loadInstrument() member function.
 */
int __stdcall WRP_LoadInstrument(const char *fileName)
{
  return gMain->loadInstrument(fileName);
}

/*!
 * Exported function that wraps the WSESWrapperMain::zeroSupplies() member function.
 */
int __stdcall WRP_ZeroSupplies()
{
  return gMain->zeroSupplies();
}

/*!
 * Exported function that wraps the WSESWrapperMain::getBindingEnergy() member function.
 */
int __stdcall WRP_GetBindingEnergy(double *bindingEnergy)
{
  return gMain->getBindingEnergy(bindingEnergy);
}

/*!
 * Exported function that wraps the WSESWrapperMain::setBindingEnergy() member function.
 */
int __stdcall WRP_SetBindingEnergy(const double bindingEnergy)
{
  return gMain->setBindingEnergy(bindingEnergy);
}

/*!
 * Exported function that wraps the WSESWrapperMain::getKineticEnergy() member function.
 */
int __stdcall WRP_GetKineticEnergy(double *kineticEnergy)
{
  return gMain->getKineticEnergy(kineticEnergy);
}

/*!
 * Exported function that wraps the WSESWrapperMain::setKineticEnergy() member function.
 */
int __stdcall WRP_SetKineticEnergy(const double kineticEnergy)
{
  return gMain->setKineticEnergy(kineticEnergy);
}

/*!
 * Exported function that wraps the WSESWrapperMain::getExcitationEnergy() member function.
 */
int __stdcall WRP_GetExcitationEnergy(double *excitationEnergy)
{
  return gMain->getExcitationEnergy(excitationEnergy);
}

/*!
 * Exported function that wraps the WSESWrapperMain::setExcitationEnergy() member function.
 */
int __stdcall WRP_SetExcitationEnergy(const double excitationEnergy)
{
  return gMain->setExcitationEnergy(excitationEnergy);
}

/*!
 * Exported function that wraps the WSESWrapperMain::getElementVoltage() member function.
 */
int __stdcall WRP_GetElementVoltage(const char *elementName, double *voltage)
{
  return gMain->getElementVoltage(elementName, voltage);
}

/*!
 * Exported function that wraps the WSESWrapperMain::setElementVoltage() member function.
 */
int __stdcall WRP_SetElementVoltage(const char *elementName, const double voltage)
{
  return gMain->setElementVoltage(elementName, voltage);
}

// Acquisition specific members

/*!
 * Exported function that wraps the WSESWrapperMain::checkAnalyzerRegion() member function.
 */
int __stdcall WRP_CheckAnalyzerRegion(SESWrapperNS::PAnalyzerRegion analyzerRegion, int *steps, double *time_ms, double *energyStep)
{
  return gMain->checkAnalyzerRegion(analyzerRegion, steps, time_ms, energyStep);
}

/*!
 * Exported function that wraps the WSESWrapperMain::initAcquisition() member function.
 */
int __stdcall WRP_InitAcquisition(const bool blockPointReady, const bool blockRegionReady)
{
  return gMain->initAcquisition(blockPointReady, blockRegionReady);
}

/*!
 * Exported function that wraps the WSESWrapperMain::startAcquisition() member function.
 */
int __stdcall WRP_StartAcquisition()
{
  return gMain->startAcquisition();
}

/*!
 * Exported function that wraps the WSESWrapperMain::stopAcquisition() member function.
 */
int __stdcall WRP_StopAcquisition()
{
  return gMain->stopAcquisition();
}

/*!
 * Exported function that wraps the WSESWrapperMain::getAcquiredData() member function when \p variable is of
 * type \c TYPE_INT32.
 */
int __stdcall WRP_GetAcquiredDataInteger(const char *variable, int index, int *data, int *size)
{
  int error = WError::ERR_NOT_APPLICABLE;
  if (gMain->parameterType(variable) == WSESWrapperMain::Property::TYPE_INT32)
    error = gMain->getAcquiredData(variable, index, data, *size);
  return error;
}

/*!
 * Exported function that wraps the WSESWrapperMain::getAcquiredData() member function when \p variable is of
 * type \c TYPE_DOUBLE.
 */
int __stdcall WRP_GetAcquiredDataDouble(const char *parameter, int index, double *data, int *size)
{
  int error = WError::ERR_NOT_APPLICABLE;
  if (gMain->parameterType(parameter) == WSESWrapperMain::Property::TYPE_DOUBLE)
    error = gMain->getAcquiredData(parameter, index, data, *size);
  return error;
}

/*!
 * Exported function that wraps the WSESWrapperMain::getAcquiredData() member function when \p variable is of
 * type \c TYPE_STRING.
 */
int __stdcall WRP_GetAcquiredDataString(const char *parameter, int index, char *data, int *size)
{
  int error = WError::ERR_NOT_APPLICABLE;
  if (gMain->parameterType(parameter) == WSESWrapperMain::Property::TYPE_STRING)
    error = gMain->getAcquiredData(parameter, index, data, *size);
  return error;
}

/*!
 * Exported function that wraps the WSESWrapperMain::getAcquiredData() member function when \p variable is of
 * type \c TYPE_VECTOR_DOUBLE.
 */
int __stdcall WRP_GetAcquiredDataVectorDouble(const char *parameter, int index, double *data, int *size)
{
  int error = WError::ERR_NOT_APPLICABLE;
  if (gMain->parameterType(parameter) == WSESWrapperMain::Property::TYPE_VECTOR_DOUBLE)
    error = gMain->getAcquiredData(parameter, index, data, *size);
  return error;
}

/*!
 * Exported function that wraps the WSESWrapperMain::getAcquiredData() member function when \p variable is of
 * type \c TYPE_VECTOR_INT32.
 */
int __stdcall WRP_GetAcquiredDataVectorInt32(const char *parameter, int index, int *data, int *size)
{
  int error = WError::ERR_NOT_APPLICABLE;
  if (gMain->parameterType(parameter) == WSESWrapperMain::Property::TYPE_VECTOR_INT32)
    error = gMain->getAcquiredData(parameter, index, data, *size);
  return error;
}

/*!
 * Exported function that wraps the WSESWrapperMain::waitForPointReady() member function.
 */
int __stdcall WRP_WaitForPointReady(int timeout_ms)
{
  return gMain->waitForPointReady(timeout_ms);
}

/*!
 * Exported function that wraps the WSESWrapperMain::waitForRegionReady() member function.
 */
int __stdcall WRP_WaitForRegionReady(int timeout_ms)
{
  return gMain->waitForRegionReady(timeout_ms);
}

/*!
 * Exported function that wraps the WSESWrapperMain::continueAcquisition() member function.
 */
int __stdcall WRP_ContinueAcquisition()
{
  return gMain->continueAcquisition();
}

/*! \} */

#ifdef __cplusplus
}
#endif
