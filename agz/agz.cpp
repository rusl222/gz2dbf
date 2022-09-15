#include "Agz.h"

using namespace gz;

Agz::Agz()
{	
}


Agz::~Agz()
{
	fclose(f);
}


bool Agz::open(const char* p)
{
	fopen_s(&f,p, "rb");
	if (!f)return false;

	if (!fread_s(&head1, sizeof(Header1), sizeof(Header1), 1, f)) 
	{
		fclose(f);
		return false;
	}
	if (!fread_s(&ahead, sizeof(Arch_header), sizeof(Arch_header), 1, f))
	{
		fclose(f);
		return false;
	}

	fclose(f);
	return true;
}

bool Agz::readData(const char* p)
{
	fopen_s(&tmp,p, "rb");
	if (!tmp)return false;

	if (!fread_s(&hData, sizeof(DataHeader), sizeof(DataHeader), 1, tmp))
	{
		fclose(tmp);
		return false;
	}

	switch (ahead.qry.arch_type)
	{
	case 'H':
	case 'D':
		fseek(tmp, sizeof(DataHeader), 0);
		while (!feof(tmp))
		{
			Record *rec = new Record();
			long size = sizeof(Record);
			if (fread_s(rec, size, size, 1, tmp) == 1)
			{
				rec->byteSwap();
				data.push_back(rec);
			}
		}
		break;
	default:
		break;
	}

	fclose(tmp);
	return true;
}

Record *Agz::getRecord(Datetime &slice)
{
	for (Record *rec : data)
		if (rec->date == slice) return rec;
	return nullptr;
}

list<Record*> gz::Agz::getRecordsAfter(Datetime& slice)
{
	list<Record*> ret;
	for (Record* rec : data)
		if (rec->date > slice) ret.push_back(rec);
	return ret;
}

template <typename T>
T Record::bswap(T val) {
	T retVal;
	char* pVal = (char*)& val;
	char* pRetVal = (char*)& retVal;
	int size = sizeof(T);
	for (int i = 0; i < size; i++)
		pRetVal[size - 1 - i] = pVal[i];
	return retVal;
}

void Record::byteSwap() {
	date.year = bswap(date.year);
	Vr = bswap(Vr);
	Vc = bswap(Vc);
	Vvost_r = bswap(Vvost_r);
	Vvost_c = bswap(Vvost_c);
	Vsum_r = bswap(Vsum_r);
	Vsum_c = bswap(Vsum_c);
	Vr_rev = bswap(Vr_rev);
	Vc_rev = bswap(Vc_rev);
	Vvost_r_rev = bswap(Vvost_r_rev);
	Vvost_c_rev = bswap(Vvost_c_rev);
	Vsum_r_rev = bswap(Vsum_r_rev);
	Vsum_c_rev = bswap(Vsum_c_rev);
	Ksg = bswap(Ksg);
	Psr = bswap(Psr);
	Tsr = bswap(Tsr);
	Kper = bswap(Kper);
	Vr_total = bswap(Vr_total);
	Vr_total_r = bswap(Vr_total_r);
	Vc_total = bswap(Vc_total);
	Vc_total_r = bswap(Vc_total_r);
	Savg = bswap(Savg);
}
