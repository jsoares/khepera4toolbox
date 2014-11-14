#! /usr/bin/perl
use strict;

my $sensor=shift;
die 'Usage: ./parse_results.pl SENSOR' if (! $sensor);

while (my $line=<>) {
	while (chomp $line) {}
	if ($line =~ /^\$POSITION,([\d\.-]+)$/) {
		print "\n";
		print $1;
	} elsif ($line =~ /^\$IRPROXIMITY,(.*)$/) {
		my @args=split(/,/, $1);
		print "\t", $args[$sensor];
	}
}
