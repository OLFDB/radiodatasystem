#!/usr/bin/perl -wT

use strict;

$ENV{PATH} = '/bin:/usr/bin';

print "\e[H";  # cursor home
print "\e[2J"; # clear screen

while(<>) {
	next if not /^RASANT:/;

	   if (/ msg=1 .*? prn=(.*?) / ) { &cursor_row(6+$1); }
	elsif (/ msg=2 / ) { &cursor_row(1); }
	elsif (/ msg=3 / ) { &cursor_row(2); }
	elsif (/ msg=4 / ) { &cursor_row(3); }
	elsif (/ msg=5 / ) { &cursor_row(4); }
	else               { &cursor_row(5); }

	s/^RASANT: //;

	&clear_line();
	s/^(...):/$1 :/; # 3->4
	s/^(..):/$1  :/; # 2->4
	print "$_";
}

sub cursor_row
{
	my ($row) = @_;
	print "\e[".$row.";0f";
}

sub clear_line
{
	print "\e[K";
}
