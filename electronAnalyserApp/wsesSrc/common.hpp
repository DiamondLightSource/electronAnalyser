#pragma warning(disable:4996)

#ifndef __SESWRAPPER_COMMON_HPP__
#define __SESWRAPPER_COMMON_HPP__

#include <string>
#include <vector>

/*! \namespace CommonNS
 * This is a namespace containing common functions not directly related to the functionality of this library.
 */
namespace CommonNS
{
  /*!
   * This function splits the contents of \p buffer using \p delimiter as a separator string. The resulting
   * strings are stored in the \p result vector.
   *
   * \param[in] buffer The input string that will be parsed.
   * \param[in] delimiter The separator. This can be a string of several characters.
   * \param[out] result A vector of type \p T that will contain the elements from the split operation.
   *
   * \return Returns the number of elements in \p result after the split operation.
   */
  template<typename T> int split(std::string buffer, const std::string &delimiter, std::vector<T> &result)
  {
    result.clear();
    int pos = buffer.find(delimiter);
    while (pos != buffer.npos)
    {
      if (pos > 0)
        result.push_back(buffer.substr(0, pos));
      buffer = buffer.substr(pos + delimiter.length());
      pos = buffer.find(delimiter);
    }
    if (buffer.length() > 0)
      result.push_back(buffer);

    return result.size();
  }
}

#endif
