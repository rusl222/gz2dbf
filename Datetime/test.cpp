
#include <iostream>
#include "Datetime.h"

using namespace std;
int main()
{
	Datetime *d = new Datetime(60);
	printf("%s", d->timestamp().c_str());
}

