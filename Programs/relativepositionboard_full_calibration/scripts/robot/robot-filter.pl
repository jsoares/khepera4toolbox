#!/usr/bin/perl

# usage: netcat -l -u 4000 | perl ./robot-filter.pl > rssi.txt

use strict;
use Time::HiRes;
use IO::Socket;

# Turn off auto-flush of stdout
$|=1;

# Extra variables
my ($secs, $micros, $t);

print STDERR "You can press [CTRL + C] to stop observing...\n";

# Catch CTRL+C
$SIG{'INT'} = 'INT_handler';

# Information for one frame may look like this:
#   $RSSI,ID1,ID1,RSSI1,...,RSSI8

# Output: time id1 id2 rssi1 ... rssi8

# Read from STDIN continously
while (my $line=<STDIN>) {

	if ($line =~ /\$RSSI,(.*)/) {
		# We want to get the timestamp
		($secs, $micros) = Time::HiRes::gettimeofday;
		$t = $secs + ($micros/1000000);
                print $t, " ", join(' ', split(/,/, $1)), "\n";
	}
}

sub INT_handler {
	print STDERR  "\nStopping observing...\n";
	exit 0;
}



