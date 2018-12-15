#ifndef __REMOTE_LOG_THREAD__
#define __REMOTE_LOG_THREAD__

#include <boost/thread.hpp>
#include <boost/shared_ptr.hpp>
#include <vector>
#include <sstream>
#include <iostream>
#include "LBase.h"
#include "LSingleton.h"
#include "RLogHttp.h"
#include "CircularArrayQueue.h"
#include "LTime.h"

#define RLOG(type, content)  do { \
    std::basic_ostringstream<char> bo; \
    bo << type; \
    bo << "|"; \
    bo << content; \
    gRLT.write_log(move(bo.str()) ); \
} while(0)

using namespace std;

class RemoteLogThread : public LSingleton<RemoteLogThread>
{
    typedef CircularArrayQueue<Lstring> LogQueue;

public:
    int init(int start, 
            const char* url, 
            const char* file_name, 
            size_t max_cached_log_size = 20 * 1024, 
            size_t max_cached_log_num = 100, 
            size_t max_file_log_size = 10 * 1024 * 1024,
            bool open_send_log_interval = true); 

    void start();
    void stop();  
    void write_log(Lstring&& log);

    // 单例LSingleton一定要强制实现这两个函数感觉比较...
    bool Init()
    {
        return true;
    }

    bool Final()
    {
        return true;
    }

private:
    void run(void);

    boost::thread* _thread;
    boost::shared_ptr<LogQueue>  _queue_ptr;
    RLogHttp* _remote_log;   
    int _start;
	bool _open_send_log_interval;
    LTime _tick_timer;
};

#define gRLT RemoteLogThread::Instance()

#endif
