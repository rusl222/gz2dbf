#include <iostream>

#include"Agz.h"
#include "EndianConverter.h"

using namespace gz;

int main()
{
	Agz gz;
	//EndianConverter c;
	char path[128] = "C:\\Users\\RGaleev\\source\\repos\\toDb\\Debug\\62c1bac7.hz";
	gz.open(path);
	char pathtmp[128] = "C:\\Users\\RGaleev\\source\\repos\\toDb\\Debug\\62c1bac7.bin";
	gz.readData(pathtmp);

	//gz::Record &rec = gz.data.pop_front();
	//unsigned short year = rec.date.year;
	//cout << year;

	return 0;
}
