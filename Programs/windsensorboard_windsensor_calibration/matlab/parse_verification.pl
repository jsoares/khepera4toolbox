#! /usr/bin/perl
use strict;

my $count=0;
while (my $line=<>) {
	while (chomp $line) {}
	if ($line =~ /^\$WINDSENSOR_VERIFICATION_ANGLE,(.*)$/i) {
		my $angle=$1;
		print "\n";
		print $angle;
		$count=256;
	} elsif ($line =~ /^\$WIND,([\d\.]*),([\d\.]*),([\d\.]*),([\d\.]*)$/i) {
		if ($count>0) {
			my $wind_angle=$3;
			my $likelihood=$4;
			print "\t", $wind_angle, "\t", $likelihood;
			$count--;
		}
	}
}

print "\n";
