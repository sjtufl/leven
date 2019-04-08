#add_executable(test_ThreadPool test_ThreadPool.cc ../ThreadPool.h ../ThreadPool.cc)
add_executable(test_ThreadPool test_ThreadPool.cc)
target_link_libraries(test_ThreadPool leven)

add_executable(test_EventLoop test_EventLoop.cc)
target_link_libraries(test_EventLoop leven)

add_executable(test_TcpServer test_TcpServer.cc)
target_link_libraries(test_TcpServer leven)

add_executable(test_TcpServerThreaded test_TcpServerThreaded.cc)
target_link_libraries(test_TcpServerThreaded leven)
