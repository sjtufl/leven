//
// Created by fl on 1/26/19.
//

#ifndef LEVEN_CALLBACKS_H
#define LEVEN_CALLBACKS_H

#include <functional>
#include <memory>

namespace leven
{

class TcpConnection;
class InetAddress;
class Buffer;

typedef std::function<void()> Task;
typedef std::function<void(size_t index)> ThreadInitCallback;

typedef std::function<void()> ErrorCallback;
typedef std::function<void(int sockfd,
                            const InetAddress& local,
                            const InetAddress& peer)> NewConnectionCallBack;

typedef std::function<void()> TimerCallBack;

typedef std::shared_ptr<TcpConnection> TcpConnPtr;
typedef std::function<void(const TcpConnPtr&, Buffer&)> MessageCallback;
typedef std::function<void(const TcpConnPtr&)> CloseCallback;
typedef std::function<void(const TcpConnPtr&)> WriteCompleteCallback;
typedef std::function<void(const TcpConnPtr&, size_t)> HighWatermarkCallback;

} // namespace leven

#endif //LEVEN_CALLBACKS_H
