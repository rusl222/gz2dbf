#pragma once

#include <fstream>
#include <list>
#include <Datetime.h>
//#include <filesystem>


//using namespace std;

namespace gz {

#pragma pack(1)

	struct Header1 {
		char rez[32];
	};

	struct Arch_qry_desc_         /* request descriptor !!! no change consistence, send via MMB */ //16-bytes
	{
		unsigned char  ver;			/* qry version */
		unsigned char  uso;			/* P: see "enum Uso_Types" */
		unsigned char  arch_type;   /* P: archive type, uso dependent char enum, example: sevc,gvc" */
		unsigned char  dir;         /* P: direction */
		unsigned char  run;         /* P: flow pipe; device (no-run) archives required 0 here */
		unsigned char  src;			/* source (see "enum USO_ARCH_QRY_SRC") */
		unsigned short dev;         /* P: device number */
		unsigned long  begin_time;  /* archive begin time */
		unsigned long  end_time;    /* archive end time */
	};	/* sizeof struct 16 bytes */


	struct ZOND_VERSION             /* Executable file vertion Версия выполняемого файла zondXYXX.exe */
	{
		unsigned short  MainVersion : 8;   /* High digit Старшая цифра версии */
		unsigned short  MajorVersion : 4;   /* Middle digitСредняя цифра версии */
		unsigned short  MinorVersion : 4;   /* Low digit Младшая цифра версии */
		short        BuildVersion;   /* Exe-file vertionВерсия EXE-файла     */
	};

	struct Arch_header           /* archive file header */ //104-bytes
	{
		unsigned char	  ver;	      /* header version */
		unsigned char	  rez0;	      /* rezerved 0 */
		char zond_name[32]; /* zond name "conf.ZondName" */
		char source_name[32]; /* flow run name, form by uso, DOS code */
		struct Arch_qry_desc_ qry;
		unsigned char	  dev_type;   /* device type / uso dependent */
		unsigned char	  arch_ver;   /* archive version / uso dependent */
		unsigned long	  arch_time;  /* creation archive date & time (KP CurrentTime) */
		struct ZOND_VERSION   zbver;      // zond build version
		unsigned long	  dev_time;   /* device time */
		unsigned char	  rez1[8];    /* reserved 1 */
	};


	struct DataHeader {
		char rez[18];
		char blockSize;
		char type;
		float co2;
		float n2;
		float ro;
	};

	struct Record {
		Datetimestruct date;
		char rez;
		unsigned long Vr;
		unsigned long Vc;
		unsigned long Vvost_r;
		unsigned long Vvost_c;
		unsigned long Vsum_r;
		unsigned long Vsum_c;
		unsigned long Vr_rev;
		unsigned long Vc_rev;
		unsigned long Vvost_r_rev;
		unsigned long Vvost_c_rev;
		unsigned long Vsum_r_rev;
		unsigned long Vsum_c_rev;
		float Ksg;
		float Psr;
		float Tsr;
		float Kper;
		char rez2[64];
		long long Vr_total;
		long long Vr_total_r;
		long long Vc_total;
		long long Vc_total_r;
		long long Savg;
		char rez3[12];

		template <typename T>
		T bswap(T val);
		void byteSwap();
	};

#pragma pack()

	class Agz
	{
	public:
		Agz();
		~Agz();

		Header1 head1;
		Arch_header ahead;
		DataHeader hData;

		bool open(const char* p);
		bool readData(const char* p);

		Record* getRecord(Datetime& slice);
		list<Record*> getRecordsAfter(Datetime& slice);

		list<Record*> data;

	private:
		FILE* f;
		FILE* tmp;
	};

}