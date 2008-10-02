#pragma once

#include "sqlite3.h"
#include "moduleapi.h"
#include <list>
#include <string>

class CEpgDatabaseListener
{
public:
	virtual void FindEventsCallback(void* param,
		const int eventid,
		const int starttime,
		const int duration,
		const char* eventname,
		const char* shortdescr,
		const char* extendeddescr) = 0;
};

class CEpgDatabase
{
public:
	CEpgDatabase();
	~CEpgDatabase();

	int Open(const char* dbFilename);
	void Close();
	void FindEvents(TChlChannel* pChannel, CTime& start, CTime& end,
		CEpgDatabaseListener* listener, void* param);

private:
	typedef std::list<std::string> TableList;

	static int TableList_Callback(void* p, int argc, char** argv, char** col);

	void GetTableList(TableList* list);
	int  CreateEpgTable();

	sqlite3* _db;
	sqlite3_stmt* _findEventsStmt;

};
