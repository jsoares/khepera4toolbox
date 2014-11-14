#! /usr/bin/perl
use strict;

while (my $line=<>) {
	while (chomp $line) {}
	if ($line =~ /^\$MEASURE_REAL_SPEED,([\d\.-]+),([\d\.-]+),([\d\.-]+),([\d\.-]+)$/) {
		print $1, "\t", $2, "\n";
	}
}
