// dbf.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//


#include "Dbf.h"

using namespace db;

Dbf::Dbf()
{
	record = (char*) malloc(header.recordLen);
}

db::Dbf::Dbf(char* path)
{
	Dbf();
	connect(path);
}


Dbf::~Dbf()
{
	if(f) fclose(f);
}

bool Dbf::connect(const char* path)
{
	if (fopen_s(&f, path, "r+") != 0) return false;

	int x = fread_s(&header, sizeof(header), sizeof(header), 1, f);

	if (header.type != 3) return false;

	int recordsNamesLen = header.headerLen - 32 - 1;
	fieldsCount = recordsNamesLen / sizeof(Field);

	fields = new Field[fieldsCount];

	fread_s(fields, recordsNamesLen, recordsNamesLen, 1, f);

	for (int i = 0; i < fieldsCount;i++) 
		fieldsmap[fields[i].name] = &fields[i];

	char stopChar;
	fread_s(&stopChar, 1, 1, 1, f);

	if (stopChar != 13) return false;

	data = header.headerLen;

	return true;
}

Dbf::Param* Dbf::get(const char* name, long position)
{
	if (position > header.recordsCount - 1) return nullptr;
	if (position <0) return nullptr;

	Param* ret = new Param();
	ret->field = findByName(name);

	if (!ret->field) return nullptr;

	char* value = (char*)malloc(ret->field->length);
	if (!value) return nullptr;
	value[ret->field->length] = 0;

	long pos = data + position * header.recordLen + ret->field->shift;
	fseek(f, pos, 0);

	fread_s(value, ret->field->length, ret->field->length, 1, f);
	ret->value = value;
	ret->fValue=NAN;
	if (ret->field->type == 'F') {
		try
		{
			ret->fValue = atof(ret->value);
		}
		catch (const std::exception&)
		{

		}
	}

	return ret;
}

//template <typename T>
bool Dbf::write(const char* name, long position, const char* value)
{
	if (position > header.recordsCount) return false;
	Param rec;
	rec.field = findByName(name);
	if (!rec.field) return false;
	if (rec.field->type != 'C') return false;
	int len = rec.field->length;

	char buf[0xFF];
	memset(buf, ' ', len);
	buf[len] = 0;

	writeTimeModification();
	writeMarker(position);

	// data
	memcpy(buf, value, len); //так чтобы без \0

	long pos = data + position * header.recordLen - 1 + rec.field->shift;
	if(!fseek(f, pos, 0))return false;;
	return (fwrite(buf, len, 1, f)==1);
}

bool Dbf::write(const char* name, long position, float value)
{
	if (position > header.recordsCount) return false;
	Param rec;
	rec.field = findByName(name);
	if (!rec.field) return false;
	if (rec.field->type != 'F') return false;
	int len = rec.field->length;

	writeTimeModification();
	writeMarker(position);

	long pos = data + position * header.recordLen - 1;
	char buf[0xFF];
	memset(buf, ' ', len);
	buf[len] = 0;

	char  format[10];
	sprintf_s(format,"%%f-%d.%d",rec.field->length,rec.field->digits);
	sprintf_s(buf,format, value);

	pos =data + position * header.recordLen - 1 + rec.field->shift;
	if(!fseek(f, pos, 0))return false;;
	return (fwrite(buf, len, 1, f)==1);
}

bool db::Dbf::writeTimeModification()
{
	// date modification
	time_t t1 = time(0);
	tm* t = localtime(&t1);
	header.year = t->tm_year - 100;
	header.month = t->tm_mon + 1;
	header.day = t->tm_mday;

	if(!fseek(f, 0, 0))return false;
	return (fwrite(&header, sizeof(Header), 1, f)==1);
}

bool db::Dbf::writeMarker(long position)
{
	long pos = data + position * header.recordLen - 1;
	char buf = ' ';

	// "delete"-marker
	if(!fseek(f, pos, 0))return false;
	if(fwrite(&buf, 1, 1, f)!=1) return false;
	if(!fseek(f, pos + header.recordLen - 1, 0))return false;
	return(fwrite(&buf, 1, 1, f)==1);
}

char* db::Dbf::select(long position)
{
	if (position > header.recordsCount - 1) return nullptr;
	if (position <0 ) return nullptr;

	long offset = data + header.recordLen * position;
	if (!fseek(f, offset, 0)) return nullptr;

	if (fread_s(record, header.recordLen, header.recordLen, 1, f) == 0) return nullptr;
	return record;
}

long db::Dbf::nextEmpty(long position)
{
	char buf[1];
	if (position > header.recordsCount - 1) return -1;
	if (position < 0) position=0;
	for (long i = position; i < header.recordsCount; i++)
	{
		if(fseek(f, data + i * header.recordLen,0)) return -2;
		if(fread_s(buf, 1, 1, 1, f)==0) return -2;
		if (buf[0] == '*') return i;
	}
	return -1;
}


Dbf::Field* Dbf::findByName(const char* name)
{
	for (int i = 0; i < fieldsCount; i++)
		if (strcmp(name, fields[i].name)==0) return &fields[i];
	return nullptr;
}


