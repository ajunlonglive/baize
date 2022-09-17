#include "net/udp_stream.h"

#include "log/logger.h"

namespace baize
{

namespace net
{

UdpStream::UdpStream()
  : conn_(std::make_unique<Socket>(CreatUdpSocket(AF_INET))),
    async_park_(conn_->sockfd())
{
}

UdpStream::UdpStream(uint16_t port)
  : bindaddr_(port),
    conn_(std::make_unique<Socket>(CreatUdpSocket(bindaddr_.family()))),
    async_park_(conn_->sockfd())
{
    conn_->BindAddress(bindaddr_);
}

UdpStream::~UdpStream() {}

UdpStreamSptr UdpStream::AsServer(uint16_t port)
{
    return std::make_shared<UdpStream>(port);
}

UdpStreamSptr UdpStream::AsClient() { return std::make_shared<UdpStream>(); }

int UdpStream::SendTo(const void* buf, int len, const InetAddress& addr)
{
    ssize_t wn = conn_->SendTo(buf, len, addr);
    return static_cast<int>(wn);
}

int UdpStream::RecvFrom(void* buf, int len, InetAddress* addr)
{
    MemZero(addr, sizeof(InetAddress));
    ssize_t rn = conn_->RecvFrom(buf, len, addr);
    return static_cast<int>(rn);
}

int UdpStream::AsyncSendto(const void* buf, int len, const InetAddress& addr)
{
    while (1) {
        async_park_.CheckTicks();
        ssize_t wn = conn_->SendTo(buf, len, addr);
        if (wn < 0) {
            int saveErrno = errno;
            if (errno == EINTR) continue;
            if (errno == EAGAIN) {
                async_park_.WatiWrite();
                continue;
            } else {
                LOG_SYSERR << "async write failed";
            }
            errno = saveErrno;
        }
        return static_cast<int>(wn);
    }
}

int UdpStream::AsyncRecvFrom(void* buf, int len, InetAddress* addr)
{
    MemZero(addr, sizeof(InetAddress));
    while (1) {
        async_park_.CheckTicks();
        ssize_t rn = conn_->RecvFrom(buf, len, addr);
        if (rn < 0) {
            int saveErrno = errno;
            if (errno == EINTR) continue;
            if (errno == EAGAIN) {
                async_park_.WaitRead();
                continue;
            } else {
                LOG_SYSERR << "async read failed";
            }
            errno = saveErrno;
        }
        return static_cast<int>(rn);
    }
}

int UdpStream::AsyncRecvFrom(
    void* buf, int len, InetAddress* addr, double ms, bool& timeout)
{
    MemZero(addr, sizeof(InetAddress));
    while (1) {
        async_park_.CheckTicks();
        ssize_t rn = conn_->RecvFrom(buf, len, addr);
        if (rn < 0) {
            int saveErrno = errno;
            if (errno == EINTR) continue;
            if (errno == EAGAIN) {
                async_park_.WaitRead(ms, timeout);
                if (!timeout) {
                    continue;
                }
            } else {
                LOG_SYSERR << "async read failed";
            }
            errno = saveErrno;
        }
        return static_cast<int>(rn);
    }
}

InetAddress UdpStream::localaddr() { return conn_->localaddr(); }

}  // namespace net

}  // namespace baize
