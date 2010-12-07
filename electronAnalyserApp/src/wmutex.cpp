#include "wmutex.h"

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x501
#include <windows.h>
#endif

namespace SciCoreNS
{
  /*! @class WMutex wmutex.h
  *
  * This class protects critical sections within one process.
  */

  /*! @class WMutexLocker wmutexlocker.h
   * 
   * This is a convenience class for a WMutex object. The following example shows how to use the class:
   *
   * @code
   * WMutex mutex;
   * 
   * void function()
   * {
   *   WMutexLocker locker(&mutex);
   *   // Do something. The mutex will be unlocked when leaving this function.
   * }
   * @endcode
   */

  /*!
  * Constructs a WMutex object by creating a Windows CriticalSection object.
  */
  WMutex::WMutex()
  : mtx_(new CRITICAL_SECTION)
  {
    InitializeCriticalSection(reinterpret_cast<LPCRITICAL_SECTION>(mtx_));
  }

  /*!
  * Destroys the WMutex object and deletes the contained CriticalSection object.
  */
  WMutex::~WMutex()
  {
    DeleteCriticalSection(reinterpret_cast<LPCRITICAL_SECTION>(mtx_));
    delete mtx_;
  }

  /*!
  * Locks the mutex by calling EnterCriticalSection().
  */
  void WMutex::lock()
  {
    EnterCriticalSection(reinterpret_cast<LPCRITICAL_SECTION>(mtx_));
  }

  /*!
  * Unlocks the mutex by calling LeaveCriticalSection().
  */
  void WMutex::unlock()
  {
    LeaveCriticalSection(reinterpret_cast<LPCRITICAL_SECTION>(mtx_));
  }
}