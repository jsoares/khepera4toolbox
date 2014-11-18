#!/usr/bin/python
# -*- coding: utf-8 -*-

# Script to configure new Khepera IV robot
# Adapted by Jorge Soares, original K3 by Sven Gowal
#
# Usage python setup_kh4.py [ROBOT ID]
#
# Make sure the robot is on, connected by USB, and that
# the user has permissions for /dev/ttyUSB0

import serial
import time
import sys

def printf(fmt, *varargs):
    sys.stdout.write(fmt % varargs)

if (len(sys.argv) == 1):
    print "\nUsage: python", sys.argv[0], "\n"
    sys.exit(-1)

robotid = sys.argv[1]
printf("You are going to configure robot #%s. ", robotid)
answer = raw_input("Are you sure? [y/n] ")
if (answer == "n"):
    print "Aborting...\n"
    sys.exit(-1)

printf("Configuring robot #%s...\n",robotid)

print "Make sure your robot is connected to /dev/ttyUSB0 and booted up"
answer = raw_input("Press [ENTER] when done")

# Open serial port
ser = serial.Serial("/dev/ttyUSB0", 115200, timeout=2)
time.sleep(0.5)

# We are logged in
print "Port open, establishing connection"

# Probe connection (try to log in)
ser.write("\n")
ser.write("root\n") # sort of hackish, but works
response = ser.readlines()
if (response[-1].startswith("root@")):
    print "Logged in as root"
elif (response[-1].startswith("-sh: root: not found")):
    print "Already logged in as root"
else:
    print "The Korebot is not running (or still booting). Aborting...\n"
    sys.exit(-1)
    
# Purge serial
time.sleep(1.0)
ser.flushOutput();

# Setup /etc/khepera folder
print "Setting up robotid file: /etc/khepera/robotid"
ser.write("mkdir /etc/khepera\n");
ser.readline()
response = ser.readline()
if (response.startswith("mkdir: cannot create directory")):
    print "/etc/khepera directory already exists."
    answer = raw_input("Are you sure, you want to continue? [y/n] ")
    if (answer == "n"):
        print "Aborting...\n"
        sys.exit(-1)
else:
    print "/etc/khepera directory created"

# Purge serial
time.sleep(1.0)
ser.flushOutput();

# Setup robotid
printf("Writing id %s in robotid file...\n", robotid);
ser.write("echo \"{0}\" > /etc/khepera/robotid\n".format(robotid));

# Purge serial
time.sleep(1.0)
ser.flushOutput();

print "Robot ID written"

# Setup hostname
printf("Writing hostname r%s in hostname file...\n", robotid);
ser.write("echo \"r{0}\" > /etc/hostname\n".format(robotid));

# Purge serial
time.sleep(1.0)
ser.flushOutput();

print "Hostname written"

# Setup interfaces
print "Writing /etc/network/interfaces file..."

ser.write("""cat > /etc/network/interfaces << END
# /etc/network/interfaces -- configuration file for ifup(8), ifdown(8)

# The loopback interface
auto lo
iface lo inet loopback

# Wireless interface
auto wlan3
iface wlan3 inet static
	wireless_mode managed
	wireless_essid disal-robots
	address 192.168.141.{0}
	netmask 255.255.255.0
	gateway 192.168.141.1


# Ethernet/RNDIS gadget (g_ether)                                                                                                                                                
# ... or on host side, usbnet and random hwaddr                                                                                                                                  
#auto usb0                                                                                                                                                                       
#iface usb0 inet static                                                                                                                                                          
#       address 192.168.1.{0}                                                                                                                                                     
#       netmask 255.255.255.0                                                                                                                                                    
#       network 192.168.1.0                                                                                                                                                      
#       gateway 192.168.1.1                                                                                                                                                      
#       pre-up /sbin/modprobe g_ether host_addr=46:0d:9e:67:69:ed dev_addr=46:0d:9e:67:69:ee                                                                                     
#       post-down /sbin/modprobe -r g_ether                                                                                                                                      
                                                                                                                                                                                 
                                                                                                                                                                                 
# Bluetooth networking                                                                                                                                                           
# iface bnep0 inet dhcp  
END\n""".format(robotid));
ser.flush();

# Purge output
time.sleep(1.0)
ser.flushOutput();

print "Network setup done"

# Close connection
ser.close()

print "All done" 
print "Please reboot the robot and try to ping 192.168.141.{0}".format(robotid)
print "Enjoy!\n"
