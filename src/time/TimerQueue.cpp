#include "TimerQueue.h"
#include <list>
#include "lua/LuaEngine.h"

lua_timer::~lua_timer()
{
	if (_luaFunction > 0)
		lua_extend::LuaEngine::Instance()->removeScriptFunction(_luaFunction);
}

bool lua_timer::OnTimer()
{
	if (_luaFunction > 0)
		return lua_extend::LuaEngine::Instance()->executeScriptFunction(_luaFunction);

	return false;
}

void TimerGear::CheckHighGear()
{
	if( m_curMoveDst >= m_maxMoveDst )
		m_curMoveDst = 0;

	if (m_curMoveDst == 0)
	{
		if(m_nextGear){
			m_nextGear->UpdateToLowGear();
		}
	}
}

void TimerGear::Update()
{
	if( m_curMoveDst >= m_maxMoveDst )
		m_curMoveDst = 0;

	timer_list* pCurTimerList = &m_timerVec[m_curMoveDst];
	if(!pCurTimerList)
	{
		m_curMoveDst++;
		return;
	}

	/*
	if(pCurTimerList->empty())
	{
		m_curMoveDst++;
		return;
	}
	*/

	while(!pCurTimerList->empty())
	{
		timer_base* pCurTimer = pCurTimerList->pop_front();
		if(!pCurTimer)
			continue; 

		bool recreate = pCurTimer->OnTimer();
		//DEBUGINFO("Jiffies:%llu/%llu", TimerQueue::Instance()->Jiffies() - 1, pCurTimer->Jiffies());
		if (recreate)
			TimerQueue::Instance()->ReCreateTimer(pCurTimer);
		else
		{
			delete pCurTimer;
		}
	}

	m_curMoveDst++;
	if(m_curMoveDst == m_maxMoveDst )
	{
		m_curMoveDst = 0;
	}
}

void TimerGear::UpdateToLowGear()
{
	if( m_curMoveDst >= m_maxMoveDst )
		m_curMoveDst = 0;

	if (m_curMoveDst == 0)
	{
		if(m_nextGear){
			m_nextGear->UpdateToLowGear();
		}
	}

	timer_list* pCurTimerList = &m_timerVec[m_curMoveDst];
	if(!pCurTimerList)
	{
		m_curMoveDst++;
		return;
	}

	std::list<timer_base*> tmpTimerList;
	while(!pCurTimerList->empty())
	{
		timer_base* pCurTimer = pCurTimerList->pop_front();
		if(!pCurTimer)
			continue;

		tmpTimerList.push_back(pCurTimer);
	}

	std::list<timer_base*>::iterator itr = tmpTimerList.begin();
	for (; itr != tmpTimerList.end(); ++itr)
	{
		TimerQueue::Instance()->schedule(*itr);
	}

	m_curMoveDst++;
}
