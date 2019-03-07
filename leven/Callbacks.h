//
// Created by fl on 1/26/19.
//

#ifndef LEVEN_CALLBACKS_H
#define LEVEN_CALLBACKS_H

#include <functional>

namespace leven
{

class TcpConnection;
class InetAddress;

typedef std::function<void()> Task;
typedef std::function<void(size_t index)> ThreadInitCallback;

typedef std::function<void()> ErrorCallback;
typedef std::function<void(int sockfd,
                            const InetAddress& local,
                            const InetAddress& peer)> NewConnectionCallBack;

typedef std::function<void()> TimerCallBack;

} // namespace leven

#endif //LEVEN_CALLBACKS_H
