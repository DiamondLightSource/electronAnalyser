/*! @file The SesWrapper interface 
 *
 * This is a C/C++ header file for importing the functions available in the SesWrapper
 * DLL. It is mainly created to allow LabVIEW integration. The wrappertypes.h
 * header file is required for a full parsing of this file.
 */

#include "seswrapper_types.h"

int STDCALL WRP_Initialize(void *reserved);
int STDCALL WRP_Finalize();
int STDCALL WRP_GetProperty(const String property, int index, void *value, int *size);
int STDCALL WRP_GetPropertyBool(const String property, int index, _bool *value, int *size);
int STDCALL WRP_GetPropertyInteger(const String property, int index, int *value, int *size);
int STDCALL WRP_GetPropertyDouble(const String property, int index, double *value, int *size);
int STDCALL WRP_GetPropertyString(const String property, int index, String value, int *size);
int STDCALL WRP_GetDetectorInfo(DetectorInfo *value);
int STDCALL WRP_GetDetectorRegion(DetectorRegion *value);
int STDCALL WRP_SetProperty(const String property, int index, const void *value);
int STDCALL WRP_SetPropertyBool(const String property, int index, const _bool *value);
int STDCALL WRP_SetPropertyInteger(const String property, int index, const int *value);
int STDCALL WRP_SetPropertyDouble(const String property, int index, const double *value);
int STDCALL WRP_SetPropertyString(const String property, int index, const String value);
int STDCALL WRP_SetDetectorRegion(DetectorRegion *detectorRegion);
int STDCALL WRP_SetAnalyzerRegion(AnalyzerRegion *analyzerRegion);

int STDCALL WRP_Validate(const String elementSet, const String lensMode, double passEnergy, double kineticEnergy);

int STDCALL WRP_ResetHW();
int STDCALL WRP_TestHW();

int STDCALL WRP_LoadInstrument(const String fileName);
int STDCALL WRP_ZeroSupplies();
int STDCALL WRP_GetKineticEnergy(double *kinetic_energy);
int STDCALL WRP_SetKineticEnergy(const double kineticEnergy);
int STDCALL WRP_GetElementVoltage(const String elementName, double *voltage);
int STDCALL WRP_SetElementVoltage(const String element, const double voltage);

int STDCALL WRP_CheckAnalyzerRegion(AnalyzerRegion *analyzerRegion, int *steps, double *time_ms, double *energyStep);
int STDCALL WRP_InitAcquisition(const _bool block_PointReady, const _bool block_RegionReady);
int STDCALL WRP_StartAcquisition();
int STDCALL WRP_StopAcquisition();
int STDCALL WRP_GetStatus(int *status);
int STDCALL WRP_GetAcquiredDataInteger(const String parameter, int index, int *data, int *size);
int STDCALL WRP_GetAcquiredDataDouble(const String parameter, int index, double *data, int *size);
int STDCALL WRP_GetAcquiredDataString(const String parameter, int index, String data, int *size);
int STDCALL WRP_GetAcquiredDataVectorDouble(const String parameter, int index, VectorDouble *data, int *size);
int STDCALL WRP_GetAcquiredDataVectorInt32(const String parameter, int index, VectorInt32 *data, int *size);
int STDCALL WRP_WaitForPointReady(int timeout_ms);
int STDCALL WRP_WaitForRegionReady(int timeout_ms);
int STDCALL WRP_ContinueAcquisition();
