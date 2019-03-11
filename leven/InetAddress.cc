#include <arpa/inet.h>
#include <strings.h>

//#include <assert.h>
//#include <netdb.h>

#include <leven/util/Logger.h>
#include <leven/InetAddress.h>

using namespace leven;

InetAddress::InetAddress(uint16_t port, bool loopback)
{
    bzero(&addr_, sizeof(addr_));
    addr_.sin_family = AF_INET;
    in_addr_t ip = loopback ? INADDR_LOOPBACK:INADDR_ANY;
    addr_.sin_addr.s_addr = htonl(ip);
    addr_.sin_port = htons(port);
}

InetAddress::InetAddress(const std::string& ip, uint16_t port)
{
    bzero(&addr_, sizeof(addr_));
    addr_.sin_family = AF_INET;
    int ret = ::inet_pton(AF_INET, ip.c_str(), &addr_.sin_addr.s_addr);
    if (ret != 1)
        SYSFATAL("InetAddress::inet_pton()");
    addr_.sin_port = htons(port);
}

std::string InetAddress::toIp() const
{
    char buf[INET_ADDRSTRLEN];
    const char* ret = inet_ntop(AF_INET, &addr_.sin_addr, buf, sizeof(buf));
    if (ret == nullptr) {
        buf[0] = '\0';
        SYSERR("InetAddress::inet_ntop()");
    }
    return std::string(buf);
}

uint16_t InetAddress::toPort() const
{ return ntohs(addr_.sin_port); }

std::string InetAddress::toIpPort() const
{
    std::string ret = toIp();
    ret.push_back(':');
    return ret.append(std::to_string(toPort()));
}

//static const int kResolveBufSize = 4096;

//bool InetAddress::resolve(StringArg hostname, InetAddress* out)
//{
//    assert(out);
//    char buf[kResolveBufSize];
//    struct hostent hent;
//    struct hostent* he = NULL;
//    int herrno = 0;
//    bzero(&hent, sizeof(hent));
//
//    int ret = gethostbyname_r(hostname.c_str(), &hent, buf, sizeof buf, &he, &herrno);
//    if (ret == 0 && he != NULL)
//    {
//        assert(he->h_addrtype == AF_INET && he->h_length == sizeof(uint32_t));
//        out->addr_.sin_addr = *reinterpret_cast<struct in_addr*>(he->h_addr);
//        return true;
//    }
//    else if (ret == ERANGE)
//    {
//        return resolveSlow(hostname.c_str(), out);
//    }
//    else
//    {
//        if (ret)
//            perror("InetAddress::resolve");
//        return false;
//    }
//}
