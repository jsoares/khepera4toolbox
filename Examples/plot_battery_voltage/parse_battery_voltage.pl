#! /usr/bin/perl
use strict;
$|=1; # Autoflush - don't buffer the output (required for real-time measures)

# A battery message, as given by the "battery" program, looks like this (all on one line, of course):
#	$BATTERY,
#		sample_number,
#		voltage [V],
#		current [A],
#		current_average [A],
#		capacity_remaining_absolute [Ah],
#		capacity_remaining_relative [%],
#		temperature [deg C]
# We parse such messages and extract the voltage information (second field).
#
# In this script, we assume that the file only contains $BATTERY messages, and anything else would result in a "unexpected line" warning. This is good here, because we do not have other messages in our data stream. If you want to use this script to parse a file which contains many different types of messages, you should disable the warning, and simply ignore all messages other than $BATTERY messages.

# Read from STDIN continously
while (my $line=<>) {
	while (chomp $line) {}
	
	if ($line =~ /\$BATTERY,\d+,([\d\.-]+),/) {
		# This is a battery record
		print $1, "\n";
	} elsif ($line eq '') {
		# This is an empty line and can be ignored
	} else {
		# Something else? That's suspicious here ...
		print STDERR 'Unexpected line: ', $line, "\n";
	}
}
