


#include <iostream>



#include <algorithm>

#include <iomanip>
#include <ctime>
#include <chrono>
#include "Dbf.h"




using namespace std;
using namespace db;


int main()
{

	// ---------------- test Dbf functions

	char path[128] = "C:\\Users\\RGaleev\\source\\repos\\toDb\\Debug\\h2207_06.dbf";

	Dbf db;

	if (!db.connect(path)) {
		cout << "no connect to Db";
		return 1;
	}
	for (int i = 0; i < db.fieldsCount; i++) {
		printf("%11s\t",db.fields[i].name);
	}
	printf("\n");
	
	for (int j = 0; j < 10; j++) {
		 {
			
			for (int i = 0; i < db.fieldsCount; i++) {
				Dbf::Param* rec;
				rec = db.get(db.fields[i].name, j);

				if (rec->field->type=='F')
				{
					printf("\t%f",rec->fValue);
				}
				else
				{
					printf("%s", rec->value);
				}
			}
			printf("\n");
		}
	}

	char name[11] = "VC_M3";
	char value[6] = "123.6";
	cout<<db.write(name, 3, value);


}
 