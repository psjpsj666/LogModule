#include "log.h"

//初始化日志
Logger::Logger()
{
    LogDir = "./LogRecord/";
    //需要创建日志文件夹
    std::string cmd = "mkdir -p ";
    cmd += LogDir;
    system(cmd.c_str());

    LogName.clear();
    MaxLines = 5000;
    LineCnt = 0;
    Level = Info;
    LogClose = true;

    char NowDate[256];
    GetTime(NowDate, false);
    //初始文件名：年-月-日.log
    LogName = std::string(NowDate) + ".log";
}

//关闭日志
Logger::~Logger()
{
    if (LogClose == false)
        LogClose = true;
}

//开启日志
void Logger::OpenLog()
{
    if (LogClose == true)
        LogClose = false;
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

//将字符串 年-月-日转化为年月日
void Logger::GetYMD(std::string Data, std::string &PureDate)
{
    int idx1 = Data.find('-');
    int idx2 = Data.rfind('-');
    PureDate = std::string(Data.begin(), Data.begin() + idx1) + std::string(Data.begin() + idx1 + 1, Data.begin() + idx2) + std::string(Data.begin() + idx2 + 1, Data.end());
}

//向文件写入日志
bool Logger::WriteFileLog(std::string FileName, std::string LogMsg, LogLevel nLevel)
{
    OpenLog();
    if (LogClose)
    {
        WriteLogToConsole(Error, "日志开启失败");
        return false;
    }
    // std::cout << "FileName = " << FileName << std::endl;
    FILE *LogFile = fopen(FileName.c_str(), "a+");
    if (!LogFile)
    {
        WriteLogToConsole(Error, "日志文件写入失败");
        return false;
    }
    char NowTime[256];
    GetTime(NowTime, true);

    //获取线程ID
    std::thread::id id = std::this_thread::get_id();
    unsigned long long threadId = *(unsigned long long *)&id;

    fprintf(LogFile, "%s[%s line:%d][thread:%llu][%s]: %s\n", NowTime, __FILE__, __LINE__, threadId, LevelName[nLevel], LogMsg.c_str());
    fflush(LogFile);
    ++LineCnt;
    fclose(LogFile);
    return true;
}

//文件日志操作
void Logger::WriteLogToFile(LogLevel nLevel, std::string LogMsg)
{
    //完整的文件路径
    std::string fileName = LogDir + LogName;

    //得到当前系统时间：年-月-日
    char NowDate[256];
    GetTime(NowDate, false);
    std::string NowPureDate;
    GetYMD(std::string(NowDate), NowPureDate);
    int NowTime = atoi(NowPureDate.c_str());
    // std::cout << "NowTime = " << NowTime << std::endl;

    //解析当前目录下日期最近的文件创建日期，即LogName的日期
    //统计LogName里面"-"出现的次数
    std::string PreDate;
    size_t idx;
    int count = std::count(LogName.begin(), LogName.end(), '-');
    if (count == 2) //即LogName = 年-月-日.log
    {
        idx = LogName.rfind('.');
    }
    if (count == 3) //即LogName = 年-月-日-num.log
    {
        idx = LogName.rfind('-');
    }
    PreDate = std::string(LogName.begin(), LogName.begin() + idx);
    std::string PrePureDate;
    GetYMD(PreDate, PrePureDate);
    int fileTime = atoi(PrePureDate.c_str());
    // std::cout << "fileTime = " << fileTime << std::endl;

    int i;

    if (NowTime > fileTime) //时间过了一天
    {
        //创建新的一天的日志文件，重新开始写日志
        LogName = std::string(NowDate) + ".log";
        fileName = LogDir + LogName;
        LineCnt = 0;
        WriteFileLog(fileName, LogMsg, nLevel);
    }
    else
    {
        if (LineCnt < MaxLines)
        {
            // std::cout<<"LineCnt = "<<LineCnt <<std::endl;
            WriteFileLog(fileName, LogMsg, nLevel);
        }
        else if (fileName == (LogDir + std::string(NowDate) + ".log"))
        {
            LineCnt = 0;
            //如果初始文件行数达到最大限定值，则将初始文件重命名为 年-月-日-1.log
            rename(fileName.c_str(), (LogDir + std::string(NowDate) + "-1.log").c_str());
            //新文件的名称为：年-月-日-2.log
            fileName.clear();
            LogName = std::string(NowDate) + "-2.log";
            fileName = LogDir + LogName;
        }
        else
        {
            LineCnt = 0;
            //已经不是初始文档了，文件名：路径/年-月-日-num.log
            //找到文件名最后一个“-”位置和“.”的位置
            int index1 = fileName.rfind('-');
            int index2 = fileName.rfind('.');
            std::string num(fileName.begin() + index1 + 1, fileName.begin() + index2);
            i = atoi(num.c_str());
            ++i;
            // std::cout << "i=" << i << std::endl;
            //  rename(fileName.c_str(), (std::string(NowDate) + std::to_string(i) + ".log").c_str());
            fileName.clear();
            LogName = std::string(NowDate) + "-" + std::to_string(i) + ".log";
            fileName = LogDir + LogName;
        }
    }
}

//控制台日志写入函数
void Logger::WriteLogToConsole(LogLevel nLevel, std::string LogMsg)
{
    char NowTime[256];
    GetTime(NowTime, true);

    //获取线程ID
    std::thread::id id = std::this_thread::get_id();
    unsigned long long threadId = *(unsigned long long *)&id;

    fprintf(stdout, "%s[%s line:%d][thread:%llu][%s]: %s\n", NowTime, __FILE__, __LINE__, threadId, LevelName[nLevel], LogMsg.c_str());
}
