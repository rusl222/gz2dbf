#include "dbf.h"
#include <chrono>

using namespace std;
using namespace dbf;

Dbf::Dbf(const char* path)
{
	f = new FILE();
	data = -1;
	header=new Header();
	if (fopen_s(&f, path, "rb+") != 0) {
		err = true;
		return;
	}
	err = !connect(path);
	if (!err)
	{
		size = header->recordsCount;
	}
}

dbf::Dbf::~Dbf()
{
	fclose(f);
}

bool Dbf::connect(const char* path)
{
	size_t size = sizeof(Header);
	if (fread_s(header, size, size, 1, f) != 1) return false;
	if(header->type != 3) return false;

	int recordsNamesLen = header->headerLen - 32 - 1;
	long fieldsCount = recordsNamesLen / sizeof(Field);

	for (int i = 0; i < fieldsCount; i++) 
	{
		Field field;
		fread_s(&field,sizeof(Field) ,sizeof(Field), 1, f);
		fields[string(field.name)]=field;
	}

	char stopChar;
	if(fread_s(&stopChar, 1, 1, 1, f)!=1) return false;
	if (stopChar != 13) return false;

	data = header->headerLen;
	return true;
}

long dbf::Dbf::nextEmpty()
{
	return nextEmpty(0);
}

bool dbf::Dbf::writeTimeModification()
{
	time_t t1 = time(0);
	tm t;
	localtime_s(&t, &t1);
	header->year = t.tm_year - 100;
	header->month = t.tm_mon + 1;
	header->day = t.tm_mday;

	if (fseek(f, 0, 0)!=0)return false;
	return (fwrite(header, sizeof(Header), 1, f) == 1);
}

bool dbf::Dbf::writeMarker(long position)
{
	long pos = data + position * header->recordLen;
	char buf = ' ';

	// "delete"-marker
	if (fseek(f, pos, 0)!=0)return false;
	if (fwrite(&buf, 1, 1, f) != 1) return false;
	pos += header->recordLen;
	if (fseek(f, pos-1, 0)!=0)return false;
	return(fwrite(&buf, 1, 1, f) == 1);
}


long Dbf::nextEmpty(long position)
{
	char buf[1];
	if (position > header->recordsCount - 1) return -1;
	if (position < 0) position = 0;

	unsigned long pos = position;
	for (auto i = pos; i < header->recordsCount; i++)
	{
		if (fseek(f, data + i * header->recordLen, 0)) return -2;
		if (fread_s(buf, 1, 1, 1, f) == 0) return -2;
		if (buf[0] == '*') 
			return i;
	}
	return -1;
}
//check record in dbf: -1 - error
int dbf::Dbf::isEmpty(long position)
{
	long pos = data + position * header->recordLen;
	if(fseek(f, pos, 0)!=0)return-1;

	char buf;
	if(fread_s(&buf, 1, 1, 1, f)!=1) return -1;
	return (buf=='*');
}

string Dbf::getStr(string name, long position)
{
	if (position > header->recordsCount - 1) return "";
	if (position < 0) return "";

	Field fld =fields[name];
	if (!&fld) return "";

	long pos = data + position * header->recordLen + fld.shift;
	fseek(f, pos, 0);

	char ret[0xFF];
	fread_s(ret, fld.length, fld.length, 1, f);
	ret[fld.length] = 0;
	return string(ret);
}


bool Dbf::write(string name, long position, string value)
{
	char buf[0xFF];
	long pos;

	if (position > header->recordsCount) return false;
	if (position < 0) return false;

	Field fld = fields[name];
	if (!&fld) return false;
	int len = fld.length;

	switch (fld.type)
	{
	case 'C':
		writeTimeModification();
		writeMarker(position);
		memcpy(buf, value.c_str(), len); //так чтобы без \0
		pos = data + position * header->recordLen + fld.shift;
		if (fseek(f, pos, 0)!=0)return false;
		return (fwrite(buf, len, 1, f) == 1);
	default:
		return false;
	}
}

bool Dbf::write(string name, long position, float value)
{
	char buf[0xFF];
	memset(buf, ' ', 0xFF);
	buf[254] = 0;
	unsigned long pos;

	if (position > header->recordsCount) return false;
	if (position < 0) return false;

	Field fld = fields[name];
	if (!&fld) return false;
	int len = fld.length;

	switch (fld.type)
	{
	case 'F':
		writeTimeModification();
		writeMarker(position);
		if (isnan(value))
		{
			memcpy(buf, "---", 3); //так чтобы без \0
			pos = data + position * header->recordLen + fld.shift;
			if (fseek(f, pos, 0) != 0)return false;
			return (fwrite(buf, len, 1, f) == 1);
		}
		else
		{
			char  format[10];
			sprintf_s(format, "%% -%d.%df", fld.length, fld.digits);
			sprintf_s(buf, format, value);
			pos = data + position * header->recordLen + fld.shift - 1;
			if (fseek(f, pos, 0) != 0)return false;
			return (fwrite(buf, len, 1, f) == 1);
		}
	default:
		return false;
	}
}

