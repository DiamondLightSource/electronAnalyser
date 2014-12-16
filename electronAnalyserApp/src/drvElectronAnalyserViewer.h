#ifndef DRV_ELECTRONANALYSERVIEWER_H
#define DRV_ELECTRONANALYSERVIEWER_H

#ifdef __cplusplus
extern "C"
{
#endif
int electronAnalyserViewerConfig(const char *portName, int maxBuffers, size_t maxMemory, int priority, int stackSize);
#ifdef __cplusplus
}
#endif
#endif
