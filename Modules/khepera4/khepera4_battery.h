/*!
 * (c) 2006 - 2008 EPFL, Lausanne, Switzerland
 * Thomas Lochmatter
 */

#ifndef KHEPERA4_BATTERY
#define KHEPERA4_BATTERY

#include "i2cal.h"

//! Battery

struct sKhepera4Battery {
    unsigned int status; //!< Status
    unsigned int voltage; //!< Voltage (mV)
    int current; //!< Instantaneous current (mA)
    int current_average; //!< Average current (mA)
    unsigned int capacity_remaining_absolute; //!< Absolute remaining capacity (mAh)
    unsigned int capacity_remaining_relative; //!< Relative remaining capacity (%)
    float temperature; //!< Temperature (degrees Celsius)
};

//! (private) Initializes this submodule.
void khepera4_battery_init();

//! (private) Reads a value from the battery and returns a plain i2c message. Transfer on I2C bus: 12 bytes.
int khepera4_battery_p(struct sKhepera4Battery *battery);

//! Reads the battery data and updates the khepera4 structure
int khepera4_battery();


#endif
