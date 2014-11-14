#! /usr/bin/perl
use strict;

# Inform the user if the result file exists
die 'The result file "run_results" exists. Delete or rename this file before launching another run.' if (-e 'run_results');

# Adjust the threshold and the paper size here
my $arguments = '--floor-threshold-on 3900 --floor-threshold-off 3450 --real-distance 0.297';

# Tests speeds 5000 to 30000 in increments of 1000
foreach my $speed (map {$_ * 1000} (5..30)) {
	# Forward motion
	print 'Speed setpoint: ', $speed, "\n";
	system('ssh root@192.168.141.203 /root/lochmatt/measure_real_speed --speed '.$speed.' '.$arguments.' >> run_results') && die 'Stopped!';

	# Backward motion
	print 'Speed setpoint: -', $speed, "\n";
	system('ssh root@192.168.141.203 /root/lochmatt/measure_real_speed --speed -'.$speed.' '.$arguments.' >> run_results') && die 'Stopped!';
}
