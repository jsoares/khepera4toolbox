#ifndef KHEPERA4_CPP_H
#define KHEPERA4_CPP_H

extern "C" {
#include "khepera4.h"
}

class Khepera {

public:
	const static int NR_SENSORS = cKhepera4SensorsInfrared_Count;
private:
	const static float steepness = 25;
	const static float thresholdValue = 250;
	const static int def_maxSpeed = 40000;
	sKhepera4SensorsInfrared irData;
	sKhepera4SensorsInfrared irAmbientData;
	int maxSpeed;

	static Khepera* khepera;

	// private constructor, singleton
	Khepera();
	void updateIR();
	void updateAmbientIR();
	static float normalizeIR(float raw);
public:
	// singleton getter
	static Khepera* getKhepera();
	// return sensor value, sensors go from 0 to NR_SENSORS -1
	float getIR(int pos);
	float getRawIR(int pos);
	void setNormalizedSpeeds(float left, float right);
	void setLeftSpeed(float speed);
	void setRightSpeed(float speed);
	int* getAllRawIR();
	int* getAllRawAmbientIR();
	void setMaxSpeed(int speed);
};


#endif
