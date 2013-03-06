#ifndef __SESWRAPPER_WVARIABLE_HPP__
#define __SESWRAPPER_WVARIABLE_HPP__

template<class _Container>
class WVariable
{
public:
  typedef _Container Container;
  typedef int (_Container::*SetCallback)(int index, const void *value);
  typedef int (_Container::*GetCallback)(int index, void *value, int &size);

  /*!
   * Enumerates the types for the properties and data parameters.
   */
  enum ValueType
  {
    TYPE_BOOL, /*!< Parameters of type \p bool (1-byte unsigned char) */
    TYPE_INT32, /*!< Parameters of type \p int (32-bit signed integer) */
    TYPE_DOUBLE, /*!< Parameters of type \p double (64-bit floating point) */
    TYPE_STRING, /*!< Null-terminated strings of type <tt>char *</tt> */
    TYPE_VECTOR_INT32, /*!< Arrays of type \p int (32-bit signed integer) */
    TYPE_VECTOR_DOUBLE, /*!< Arrays of type \p double (64-bit floating point) */
    TYPE_DETECTORINFO, /*!< Pointers to struct DetectorInfo */
    TYPE_DETECTORREGION, /*!< Pointers to struct DetectorRegion */
    TYPE_ANALYZERREGION /*!< Pointers to struct AnalyzerRegion */
  };

  WVariable(_Container *container, SetCallback setCallback, GetCallback getCallback, ValueType valueType);
  WVariable(const WVariable &src);
  ~WVariable();

  int valueType() const;
  int set(int index, const void *value);
  int get(int index, void *value, int &size);

private:
  _Container *container_;
  SetCallback setCallback_;
  GetCallback getCallback_;
  ValueType valueType_;
};

/*!
 * Constructor for the WVariable class. Creates a getter and a setter from the callbacks provided by \p container.
 *
 * \param[in] container Pointer to an instance of the containing class.
 * \param[in] setCallback Member function pointer representing the setter of the container's variable.
 * \param[in] getCallback Member function pointer representing the getter of the container's variable.
 * \param[in] valueType Specifies the type of variable (one of the ValueType enums).
 */
template<class _Container>
WVariable<_Container>::WVariable(_Container *container, SetCallback setCallback, GetCallback getCallback, ValueType valueType)
: container_(container), setCallback_(setCallback), getCallback_(getCallback), valueType_(valueType)
{
}

/*!
 * Copy constructor for the WVariable class.
 *
 * \param[in] src The source object to copy.
 */
template<class _Container>
WVariable<_Container>::WVariable(const WVariable &src)
: container_(src.container_), setCallback_(src.setCallback_), getCallback_(src.getCallback_), valueType_(src.valueType_)
{
}

/*!
 * Destroys the WVariable object.
 */
template<class _Container>
WVariable<_Container>::~WVariable()
{
}

/*!
 * Queries the value type of this variable.
 *
 * \return The type of variable, one of the ValueType enums.
 */
template<class _Container>
int WVariable<_Container>::valueType() const
{
  return valueType_;
}

/*!
 * Calls the setter for the variable.
 *
 * \param[in] index An index used for variables of vector or array types.
 * \param[in] value A pointer to the value to be set.
 *
 * \returns Usually WError::ERR_OK on success, otherwise an error code.
 */
template<class _Container>
int WVariable<_Container>::set(int index, const void *value)
{
  return (container_->*setCallback_)(index, value);
}

/*!
 * Calls the getter for the variable.
 *
 * \param[in] index An index used for variables of vector or array types.
 * \param[out] value A pointer to the value to obtain. For vectors or arrays, this can usually be 0 (NULL),
 *                   in which case \p size is modified to request the size of the vector/array.
 * \param[in,out] size The size of the value, if that value is a vector or array.
 *
 * \returns Usually WError::ERR_OK on success, otherwise an error code.
 */
template<class _Container>
int WVariable<_Container>::get(int index, void *value, int &size)
{
  return (container_->*getCallback_)(index, value, size);
}

#endif
