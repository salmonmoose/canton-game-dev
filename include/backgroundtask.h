#ifndef BACKGROUNDTASK_H
#define BACKGROUNDTASK_H

#include "boost/thread.hpp"
#include "boost/asio.hpp"

class BackgroundTask
{
public:
	BackgroundTask(int numThreads = 1) : m_work(m_BackgroundTask), m_numThreads(numThreads)
	{
		if(m_numThreads > 0)
		{
			for (int ind = 0; ind < m_numThreads; ++ind)
			{
				m_threads.push_back(
					new boost::thread(
						boost::bind(&boost::asio::io_service::run, &m_BackgroundTask)
					)
				);
			}
		}
	}

	~BackgroundTask()
	{
		m_BackgroundTask.stop();
		for(V_Threads::iterator it = m_threads.begin(); it != m_threads.end(); ++it)
		{
			(*it)->join();
			delete (*it);
		}
	}

	template<typename CompletionHandler>
	void addBackgroundTask(CompletionHandler handler)
	{
		m_BackgroundTask.post(handler);
	}

private:
	boost::asio::io_service m_BackgroundTask;
	boost::asio::io_service::work m_work;
	int m_numThreads;
	typedef std::vector<boost::thread*> V_Threads;
	V_Threads m_threads;
};

#endif