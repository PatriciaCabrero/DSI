#pragma once

typedef unsigned short WORD;
enum Rotation{pp,pn,nn,np};
class HID
{
public:
	HID(){}
	virtual ~HID(){}

	bool bConected; //Mando conectado
	
	//Entradas
	WORD wbuttons;
	WORD wlastbuttons;

	float fLeftTrigger, fRightTrigger;			   // [0.0 - 1.0]
	float fThumbLX, fThumbLY, fThumbRX, fThumbRY; // [-1.0 - 1.0]
	//float fThumbRXf, fThumbRYf; //[-1.0 - 1.0] de momento no hace falta
	void Actualiza();
	//GESTOS
	WORD wbuttonsDown;
	
protected:
	virtual bool leeMando() = 0;
	virtual void escribeMando() = 0;
	virtual void mando2HID() = 0;
	virtual void calibra()= 0;
};

void HID::Actualiza(){
	wlastbuttons = wbuttons;
	bConected = leeMando();
	if (bConected){
		mando2HID();
		wbuttonsDown = (~wlastbuttons)&(wbuttons);
	}
}