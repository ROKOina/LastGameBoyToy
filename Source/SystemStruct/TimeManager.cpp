#include "TimeManager.h"

//XVˆ—
void TimeManager::Update(const float& elapsedTime)
{
    m_effecttime -= elapsedTime;

    if (m_effecttime < 0.0f)
    {
        m_timescale = 1.0f;
    }

    m_elapsedtime = elapsedTime;
}