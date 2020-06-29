#include "Pch/Pch.h"
#include "TimedCallback.h"

TimedCallback::TimedCallback()
{
	m_hasCallbackRegistered = false;
}

TimedCallback::~TimedCallback()
{
}

void TimedCallback::Update(const float& dt)
{

	if (m_hasStarted == false) return;

	if (m_runInfinity) {

		m_intervalCounter += dt;

		if (m_intervalCounter >= m_interval) {
			m_intervalCounter = 0.0f;
			m_callbackFunc();
		}
	}
	else {
		if (m_totalTimeLeft > 0.0f) {
			m_totalTimeLeft -= dt;
			
			m_intervalCounter += dt;

			if (m_intervalCounter >= m_interval) {
				m_intervalCounter = 0.0f;
				m_callbackFunc();
			}
		}
		else {
			m_doneExecuting = true;
		}


	}

}

void TimedCallback::registerCallback(std::function<void()> BulletCallback)
{
	m_callbackFunc = BulletCallback;
	m_hasCallbackRegistered = true;
}

void TimedCallback::forceExecute()
{
	m_callbackFunc();
}

// Only really needed if you are not running an infinity loop
void TimedCallback::restart()
{
	m_totalTimeLeft = m_initialTotalTime;
	m_intervalCounter = 0.0f;
	m_doneExecuting = false;
}

void TimedCallback::restartIntervalTimer()
{
	m_intervalCounter = 0.0f;
}

void TimedCallback::start()
{
	m_hasStarted = true;
}

void TimedCallback::stop()
{
	m_hasStarted = false;
}

const bool& TimedCallback::isDone() const
{
	return m_doneExecuting;
}

void TimedCallback::setTotalExecutionTime(float totalExecutionTime)
{
	m_initialTotalTime = totalExecutionTime;
	m_totalTimeLeft = totalExecutionTime;
}

void TimedCallback::setExecutionInterval(float executionInterval)
{
	m_interval = executionInterval;
}

void TimedCallback::setInfinityExecutionTime(bool condition)
{
	m_runInfinity = condition;
}

const float TimedCallback::getTimeLeftOnInterval() const
{
	return m_interval - m_intervalCounter;
}

const float& TimedCallback::getTimeLeft() const
{
	return m_totalTimeLeft;
}
