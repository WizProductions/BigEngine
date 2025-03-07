#pragma once
#ifndef TIMER_H_
#define TIMER_H_

class TimerManager {

private:

/**-------- TIMERS VARIABLES --------*/

	std::chrono::high_resolution_clock::time_point m_timerStartTime;
	std::chrono::high_resolution_clock::time_point m_timerEndTime;
	std::chrono::high_resolution_clock::time_point m_timerPauseTime;
	std::chrono::high_resolution_clock::time_point m_timerPointIndicator;
	std::chrono::duration<double> m_timerTotalPauseTime;
	bool m_timerIsStarted;
	bool m_timerIsPaused;
	bool m_timerPointIndicatorIsSet;

/**----------------------------------*/

public:

/**---------- CONSTRUCTORS ----------*/
	
	TimerManager();

/**----------------------------------*/

	/** Starts the timer. */
	void StartTimer();

	/** Restarts the timer. */
	void RestartTimer();

	/** Stops the timer. */
	void StopTimer();

	/** Pauses the timer. */
	void PauseTimer();

	/** Resumes the timer. */
	void ResumeTimer();

	/** Set a timerPoint to the timer. (For GetElapsedTimeBtwLastTimerPointIndicatorInSeconds) */
	void SetTimerPointIndicator();

	/** Returns time betwee lastTimerPointIndicator and now in seconds otherwise returns -1 */
	double GetElapsedTimeBtwLastTimerPointIndicatorInSeconds() const;
	
	/** Returns converted Timepoint to string data */
	static std::string TimerPointConvertToDate(std::chrono::high_resolution_clock::time_point InTimePoint);

	/** If timer is valid returns time elapsed since timer start in seconds otherwise returns -1. */
	double GetElapsedTimeInSeconds() const;

	/** Returns timer start time. */
	std::chrono::high_resolution_clock::time_point GetStartTime() const;

	/** Returns timer end time. */
	std::chrono::high_resolution_clock::time_point GetEndTime() const;

	/** Returns true if the timer is paused otherwise returns false. */
	bool GetTimerIsPaused() const;

	/** Returns true if the timer is started otherwise returns false. */
	bool GetTimerIsStarted() const;
};

#endif // TIMER_H