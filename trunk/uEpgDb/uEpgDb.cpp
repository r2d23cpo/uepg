// uEpgDb.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "moduleAPI.h"
#include "sqlite3.h"

#include <list>
#include <string>
#include <algorithm>

typedef std::list<std::string> TableList;

static int table_list_cb(void* p, int argc, char** argv, char** col)
{
	TableList* list = (TableList*)p;
	while (argc--)
		list->push_back(std::string(*argv++));
	return 0;
}

sqlite3* open_database(const char* filename)
{
	sqlite3* db = 0;
	char* szError = 0;
	int rc = sqlite3_open(filename, &db);

	TableList list;
	sqlite3_exec(db, "SELECT name FROM sqlite_master WHERE type='table' ORDER BY name;", 
		table_list_cb, &list, NULL);

	if (std::find(list.begin(), list.end(), "epg") == list.end())
	{
		rc = sqlite3_exec(db, 
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

	return db;
}

int main(int argc, char* argv[])
{
	FILE* file;
	TCHL* pChl;
	TEPGInfo epg;
		
	const char* binFilename = "c:\\code\\epg.bin";
	const char* dbFilename  = "c:\\code\\epg.db";

	pChl = new TCHL;
	fopen_s(&file, binFilename, "rb");
	fread(pChl, sizeof(TCHL), 1, file);

	sqlite3* db = open_database(dbFilename);

	int count = 0;
	sqlite3_stmt* stmt;
	sqlite3_exec(db, "begin;", NULL, NULL, NULL);
	int r = sqlite3_prepare(db, "INSERT INTO epg (sat, netid, sid, eventid, starttime, duration, eventname, shortdescr, extendeddescr) \
								VALUES(?, ?, ?, ?, ?, ?, ?, ?, ?); ", -1, &stmt, NULL);

	while (fread(&epg, sizeof(TEPGInfo), 1, file))
	{
		sqlite3_bind_int(stmt,  1, epg.wSat);
		sqlite3_bind_int(stmt,  2, epg.wNetID);
		sqlite3_bind_int(stmt,  3, epg.wSID);
		sqlite3_bind_int(stmt,  4, epg.wEventID);
		sqlite3_bind_int(stmt,  5, epg.dwStartTime);
		sqlite3_bind_int(stmt,  6, epg.dwDuration);
		sqlite3_bind_text(stmt, 7, epg.EventName, -1, SQLITE_STATIC);
		sqlite3_bind_text(stmt, 8, epg.ShortDescr, -1, SQLITE_STATIC);
		sqlite3_bind_text(stmt, 9, epg.ExtendedDescr, -1, SQLITE_STATIC);

		printf("%d: %s\r\n", count++, epg.EventName);
		while (sqlite3_step(stmt) != SQLITE_DONE)
		{
		}

		sqlite3_reset(stmt);
	}
	sqlite3_exec(db, "commit;", NULL, NULL, NULL);

	sqlite3_close(db);
	fclose(file);
	return 0;
}

