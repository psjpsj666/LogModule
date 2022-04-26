#include "log.h"

//初始化日志
Logger::Logger()
{
    LogDir = "../SoftLog/";
    LogName.clear();
    MaxLines = 5000;
    LineCnt = 0;
    Level = Info;
    LogClose = 0;

    char NowDate[256];
    GetTime(NowDate, false);
    //初始文件名：年-月-日.log
    LogName = std::string(NowDate) + ".log";
}

//关闭日志
Logger::~Logger()
{
    LogClose = 0;
}

//获取系统当前时间格式
void Logger::GetTime(char *Nowtime, bool IsGetTime)
{
    time_t timep;
    //获取系统当前日历时间 UTC 1970-01-01 00：00：00开始的unix时间戳
    time(&timep);

    char tmp[256];
    //获取系统当前准确时间
    if (IsGetTime)
        strftime(tmp, sizeof(tmp), "[%Y-%m-%d][%I:%M:%S]", localtime(&timep));
    else
        strftime(tmp, sizeof(tmp), "%Y-%m-%d", localtime(&timep));
    strcpy(Nowtime, tmp);
}

//向文件写入日志
bool Logger::WriteFileLog(std::string FileName, std::string LogMsg, LogLevel nLevel)
{
    FILE *LogFile = fopen(FileName.c_str(), "a");
    if (!LogFile)
    {
        WriteLogToConsole(Error, "日志文件写入失败");
        return false;
    }
    char NowTime[256];
    GetTime(NowTime, true);

    //获取线程ID
    std::thread::id id = std::this_thread::get_id();
    unsigned int threadId = *(unsigned int *)&id;

    fprintf(LogFile, "%s[%s line:%d][thread:%d][%s]: %s\n", NowTime, __FILE__, __LINE__, threadId, LevelName[nLevel], LogMsg.c_str());
    ++LineCnt;
    return true;
}

//文件日志写入函数
void Logger::WriteLogToFile(LogLevel nLevel, std::string LogMsg)
{
    //完整的文件路径
    std::string fileName = LogDir + LogName;

    char NowDate[256];
    GetTime(NowDate, false);

    int i;

    if (LineCnt < MaxLines)
    {
        WriteFileLog(fileName, LogMsg, nLevel);
    }
    else if (LogName == (std::string(NowDate) + ".log"))
    {
        LineCnt = 0;
        //如果初始文件行数达到最大限定值，则将初始文件重命名为 年-月-日-1.log
        rename(fileName.c_str(), (std::string(NowDate) + "-1.log").c_str());
    }
    else
    {
        LineCnt = 0;
        //已经不是初始文档了，文件名：年-月-日-num.log
        //找到文件名最后一个“-”位置和“.”的位置
        int index1 = fileName.find_last_of('-');
        int index2 = fileName.find('.');
        std::string num(fileName.begin()+index1,fileName.begin()+index2);
        i = atoi(num.c_str());
        ++i;
        rename(fileName.c_str(), (std::string(NowDate) + std::to_string(i) + ".log").c_str());
    }
}

//控制台日志写入函数
void Logger::WriteLogToConsole(LogLevel nLevel, std::string LogMsg)
{
    char NowTime[256];
    GetTime(NowTime, true);

    //获取线程ID
    std::thread::id id = std::this_thread::get_id();
    unsigned int threadId = *(unsigned int *)&id;

    fprintf(stdout, "%s[%s line:%d][thread:%d][%s]: %s\n", NowTime, __FILE__, __LINE__, threadId, LevelName[nLevel], LogMsg.c_str());
}