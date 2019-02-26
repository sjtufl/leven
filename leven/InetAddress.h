#ifndef LEVEN_INETADDRESS_H
#define LEVEN_INETADDRESS_H

#include <string>
#include <netinet/in.h>

namespace leven
{

class InetAddress {
public:
    explicit InetAddress(uint16_t port = 0, bool loopback = false);
    InetAddress(const std::string& ip, uint16_t port);

    void setAddress(const struct sockaddr_in& addr)
    { addr_ = addr; }
    const struct sockaddr* getSockaddr() const
    { return reinterpret_cast<const struct sockaddr*>(&addr_); }
    socklen_t getSocklen() const
    { return sizeof(addr_); }

    // For passing C-style string argument to a function.
//    class StringArg
//    {
//    public:
//        StringArg(const char* str)
//                : str_(str)
//        { }
//
//        StringArg(const std::string& str)
//                : str_(str.c_str())
//        { }
//
//        const char* c_str() const { return str_; }
//
//    private:
//        const char* str_;
//    };
//    static bool resolve(StringArg hostname, InetAddress*);

    std::string toIp() const;
    uint16_t toPort() const;
    std::string toIpPort() const;


private:

    struct sockaddr_in addr_;
};

} // namespace leven


#endif //LEVEN_INETADDRESS_H
