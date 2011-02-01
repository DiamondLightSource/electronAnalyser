#ifndef __SESWRAPPER_COMMON_H__
#define __SESWRAPPER_COMMON_H__

#include <string>
#include <vector>
#include <windows.h>

/*!
 * This is a namespace containing common functions not directly related to the functionality of this library.
 */
namespace CommonNS
{
  template<typename T> int split(std::string buffer, const std::string &delimiter, std::vector<T> &result);
  template<typename T> void copyString(const std::string &src, T trg, unsigned int trgSize);
  template<typename T> bool resolve(void *library, const char *name, T &function);
}

#include "common.hpp"
#endif
