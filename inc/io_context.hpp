#ifndef __LOEVENT_IO_CONTEXT__
#define __LOEVENT_IO_CONTEXT__

#include <memory>
namespace loevent {

class IoContext {
 public:
  IoContext() {}
  virtual ~IoContext() {}
};
typedef std::shared_ptr<IoContext> IoContextPtr;

}  // namespace loevent
#endif  // !__LOEVENT_IO_CONTEXT__
