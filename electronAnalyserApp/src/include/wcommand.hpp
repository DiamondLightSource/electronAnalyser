#ifndef __TDC_CHAT_WCOMMAND_HPP__
#define __TDC_CHAT_WCOMMAND_HPP__

#include <string>

// The following defines are necessary due to compiler limitations concerning partial template specialization
#define WCommand4(Class, ReturnType, Arg1, Arg2, Arg3, Arg4) SciCoreNS::WCommand<##Class, ##ReturnType, ##Arg1, ##Arg2, #Arg3, #Arg4>
#define WCommand3(Class, ReturnType, Arg1, Arg2, Arg3) SciCoreNS::WCommand<##Class, ##ReturnType, ##Arg1, ##Arg2, ##Arg3, SciCoreNS::NoArg>
#define WCommand2(Class, ReturnType, Arg1, Arg2) SciCoreNS::WCommand<##Class, ##ReturnType, ##Arg1, ##Arg2, SciCoreNS::NoArg, SciCoreNS::NoArg>
#define WCommand1(Class, ReturnType, Arg1) SciCoreNS::WCommand<##Class, ##ReturnType, ##Arg1, SciCoreNS::NoArg, SciCoreNS::NoArg, SciCoreNS::NoArg>
#define WCommand0(Class, ReturnType) SciCoreNS::WCommand<##Class, ##ReturnType, SciCoreNS::NoArg, SciCoreNS::NoArg, SciCoreNS::NoArg, SciCoreNS::NoArg>

namespace SciCoreNS
{
  class NoArg {};

  // WCommand with 4 arguments
  template<class _Container, typename _Return, typename _Arg1, typename _Arg2, typename _Arg3, typename _Arg4>
  class WCommand
  {
  public:
    typedef _Return (_Container::*Callback)(_Arg1 arg1, _Arg2 arg2, _Arg3 arg3, _Arg4 arg4);

    WCommand(_Container *object, Callback callback, const std::string &syntax = "", const std::string &description = "")
    : object_(object), callback_(callback), syntax_(syntax), description_(d){}

    WCommand(const WCommand &src)
    : object_(src.object_), callback_(src.callback_), syntax_(src.syntax_), description_(src.description_) {}

    ~WCommand() {}

    _Return operator()(_Arg1 arg1, _Arg2 arg2, _Arg3 arg3, _Arg4 arg4)
    {
      return (object_->*callback_)(arg1, arg2, arg3, arg4);
    }

    void setCallback(Callback cb) {callback_ = cb;}
    void setSyntax(const std::string &s) {syntax_ = s;}
    std::string syntax() const {return syntax_;}
    void setDescription(const std::string &description) {description_ = description;}
    std::string description() const {return description_;}

  private:
    _Container *object_;
    Callback callback_;
    std::string syntax_;
    std::string description_;
  };

  // WCommand with 3 arguments
  template<class _Container, typename _Return, typename _Arg1, typename _Arg2, typename _Arg3>
  class WCommand<_Container, _Return, _Arg1, _Arg2, _Arg3, NoArg>
  {
  public:
    typedef _Return (_Container::*Callback)(_Arg1 arg1, _Arg2 arg2, _Arg3 arg3);

    WCommand(_Container *object, Callback callback, const std::string &syntax = "", const std::string &description = "")
    : object_(object), callback_(callback), syntax_(s), description_(d){}

    WCommand(const WCommand &src)
    : object_(src.object_), callback_(src.callback_), syntax_(src.syntax_), description_(src.description_) {}

    ~WCommand() {}

    _Return operator()(_Arg1 arg1, _Arg2 arg2, _Arg3 arg3)
    {
      return (object_->*callback_)(arg1, arg2, arg3);
    }

    void setCallback(Callback cb) {callback_ = cb;}
    void setSyntax(const std::string &s) {syntax_ = s;}
    std::string syntax() const {return syntax_;}
    void setDescription(const std::string &description) {description_ = description;}
    std::string description() const {return description_;}

  private:
    _Container *object_;
    Callback callback_;
    std::string syntax_;
    std::string description_;
  };

    // WCommand with 2 arguments
  template<class _Container, typename _Return, typename _Arg1, typename _Arg2>
  class WCommand<_Container, _Return, _Arg1, _Arg2, NoArg, NoArg>
  {
  public:
    typedef _Return (_Container::*Callback)(_Arg1 arg1, _Arg2 arg2);

    WCommand(_Container *object, Callback callback, const std::string &syntax = "", const std::string &description = "")
    : object_(object), callback_(callback), syntax_(syntax), description_(description){}

    WCommand(const WCommand &src)
    : object_(src.object_), callback_(src.callback_), syntax_(src.syntax_), description_(src.description_) {}

    ~WCommand() {}

    _Return operator()(_Arg1 arg1, _Arg2 arg2)
    {
      return (object_->*callback_)(arg1, arg2);
    }

    void setCallback(Callback cb) {callback_ = cb;}
    void setSyntax(const std::string &s) {syntax_ = s;}
    std::string syntax() const {return syntax_;}
    void setDescription(const std::string &description) {description_ = description;}
    std::string description() const {return description_;}

  private:
    _Container *object_;
    Callback callback_;
    std::string syntax_;
    std::string description_;
  };

  // WCommand with 1 argument
  template<class _Container, typename _Return, typename _Arg1>
  class WCommand<_Container, _Return, _Arg1, NoArg, NoArg, NoArg>
  {
  public:
    typedef _Return (_Container::*Callback)(_Arg1 arg1);

    WCommand(_Container *object, Callback callback, const std::string &syntax = "", const std::string &description = "")
    : object_(object), callback_(callback), syntax_(syntax), description_(description){}

    WCommand(const WCommand &src)
    : object_(src.object_), callback_(src.callback_), syntax_(src.syntax_), description_(src.description_) {}

    ~WCommand() {}

    _Return operator()(_Arg1 arg1)
    {
      return (object_->*callback_)(arg1);
    }

    void setCallback(Callback cb) {callback_ = cb;}
    void setSyntax(const std::string &s) {syntax_ = s;}
    std::string syntax() const {return syntax_;}
    void setDescription(const std::string &description) {description_ = description;}
    std::string description() const {return description_;}

  private:
    _Container *object_;
    Callback callback_;
    std::string syntax_;
    std::string description_;
  };

  // WCommand with no arguments
  template<class _Container, typename _Return>
  class WCommand<_Container, _Return, NoArg, NoArg, NoArg, NoArg>
  {
  public:
    typedef _Return (_Container::*Callback)();

    WCommand(_Container *object, Callback callback, const std::string &syntax = "", const std::string &description = "")
    : object_(object), callback_(callback), syntax_(syntax), description_(description){}

    WCommand(const WCommand &src)
    : object_(src.object_), callback_(src.callback_), syntax_(src.syntax_), description_(src.description_) {}

    ~WCommand() {}

    _Return operator()()
    {
      return (object_->*callback_)();
    }

    void setCallback(Callback cb) {callback_ = cb;}
    void setSyntax(const std::string &s) {syntax_ = s;}
    std::string syntax() const {return syntax_;}
    void setDescription(const std::string &description) {description_ = description;}
    std::string description() const {return description_;}

  private:
    _Container *object_;
    Callback callback_;
    std::string syntax_;
    std::string description_;
  };
}

#endif
