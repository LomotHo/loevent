## server 工作流程: 

1. 初始化: 

  - 初始化 EventLoop
    - 初始化多个 SingleEventLoop, 初始化 IoEventMap
  - 初始化 TcpServer
    - 创建 Accepter
    - 把 TcpServer::onAcceptEvent 写入 EventLoop 的 IoEventMap
  - 给 TcpServer 绑定用户定义的 messageCallback_ 和 connectionCallback_

2. 新连接到来时

  - 触发 TcpServer::onAcceptEvent
    - 执行 accepter_->doAccept() 获取新连接的fd
    - 执行 newConnection 处理新连接
      - 创建 TcpConnection 对象
      - 执行用户传入的 TcpServer::connectionCallback_ 回调
      - 给 TcpConnection 绑定 TcpServer 的 messageCallback_ 回调
      - 把 TcpConnection::onRecv 写入 EventLoop 的 IoEventMap

3. 连接上的消息到来时

  - 触发 TcpConnection::onRecv
    - 处理tcp数据流, 把数据读到buffer, 传入 messageCallback_ 函数, 此时进入用户处理流程

## client 工作流程

1. 初始化 EventLoop
2. 初始化 TcpClient
3. setMessageCallback, setConnectionCallback, 绑定回调
4. TcpClient::run
  - 初始化 Connector
  - Connector::doConnect() (非阻塞)
  - 如果连接失败就创建 POLLOUT 事件, 推迟连接请求, 等待连接成功后再移除事件
5. 连接成功后执行 TcpClient::newConnection(), 与 server 流程类似

## 多线程设计

loevent网络库的多线程实现使用 one loop per thread 模型, 即每个线程有一个 event loop 循环. 这种方案每个loop之间相对独立, 且读写事件负载调度简单.

## buffer

数据读写需要应用层Buffer，read一次可能不能把内核缓冲区的数据全部读完，应该把已经读到的数据保存到应用层接收Buffer，由应用层接收Buffer解决粘包问题，write一次可能不能把所有数据全部写入内核缓冲区，应该有一个应用层发送Buffer，当数据未全部写入内核时会先被填充到应用层发送Buffer，然后在epoll的LT模式下关注POLLOUT事件。POLLOUT事件触发会从应用层发送Buffer取出数据写入内核缓冲区，直到应用层发送Buffer数据全部写完，最后取消关注POLLOUT事件。
