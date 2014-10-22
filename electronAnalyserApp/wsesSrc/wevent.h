#ifndef __SESWRAPPER_WEVENT_H__
#define __SESWRAPPER_WEVENT_H__

#define NOMINMAX
#define _WIN32_WINNT 0x0502
#include <windows.h>

class WEvent
{
public:
  enum ErrorCodes
  {
    ERR_OK = 0,
    ERR_TIMEOUT = -1
  };

  WEvent();
  ~WEvent();

  void set();
  void reset();
  int wait(int timeout = -1) const;
  HANDLE handle() const;

private:
  HANDLE event_;
};

/*!
  *  Constructs a WEvent object that contains an event handle.
  */
inline WEvent::WEvent()
{
  event_ = CreateEvent(0, TRUE, FALSE, 0);
}

/*!
  * Destroys an event object and closes the contained event handle.
  */
inline WEvent::~WEvent()
{
  CloseHandle(event_);
}

/*!
  * Signals the contained event object.
  */
inline void WEvent::set()
{
  SetEvent(event_);
}

/*!
  * Places the contained event object in an unsignalled state.
  */
inline void WEvent::reset()
{
  ResetEvent(event_);
}

/*!
* Waits for the contained event object to be in a signalled state.
*
* @param[in] timeout The number of milliseconds to wait before time-out. The default is
*                    to never time out.
*
* @return ERR_OK if the current event is signalled. otherwise ERR_TIMEOUT.
*/
inline int WEvent::wait(int timeout) const
{
  return WaitForSingleObject(event_, timeout) == WAIT_OBJECT_0 ? ERR_OK : ERR_TIMEOUT;
}

/*!
  * Convenience member function for obtaining the handle to the contained event object.
  *
  * @return Returns the event handle for use in calls to e.g. WaitForMultipleObjects().
  */
inline HANDLE WEvent::handle() const
{
  return event_;
}

#endif
