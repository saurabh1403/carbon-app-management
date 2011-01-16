

#include "SessionMgr.h"

/*
template <class T>
SessionMgr<T>::SessionMgr():activeSession("")
{
}

template <class T>
SessionMgr<T>::~SessionMgr()
{
	_sessions.clear();
}

template <class T>
SessionMgr<T> & SessionMgr<T>::getInstance(void)
{
	static SessionMgr<T> instance;
	return instance;
}


template <class T>
T * SessionMgr<T>::getActiveSession(void)
{
	if(activeSession.empty())
		return NULL;

	else
		return getSessionWithId(activeSession);

}

template <class T>
T *SessionMgr<T>::getSessionWithId(const std::string &sessionId)
{
	T *session = (sessionId.empty()) ? _sessions.find(activeSession) : _sessions.find(sessionId);

	return (session == _sessions.end())? NULL : session;
}

template <class T>
void SessionMgr<T>::removeAllSession()
{
	_sessions.clear();
}

template <class T>
bool SessionMgr<T>::createSession(std::string sessionId, const T* sessionPtr)
{

	if(_sessions.find(sessionId) != _sessions.end())
	{
		_sessions.insert(sessionPtr);
		return true;
	}

	else
		return false;
}
*/