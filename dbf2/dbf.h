#pragma once


#include <fstream>
#include <map>
#include <list>
#include <string>


namespace dbf {

	class Dbf
	{
	public:
		Dbf(const char* path);
		~Dbf();
		bool connect(const char* path);
		long nextEmpty();
		long nextEmpty(long position);
		int isEmpty(long position);
		long size = 0;

		std::string getStr(std::string name, long position);

		bool write(std::string name, long position, std::string value);
		bool write(std::string name, long position, float value);

		int err;

	private:
		FILE* f;

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

		Header *header;
		std::map <std::string,Field> fields;
		//long fieldsCount;
		long data=-1;

		bool writeTimeModification();
		bool writeMarker(long pos);
	};

}