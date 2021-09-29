// 1

void handleEvent(Timestamp receiveTime);
void setReadCallback(ReadEventCallback cb) { readCallback_ = std::move(cb); }
void setWriteCallback(EventCallback cb) { writeCallback_ = std::move(cb); }
void setCloseCallback(EventCallback cb) { closeCallback_ = std::move(cb); }
void setErrorCallback(EventCallback cb) { errorCallback_ = std::move(cb); }

// 2
struct io_event  //注册的IO事件
{
  io_event() : read_cb(NULL), write_cb(NULL), rcb_args(NULL), wcb_args(NULL) {}
  int mask;               // EPOLLIN EPOLLOUT
  io_callback* read_cb;   // callback when EPOLLIN comming
  io_callback* write_cb;  // callback when EPOLLOUT comming
  void* rcb_args;         // extra arguments for read_cb
  void* wcb_args;         // extra arguments for write_cb
};
