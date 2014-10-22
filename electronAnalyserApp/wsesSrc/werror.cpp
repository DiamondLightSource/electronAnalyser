#include "werror.h"
#include "constants.h"

#include <algorithm>

WError *WError::this_ = 0;
int WError::refCount_ = 0;

/*!
 * This class is a singleton that manages the SESWrapper library error codes.
 */
WError::WError()
{
  this_ = this;

  typedef std::pair<int, std::string> Message;

  const Message messages[] = {
    Message(ERR_OK, "No Error"),
    Message(ERR_UNKNOWN, "Unknown Error"),
    Message(ERR_FAIL, "Operation failed"),
    Message(ERR_NOT_INITIALIZED, "Not initialized"),
    Message(ERR_NOT_APPLICABLE, "Not applicable"),
    Message(ERR_PARAMETER_NOT_FOUND, "Parameter not found"),
    Message(ERR_INDEX, "Index out-of-range"),
    Message(ERR_INCORRECT_ELEMENT_SET, "Incorrect element set"),
    Message(ERR_INCORRECT_LENS_MODE, "Incorrect lens mode"),
    Message(ERR_INCORRECT_PASS_ENERGY, "Incorrect pass energy"),
    Message(ERR_INCORRECT_ANALYZER_REGION, "Error in analyzer region"),
    Message(ERR_INCORRECT_DETECTOR_REGION, "Error in detector region"),
    Message(ERR_READONLY, "Variable is read-only"),
    Message(ERR_NO_INSTRUMENT, "No instrument loaded"),
    Message(ERR_ACQUIRING, "Acquisition in progress"),
    Message(ERR_TIMEOUT, "Operation time-out"),
    Message(ERR_INITIALIZE_FAIL, "Error in call to function GDS_Initialize()"),
    Message(ERR_LOAD_LIBRARY, "Could not load the instrument library"),
    Message(ERR_OPEN_INSTRUMENT, "Could not open the instrument configuration file"),
    Message(ERR_NOT_IMPLEMENTED, "Not implemented")
  };
  for (int i = 0; i < sizeof(messages) / sizeof(Message); i++)
    messages_.insert(messages[i]);
}

/*!
 * Destroys the WError object.
 */
WError::~WError()
{
}

/*!
 * Extracts the error message based on \p errorCode.
 *
 * \param[in] errorCode An error code previously returned from a function. If no error with this code is found,
 *                      the resulting string is "Unknown error".
 *
 * \return A string describing the error.
 */
std::string WError::message(int errorCode)
{
  ErrorMessageMap::iterator it = messages_.find(errorCode);
  if (it != messages_.end())
    return it->second;

  return "Unknown error";
}

/*!
 * Returns the address of the WError instance. If this is the first call to this function, a new instance is
 * created. The reference counter is always incremented, and all calls to this function must end with a corresponding
 * call to the release() function for a proper deallocation of the singleton when closing the library.
 *
 * \see release()
 */
WError *WError::instance()
{
  if (this_ == 0)
  {
    this_ = new WError();
  }

  refCount_++;
  return this_;
}

/*!
 * Releases the reference to the WError singleton. This function must be called in combination with the instance()
 * function for proper decrement of the reference counter and deallocation when closing the SESWrapper library.
 *
 * \see instance()
 */
void WError::release()
{
  refCount_--;
  if (refCount_ == 0)
  {
    delete this_;
    this_ = 0;
  }
  if (refCount_ < 0)
    refCount_ = 0;
}
