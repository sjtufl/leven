add_executable(ChargenServer ChargenServer.cc)
target_link_libraries(ChargenServer leven)

add_executable(EchoServer EchoServer.cc)
target_link_libraries(EchoServer leven)