#include "kheptools.h"
#include <math.h>
#include <iostream>

Khepera* Khepera::khepera = 0;

float Khepera::normalizeIR(float raw) {
	float ret = 1.0 / ( 1.0 + exp((thresholdValue - raw) / steepness));
	return ret;
}

Khepera::Khepera() : maxSpeed(def_maxSpeed) {
	khepera4_init();
	khepera4_motor_initialize(&(khepera4.motor_left));
	khepera4_motor_initialize(&(khepera4.motor_right));
	khepera4_drive_stop();
	khepera4_drive_start();
}

Khepera* Khepera::getKhepera() {
	if (!Khepera::khepera) {
		Khepera::khepera = new Khepera();
	}
	return Khepera::khepera;
}

float Khepera::getIR(int pos) {
	// TODO error handling
	updateIR();
	if (pos < 0)
		pos = 0;
	else if (pos >= NR_SENSORS) {
		pos = NR_SENSORS - 1;
	}
	return normalizeIR(irData.sensor[pos]);
}

float Khepera::getRawIR(int pos) {
	updateIR();
	if (pos < 0)
		pos = 0;
	else if (pos >= NR_SENSORS) {
		pos = NR_SENSORS - 1;
	}
	return irData.sensor[pos];
}

void Khepera::updateIR() {
	khepera4_infrared_proximity_p(&irData);
}

void Khepera::updateAmbientIR() {
	khepera4_infrared_ambient_p(&irAmbientData);
}

int* Khepera::getAllRawIR() {
	updateIR();
	return irData.sensor;
}

int* Khepera::getAllRawAmbientIR() {
	updateAmbientIR();
	return irAmbientData.sensor;
}

void Khepera::setLeftSpeed(float speed) {
	if (speed > 1)
		speed = 1;
	else if (speed < -1)
		speed = -1;
	khepera4_motor_set_speed(&(khepera4.motor_left), (int)(speed * (float)maxSpeed));
}

void Khepera::setRightSpeed(float speed) {
	if (speed > 1)
		speed = 1;
	else if (speed < -1)
		speed = -1;
	khepera4_motor_set_speed(&(khepera4.motor_right), (int)(speed * (float)maxSpeed));
}

void Khepera::setNormalizedSpeeds(float left, float right) {
	setLeftSpeed(left*2.0 - 1.0);
	setRightSpeed(right*2.0 - 1.0);
}

void Khepera::setMaxSpeed(int speed) {
	maxSpeed = speed;
}
