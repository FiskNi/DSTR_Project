#ifndef _TIMED_CALLBACK_H
#define _TIMED_CALLBACK_H
#include <Pch/Pch.h>

class TimedCallback {
public:
	TimedCallback();
	~TimedCallback();

	void Update(const float& dt);
	void registerCallback(std::function<void()> BulletCallback);
	void forceExecute();
	void restart();
	void restartIntervalTimer();
	void start();
	void stop();
	const bool& isDone() const;
	void setTotalExecutionTime(float totalExecutionTime);
	void setExecutionInterval(float executionInterval);
	void setInfinityExecutionTime(bool condition);
	
	const float getTimeLeftOnInterval() const;
	const float& getTimeLeft() const;

private:
	std::function<void()> m_callbackFunc;

	bool m_runInfinity;
	bool m_hasCallbackRegistered;
	bool m_hasStarted;
	bool m_doneExecuting;

	float m_initialTotalTime;
	float m_totalTimeLeft;
	float m_interval;
	float m_intervalCounter;

};



#endif
