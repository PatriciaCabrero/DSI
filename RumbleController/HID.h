#pragma once
class HID
{
public:

	HID(){}

	virtual ~HID(){}

protected:
	virtual void calibra()= 0;
	virtual void leeMando() = 0;
};

