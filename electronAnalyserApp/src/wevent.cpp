#include "wevent.h"

/*! @class WEvent wevent.h
* @brief This class is used as a container for one event.
*
* The following example shows one way of using WEvent:
*
* @code
using SciCoreNS::WThread;
using SciCoreNS::WEvent;

class MyThread : public WThread
{
public:
  void stop()
  {
    abort_.set();
  }

  void trigger()
  {
    ready_.set();
  }

protected:
  void run()
  {
    abort_.reset();
    ready_.reset();
    bool running_ = true;
    std::vector<WEvent *> events;
    events.push_back(&abort_);

    while(running_)
    {
      int result = ready_.wait(-1, events);
      switch (result)
      {
      case 0:
        std::cout << "Data arrived!" << std::endl;
        ready_.reset();
        break;
      case 1:
        std::cout << "Exiting..." << std::endl;
        running_ = false;
        break;
      }
    }
  }

private:
  WEvent ready_;
  WEvent abort_;
};

int main(int argc, char **argv)
{
  using std::cout;
  using std::cin;
  using std::endl;

  MyThread thread;
  thread.start();
  while (1)
  {
    cout << "Command (1: Data arrived, 2: Exit): ";
    char c = cin.get();
    cin.get();
    if (c == '1')
    {
      thread.trigger();
    }
    else if (c == '2')
    {
      thread.stop();
      thread.wait();
      cout << "Done" << endl;
      break;
    }
  }
  return 0;
}
@endcode
*/
