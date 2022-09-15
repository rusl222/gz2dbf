#pragma once

#include <ctime>
#include <string>

using namespace std;

#pragma pack(1)
	struct Datetimestruct{
		char day;
		char month;
		unsigned short year;
		char hour;
		char minit;
		char second;
	};
#pragma pack()

class Datetime
{
public:
	Datetime();
	Datetime(tm in);
	Datetime(time_t ctime);
	Datetime(Datetimestruct in);
	Datetime(const string timestamp); // "dd.mm.yyyy hh:mm:ss"
	~Datetime();

	bool shift(long second);

	Datetimestruct dtStruct();
	string timestamp();

	time_t ctime();
private:
	tm t;
};

bool operator == (Datetime t1, Datetime t2);
bool operator > (Datetime t1, Datetime t2);

