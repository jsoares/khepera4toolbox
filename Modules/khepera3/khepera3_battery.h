/*!
 * (c) 2006 - 2008 EPFL, Lausanne, Switzerland
 * Thomas Lochmatter
 */

#ifndef KHEPERA3_BATTERY
#define KHEPERA3_BATTERY

#include "i2cal.h"

//! Battery

struct sKhepera3Battery {
    unsigned int status; //!< Status
    unsigned int voltage; //!< Voltage (mV)
    int current; //!< Instantaneous current (mA)
    int current_average; //!< Average current (mA)
    unsigned int capacity_remaining_absolute; //!< Absolute remaining capacity (mAh)
    unsigned int capacity_remaining_relative; //!< Relative remaining capacity (%)
    float temperature; //!< Temperature (degrees Celsius)
};

//! (private) Initializes this submodule.
void khepera3_battery_init();

//! (private) Reads a value from the battery and returns a plain i2c message. Transfer on I2C bus: 12 bytes.
int khepera3_battery_p(struct sKhepera3Battery *battery);

//! Reads the battery data and updates the khepera3 structure
int khepera3_battery();


#endif
