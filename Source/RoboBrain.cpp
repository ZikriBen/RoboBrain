// ConsoleApplication1.cpp : This file contains the 'main' function. Program execution begins and ends there.
// My assumptions:
// If I understand correctly, we are using rotating motors (for simplicity only clockwise).
// The motor positions are angular positions, and will be represented  as angles 0-360 when zero is the motor starting point.
// The motors main movement is by calling "netFunctions" in the "mainLoop" that control motors position.
// The netFunction will be added to a queue and removed in FIFO. netFunction will be exhausted in mainLoop, but this behaviour could be to run single netFunction per cycle.
// ALSO, the motors are constantly moving in "motorsLoop" in one direction.
// ALSO, the motors also have some "noise" produced in "positionLoop" that are constantly rotating but in a small portion
// The difference in Hz frequency will let the motors change positions with "priority where the lower Hz will get precedence like so: NetFunction(MainLoop) -> MotorsLoop -> PositionLoop 


#include "pch.h" 
#include <iostream>
#include <functional>
#include <string.h>
#include "RoboBrain.h"
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>


// I searched for a random number that is not seeded by timestamp, as i noticed it's updated by the second
std::random_device rd;
std::mt19937 gen(rd());
std::uniform_real_distribution<double> dis(0.0, 1.0);

RoboBrain::RoboBrain() {
    m_currentNetFunction = "";
    m_timer = 0;
    m_isConfigured = false;
    m_isRunning = false;
}

void RoboBrain::loadConfig(std::string path) {
    
    YAML::Node configNode = YAML::LoadFile(path);

    if (configNode["motorCount"] && configNode["netFunctionNames"]) {
        m_Config.netFunctionNames = configNode["netFunctionNames"].as<std::vector<std::string>>();
        m_Config.motorCount = configNode["motorCount"].as<int>();
    }
    else {
        std::cout << "No motorCount or netFunctionNames provided" << std::endl;
        return;
    }
    m_MotorPositions.resize(m_Config.motorCount, 0.1);

    m_isConfigured = true;

    for (auto& name : m_Config.netFunctionNames) {
        if (name == "toBase") {
            m_factory.registerFunction("toBase", []() { return new NetFunctionToBase(); });
        }
        else if (name == "reverse") {
            m_factory.registerFunction("reverse", []() { return new NetFunctionReverse(); });
        }
        else
            std::cout << "Function " << name << " is not recognized!" << std::endl;
    }
}

void RoboBrain::addNetFunc(std::string funcName) {
    if (!m_factory.funcExists(funcName)) {
        std::cout << "Function " << funcName << " can not be added" << std::endl;
    }
    
    std::lock_guard<std::mutex> lock(m_funcMutex);
    m_funcQ.push(funcName);
}

void RoboBrain::run() {
    if (!m_isConfigured) {
        std::cout << "You must Configure before run!" << std::endl;
        return;
    }

    std::cout << "Start running" << std::endl;
    m_isRunning = true;
    m_mainLoopThread = std::thread(&RoboBrain::mainLoop, this);
    m_motorsLoopThread = std::thread(&RoboBrain::motorsLoop, this);
    m_positionLoopThread = std::thread(&RoboBrain::positionLoop, this);
}

void RoboBrain::stop() {
    m_isRunning = false;
    std::cout << "stop running" << std::endl;
}

void RoboBrain::mainLoop() {
    while (m_isRunning) {
        m_timer = fmod(m_timer + 2, 100);
        // std::cout << m_timer << std::endl;
        if (m_timer % 4 == 0) {
            m_positionLoopCondition.notify_all();
        }
        if (m_timer % 100 == 0) {

            while (!m_funcQ.empty()) {
                std::unique_lock<std::mutex> lock(m_funcMutex);

                std::string functionName = m_funcQ.front();
                m_funcQ.pop();
                lock.unlock();  // releasing to make criticl section shorter

                NetFunction* netFunction = m_factory.createFunction(functionName);

                if (netFunction) {
                    netFunction->execute(m_MotorPositions);
                    printPositions("NetFunction");
                }
            }
        }
        if (m_timer % 10 == 0) {
            m_motorsLoopCondition.notify_all();
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100)); // 10 Hz
    }
}

void RoboBrain::motorsLoop() {
    while (m_isRunning) {
        std::unique_lock<std::mutex> lock(m_Mutex);
        m_motorsLoopCondition.wait(lock);
        double noise = dis(gen);

        for (auto& pos : m_MotorPositions) {
            pos = normalizeAngle(pos * (1 + 0.1));
        }
        printPositions("Motors");
        std::this_thread::sleep_for(std::chrono::milliseconds(10)); // 100 Hz
    }
}

void RoboBrain::positionLoop() {
    while (m_isRunning) {
        std::unique_lock<std::mutex> lock(m_Mutex);
        m_positionLoopCondition.wait(lock);

        double noise = dis(gen);

        for (auto& pos : m_MotorPositions) {

            pos = normalizeAngle(pos * (1 + 0.001 * noise));
        }
        printPositions("Position");
        std::this_thread::sleep_for(std::chrono::milliseconds(4)); // 250 Hz
    }
}

double RoboBrain::normalizeAngle(double angle) {
    angle = fmod(angle, FULLCIRCLE);
    if (angle < 0.0) {
        angle += FULLCIRCLE;
    }

    return angle;
}

void RoboBrain::printPositions(std::string title) {
    std::cout << "- [" << title << "] - ";

    for (auto& pos : m_MotorPositions) {
        std::cout << pos << ",";
    }
    std::cout << std::endl;
}

void NetFunctionReverse::execute(std::vector<double> &motorPositions) {
    std::reverse(motorPositions.begin(), motorPositions.end());
}

void NetFunctionToBase::execute(std::vector<double> &motorPositions) {
    std::fill(motorPositions.begin(), motorPositions.end(), 0.1);
}

void NetFunctionFactory::registerFunction(const std::string& funcName, std::function<NetFunction* ()> funcPtr) {
    std::cout << "Function " << funcName << " is now registered" << std::endl;
    m_functionMap[funcName] = funcPtr;
}

NetFunction* NetFunctionFactory::createFunction(const std::string& funcName) {
    if (funcExists(funcName)) {
        return m_functionMap[funcName]();
    }
    return nullptr;
}

bool NetFunctionFactory::funcExists(const std::string& funcName) {
    if (m_functionMap.find(funcName) != m_functionMap.end()) {
        return true;
    }
    return false;
}

PYBIND11_MODULE(RoboBrain, m) {
    pybind11::class_<RoboBrain>(m, "RoboBrain")
        .def(pybind11::init<>())
        .def("run", &RoboBrain::run)
        .def("add_net_func", &RoboBrain::addNetFunc, pybind11::arg("funcName"))
        .def("load_config", &RoboBrain::loadConfig, pybind11::arg("path"));
}