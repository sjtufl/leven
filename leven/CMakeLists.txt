set(SOURCE_FILES
        noncopyable.h
        Acceptor.cc Acceptor.h
        InetAddress.cc InetAddress.h
        Connector.cc Connector.h
        util/Logger.c util/Logger.h
        util/ThreadPool.cc util/ThreadPool.h
        util/Buffer.cc util/Buffer.h
        Channel.cc Channel.h
        EventLoop.cc EventLoop.h
        Epoller.cc Epoller.h
        TcpConnection.cc TcpConnection.h
        Timer.h
        TimerQueue.h TimerQueue.cc
        Callbacks.h
        TcpServer.h TcpServer.cc
        )

add_library(leven STATIC ${SOURCE_FILES})
target_link_libraries(leven pthread)

install(TARGETS leven DESTINATION lib)
set(HEADERS
        Acceptor.h
        Connector.h
        InetAddress.h
        noncopyable.h
        Logger.h
        Callbacks.h
        ThreadPool.h
        Buffer.h
        TcpConnection.h
        Channel.h
        EventLoop.h
        TimerQueue.h
        Timer.h
        TcpServer.h
        )
install(FILES ${HEADERS} DESTINATION include)
