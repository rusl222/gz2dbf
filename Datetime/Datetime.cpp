#include "Datetime.h"

using namespace std;

Datetime::Datetime()
{
	time_t now = time(0);
	localtime_s(&t, &now);
}

Datetime::Datetime(tm in)
{
	t = in;
}

Datetime::Datetime(time_t ctime)
{
	gmtime_s(&t,&ctime);
}

Datetime::Datetime(Datetimestruct in)
{
	t.tm_mday = in.day;
	t.tm_mon = in.month-1;
	t.tm_year = in.year-1900;
	t.tm_hour = in.hour;
	t.tm_min = in.minit;
	t.tm_sec = in.second;
}
bool Datetime::shift(long second) 
{
	time_t t1 = mktime(&t)+second;
	localtime_s(&t, &t1);
	return true;
}
Datetime::Datetime(const string str)
{
	char buf[20] = { 0 };

	strncpy_s(buf, &str[0], 2);
	t.tm_mday = atoi(buf);
	strncpy_s(buf, &str[3], 2);
	t.tm_mon = atoi(buf)-1;
	strncpy_s(buf, &str[6], 4);
	t.tm_year = atoi(buf)-1900;
	strncpy_s(buf, &str[11], 2);
	t.tm_hour = atoi(buf);
	strncpy_s(buf, &str[14], 2);
	t.tm_min = atoi(buf);
	strncpy_s(buf, &str[17], 2);
	t.tm_sec = atoi(buf);
}


Datetime::~Datetime()
{
}

Datetimestruct Datetime::dtStruct()
{
	Datetimestruct ret = Datetimestruct();
	ret.day = t.tm_mday;
	ret.month = t.tm_mon-1;
	ret.year = t.tm_year + 1900;
	ret.hour = t.tm_hour;
	ret.minit = t.tm_min;
	ret.second = t.tm_sec;
	return (ret);
}

string Datetime::timestamp()
{
	char buf[20];
	sprintf_s(buf, "%02d.%02d.%4d %02d:%02d:%02d", 
		t.tm_mday, t.tm_mon+1, t.tm_year+1900,
		t.tm_hour, t.tm_min, t.tm_sec);
	buf[19] = 0;

	return buf;
}

time_t Datetime::ctime() 
{
	return mktime(&t);
}

bool operator == (Datetime t1, Datetime t2)
{
	return  t1.ctime() == t2.ctime();
}
bool operator > (Datetime t1, Datetime t2)
{
	return  t1.ctime() > t2.ctime();
}
