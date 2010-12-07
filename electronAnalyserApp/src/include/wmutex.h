#ifndef __SCCOMMONWIN32_WMUTEX_H__
#define __SCCOMMONWIN32_WMUTEX_H__

namespace SciCoreNS
{
  class WMutex
  {
  public:
    WMutex();
    ~WMutex();

    void lock();
    void unlock();

  private:
    void *mtx_;
  };

  class WMutexLocker
  {
  public:
    WMutexLocker(WMutex *mutex);
    ~WMutexLocker();

  private:
    WMutex *mtx_;
  };

  /*!
  * Constructs a WMutexLocker object and locks @c mutex.
  *
  * @param[in] mutex Pointer to the mutex to lock.
  */
  inline WMutexLocker::WMutexLocker(WMutex *mutex)
  : mtx_(mutex)
  { 
    mtx_->lock();
  }

  /*!
  * Destroys the WMutexLocker object and unlocks the contained mutex.
  */
  inline WMutexLocker::~WMutexLocker()
  {
    mtx_->unlock();
  }
}

#endif
