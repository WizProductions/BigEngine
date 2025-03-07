
#include "pch.h"
#include "TimerManager.h"
#include <iostream>
#include <chrono>
#include <iomanip>
#include <sstream>

TimerManager::TimerManager() :
    m_timerTotalPauseTime(0),
    m_timerIsStarted(false),
    m_timerIsPaused(false),
    m_timerPointIndicatorIsSet(false)
{}

using namespace std;

void TimerManager::StartTimer() {
    if (!m_timerIsStarted) {
        m_timerStartTime = chrono::high_resolution_clock::now();
        m_timerTotalPauseTime = chrono::duration<double>::zero();
        m_timerIsStarted = true;
        m_timerIsPaused = false;
    } else {
        cout << "Error->StartTimer(): Timer is already started." << endl;
    }
}

void TimerManager::RestartTimer() {
    m_timerStartTime = chrono::high_resolution_clock::now();
    m_timerTotalPauseTime = chrono::duration<double>::zero();
    m_timerPointIndicatorIsSet = false;
    m_timerPointIndicator = chrono::high_resolution_clock::time_point();
    m_timerIsStarted = true;
    m_timerIsPaused = false;
}

void TimerManager::StopTimer() {
    if (m_timerIsStarted) {
        m_timerEndTime = chrono::high_resolution_clock::now();
        m_timerIsStarted = false;
        m_timerIsPaused = false;
    } else {
        cout << "Error->StopTimer(): Timer is not started." << endl;
    }
}

void TimerManager::PauseTimer() {
    if (m_timerIsStarted && !m_timerIsPaused) {
        m_timerIsPaused = true;
        m_timerPauseTime = chrono::high_resolution_clock::now();
    } else {
        cout << "Error->PauseTimer(): Timer is either not started or already paused." << endl;
    }
}

void TimerManager::ResumeTimer() {
    if (m_timerIsStarted && m_timerIsPaused) {
        m_timerTotalPauseTime += chrono::high_resolution_clock::now() - m_timerPauseTime;
        m_timerIsPaused = false;
    } else {
        cout << "Error->ResumeTimer(): Timer is either not started or not paused." << endl;
    }
}

void TimerManager::SetTimerPointIndicator() {
    m_timerPointIndicator = chrono::high_resolution_clock::now();
    m_timerPointIndicatorIsSet = true;
}

double TimerManager::GetElapsedTimeBtwLastTimerPointIndicatorInSeconds() const {
    if (m_timerPointIndicatorIsSet) {
        auto elapsed = chrono::high_resolution_clock::now() - m_timerPointIndicator;
        return chrono::duration<double>(elapsed).count() - m_timerTotalPauseTime.count();
    } else {
        cout << "Error->GetElapsedTimeBtwLastTimerPointIndicatorInSeconds(): Timer point indicator is not set." << endl;
        return -1.0;
    }
}

string TimerManager::TimerPointConvertToDate(chrono::high_resolution_clock::time_point timePoint) {
    auto systemTimePoint = chrono::time_point_cast<chrono::system_clock::duration>(
        timePoint - chrono::high_resolution_clock::now() + chrono::system_clock::now()
    );

    time_t timeT = chrono::system_clock::to_time_t(systemTimePoint);
    tm tm;
    localtime_s(&tm, &timeT); 

    ostringstream oss;
    oss << put_time(&tm, "%Y-%m-%d %H:%M:%S"); // Format: "YYYY-MM-DD HH:MM:SS"
    return oss.str();
}

double TimerManager::GetElapsedTimeInSeconds() const {
    if (m_timerIsStarted) {
        auto elapsed = chrono::high_resolution_clock::now() - m_timerStartTime;
        if (m_timerIsPaused) {
            return chrono::duration<double>(m_timerPauseTime - m_timerStartTime - m_timerTotalPauseTime).count();
        } else {
            return chrono::duration<double>(elapsed).count() - m_timerTotalPauseTime.count();
        }
    } else {
        cout << "Error->GetElapsedTimeInSeconds(): Timer is not started." << endl;
        return -1.0;
    }
}

chrono::high_resolution_clock::time_point TimerManager::GetStartTime() const {
    return m_timerStartTime;
}

chrono::high_resolution_clock::time_point TimerManager::GetEndTime() const {
    return m_timerEndTime;
}

bool TimerManager::GetTimerIsPaused() const {
    return m_timerIsPaused;
}

bool TimerManager::GetTimerIsStarted() const {
    return m_timerIsStarted;
}