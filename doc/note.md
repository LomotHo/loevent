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
