#ifndef __SESWRAPPER_C_FUNCTION_HPP__
#define __SESWRAPPER_C_FUNCTION_HPP__

class NoArg {};

/*!
* The C_Function struct is a container for a template C-function pointer typedef. This is useful when creating libraries that export C functions.
*
* Example 1:
* \code
*   C_Function<int, int>::Pointer myFunction;
*   myFunction = GetProcAddress(libraryHandle, "MyFunction");
*   int result = myFunction(42);
* \endcode
*
* The example shows a callback (or library function) that takes one int as argument, and returns another int value. There can be up to 5 arguments
* to the function. A void function without arguments can be declared like this:
*
* \code
* C_Function<>::Pointer myFunction;
* \endcode
*/
template<typename Return = NoArg, typename Arg1 = NoArg, typename Arg2 = NoArg, typename Arg3 = NoArg, typename Arg4 = NoArg, typename Arg5 = NoArg, typename Arg6 = NoArg>
struct C_Function
{
  /*!
    * This typedef contains the actual function pointer type.
    */
  typedef Return (__stdcall *Pointer)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6);
};

/*!
  * This is a template specialization of the C_Function struct for defining functions with 5 arguments.
  */
template<typename Return, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5>
struct C_Function<Return, Arg1, Arg2, Arg3, Arg4, Arg5, NoArg>
{
  typedef Return (__stdcall *Pointer)(Arg1, Arg2, Arg3, Arg4, Arg5);
};

/*!
  * This is a template specialization of the C_Function struct for defining functions with 4 arguments.
  */
template<typename Return, typename Arg1, typename Arg2, typename Arg3, typename Arg4>
struct C_Function<Return, Arg1, Arg2, Arg3, Arg4, NoArg, NoArg>
{
  typedef Return (__stdcall *Pointer)(Arg1, Arg2, Arg3, Arg4);
};

/*!
  * This is a template specialization of the C_Function struct for defining functions with 3 arguments.
  */
template<typename Return, typename Arg1, typename Arg2, typename Arg3>
struct C_Function<Return, Arg1, Arg2, Arg3, NoArg, NoArg, NoArg>
{
  typedef Return (__stdcall *Pointer)(Arg1, Arg2, Arg3);
};

/*!
  * This is a template specialization of the C_Function struct for defining functions with 2 arguments.
  */
template<typename Return, typename Arg1, typename Arg2>
struct C_Function<Return, Arg1, Arg2, NoArg, NoArg, NoArg, NoArg>
{
  typedef Return (__stdcall *Pointer)(Arg1, Arg2);
};

/*!
  * This is a template specialization of the C_Function struct for defining functions with 1 argument.
  */
template<typename Return, typename Arg1>
struct C_Function<Return, Arg1, NoArg, NoArg, NoArg, NoArg, NoArg>
{
  typedef Return (__stdcall *Pointer)(Arg1);
};

/*!
  * This is a template specialization of the C_Function struct for defining functions with no arguments.
  */
template<typename Return>
struct C_Function<Return, NoArg, NoArg, NoArg, NoArg, NoArg, NoArg>
{
  typedef Return (__stdcall *Pointer)();
};

/*!
  * This is a template specialization of the C_Function struct for defining void functions with no arguments.
  */
template<>
struct C_Function<NoArg, NoArg, NoArg, NoArg, NoArg, NoArg, NoArg>
{
  typedef void (__stdcall *Pointer)();
};

#endif
