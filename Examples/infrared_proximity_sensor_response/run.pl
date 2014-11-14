#! /usr/bin/perl
use strict;

# Inform the user if the result file exists
die 'The result file "run_results" exists. Delete or rename this file before launching another run.' if (-e 'run_results');

# Reset the motor position
system('ssh root@192.168.141.219 /root/lochmatt/motor_setposition 0 0') && die 'Stopped!';

# Take 10 measurements each at 100 distances 
foreach my $position (1..100) {
	print 'Going to position ', $position, "\n";
	system('ssh root@192.168.141.219 /root/lochmatt/motor_gotoposition -'.($position*100).' -'.($position*100)) && die 'Stopped!';

	print 'Taking infrared measures', "\n";
	system('echo "\$POSITION,'.$position.'" >> calibration_results');
	system('ssh root@192.168.141.219 /root/lochmatt/infrared_proximity -r 10 -w 100000 >> calibration_results') && die 'Stopped!';
}
