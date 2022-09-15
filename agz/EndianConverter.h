#pragma once
class EndianConverter
{
public:
	EndianConverter();
	~EndianConverter();

	unsigned short us(unsigned short* buf);
	unsigned int ui(char* buf);
	float f(char* buf);


};

