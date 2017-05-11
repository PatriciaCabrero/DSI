#pragma once

typedef unsigned short WORD;
enum Rotation{pp,pn,nn,np};
class HID
{
public:
	HID(){}
	virtual ~HID(){}

	bool bConected; //Mando conectado
	void actualiza(){
		wlastbuttons = wbuttons;
		bConected = leeMando();
		if (bConected){
			mando2HID();
			wbuttonsDown = (~wlastbuttons)&(wbuttons);
			wbuttonsUp = (wlastbuttons)&(~wbuttons);
		}
	};

	float LT(){ return fLeftTrigger; }
	float RT(){ return fRightTrigger; }

	float TLX(){ return fThumbLX; }
	float TLY(){ return fThumbLY; }

	float TRX(){ return fThumbRX; }
	float TRY(){ return fThumbRY; }

	bool BD(WORD bot){
		return (wbuttonsDown & bot);
	}
	bool BU(WORD bot){
		return (wbuttonsUp & bot) ;

	}

	//GESTOS
	WORD wbuttonsDown;
	WORD wbuttonsUp; 
	

protected:
	//Entradas
	WORD wbuttons;
	WORD wlastbuttons;

	float fLeftTrigger, fRightTrigger;			   // [0.0 - 1.0]
	float fThumbLX, fThumbLY, fThumbRX, fThumbRY; // [-1.0 - 1.0]
	//float fThumbRXf, fThumbRYf; //[-1.0 - 1.0] de momento no hace falta

	virtual bool leeMando() = 0;
	virtual void escribeMando() = 0;
	virtual void mando2HID() = 0;
	virtual void calibra()= 0;
};

