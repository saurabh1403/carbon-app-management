
#pragma once

#include "Constants.h"
//#include "Log.h"

#include <list>
#include <map>

template <class T>
class SessionMgr
{

private:
	SessionMgr();
	~SessionMgr();

	std::map<std::string, T*> _sessions;

	std::string activeSession;

public:

	//returns the last added session. not of use. instead use the function getSessionWithId with empty string as argument for getting active session
	T *getActiveSession(void);

	//if session id is empty, it returns the active session
	T *getSessionWithId(const std::string &sessId = "");

	static SessionMgr<T> &getInstance(void);
	void removeAllSession();
	bool createSession(std::string, T*);

	//_TODO: it will set the session as active one for the given id
	bool setSessionAsActive(std::string);

};


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
	std::map<std::string, T*>::const_iterator itr = (sessionId.empty()) ? _sessions.find(activeSession) : _sessions.find(sessionId);

	return (itr == _sessions.end())? NULL : itr->second;
}

template <class T>
void SessionMgr<T>::removeAllSession()
{
	_sessions.clear();
}

template <class T>
bool SessionMgr<T>::createSession(std::string sessionId, T* sessionPtr)
{
	_sessions[sessionId] = sessionPtr;
	activeSession = sessionId;

	return true;
}
