#add_executable(test_ThreadPool test_ThreadPool.cc ../ThreadPool.h ../ThreadPool.cc)
add_executable(test_ThreadPool test_ThreadPool.cc)
target_link_libraries(test_ThreadPool leven)

add_executable(test_EventLoop test_EventLoop.cc)
target_link_libraries(test_EventLoop leven)
