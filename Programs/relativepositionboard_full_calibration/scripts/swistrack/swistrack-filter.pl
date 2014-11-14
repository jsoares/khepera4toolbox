#!/usr/bin/perl

# usage: netcat localhost 3001 | perl ./swistrack-filter.pl 202 204 206 214 > position.txt  

use strict;
use Time::HiRes;
use IO::Socket;

# Definitions
my @robot_ids    = @ARGV;

my $max_dist     = 0.2;
my $port_base    = 4000;
my $pos_msgs_mod = 1;

# Robot positions
my @robot_positions = ();

# Turn off auto-flush of stdout
$|=1;

# Extra variables
my ($secs, $micros, $t);
my ($x, $y, $theta);
my ($r_ip, $buffer);
my @robot_sockets = ();
my $frame_cnt = 0;
my $pi = 3.14159265;

print STDERR "You can press [CTRL + C] to stop the tracking...\n";

# Create sockets
for (my $i = 0; $i <= $#robot_ids; $i++) {
	$r_ip = "192.168.141." . $robot_ids[$i]; # robot IP
        print STDERR "Going to track robot ",  $robot_ids[$i], "\n";
	push @robot_sockets , IO::Socket::INET->new(Proto => "udp", PeerHost => $r_ip, PeerPort => $port_base);
}

# Catch CTRL+C
$SIG{'INT'} = 'INT_handler';

# Information for one frame may look like this:
#   $STEP_START
#   $FRAMENUMBER,34
#   $PARTICLE,0,0,0,0
#   $STEP_STOP

# Output: time x1 y1 theta1 x2 y2 theta2 ...

# Read from STDIN continously
while (my $line=<STDIN>) {

	if ($line =~ /\$STEP_START/) {
		# We want to get the timestamp
		($secs, $micros) = Time::HiRes::gettimeofday;
		$t = $secs + ($micros/1000000);
	}

	if ($line =~ /\$TRIGGER_STOP/) {
		# Should stop the program?
		print STDERR "Tracking stopped.\n";
	}
	
	if ($line =~ /\$PARTICLE,([\d-]*),([\d\.-]*),([\d\.-]*),([\d\.-]*),([\d\.-]*),([\d\.-]*)/) {
		# This is a particle: ID, image_x, image_y, theta, x, y
		$x = $5;
		$y = $6;
		$theta = -$4 + $pi/2.0;
		if ($theta > $pi) {
			$theta = $theta - 2.0*$pi;
		}

		# Is there any robot already?
		if ($#robot_positions == -1) {
			print STDERR  "Robot ", $robot_ids[0], " found.\n";
			push @robot_positions, [$x, $y, $theta];
			if ($#robot_positions == $#robot_ids) {
				print STDERR "Tracking in progress\n";
			}
			
			next;
		}

		# Try to assign to nearest neighbor
		my $min_dist = 1000.0;
		my $min_index = 0;
		for (my $i = 0; $i <= $#robot_positions; $i++) {
			my $dx = $robot_positions[$i][0] - $x;
			my $dy = $robot_positions[$i][1] - $y;
			my $dist = sqrt($dx*$dx + $dy*$dy);
			if ($dist < $min_dist) {
				$min_dist = $dist;
				$min_index = $i;
			}
		}

		if ($min_dist < $max_dist) {
			$robot_positions[$min_index][0] = $x;
			$robot_positions[$min_index][1] = $y;
			$robot_positions[$min_index][2] = $theta;
		} elsif ($#robot_positions < $#robot_ids) {
			# Maybe it is a new robot
			print STDERR  "Robot ", $robot_ids[$#robot_positions+1], " found.\n";
			push @robot_positions, [$x, $y, $theta];
			if ($#robot_positions == $#robot_ids) {
				print STDERR "Tracking in progress\n";
			}
			
			next;
		}
	}

	if ($line =~ /\$STEP_STOP/ && $#robot_positions == $#robot_ids) {
		# Print all robot positions
		print $t, " ";
		for(my $i = 0; $i <= $#robot_positions; $i++) {
			print $robot_positions[$i][0], " ", $robot_positions[$i][1], " ", $robot_positions[$i][2], " ";

			# Send position to robots
			if (($frame_cnt % $pos_msgs_mod) == 0) {
				# Message format: int type, int sender, float x, float y, float theta.
				$buffer = pack("llfff", 2, -1, $robot_positions[$i][0], $robot_positions[$i][1], $robot_positions[$i][2]);
				$robot_sockets[$i]->send($buffer);
			}
		}
		$frame_cnt++;
		print "\n";
	}
}

sub INT_handler {
	print STDERR  "\nStopping tracking...\n";
	for (my $i = 0; $i <= $#robot_ids; $i++) {
		shutdown($robot_sockets[$i], 2);
	}
	exit 0;
}



