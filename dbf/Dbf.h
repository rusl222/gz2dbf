#pragma once

#include <fstream>
#include <ctime>
#include <map>

namespace db {

	class Dbf
	{
	public:



	struct Header {
		char type; //type of dbase 3 - basic table
		char year;
		char month;
		char day;
		unsigned int recordsCount;
		unsigned short headerLen;
		unsigned short recordLen;
		short rezerv;
		char transactionEnd;
		char isCrypt;
		char multyUser[12];
		char rezerv2;
		char codePage;
		short rezerv3;
	};

	struct Field {
		char name[11];
		char type;
		unsigned int shift;
		char length;
		char digits;
		char rez[14];
	};

	struct Param {
		Field* field;
		char* value;
		float fValue;
		bool isDeleted;
	};

	//------------------------------
	public:
		Dbf();
		Dbf(char *path);
		~Dbf();

		bool connect(const char* path);
		Param* get(const char* name, long position);

		//template <typename T>
		bool write(const char* name, long position,const char* value);
		bool write(const char* name, long position, float value);
		//bool write(const char* name, long position,const char* value, int len);

		char* select(long position);
		long nextEmpty(long position = 0);

		Header header;
		Field* fields;
		int fieldsCount;
		long data;

		std::map<char*, Field*> fieldsmap;

	protected:
		FILE* f;
		Field* findByName(const char* name);
		bool writeTimeModification();
		bool writeMarker(long pos);
		char* record;

	};
}

