/* drvElectronAnalyser.h
 *
 * This is a driver for VG Scienta Electron Analyser EW4000 area detector.
 *
 * Author: Fajin Yuan
 *
 * Created:  06/12/2010
 *
 */
#ifndef DRV_ELECTRONANALYSER_H
#define DRV_ELECTRONANALYSER_H

#ifdef __cplusplus
extern "C"
{
#endif
/*int electronAnalyserConfig(const char *portName, const char *workingDir, const char *instrumentFile,
		int maxBuffers, size_t maxMemory, int priority, int stackSize);*/
int electronAnalyserConfig(const char *portName, int maxBuffers, size_t maxMemory, int priority, int stackSize);
#ifdef __cplusplus
}
#endif
#endif
