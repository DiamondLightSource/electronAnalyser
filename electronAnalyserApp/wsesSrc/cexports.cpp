#include "wseswrappermain.h"
#include "types.h"
#include "sestypes.h"
#include "werror.h"

#define _WIN32_WINNT 0x0502
#include <windows.h>

WSESWrapperMain *gMain = 0;
int gAttachedThreads = 0;

BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID)
{
  BOOL result = TRUE;
  switch( dwReason ) 
  { 
      case DLL_PROCESS_ATTACH:
        if (gMain == 0)
        {
          char buffer[512];
          int size = 512;
          ::GetModuleFileName(hInstance, buffer, size);
          std::string path = buffer;
          int pos = path.find_last_of('/');
          if (pos == path.npos)
            pos = path.find_last_of('\\');
          path = path.substr(0, pos + 1) + "..";
          gMain = new WSESWrapperMain(path.c_str());
          DWORD errorCode = GetLastError();
        }
        break;

      case DLL_THREAD_ATTACH:
        gAttachedThreads++;
          break;

      case DLL_THREAD_DETACH:
        gAttachedThreads--;
          break;

      case DLL_PROCESS_DETACH:
        if (gMain != 0 && !gMain->isInitialized())
        {
          delete gMain;
          gMain = 0;
        }
        break;
  }
  return result;
}

