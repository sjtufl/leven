set(SOURCE_FILES
        noncopyable.h
        Acceptor.cc Acceptor.h
        InetAddress.cc InetAddress.h
        Connector.cc Connector.h
        Logger.c Logger.h
        ThreadPool.cc ThreadPool.h
        Buffer.cc Buffer.h
#        TcpConnection.cc TcpConnection.h
        Callbacks.h
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
#        TcpConnection.h
        )
install(FILES ${HEADERS} DESTINATION include)
