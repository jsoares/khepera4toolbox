#! /usr/bin/env python

import re
import sys

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

# Prepare regular expressions
re_battery = re.compile('\$BATTERY,\d+,([\d\.\-]+)')

# Read from STDIN continuously
while True:
	line = raw_input()

	# Check if this is a battery record
	result = re_battery.search(line)
	if result != None:
		print result.group(1)
		continue

	# If this is an empty line, we ignore it
	if line == '':
		continue

	# Something else? That's suspicious here ...
	print >> sys.stderr, 'Unexpected line: ', line;
