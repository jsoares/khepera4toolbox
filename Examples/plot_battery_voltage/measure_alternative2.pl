#! /usr/bin/perl
use strict;

# Invokes the battery program every 60 seconds.
# Make sure you have SSH public key authentication set up between your computer and the robot, e.g. using the "k3-ssh-keysetup" script.

system('echo "Start of measure_alternative2.pl" >> battery_measures');

while (1) {
	# Measure the battery voltage once
	system('ssh root@192.168.1.2 "/root/your_username/battery" >> battery_measures') && die 'Stopped!';

	# Sleep one minute
	sleep 60;
}
