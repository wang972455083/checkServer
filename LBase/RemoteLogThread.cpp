#include "RemoteLogThread.h"
#include "LLog.h"


#define MAX_SEND_LOG_INTERVAL  5*60  // five min

int RemoteLogThread::init(int start, 
                        const char* url, 
                        const char* file_name, 
                        size_t max_cached_log_size, 
                        size_t max_cached_log_num, 
                        size_t max_file_log_size,
                        bool open_send_log_interval)
{
//#ifndef _LINUX
//    return 0;
//#endif

    _start = start;
    if(_start == 0)
    {
        LLOG_DEBUG("RemoteLogThread not start, config start = %d.", _start);
        return 0; 
    }

    _queue_ptr = boost::shared_ptr<LogQueue>(new LogQueue());
    assert(_queue_ptr != NULL);
    _queue_ptr->init(100000 + 1);


    _remote_log = RLogHttp::getInstance();
    assert(_remote_log != NULL);
    _open_send_log_interval = open_send_log_interval;
    return _remote_log->init(url, file_name, max_cached_log_size, max_cached_log_num, max_file_log_size);
}

void RemoteLogThread::start()
{
//#ifndef _LINUX
//    return;
//#endif
    if(_start == 0)
    {
        return;    
    }

    _thread = new boost::thread(boost::bind(&RemoteLogThread::run, this));
    assert(_thread != NULL); 
}

void RemoteLogThread::stop()
{
//#ifndef _LINUX
//    return;
//#endif
    if(_start == 0)
    {
        return;    
    }

    if(_queue_ptr != NULL)
    {
        _queue_ptr->push(Lstring());     
    }

    /** join函数，作用是等待直到线程执行结束；可不加，但不能保证退出Destroy函数前线程被终结 */
    if(_thread != NULL)
    {
        _thread->join();
    }

    LLOG_DEBUG("remote log thread exit!\n");
}

void RemoteLogThread::write_log(Lstring&& log)
{
//#ifndef _LINUX
//    return;
//#endif

    if(_start == 0)
    {
        return;    
    }

    if(move(log).empty())
    {
        LLOG_DEBUG("[WARN] write log is empty!\n");   
        return;
    }

    if(_queue_ptr == NULL)
    {
        LLOG_ERROR("[ERROR] function:%s, _queue_ptr is null!", __FUNCTION__);    
        return;
    }
    int ret = _queue_ptr->push(move(log));
    if(ret < 0)
    {
        LLOG_DEBUG("push ret: %d, current queue size: %u, queue is full!\n", ret, _queue_ptr->size());
    }
}

void RemoteLogThread::run()
{
//#ifndef _LINUX
//	return;
//#endif
    while(true)
    {
        if(_start == 0)
        {
            return;    
        }

        if(_queue_ptr == NULL)
        {
            LLOG_ERROR("[ERROR] function:%s, _queue_ptr is null!", __FUNCTION__);    
            return;
        }
        
        if(_remote_log == NULL)
        {
            LLOG_ERROR("[ERROR] function:%s, _remote_log is null!", __FUNCTION__);
            return;    
        }

        if(_open_send_log_interval)
        {
            LTime cur;
            
            if(cur.Secs() - _tick_timer.Secs() >= MAX_SEND_LOG_INTERVAL)
            {   
                _tick_timer = cur;
                _remote_log->send_all_log_now();
            }
        }

        if(_queue_ptr->empty())
        {
#ifdef _LINUX
			::usleep(10);
#else
			Sleep(1);
#endif // _LINUX
            continue;
        }
        Lstring log = _queue_ptr->front();
        _queue_ptr->pop();
        if(log.empty())
        {
            LLOG_DEBUG("[WARN] log is empty, break run!\n");
            _remote_log->send_all_log_now();
            break;
        }

        _remote_log->rlog(log); 
    }
}
