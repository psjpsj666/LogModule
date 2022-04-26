#ifndef LOG_H
#define LOG_H

#include <string>
#include <iostream>
#include <time.h>
#include <cstring>
#include <thread>
#include <exception>

enum LogLevel
{
    Debug = 0,
    Info,
    Warning,
    Error,
    Fatal
};

//定义与日志等级枚举类型相对应的数组，方便等级的输出
const char *LevelName[] = {
    "Debug",
    "Info",
    "Warning",
    "Error",
    "Fatal"};

class Logger
{
public:
    //初始化日志
    Logger();
    //关闭日志
    ~Logger();
    /*
    获取系统当前时间字符串
    Nowtime：得到当前时间
    IsGetTime：是否得到当前时分秒
    */
    void GetTime(char *Nowtime, bool IsGetTime = true);
    //向文件写入日志
    bool WriteFileLog(std::string FileName, std::string LogMsg, LogLevel nLevel = Info);
    //文件日志写入函数
    void WriteLogToFile(LogLevel nLevel, std::string LogMsg);
    //控制台日志写入函数
    void WriteLogToConsole(LogLevel nLevel, std::string LogMsg);

private:
    std::string LogDir;  //日志文件的保存路径
    std::string LogName; //日志文件名
    int MaxLines;        //单个日志文件最大存储行数
    int LineCnt;         //单个日志文件行数记录
    LogLevel Level;      //日志等级
    int LogClose;        //日志关闭标志：1开启，0关闭
};

#endif