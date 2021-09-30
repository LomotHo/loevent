#include <functional>

// typedef std::function<void(void)> ReadEventCallback;
typedef std::function<void(void)> EventCallback;
class Channel {
public:
  Channel(/* args */);
  ~Channel();
  void setReadCallback(EventCallback cb) { readCallback = std::move(cb); }
  void setWriteCallback(EventCallback cb) { writeCallback = std::move(cb); }

  EventCallback readCallback;
  EventCallback writeCallback;

private:
};

Channel::Channel() {}

Channel::~Channel() {}
