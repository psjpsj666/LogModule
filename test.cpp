#include "log.h"
#include <vector>
#include <mutex>
using namespace std;

mutex m_mutex;
Logger m_log;
void func()
{
    int i=0;
    ++i;
    m_mutex.lock();
    m_log.WriteLogToFile(Info, "Hello");
    m_mutex.unlock();
}

int main()
{
    Logger m_log;

   // m_log.WriteLogToFile(Info, "Hello");   //ok
   // m_log.WriteFileLog("test.log", "hello", Info); // ok
    //m_log.WriteFileLog(m_log.GetFileName(),"hello"); ok

   
    // string fileName = m_log.GetFileName();  // ok
    // cout << "filename = " << fileName << endl;
    // string puredate;
    // m_log.GetYMD("2022-04-07",puredate);   //OK
    //cout<<"puredate = " <<puredate<<endl;
    vector<thread> mythreads;
    for(int i=0; i<11000; ++i){
        mythreads.push_back(thread(func));
    }
    for(auto iter = mythreads.begin(); iter!=mythreads.end(); ++iter)
    {
        iter->join();
    }
    m_log.WriteLogToConsole(Info, "Hello"); // ok
    //this_thread::sleep_for(std::chrono::seconds(30));
    return 0;
}