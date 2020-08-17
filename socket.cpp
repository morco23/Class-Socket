#include <cstring>          //strerror
#include <cassert>           //assert
#include <unistd.h>         //close
#include <netinet/in.h>     //sockaddin
#include <fcntl.h>          //fctrl
#include "socket.hpp"

namespace morcohen
{

int Socket::CreateSocket(int domain_, int type_, int protocol_)
{
    int fd = socket(domain_, type_, protocol_);
    if (-1 == fd)
    {
        throw std::runtime_error(strerror(errno));
    }
    return fd;
}

Socket::Socket(int domain_, int type_, int protocol_):
                    m_fd(CreateSocket(domain_, type_, protocol_)),
                    m_wasClosed(0)
{
}


Socket::Socket(int fd_) noexcept:
                m_fd(fd_),
                m_wasClosed(0)
{


}

Socket::~Socket()
{
    if(!m_wasClosed)
    {
        try{
            Close();
        }
        catch(const std::exception& e)
        {
            //log
        }
    }
}

void Socket::Bind(int port_)
{
    struct sockaddr_in server_addr;

    assert(port_);

    memset(&server_addr , 0, sizeof(struct sockaddr_in));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port_);
    server_addr.sin_addr.s_addr = INADDR_ANY; 

    if (-1 == bind(m_fd, (struct sockaddr *)&server_addr,
                                 sizeof(struct sockaddr_in)))
    {
        throw std::runtime_error(strerror(errno));
    }
}

void Socket::Bind(sockaddr *addr_, socklen_t addrlen_)
{
    assert(addr_);

    struct sockaddr_in server_addr;

    if (-1 == bind(m_fd, addr_,addrlen_))
    {
        throw std::runtime_error(strerror(errno));
    }

}

void Socket::Listen(int maxPending_)
{
    if (-1 == listen(m_fd, maxPending_))
    {
        throw std::runtime_error(strerror(errno));
    }
}

std::shared_ptr<Socket> Socket::Accept(sockaddr *address_, socklen_t *addressLen_)
{

    int fd = accept(m_fd, address_, addressLen_);
    if (-1 == fd)
    {
        throw std::runtime_error(strerror(errno));
    }
    return std::shared_ptr<Socket>(new Socket(fd));
}

ssize_t Socket::Recieve(void *buf_, std::size_t len_, int flags_)
{
    assert(buf_);
    assert(len_ > 0);

    ssize_t byteWasRead = recv(m_fd, buf_, len_, flags_);
    if (-1 == byteWasRead)
    {
        throw std::runtime_error(strerror(errno));
    }

    return (byteWasRead);
}

ssize_t Socket::RecieveFrom(void *buf_, std::size_t len_, int flags_,
                        struct sockaddr *srcAddr_, socklen_t *addrLen_)
{
    assert(buf_);
    assert(len_ > 0);

    ssize_t byteWasRead = recvfrom(m_fd, buf_, len_, flags_, srcAddr_,
                                                            addrLen_);
    if (-1 == byteWasRead)
    {
        throw std::runtime_error(strerror(errno));
    }
    return (byteWasRead);
}

ssize_t Socket::Send(const void *buf_, size_t len_, int flags_)
{
    assert(buf_);
    assert(len_ > 0);

    ssize_t byteWasSend = send(m_fd, buf_, len_, flags_);

    if (-1 == byteWasSend)
    {
        throw std::runtime_error(strerror(errno));
    }

    return byteWasSend;
}

ssize_t Socket::SendTo(const void *buf_, size_t len_, int flags_,
                      const struct sockaddr *destAdd_, socklen_t addrlen_)
{
    assert(buf_);
    assert(len_ > 0);
    assert(destAdd_);
    assert(addrlen_);

    ssize_t byteWasSend = sendto(m_fd, buf_, len_, flags_, destAdd_, addrlen_);

    if (-1 == byteWasSend)
    {
        throw std::runtime_error(strerror(errno));
    }

    return byteWasSend;
}

void Socket::Close()
{
    if (-1 == close(m_fd))
    {
      throw std::runtime_error(strerror(errno));
    }
    m_wasClosed = 1;
}

void Socket::SetBlock(bool isBlock_)
{
    int flags = fcntl(m_fd, F_GETFL, 0);
    if (-1 == flags)
    {
        throw std::runtime_error(strerror(errno));
    }

    flags = isBlock_ ? (flags | O_NONBLOCK) : (flags & ~O_NONBLOCK);

    if (-1 == fcntl(m_fd, F_SETFL, flags))
    {
        throw std::runtime_error(strerror(errno));
    }
}

int Socket::GetFd() const noexcept
{
    return m_fd;
}



}//morcohen
