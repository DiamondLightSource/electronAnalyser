#ifndef __SESWRAPPER_WLIBRARY_H__
#define __SESWRAPPER_WLIBRARY_H__

#include <string>
#include <exception>

class WLibrary;

namespace SESWrapperNS
{
  enum ImportOption
  {
    FUNCTION_REQUIRED,
    FUNCTION_OPTIONAL
  };
}

/*!
 * This class is used by ::import() when failing to import a function from the instrument library.
 */
class WFunctionException : public std::exception
{
public:
  WFunctionException(const std::string &functionName) : functionName_(functionName) {}

  std::string functionName_;
};

/*! 
  * This is a template function to simplify the importing of C functions used by
  * the WLibrary::import() member function. It uses a reinterpret_cast to cast the
  * void* usually obtained when importing C functions to the type defined by the
  * \p function parameter.
  *
  * \param[in] library Address of the WLibrary library that will be queried.
  * \param[in] name The name of the function to be imported from \p library.
  * \param[in] option Setting this to SESWrapperNS::OPTIONAL indicates that the imported function is optional.
  *                   If the import fails and \c option is SESWrapperNS::REQUIRED, an exception of type WFunctionException is thrown.
  * \param[out] function A variable that can contain the address to the imported function.
  *                      The type of the function (\p T) must be of a correct funciton pointer format.
  *
  * \return Returns \c true if import was successful, otherwise \c false (provided the function was optional).
  *
  * \see WLibrary::import()
  */
template<typename T> bool import(WLibrary &library, const char *name, SESWrapperNS::ImportOption option, T &function)
{
  function = reinterpret_cast<T>(library.import(name));
  if (option == SESWrapperNS::FUNCTION_REQUIRED && function == 0)
    throw WFunctionException(name);
  return (function != 0);
}

class WLibrary_P;

class WLibrary
{
public:
  WLibrary();
  virtual ~WLibrary();

  virtual bool load(const char *fileName);
  virtual void unload();    
  bool isLoaded() const;
  int path(char *buffer, int size) const;
  void *import(const char *functionName);
protected:
  WLibrary_P *p_;
};

#endif
