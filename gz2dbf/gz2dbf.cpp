
#include <dbf.h> 
#include <Agz.h>
#include <filesystem>
namespace win {
#include <Windows.h>
#include <tchar.h>
}
//using namespace std;
using namespace filesystem;

long calcPosition(Datetime &base, Datetime &t, char archType);
int proc(const char* pathAgz, const char* pathBin, const char* pathDbf);
bool runAndWaitProc(string command);

 
///---------------------main------------------------///
int main(int argc, char* args[])
{

	path pathGzFolder(L"C:\\Users\\RGaleev\\Downloads\\eitp\\USO_ARCH.U1\\MODBUS_M\\1\\1\\1");
	path pathDbFolder(L"C:\\Users\\RGaleev\\Downloads\\eitp\\arc\\TUR_SHK2\\modbus_m\\1\\1\\1");

	// check hours data
	for (path it : filesystem::directory_iterator(path(pathGzFolder/L"H")))
	{
		//path p = it.path();
		printf("%s\n", it.string().c_str());
		string args;
		path bin;
		
		if (it.extension() == L".hz")
		{
			bin = it.parent_path()/(it.filename().wstring() + L".bin");
			string command = "C:\\Program Files (x86)\\Scada_Pro\\Zond2006\\DButil\\ungz.exe " + it.string() +" " + bin.string();
			if (runAndWaitProc(command))
			{
				proc(it.string().c_str(), bin.string().c_str(), (pathDbFolder / "H" / "h2207_06.dbf").string().c_str());
				filesystem::remove(bin);
				path newp = it.string()+ ".mz";
				try
				{
					rename(it, newp);
				}
				catch (const std::exception& e)
				{
					printf(e.what());
				}
				
					
			}

		}
		else
		{
printf("ignore\n");
		}
		printf("---\n");
	}

}




///--------------process-----------------------------///
int proc(const char* pathAgz, const char* pathBin, const char* pathDbf)
{

	gz::Agz* agz = new gz::Agz();
	if (!agz->open(pathAgz))
	{
		printf("error: open file .hz path: %s\n",pathAgz);
		return 1;
	}
	if (!agz->readData(pathBin))
	{
		printf("error: open file .bin path: %s\n",pathBin);
		return 1;
	}

	dbf::Dbf* adb = new dbf::Dbf(pathDbf);
	if (adb->err)
	{
		printf("error: open/read file .dbf path: %s\n",pathDbf);
		return 1;
	}

	if (agz->ahead.qry.uso != 6) 
	{
		printf("uso #%d no support\n",agz->ahead.qry.uso);
	}
	
	char bdName[255];
	Datetime base("01.01.2022 12:00:00");
	bool rez=false;

	for (gz::Record* rec : agz->data)
	{
		string type = "";
		Datetime t2(rec->date);
		long position = calcPosition(base,t2, agz->ahead.qry.arch_type);
		if (position <0 || position > adb->size - 1) continue;
		switch (agz->ahead.qry.arch_type)
		{
		case 'H':
			sprintf_s(bdName, "h%02d%02d_%02d", (rec->date.year - 2000), rec->date.month, agz->ahead.dev_type);
			printf("%s",bdName);
			type = "hours";
			if (adb->isEmpty(position) == 0)continue;
			rez=adb->write("TIME_STAMP", position, t2.timestamp());

			break;
		case 'D':
			sprintf_s(bdName, "d%02d01_%02d", (rec->date.year - 2000), rec->date.month, agz->ahead.dev_type);
			printf("%s", bdName);
			type = "days";
			t2.shift(24 * 60 * 60);
			position++;
			if (adb->isEmpty(position) == 0)continue;
			rez = adb->write("TIME_STAMP", position, t2.timestamp());
			break;
		default:
			printf("gz no support\n");
			return 1;
		}

		if (rez) printf("write %s record %s\n",type.c_str(), t2.timestamp().c_str());
		adb->write("VC_M3", position, ((float)rec->Vc) / 1000);
		adb->write("P_MPA", position, (float)rec->Psr);
		adb->write("T_GR", position, (float)rec->Tsr);
		adb->write("DP_KPA", position, NAN);
		adb->write("VCTOTAL_M3", position, (float)rec->Vsum_c);
		adb->write("VNTOTAL_M3", position, (float)rec->Vsum_r);
		adb->write("VN_M3", position, ((float)rec->Vr) / 1000);
		adb->write("Q_M3H", position, NAN);
		adb->write("EI_MG", position, NAN);
	}
	if (!rez) printf("new records not found\n");
	delete (adb);
	delete(agz);
	return 0;
}

long calcPosition(Datetime &base, Datetime &t,  char archType)
{
	long ret = -1;
	switch (archType)
	{
	  case 'H':
		  ret = (t.ctime() - base.ctime()) / (60 * 60);
		break;
	  case 'D':
		  ret = (t.ctime() - base.ctime()) / (24 * 60 * 60);
		  break;
	  default:
		break;
	}

	return ret;
}

bool runAndWaitProc(string command) 
{

	win::STARTUPINFOA si;
	win::PROCESS_INFORMATION pi;

	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));

	win::LPSTR comm = _strdup(command.c_str());
	// Start the child process. 
	if (!CreateProcessA(NULL,   // No module name (use command line)
		comm,     // Command line
		NULL,     // Process handle not inheritable
		NULL,     // Thread handle not inheritable
		FALSE,    // Set handle inheritance to FALSE
		0,        // No creation flags
		NULL,     // Use parent's environment block
		NULL,     // Use parent's starting directory 
		&si,      // Pointer to STARTUPINFO structure
		&pi)      // Pointer to PROCESS_INFORMATION structure
		)
	{
		printf("CreateProcess failed (%d).\n", win::GetLastError());
		return false;
	}

	// Wait until child process exits.
	win::WaitForSingleObject(pi.hProcess, INFINITE);

	// Close process and thread handles. 
	win::CloseHandle(pi.hProcess);
	win::CloseHandle(pi.hThread);
	return true;
}