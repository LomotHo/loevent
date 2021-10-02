#include <functional>

typedef std::function<void(int)> MessageCallback;
typedef std::function<void(int)> ConnectionCallback;
class CbObj {
private:
  /* data */
  MessageCallback messageCallback_;
  ConnectionCallback connectionCallback_;

public:
  ~CbObj() {}
  CbObj(/* args */) {}
  void setMessageCallback(const MessageCallback &cb) { messageCallback_ = cb; }
  void setConnectionCallback(const ConnectionCallback &cb) {
    connectionCallback_ = cb;
  }
  void start(int a) { messageCallback_(a); }
};

void onMessage(int msg) { printf("msg: %d\n", msg); }

int main(int argc, char const *argv[]) {
  CbObj co;
  co.setMessageCallback(onMessage);
  co.start(233);
  return 0;
}
