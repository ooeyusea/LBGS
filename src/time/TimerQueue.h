#ifndef TIMEQUEUE_H
#define TIMEQUEUE_H

#include "util/singleton.h"

#define BASE_TIMER_INTERVAL 50 //
#define JIFFIES_INTERVAL   100 //
typedef unsigned long long jiffies_t; 

struct timer_list;
struct timer_base
{
	jiffies_t    expire_;
	jiffies_t    live_;
	timer_base*  pre_;
	timer_base*  next_;
	timer_list*  list_;

	timer_base():expire_(0),live_(0),pre_(0),next_(0),list_(0)
	{}
	virtual ~timer_base() {}

	virtual bool OnTimer() = 0;

	jiffies_t Jiffies(){ return expire_; }
};

template <typename F>
class custom_timer : public timer_base
{
public:
	custom_timer(F& f) : _f(f) {}
	virtual ~custom_timer() {}

	virtual bool OnTimer()
	{
		return _f();
	}

private:
	F _f;
};

class lua_timer : public timer_base
{
public:
	lua_timer(int luaFunction) : _luaFunction(luaFunction) {}
	virtual ~lua_timer();

	virtual bool OnTimer();

private:
	int _luaFunction;
};

struct timer_list
{
public:
	timer_list():head_(0),tail_(0)
	{}

	void push_back(timer_base* p)
	{
		p->list_ = this;
		p->next_ = 0;
		p->pre_  = tail_;
		if ( tail_ )
			 tail_->next_ = p;
		else
			head_ = p;

		tail_ = p;
	}

	timer_base* pop_front()
	{
		timer_base* p =  head_;
		if ( head_ )
		{
			head_ = head_->next_;
			if (!head_)
				tail_ = 0;
			else
				head_->pre_ = 0;

			p->next_ = p->pre_ = 0;
		}
		return p;
	}

	void remove(timer_base* p)
	{
		if (p->pre_)
			p->pre_->next_ = p->next_;

		if (p->next_)
			p->next_->pre_ = p->pre_;

		if (p==head_)
			head_=head_->next_;

		if (p==tail_)
			tail_ = tail_->pre_;

		p->next_ = p->pre_ = 0;
	}


	bool empty()
	{
		return head_ == 0;
	}


	timer_base* head_;
	timer_base* tail_;
private:
	timer_list(timer_list&);
	timer_list& operator=(timer_list &);
};


class  TimerQueue;
struct TimerGear
{
	TimerGear(int maxMoveDst,TimerGear* nextGear )
		: m_timerVec(0)
		, m_nextGear(nextGear)
		, m_curMoveDst(0)
		, m_maxMoveDst(maxMoveDst)
	{
		if(!m_timerVec)
		{
			m_timerVec = new timer_list[m_maxMoveDst];
		}
	}

	~TimerGear()
	{
		if (m_timerVec)
		{
			delete[] m_timerVec;
			m_timerVec = 0;
		}
	}

	void CheckHighGear();
	void Update();
	void UpdateToLowGear();

	timer_list* m_timerVec;//
	TimerGear*  m_nextGear;//
	int m_curMoveDst;//
	int m_maxMoveDst;//

private:
	TimerGear(TimerGear&);
	TimerGear& operator=(TimerGear&);
};

class TimerQueue:public util::Singleton<TimerQueue>
{
public:
	enum
	{
		TQ_TVN_BITS = 6,
		TQ_TVR_BITS = 8,
		TQ_TVN_SIZE = 1<<TQ_TVN_BITS,//64
		TQ_TVR_SIZE = 1<<TQ_TVR_BITS,//256
		TQ_TVN_MASK = TQ_TVN_SIZE -1,//63
		TQ_TVR_MASK = TQ_TVR_SIZE -1,//255
	};

public:
	TimerQueue():m_timer_click(0),m_timer_jiffies(0),m_tv1(0),m_tv2(0),m_tv3(0),m_tv4(0),m_tv5(0)
	{
		m_tv5 = new TimerGear(TQ_TVN_SIZE,0);
		m_tv4 = new TimerGear(TQ_TVN_SIZE,m_tv5);
		m_tv3 = new TimerGear(TQ_TVN_SIZE,m_tv4);
		m_tv2 = new TimerGear(TQ_TVN_SIZE,m_tv3);
		m_tv1 = new TimerGear(TQ_TVR_SIZE,m_tv2);
	}

