#!/usr/bin/perl -wT

use strict;

$ENV{PATH} = '/bin:/usr/bin';

print "\e[H";  # cursor home
print "\e[2J"; # clear screen

while(<>) {
	if    (/^AF:/  ) { &cursor_row( 1); }
	elsif (/^CT:/  ) { &cursor_row( 2); }
	elsif (/^DI:/  ) { &cursor_row( 3); }
	elsif (/^ECC:/ ) { &cursor_row( 4); }
	elsif (/^EON:/ ) { &cursor_row( 5); }
	elsif (/^ERT:/ ) { &cursor_row( 6); }
	elsif (/^IH:/  ) { &cursor_row( 7); }
	elsif (/^LIC:/ ) { &cursor_row( 8); }
	elsif (/^MS:/  ) { &cursor_row( 9); }
	elsif (/^ODA:/ ) { &cursor_row(10); }
	elsif (/^PI:/  ) { &cursor_row(11); }
	elsif (/^PIN:/ ) { &cursor_row(12); }
	elsif (/^PS:/  ) { &cursor_row(13); }
	elsif (/^PTY:/ ) { &cursor_row(14); }
	elsif (/^PTYN:/) { &cursor_row(15); }
	elsif (/^RP:/  ) { &cursor_row(16); }
	elsif (/^RT:/  ) { &cursor_row(17); }
	elsif (/^RTP:/ ) { &cursor_row(18); }
	elsif (/^TDC:/ ) { &cursor_row(19); }
	elsif (/^TMC:/ ) { &cursor_row(20); }
	elsif (/^TPTA:/) { &cursor_row(21); }
	elsif (/^TCA:/ ) { &cursor_row(22); }
	elsif (/^TCB:/ ) { &cursor_row(23); }
	else             { &cursor_row(24); }

	&clear_line();
	s/^(...):/$1 :/; # 3->4
	s/^(..):/$1  :/; # 2->4
	print $_;
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
