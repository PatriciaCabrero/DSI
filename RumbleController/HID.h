#pragma once

typedef unsigned short WORD;

enum Rotation{pp,pn,nn,np};
#define T 0.01
class HID
{
public:
	HID(){}
	virtual ~HID(){}

	bool bConected; //Mando conectado
	void actualiza(){

		wlastbuttons = wbuttons;
		bConected = leeMando();
		if (bConected) {
			mando2HID();

			//filtrado
			fThumbLXf = (1 - aTR) * fThumbLXf + aTR*fThumbLX;
			fThumbLYf = (1 - aTR) * fThumbLYf + aTR*fThumbLY;


			//filtrado
			fThumbRXf = (1 - aTR) * fThumbRXf + aTR*fThumbRX;
			fThumbRYf = (1 - aTR) * fThumbRYf + aTR*fThumbRY;



			//Rotacion
			if ((fThumbLX > 0)&(fThumbLY > 0)) {
				rot = pp;
				tRot = 1.0;
			}
			if (tRot > 0) {
				tRot = tRot - T;

				if ((fThumbLX > 0 & fThumbLY <= 0 & rot == pp)) {
					rot = pn;
				}
				if ((fThumbLX <= 0 & fThumbLY < 0 & rot == pn))rot = nn;
				if ((fThumbLX < 0 & fThumbLY >= 0 & rot == nn))rot = np;
			}
			else rot = pp;

			if (rot == np) {
				fLeftMotor = 1;
			}else
				fLeftMotor *= 1 - (T / (0.5 + T));

			wbuttonsDown = (~wlastbuttons)&(wbuttons);
			wbuttonsUp = (wlastbuttons)&(~wbuttons);

			if (wbuttonsDown) { 
				fRightMotor = 1; 
			}
			else fRightMotor *= 1 - aFB;

			escribeMando();

		}
	};

	//GETTERS
	//Triggers
	float LT(){ return fLeftTrigger; }
	float RT(){ return fRightTrigger; }

	//THUMBS
	float TLX(){ return fThumbLX; }
	float TLY(){ return fThumbLY; }

	float TRX(){ return fThumbRX; }
	float TRY(){ return fThumbRY; }
	
	float TRXf() { return fThumbRXf; }
	float TRYf() { return fThumbRYf; }

	float TLXf() { return fThumbLXf; }
	float TLYf() { return fThumbLYf; }

	float velX() { return fVelX; }
	float velY() { return fVelX; }

	//CONSULTAS
	//Consultar si un boton se ha pulsado
	bool BD(WORD bot){
		return (wbuttonsDown & bot);
	}

	//Consultar si un boton se ha soltado
	bool BU(WORD bot){
		return (wbuttonsUp & bot) ;
	}

	//SETTERS
	//Cambiar la vibracion izquierda
	void LM(float l) { fLeftMotor = l; };
	
	//Cambiar la vibracion derecha
	void RM(float l) { fLeftMotor = l; };


	//GESTOS
	WORD wbuttonsDown;
	WORD wbuttonsUp;
	

protected:
	//Entradas
	WORD wbuttons;
	WORD wlastbuttons;

	float fLeftTrigger, fRightTrigger;			   // [0.0 - 1.0]
	float fThumbLX, fThumbLY, fThumbRX, fThumbRY; // [-1.0 - 1.0]
	float fThumbRXf, fThumbRYf, fThumbLXf, fThumbLYf; //[-1.0 - 1.0] de momento no hace falta
	const float aTR = T / (0.07 + T); //Filtro de los Thumb

	//MOTOR
	float fLeftMotor, fRightMotor; //[0.0 - 1.0]
	const float aFB = T / (0.05 + T); //Filtro motores

	//VELOCIDAD
	float fVelX, fVelY; //Velocidad cursor [-2.0 , 2.0]
	const float aceleracion = 0.3; //Aceleracion cursor

	Rotation rot;
	float tRot = 0.0;

	virtual bool leeMando() = 0;
	virtual void escribeMando() = 0;
	virtual void mando2HID() = 0;
	virtual void calibra()= 0;
};

