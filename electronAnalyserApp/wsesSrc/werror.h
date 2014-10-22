#ifndef __SESWRAPPER_WERROR_H__
#define __SESWRAPPER_WERROR_H__

#include <string>
#include <map>

/*!
 * \brief Maps a message index to a message string from the return values from the exported functions in
 * SESWrapper.
 */
class WError
{
public:
  typedef std::map<int, std::string> ErrorMessageMap;

  /*! 
   * This enumerator contains the return codes from the exported functions.
   * Errors are represented by negative numbers, whereas positive values indicate warnings. Success
   * is always indicated by 0 (NULL).
   *
   * \see gErrors
   */
  enum ErrorCodes
  {
    ERR_OK = 0, /*!< Normal return code indicating success */
    ERR_UNKNOWN = -1, /*!< Unknown error */
    ERR_FAIL = -2, /*!< Operation failed */
    ERR_NOT_INITIALIZED = -3, /*!< Library has not been properly initialized */
    ERR_NOT_APPLICABLE = -4, /*!< Property or data parameter access not applicable (e.g. attempted to write to a read-only property) */
    ERR_PARAMETER_NOT_FOUND = -5, /*!< Parameter string was not found */
    ERR_INDEX = -6, /*!< Index out-of-range */
    ERR_INCORRECT_ELEMENT_SET = -7, /*!< The selected element set could not be set */
    ERR_INCORRECT_LENS_MODE = -8, /*!< The selected lens mode is not available */
    ERR_INCORRECT_PASS_ENERGY = -9, /*!< The selected pass energy is not available */
    ERR_INCORRECT_ANALYZER_REGION = -10, /*!< Error in analyzer region settings */
    ERR_INCORRECT_DETECTOR_REGION = -11, /*!< Error in detector region settings */
    ERR_READONLY = -12, /*!< Attempted to access a read-only variable or data parameter */
    ERR_NO_INSTRUMENT = -13, /*!< No instrument is loaded (see \ref WRP_LoadInstrument) */
    ERR_ACQUIRING = -14, /*!< Acquisition in progress */
    ERR_INITIALIZE_FAIL = -15, /*!< Error in call to function GDS_Initialize() */
    ERR_LOAD_LIBRARY = -16, /*!< Could not load the instrument library */
    ERR_OPEN_INSTRUMENT = -17, /*!< Could not open the instrument configuration file */
    ERR_TIMEOUT = 8, /*!< Operation timed out */
    ERR_NOT_IMPLEMENTED = 9 /*!< Function has not been implemented */
  };

  static WError *instance();
  void release();
  std::string message(int errorCode);
  
private:
  WError();
  ~WError();

  ErrorMessageMap messages_;
  static WError *this_;
  static int refCount_;
};

#endif
