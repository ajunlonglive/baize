#include "log/logger.h"
#include "net/tcp_listener.h"
#include "runtime/event_loop.h"
#include "time/timer.h"

using namespace baize;
using namespace baize::runtime;
using namespace baize::net;
using namespace baize::time;

void routine_test_timeout()
{
    TcpListener listener(6060);
    listener.Start();

    TcpStreamSptr stream = listener.AsyncAccept(3000);
    if (!stream) {
        LOG_INFO << "routine timeout";
    }
}

int main()
{
    EventLoop loop(2);
    loop.Do(routine_test_timeout);
    loop.Start();
}