	~TimerQueue()
	{
		if(m_tv5)
		{
			delete m_tv5;
			m_tv5 = 0;
		}

		if(m_tv4)
		{
			delete m_tv4;
			m_tv4 = 0;
		}

		if(m_tv3)
		{
			delete m_tv3;
			m_tv3 = 0;
		}
		
		if(m_tv2)
		{
			delete m_tv2;
			m_tv2 = 0;
		}

		if(m_tv1)
		{
			delete m_tv1;
			m_tv1 = 0;
		}
	}

	template <typename F>
	void CreateTimer(F f,long msseconds)
	{
		if (msseconds < 100)
			msseconds = 100;

		custom_timer<F> * pTimer = new custom_timer<F>(f);
		pTimer->live_   = msseconds/JIFFIES_INTERVAL;
		pTimer->expire_ = Jiffies() + pTimer->live_;
		schedule(pTimer);
	}

	void CreateLuaTimer(int handler, long msseconds)
	{
		if (msseconds < 100)
			msseconds = 100;

		lua_timer * pTimer = new lua_timer(handler);
		pTimer->live_   = msseconds/JIFFIES_INTERVAL;
		pTimer->expire_ = Jiffies() + pTimer->live_;
		schedule(pTimer);
	}

	void ReCreateTimer(timer_base* pTimer)
	{
		if(!pTimer)
			return;

		pTimer->list_->remove(pTimer);
		pTimer->expire_ = Jiffies() + pTimer->live_;
		schedule(pTimer);
	}

	void Update()
	{
		m_timer_click++;
		if ( m_timer_click == 2 )
		{
			OnJiffiesUpdate();
			m_timer_click = 0;
		}
	}

	timer_list* schedule(timer_base* timer)
	{
		jiffies_t expireJiffies = timer->Jiffies();
		jiffies_t liveJiffies   = expireJiffies - m_timer_jiffies;
		timer_list* vec = 0;
		if ( liveJiffies < TQ_TVR_SIZE )
		{
			vec = m_tv1->m_timerVec + (expireJiffies&TQ_TVR_MASK);
		}
		else if( liveJiffies < (1<<(TQ_TVR_BITS+TQ_TVN_BITS)) )
		{
			vec = m_tv2->m_timerVec + (
				(expireJiffies>>TQ_TVR_BITS)&TQ_TVN_MASK);
		}
		else if( liveJiffies < (1<<(TQ_TVR_BITS+2*TQ_TVN_BITS)) )
		{
			vec = m_tv3->m_timerVec + (
				(expireJiffies>>(TQ_TVR_BITS + TQ_TVN_BITS) )&TQ_TVN_MASK);
		}
		else if( liveJiffies < (1<<(TQ_TVR_BITS+3*TQ_TVN_BITS)) )
		{
			vec = m_tv4->m_timerVec + (
				(expireJiffies>>(TQ_TVR_BITS + 2*TQ_TVN_BITS))&TQ_TVN_MASK);
		}
		else if( (long long)liveJiffies < 0 )
		{
			vec = m_tv1->m_timerVec + (m_timer_jiffies&TQ_TVR_MASK);
		}
		else
		{
			vec = m_tv5->m_timerVec + (
				(expireJiffies>>(TQ_TVR_BITS + 3*TQ_TVN_BITS))&TQ_TVN_MASK);
		}

		if ( vec )
			 vec->push_back(timer);

		return vec;
	}

	void OnJiffiesUpdate()
	{
		if(m_tv1){
			m_tv1->CheckHighGear();
		}
		m_timer_jiffies++;
		if(m_tv1){
		   m_tv1->Update();
		}
	}

	jiffies_t Jiffies(){return m_timer_jiffies;}

private:
	int           m_timer_click;
	jiffies_t     m_timer_jiffies;
	TimerGear*    m_tv1;
	TimerGear*    m_tv2;
	TimerGear*    m_tv3;
	TimerGear*    m_tv4;
	TimerGear*    m_tv5;
};


#endif