#ifdef __cplusplus
extern "C"
{
#endif

// Standard Library functions

/*!
 * @defgroup exported_functions Exported Functions
 * @brief This is a list of C functions exported from SESWrapper. All functions wrap calls
 * to a global instance of the WSesWrapperMain class.
 * @{
 */


/*!
 * Exported function that wraps the WSESWrapperMain::initialize() member function.
 */
int STDCALL WRP_Initialize(void *reserved)
{
  return gMain->initialize(reserved);
}

/*!
 * Exported function that wraps the WSESWrapperMain::finalize() member function.
 */
int STDCALL WRP_Finalize()
{
  return gMain->finalize();
}

// Property functions

/*!
 * Exported function that wraps the WSESWrapperMain::getProperty() member funciton. Does not verify the
 * type of @p value.
 */
int STDCALL WRP_GetProperty(const char *property, int index, void *value, int *size)
{
  return gMain->getProperty(property, index, value, *size);
}

/*!
 * Exported function that wraps the WSESWrapperMain::getProperty() member function for properties
 * of type TYPE_BOOL.
 */
int STDCALL WRP_GetPropertyBool(const char *property, int index, bool *value, int *size)
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
int STDCALL WRP_GetPropertyInteger(const char *property, int index, int *value, int *size)
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
int STDCALL WRP_GetPropertyDouble(const char *property, int index, double *value, int *size)
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
int STDCALL WRP_GetPropertyString(const char *property, int index, char *value, int *size)
{
  int error = WError::ERR_NOT_APPLICABLE;
  if (gMain->parameterType(property) == WSESWrapperMain::Property::TYPE_STRING)
    error = gMain->getProperty(property, index, value, *size);
  return error;
}

/*!
 * Exported function that wraps the WSESWrapperMain::getProperty() member function for the @c detector_info property.
 */
int STDCALL WRP_GetDetectorInfo(SESWrapperNS::PDetectorInfo detectorInfo)
{
  int size = sizeof(SESWrapperNS::DetectorInfo);
  return gMain->getProperty("detector_info", 0, detectorInfo, size);
}

/*!
 * Exported function that wraps the WSESWrapperMain::getProperty() member function for the @c detector_region property.
 */
int STDCALL WRP_GetDetectorRegion(SESWrapperNS::PDetectorRegion detectorRegion)
{
  int size = sizeof(SESWrapperNS::DetectorRegion);
  return gMain->getProperty("detector_region", 0, detectorRegion, size);
}

/*!
 * Exported function that wraps the WSESWrapperMain::setProperty() member function. Does not verify the
 * type of @p value.
 */
int STDCALL WRP_SetProperty(const char *property, int reserved, const void *value)
{
  return gMain->setProperty(property, reserved, value);
}

/*!
 * Exported function that wraps the WSESWrapperMain::setProperty() member function for properties of type
 * TYPE_BOOL.
 */
int STDCALL WRP_SetPropertyBool(const char *property, int reserved, const bool *value)
{
  int error = WError::ERR_NOT_APPLICABLE;
  if (gMain->parameterType(property) == WSESWrapperMain::Property::TYPE_BOOL)
    error = gMain->setProperty(property, reserved, value);
  return error;
}

/*!
 * Exported function that wraps the WSESWrapperMain::setProperty() member function for properties of type
 * TYPE_INT32.
 */
int STDCALL WRP_SetPropertyInteger(const char *property, int reserved, const int *value)
{
  int error = WError::ERR_NOT_APPLICABLE;
  if (gMain->parameterType(property) == WSESWrapperMain::Property::TYPE_INT32)
    error = gMain->setProperty(property, reserved, value);
  return error;
}

/*!
 * Exported function that wraps the WSESWrapperMain::setProperty() member function for properties of type
 * TYPE_DOUBLE.
 */
int STDCALL WRP_SetPropertyDouble(const char *property, int reserved, const double *value)
{
  int error = WError::ERR_NOT_APPLICABLE;
  if (gMain->parameterType(property) == WSESWrapperMain::Property::TYPE_DOUBLE)
    error = gMain->setProperty(property, reserved, value);
  return error;
}

/*!
 * Exported function that wraps the WSESWrapperMain::setProperty() member function for properties of type
 * TYPE_STRING.
 */
int STDCALL WRP_SetPropertyString(const char *property, int reserved, const char *value)
{
  int error = WError::ERR_NOT_APPLICABLE;
  if (gMain->parameterType(property) == WSESWrapperMain::Property::TYPE_STRING)
    error = gMain->setProperty(property, reserved, value);
  return error;
}

/*!
 * Exported function that wraps the WSESWrapperMain::setProperty() member function for the @c detector_region
 * property.
 */
int STDCALL WRP_SetDetectorRegion(SESWrapperNS::PDetectorRegion detectorRegion)
{
  return gMain->setProperty("detector_region", 0, detectorRegion);
}

/*!
 * Exported function that wraps the WSESWrapperMain::setProperty() member function for the @c analyzer_region
 * property.
 */
int STDCALL WRP_SetAnalyzerRegion(SESWrapperNS::PAnalyzerRegion analyzerRegion)
{
  return gMain->setProperty("analyzer_region", 0, analyzerRegion);
}

/*!
 * Exported function that wraps the WSESWrapperMain::validate() member function.
 */
int STDCALL WRP_Validate(const char *elementSet, const char *lensMode, double passEnergy, double kineticEnergy)
{
  return gMain->validate(elementSet, lensMode, passEnergy, kineticEnergy);
}

// HW specific functions

/*!
 * Exported function that wraps the WSESWrapperMain::resetHW() member function.
 */
int STDCALL WRP_ResetHW()
{
  return gMain->resetHW();
}

/*!
 * Exported function that wraps the WSESWrapperMain::testHW() member function.
 */
int STDCALL WRP_TestHW()
{
  return gMain->testHW();
}

// Analyzer specific members

/*!
 * Exported function that wraps the WSESWrapperMain::loadInstrument() member function.
 */
int STDCALL WRP_LoadInstrument(const char *fileName)
{
  return gMain->loadInstrument(fileName);
}

/*!
 * Exported function that wraps the WSESWrapperMain::zeroSupplies() member function.
 */
int STDCALL WRP_ZeroSupplies()
{
  return gMain->zeroSupplies();
}

/*!
 * Exported function that wraps the WSESWrapperMain::getKineticEnergy() member function.
 */
int STDCALL WRP_GetKineticEnergy(double *kineticEnergy)
{
  return gMain->getKineticEnergy(kineticEnergy);
}

/*!
 * Exported function that wraps the WSESWrapperMain::setKineticEnergy() member function.
 */
int STDCALL WRP_SetKineticEnergy(const double kineticEnergy)
{
  return gMain->setKineticEnergy(kineticEnergy);
}

/*!
 * Exported function that wraps the WSESWrapperMain::getElementVoltage() member function.
 */
int STDCALL WRP_GetElementVoltage(const char *elementName, double *voltage)
{
  return gMain->getElementVoltage(elementName, voltage);
}

/*!
 * Exported function that wraps the WSESWrapperMain::setElementVoltage() member function.
 */
int STDCALL WRP_SetElementVoltage(const char *elementName, const double voltage)
{
  return gMain->setElementVoltage(elementName, voltage);
}

// Acquisition specific members

/*!
 * Exported function that wraps the WSESWrapperMain::checkAnalyzerRegion() member function.
 */
int STDCALL WRP_CheckAnalyzerRegion(SESWrapperNS::PAnalyzerRegion analyzerRegion, int *steps, double *time_ms, double *energyStep)
{
  return gMain->checkAnalyzerRegion(analyzerRegion, steps, time_ms, energyStep);
}

/*!
 * Exported function that wraps the WSESWrapperMain::initAcquisition() member function.
 */
int STDCALL WRP_InitAcquisition(const bool blockPointReady, const bool blockRegionReady)
{
  return gMain->initAcquisition(blockPointReady, blockRegionReady);
}

/*!
 * Exported function that wraps the WSESWrapperMain::startAcquisition() member function.
 */
int STDCALL WRP_StartAcquisition()
{
  return gMain->startAcquisition();
}

/*!
 * Exported function that wraps the WSESWrapperMain::stopAcquisition() member function.
 */
int STDCALL WRP_StopAcquisition()
{
  return gMain->stopAcquisition();
}

/*!
 * Exported function that wraps the WSESWrapperMain::getAcquiredData() member function when @p variable is of
 * type @c TYPE_INT32.
 */
int STDCALL WRP_GetAcquiredDataInteger(const char *variable, int index, int *data, int *size)
{
  int error = WError::ERR_NOT_APPLICABLE;
  if (gMain->parameterType(variable) == WSESWrapperMain::Property::TYPE_INT32)
    error = gMain->getAcquiredData(variable, index, data, *size);
  return error;
}

/*!
 * Exported function that wraps the WSESWrapperMain::getAcquiredData() member function when @p variable is of
 * type @c TYPE_DOUBLE.
 */
int STDCALL WRP_GetAcquiredDataDouble(const char *parameter, int index, double *data, int *size)
{
  int error = WError::ERR_NOT_APPLICABLE;
  if (gMain->parameterType(parameter) == WSESWrapperMain::Property::TYPE_DOUBLE)
    error = gMain->getAcquiredData(parameter, index, data, *size);
  return error;
}

/*!
 * Exported function that wraps the WSESWrapperMain::getAcquiredData() member function when @p variable is of
 * type @c TYPE_STRING.
 */
int STDCALL WRP_GetAcquiredDataString(const char *parameter, int index, char *data, int *size)
{
  int error = WError::ERR_NOT_APPLICABLE;
  if (gMain->parameterType(parameter) == WSESWrapperMain::Property::TYPE_STRING)
    error = gMain->getAcquiredData(parameter, index, data, *size);
  return error;
}

/*!
 * Exported function that wraps the WSESWrapperMain::getAcquiredData() member function when @p variable is of
 * type @c TYPE_VECTOR_DOUBLE.
 */
int STDCALL WRP_GetAcquiredDataVectorDouble(const char *parameter, int index, double *data, int *size)
{
  int error = WError::ERR_NOT_APPLICABLE;
  if (gMain->parameterType(parameter) == WSESWrapperMain::Property::TYPE_VECTOR_DOUBLE)
    error = gMain->getAcquiredData(parameter, index, data, *size);
  return error;
}

/*!
 * Exported function that wraps the WSESWrapperMain::getAcquiredData() member function when @p variable is of
 * type @c TYPE_VECTOR_INT32.
 */
int STDCALL WRP_GetAcquiredDataVectorInt32(const char *parameter, int index, int *data, int *size)
{
  int error = WError::ERR_NOT_APPLICABLE;
  if (gMain->parameterType(parameter) == WSESWrapperMain::Property::TYPE_VECTOR_INT32)
    error = gMain->getAcquiredData(parameter, index, data, *size);
  return error;
}

/*!
 * Exported function that wraps the WSESWrapperMain::waitForPointReady() member function.
 */
int STDCALL WRP_WaitForPointReady(int timeout_ms)
{
  return gMain->waitForPointReady(timeout_ms);
}

/*!
 * Exported function that wraps the WSESWrapperMain::waitForRegionReady() member function.
 */
int STDCALL WRP_WaitForRegionReady(int timeout_ms)
{
  return gMain->waitForRegionReady(timeout_ms);
}

/*!
 * Exported function that wraps the WSESWrapperMain::continueAcquisition() member function.
 */
int STDCALL WRP_ContinueAcquisition()
{
  return gMain->continueAcquisition();
}

/*! @} */

#ifdef __cplusplus
}
#endif
