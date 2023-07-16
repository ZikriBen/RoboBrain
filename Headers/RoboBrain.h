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
#include <functional>

#include <yaml-cpp/node/node.h>
#include <yaml-cpp/parser.h>
#include <yaml-cpp/yaml.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

//RoboBrain
class RoboBrain {
private:
    Configuration m_Config;
    NetFunctionFactory m_factory;
    
    bool m_isConfigured;
    bool m_isRunning;
    bool m_currentNetFunction;
    long m_timer;

    std::vector<double> m_MotorPositions;
    std::queue<std::string> m_funcQ;
    std::mutex m_Mutex;
    std::mutex m_funcMutex;
    
    std::thread m_mainLoopThread;
    std::thread m_motorsLoopThread;
    std::thread m_positionLoopThread;
    std::thread m_timerLoopThread;
    
    std::condition_variable m_motorsLoopCondition;
    std::condition_variable m_positionLoopCondition;
    std::condition_variable m_mainLoopCondition;
    
    
    void timerLoop();
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

// NetFunction
class NetFunction {
public:
    virtual void execute(std::vector<double>& motorPositions) = 0;
};

class NetFunctionReverse : public NetFunction {
public:
    void execute(std::vector<double>& motorPositions) override;
};

class NetFunctionToBase : public NetFunction {
public:
    void execute(std::vector<double>& motorPositions) override;
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


// Configuration
struct Configuration {
    int motorCount;
    std::vector<std::string> netFunctionNames;
};
