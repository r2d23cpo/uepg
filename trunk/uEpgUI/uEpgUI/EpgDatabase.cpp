#include "StdAfx.h"
#include "EpgDatabase.h"

CEpgDatabase::CEpgDatabase()
{
	_db = 0;
	_findEventsStmt = 0;
}

CEpgDatabase::~CEpgDatabase()
{
}

int CEpgDatabase::TableList_Callback(void* p, int argc, char** argv, char** col)
{
	TableList* list = (TableList*)p;

	while (argc--)
		list->push_back(std::string(*argv++));

	return 0;
}

void CEpgDatabase::GetTableList(TableList* list)
{
	sqlite3_exec(_db, "SELECT name FROM sqlite_master WHERE type='table' ORDER BY name;", 
		TableList_Callback, list, NULL);
}

int  CEpgDatabase::CreateEpgTable()
{
	return sqlite3_exec(_db, 
		"CREATE TABLE epg \
		( \
			id INTEGER PRIMARY KEY AUTOINCREMENT, \
			sat INTEGER, \
			netid INTEGER, \
			sid INTEGER, \
			eventid INTEGER, \
			starttime INTEGER, \
			duration INTEGER, \
			eventname TEXT, \
			shortdescr TEXT, \
			extendeddescr TEXT \
		);", 
		NULL, NULL, NULL);
}

int CEpgDatabase::Open(const char* dbFilename)
{
	if (sqlite3_open(dbFilename, &_db))
		return -1;

	TableList list;
	GetTableList(&list);

	if (std::find(list.begin(), list.end(), "epg") == list.end())
		if (CreateEpgTable())
			return -1;

	sqlite3_prepare(_db, "SELECT eventid, starttime, duration, eventname, shortdescr, extendeddescr \
								FROM epg WHERE sat=? AND sid=? AND startime>=? AND starttime<=?", -1, &_findEventsStmt, NULL);

	return 0;
}

void CEpgDatabase::Close()
{
	sqlite3_finalize(_findEventsStmt);

	if (_db)
		sqlite3_close(_db);
}

void CEpgDatabase::FindEvents(TChlChannel* pChannel, CTime& start, CTime& end, 
							  CEpgDatabaseListener* listener, void* param)
{
	sqlite3_reset(_findEventsStmt);

	sqlite3_bind_int(_findEventsStmt, 1, pChannel->wSatPos);
	sqlite3_bind_int(_findEventsStmt, 2, pChannel->wSID);
	sqlite3_bind_int(_findEventsStmt, 3, (int)start.GetTime());
	sqlite3_bind_int(_findEventsStmt, 4, (int)end.GetTime());

	int rc = 0;
	while ((rc = sqlite3_step(_findEventsStmt)) != SQLITE_DONE)
	{
		if (rc == SQLITE_ROW)
		{
			// eventid, starttime, duration, eventname, shortdescr, extendeddescr

			int         eventid       = sqlite3_column_int(_findEventsStmt, 0);
			int         starttime     = sqlite3_column_int(_findEventsStmt, 1);
			int         duration      = sqlite3_column_int(_findEventsStmt, 2);
			const char* eventname     = (const char*)sqlite3_column_text(_findEventsStmt, 3);
			const char* shortdescr    = (const char*)sqlite3_column_text(_findEventsStmt, 4);
			const char* extendeddescr = (const char*)sqlite3_column_text(_findEventsStmt, 5);

			listener->FindEventsCallback(param, eventid, starttime, duration, eventname, shortdescr, extendeddescr);
		}
	}
}