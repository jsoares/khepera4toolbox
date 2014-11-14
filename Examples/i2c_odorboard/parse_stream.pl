#! /usr/bin/perl
use strict;

while (my $line = <>) {
	while (chomp $line) {}
	$line =~ /\$BLOCK,(\d+),(\d+),(.*)$/i || next;
	print $1, "\t", $2, "\t";
	print join("\t", map {hex($_)} split(/ /, $3)), "\n";
}
