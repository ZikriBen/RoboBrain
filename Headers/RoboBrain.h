#pragma once

#include <iostream>
#include <cstdlib>
#include <random>
#include <mutex>
#include <vector>
#include <chrono>
#include <queue>
#include <thread>
#include <vector>

#include <map>
#include <condition_variable>
#include <yaml-cpp/node/node.h>
#include <yaml-cpp/parser.h>
#include <yaml-cpp/yaml.h>

class NetFunction {
public:
    virtual void execute(std::vector<double> &motorPositions) = 0;
};

class NetFunctionReverse : public NetFunction {
public:
    void execute(std::vector<double> &motorPositions) override;
};

class NetFunctionToBase : public NetFunction {
public:
    void execute(std::vector<double> &motorPositions) override;
};

class NetFunctionFactory {
public:
    void registerFunction(const std::string& funcName, std::function<NetFunction* ()> funcPtr);
    NetFunction* createFunction(const std::string& funcName);
    bool funcExists(const std::string& funcName);
private:
    std::map<std::string, std::function<NetFunction* ()>> m_functionMap;
};
constexpr double FULLCIRCLE = 360.0;

struct Configuration {
    int motorCount;
    std::vector<std::string> netFunctionNames;
};


class RoboBrain {
private:
    Configuration m_Config;
    bool m_isConfigured;
    bool m_isRunning;
    std::vector<double> m_MotorPositions;
    std::mutex m_Mutex;
    std::mutex m_funcMutex;
    std::queue<std::string> m_funcQ;
    bool m_currentNetFunction;
    std::thread m_mainLoopThread;
    std::thread m_motorsLoopThread;
    std::thread m_positionLoopThread;
    long m_timer;
    std::condition_variable m_motorsLoopCondition;
    std::condition_variable m_positionLoopCondition;
    NetFunctionFactory m_factory;

    void mainLoop();
    void motorsLoop();
    void positionLoop();
    
    double normalizeAngle(double angle);
    void printPositions(std::string title);

public:
    RoboBrain();
    void loadConfig(std::string path);
    void addNetFunc(std::string funcName);
    void run();
    void stop();
};
