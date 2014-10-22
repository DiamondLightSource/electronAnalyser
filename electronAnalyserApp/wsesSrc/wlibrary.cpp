#include "wlibrary.h"
#include "common.hpp"

#define NOMINMAX
#include <windows.h>
#include <string>

class WLibrary_P
{
public:
  WLibrary_P()
    : lib_(0)
  {
  }

  HMODULE lib_;
  std::string path_;
};

/*!
  * \class WLibrary
  * \brief This is a generic class for importing shared libraries dynamically.
  *
  * It should be subclassed to include the C functions exported from the library it contains. Each function will
  * then be added as a callback function within the subclass.
  */

/*!
  * Constructs a WLibrary object.
  */
WLibrary::WLibrary()
  : p_(new WLibrary_P)
{
}

/*!
  * Destroys the WLibrary object. If the contained library was loaded, it is first unloaded.
  */
WLibrary::~WLibrary()
{
  unload();
  delete p_;
}

/*!
  * Loads a library. This function can be reimplemented by a subclass to also resolve the exported functions
  * available in the library.
  *
  * \param[in] fileName The name (including path if needed) of the library to be loaded.
  *
  * \return \c true if successful, otherwise \c false.
  */
bool WLibrary::load(const char *fileName)
{
  if (p_->lib_ != 0)
    unload();

  if (p_->lib_ == 0)
  {
    p_->lib_ = LoadLibrary(fileName);
    
    if (p_->lib_ != 0)
      p_->path_ = fileName;
  }

  return p_->lib_ != 0;
}

/*!
  * Unloads the library and clears the internal variables. If no library has been loaded, this function returns
  * immediately.
  */
void WLibrary::unload()
{
  if (FreeLibrary(p_->lib_) == TRUE)
  {
    p_->lib_ = 0;
    p_->path_.clear();
  }
}

/*!
  * \return \c true if a library is loaded, otherwise \c false.
  */
bool WLibrary::isLoaded() const
{
  return p_->lib_ != 0;
}

/*!
  * Queries the path and name of the currently loaded library. The \p buffer
  * parameter can be NULL if only the required buffer length is requested.
  *
  * \param[in] buffer The buffer that will contain the path. Can be NULL.
  * \param[in] size The size of \p buffer.
  *
  * \return the length of the path string.
  */
int WLibrary::path(char *buffer, int size) const
{
  if (buffer != 0)
    buffer[p_->path_.copy(buffer, size)] = 0;
  return p_->path_.length();
}

/*!
  * Imports a function from the contained library.
  *
  * \param[in] functionName A string specifying the name of the function to be imported.
  *
  * \return The address of the imported function. This should be type cast to the appropriate
  *         function pointer type from the current void* type. If the import fails,
  *         the function returns 0.
  *
  * \see SciCoreNS::import()
  */
void *WLibrary::import(const char *functionName)
{
  return GetProcAddress(p_->lib_, functionName);
}
