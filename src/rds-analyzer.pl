#!/usr/bin/perl -wT

use strict;

my @rds;

my @oda_aid;	my @oda_avail;
$oda_aid[ 0][0] = 0;	$oda_avail[ 0][0] = 0;
$oda_aid[ 0][1] = 0;	$oda_avail[ 0][1] = 0;
$oda_aid[ 1][0] = 0;	$oda_avail[ 1][0] = 0;
$oda_aid[ 1][1] = 0;	$oda_avail[ 1][1] = 0;
$oda_aid[ 2][0] = 0;	$oda_avail[ 2][0] = 0;
$oda_aid[ 2][1] = 0;	$oda_avail[ 2][1] = 0;
$oda_aid[ 3][0] = 0;	$oda_avail[ 3][0] = 0;
$oda_aid[ 3][1] = 0;	$oda_avail[ 3][1] = 1;
$oda_aid[ 4][0] = 0;	$oda_avail[ 4][0] = 0;
$oda_aid[ 4][1] = 0;	$oda_avail[ 4][1] = 1;
$oda_aid[ 5][0] = 0;	$oda_avail[ 5][0] = 1;
$oda_aid[ 5][1] = 0;	$oda_avail[ 5][1] = 1;
$oda_aid[ 6][0] = 0;	$oda_avail[ 6][0] = 1;
$oda_aid[ 6][1] = 0;	$oda_avail[ 6][1] = 1;
$oda_aid[ 7][0] = 0;	$oda_avail[ 7][0] = 1;
$oda_aid[ 7][1] = 0;	$oda_avail[ 7][1] = 1;
$oda_aid[ 8][0] = 0;	$oda_avail[ 8][0] = 1;
$oda_aid[ 8][1] = 0;	$oda_avail[ 8][1] = 1;
$oda_aid[ 9][0] = 0;	$oda_avail[ 9][0] = 1;
$oda_aid[ 9][1] = 0;	$oda_avail[ 9][1] = 1;
$oda_aid[10][0] = 0;	$oda_avail[10][0] = 0;
$oda_aid[10][1] = 0;	$oda_avail[10][1] = 1;
$oda_aid[11][0] = 0;	$oda_avail[11][0] = 1;
$oda_aid[11][1] = 0;	$oda_avail[11][1] = 1;
$oda_aid[12][0] = 0;	$oda_avail[12][0] = 1;
$oda_aid[12][1] = 0;	$oda_avail[12][1] = 1;
$oda_aid[13][0] = 0;	$oda_avail[13][0] = 1;
$oda_aid[13][1] = 0;	$oda_avail[13][1] = 1;
$oda_aid[14][0] = 0;	$oda_avail[14][0] = 0;
$oda_aid[14][1] = 0;	$oda_avail[14][1] = 0;
$oda_aid[15][0] = 0;	$oda_avail[15][0] = 1;
$oda_aid[15][1] = 0;	$oda_avail[15][1] = 0;

sub tmc_assign
{
	my ($y) = @_;

	my $vc = $y >> 14;
	printf("VC=%i ", $vc);
	if ($vc == 0) {
		printf("x=%i LTN=%i AFI=%i M=%i MGSI=%i MGSN=%i MGSR=%i MGSU=%i",
			($y >> 12) & 3, ($y >> 6) & 0x3f, ($y >> 5) & 1, ($y >> 4) & 1, ($y >> 3) & 1, ($y >> 2) & 1, ($y >> 1) & 1, $y & 1);
	} elsif ($vc == 1) {
		printf("G=%i SID=%i Ta=%i Tw=%i Td=%i",
			($y >> 12) & 3, ($y >> 6) & 0x3f, ($y >> 4) & 3, ($y >> 2) & 3, $y & 3);
	} elsif ($vc == 2) {
		printf("unknown=%i", $y);
	} elsif ($vc == 3) {
		printf("unknown=%i", $y);
	}
}

sub oda_assign
{
	my ($agtc, $agtv, $msg, $aid) = @_;
	$oda_aid[$agtc][$agtv] = $aid;

	if (($agtc == 15) && ($agtv == 1)) {
		printf("ODAID(AID=%4.4x=unknown) fault msg=%4.4x", $aid, $msg);
	} elsif ($aid == 0x0D45) { # TMC
		printf("ODAID(AID=%4.4x=TMC) ", $aid);
		tmc_assign($msg);
	} elsif ($aid == 0x4AA1) { # Rasant
		printf("ODAID(AID=%4.4x=Rasant) msg=%4.4x", $aid, $msg);
	} elsif ($aid == 0x4BD7) { # RT+
		printf("ODAID(AID=%4.4x=RT+) rfu=%i ERT=%i CB=%i SCB=%i TN=%i",
			$aid, $msg >> 14, ($msg >> 13) & 1, ($msg >> 12) & 1, ($msg >> 8) & 0xf, $msg & 0xff);
	} elsif ($aid == 0x6552) { # eRT
		printf("ODAID(AID=%4.4x=eRT) rfu=%i CTID=%i DTFD=%i UTF8=%i", $aid, $msg >> 6, ($msg >> 2) & 0xf, ($msg >> 1) & 1, $msg & 1);
	} elsif ($aid == 0xCD46) { # TMC
		printf("ODAID(AID=%4.4x=TMC) ", $aid);
		tmc_assign($msg);
	} else {
		printf("ODAID(AID=%4.4x=unknown) msg=%4.4x", $aid, $msg);
	}
}

sub tmc_a
{
	my ($x, $y, $z) = @_;
	my $t = $x >> 4;
	my $f = ($x >> 3) & 1;
	my $dp = $x & 7;
	my $d = $y >> 15;
	my $dir = ($y >> 14) & 1;
	my $extent = ($y >> 11) & 7;
	my $event = $y & 0x7ff;
	my $location = $z;
	my $ci = $x & 7;
	my $sg = ($y >> 14) & 1;
	my $gsi = ($y >> 12) & 3;

	printf("T=%i F=%i ", $t, $f);
	if ($t == 0) {
		if ($f == 0) {
			if ($d == 0) {
				if ($dp == 0) {
					my $vc = $y >> 13;
					printf("EAG VC=%i ", $vc);
					if ($vc == 0) {
						printf("Test=%i SID=%i ENCID=%i LTNBE=%i rfu=%i",
							($y >> 11) & 3, ($y >> 6) & 0x3f, $y & 0x3f,
							($z >> 10) & 0x3f, $z & 0x1f);
					} else {
						printf("rfu");
					}
				} elsif ($dp == 7) {
					printf("x3..0=%i rfu", $dp);
				} else {
					printf("MGS CI=%i SG=%i GSI=%i FF=%3.3x,%4.4x", $ci, $sg, $gsi, $y & 0xfff, $z);
				}
			} else {
				printf("MGF CI=%i ", $ci);
				print("+ ") if ($dir == 0);
				print("- ") if ($dir == 1);
				printf("Extent=%i Event=%i Location=%i", $extent, $event, $location);
			}
		} else {
			printf("SG DP=%i D=%i ", $t, $f, $dp, $d);
			print("+ ") if ($dir == 0);
			print("- ") if ($dir == 1);
			printf("Extent=%i Event=%i Location=%i", $extent, $event, $location);
		}
	} else {
		my $vc = $x & 0xf;
		printf("VC=%i ", $vc);
		printf("unknown") if ($vc <= 3);
		printf("SPN=%i,%i,%i,%i", ($y >> 8) & 0xff, $y & 0xff, ($z >> 8) & 0xff, $z & 0xff) if ($vc == 4);
		printf("SPN=%i,%i,%i,%i", ($y >> 8) & 0xff, $y & 0xff, ($z >> 8) & 0xff, $z & 0xff) if ($vc == 5);
		printf("AF(ON)=%i,%i PI(ON)=%4.4x", ($y >> 8) & 0xff, $y & 0xff, $z) if ($vc == 6);
		printf("TF(TN)=%i MF(ON)=%i PI(ON)=%4.4x", ($y >> 8) & 0xff, $y & 0xff, $z) if ($vc == 7);
		printf("PI(ON)=%4.4x PI(ON)=%4.4x", $y, $z) if ($vc == 8);
		printf("LTN(ON)=%i MGS(ON)=%i SID(ON)=%i PI(ON)=%4.4x", ($y >> 10) & 0x3f, ($y >> 6) & 0xf, $y & 0x3f, $z) if ($vc == 9);
		printf("unknown") if ($vc >= 10);
	}
}

sub oda_a
{
	my ($gtc, $x, $y, $z) = @_;

	return if $oda_avail[$gtc][0] == 0;

	my $aid = $oda_aid[$gtc][0];
	if ($aid == 0x0D45) { # TMC
		printf("ODAA(AID=%4.4x=TMC) ", $aid);
		&tmc_a($x, $y, $z);
	} elsif ($aid == 0x4AA1) { # Rasant
		printf("ODAA(AID=%4.4x=Rasant) msg=%2.2x,%4.4x,%4.4x", $aid, $x, $y, $z);
	} elsif ($aid == 0x4BD7) { # RT+
		printf("ODAA(AID=%4.4x=RT+) ITB=%i IRT=%i CT1= SM1= LM1= CT2= SM2= LM2=", $aid, $x >> 4, ($x >> 3) & 1,
			(($x & 7) << 3) | ($y >> 13), ($y >> 7) & 0x3f, ($y >> 1) & 0x3f,
			(($y & 1) << 5) | ($z >> 11), ($z >> 5) & 0x3f, $z & 0x1f);
	} elsif ($aid == 0x6552) { # eRT
		printf("ODAA(AID=%4.4x=eRT) C=%i ERT=%i,%i,%i,%i", $aid, $x, $y >> 8, $y & 0xff, $z >> 8, $z & 0xff);
	} elsif ($aid == 0xCD46) { # TMC
		printf("ODAA(AID=%4.4x=TMC) ", $aid);
		&tmc_a($x, $y, $z);
	} elsif ($gtc == 8) { # TMC
		printf("ODAA(AID=%4.4x=TMC) ", $aid);
		&tmc_a($x, $y, $z);
	} else {
		printf("ODAA(AID=%4.4x=unknown) msg=%2.2x,%4.4x,%4.4x", $aid, $x, $y, $z);
	}
}

sub oda_b
{
	my ($gtc, $gtv, $x, $z) = @_;

	return if $oda_avail[$gtc][1] == 0;

	my $aid = $oda_aid[$gtc][1];
	if ($aid == 0x0D45) { # TMC
		printf("ODAB(AID=%4.4x=TMC) msg=%2.2x,%4.4x", $aid, $x, $z);
	} elsif ($aid == 0x4AA1) { # Rasant
		printf("ODAB(AID=%4.4x=Rasant) msg=%2.2x,%4.4x", $aid, $x, $z);
	} elsif ($aid == 0x4BD7) { # RT+
		printf("ODAB(AID=%4.4x=RT+) msg=%2.2x,%4.4x", $aid, $x, $z);
	} elsif ($aid == 0x6552) { # eRT
		printf("ODAB(AID=%4.4x=eRT) msg=%2.2x,%4.4x", $aid, $x, $z);
	} elsif ($aid == 0xCD46) { # TMC
		printf("ODAB(AID=%4.4x=TMC) msg=%2.2x,%4.4x", $aid, $x, $z);
	} elsif ($gtc == 8) { # TMC
		printf("ODAB(AID=%4.4x=TMC) msg=%2.2x,%4.4x", $aid, $x, $z);
	} else {
		printf("ODAB(AID=%4.4x=unknown) msg=%2.2x,%4.4x", $aid, $x, $z);
	}
}

# main loop
while(<>) {
	chomp($_);
	s/\r//g;
	@rds = map(hex, split(/ /, $_));
	printf("%4.4x %4.4x %4.4x %4.4x: ", $rds[0], $rds[1], $rds[2], $rds[3]);
	my $gtc = $rds[1] >> 12;
	my $gtv = (($rds[1] >> 11) & 1);
	printf("PI=%4.4x ", $rds[0]);
	printf("GT=%i", $gtc);
	print "A " if ($gtv==0);
	print "B " if ($gtv==1);
	printf("TP=%i ", ($rds[1] >> 10) & 1);
	printf("PTY=%i ", ($rds[1] >> 5) & 0x1f);
	printf("pi2=%4.4x ", $rds[2]) if (($gtv == 1) && ($rds[0] != $rds[2]));
	my $x = $rds[1] & 0x1f;
	my $y = $rds[2];
	my $z = $rds[3];
	if (($gtc == 0) && ($gtv == 0)) {
		printf("TA=%i MS=%i DI=%i C=%i ", ($x >> 4) & 1, ($x >> 3) & 1, ($x >> 2) & 1, ($x & 3));
		printf("AF=%i,%i ", $y >> 8, $y & 0xff);
		printf("PS=%i,%i", $z >> 8, $z & 0xff);
	} elsif (($gtc == 0) && ($gtv == 1)) {
		printf("TA=%i MS=%i DI=%i C=%i ", ($x >> 4) & 1, ($x >> 3) & 1, ($x >> 2) & 1, ($x & 3));
		printf("PS=%i,%i", $z >> 8, $z & 0xff);
	} elsif (($gtc == 1) && ($gtv == 0)) {
		printf("rpc=%2.2x ", $x);
		my $vc = ($y >> 12) & 0x7;
		printf("LA=%i VC=%i ", $y >> 15, $vc);
		printf("rpopc=%i ECC=%i ", ($y >> 8) & 0xf, $y & 0xff) if ($vc == 0);
		printf("na ") if ($vc == 1);
		printf("rpi=%i ", $y & 0xfff) if ($vc == 2);
		printf("x=%i LIC=%i ", ($y >> 8) & 0xf, $y & 0xff) if ($vc == 3);
		printf("na ") if ($vc == 4);
		printf("na ") if ($vc == 5);
		printf("fubb ") if ($vc == 6);
		printf("EWSID=%i ", $y & 0xfff) if ($vc == 7);
		printf("PIN=%i,%i,%i", ($z >> 11) & 0x1f, ($z >> 6) & 0x1f, $z & 0x3f);
	} elsif (($gtc == 1) && ($gtv == 1)) {
		printf("spare=%2.2x ", $x);
		printf("PIN=%i,%i,%i", ($z >> 11) & 0x1f, ($z >> 6) & 0x1f, $z & 0x3f);
	} elsif (($gtc == 2) && ($gtv == 0)) {
		printf("AB=%i C=%i ", $x >> 4, $x & 0xf);
		printf("RT=%i,%i,%i,%i", $y >> 8, $y & 0xff, $z >> 8, $z & 0xff);
	} elsif (($gtc == 2) && ($gtv == 1)) {
		printf("AB=%i C=%i ", $x >> 4, $x & 0xf);
		printf("RT=%i,%i", $z >> 8, $z & 0xff);
	} elsif (($gtc == 3) && ($gtv == 0)) {
		&oda_assign($x >> 1, $x & 1, $y, $z);
	} elsif (($gtc == 3) && ($gtv == 1)) {
	} elsif (($gtc == 4) && ($gtv == 0)) {
		printf("spare=%i ", $x >> 2);
		printf("MJD=%i ", (($x & 3) << 15) | ($y >> 1));
		printf("H=%i M=%i LTO=", (($y & 1) << 4) | ($z >> 12), ($z >> 6) & 0x3f);
		print "+" if ((($z >> 5) & 1) == 0);
		print "-" if ((($z >> 5) & 1) == 1);
		printf("%i", $z & 0x1f);
	} elsif (($gtc == 4) && ($gtv == 1)) {
	} elsif (($gtc == 5) && ($gtv == 0)) {
		print "TDC(/ODA) ";
		printf("C=%i ", $x);
		printf("TD=%4.4x,%4.4x ", $y, $z);
	} elsif (($gtc == 5) && ($gtv == 1)) {
		print "TDC(/ODA) ";
		printf("C=%i ", $x);
		printf("TD=%4.4x", $z);
	} elsif (($gtc == 6) && ($gtv == 0)) {
		print "IH(/ODA) ";
	} elsif (($gtc == 6) && ($gtv == 1)) {
		print "IH(/ODA) ";
	} elsif (($gtc == 7) && ($gtv == 0)) {
		print "RP(/ODA) ";
		printf("AB=%i C=%i ", $x >> 4, $x & 0xf);
		printf("rp=%4.4x,%4.4x", $y, $z);
	} elsif (($gtc == 7) && ($gtv == 1)) {
	} elsif (($gtc == 8) && ($gtv == 0)) {
	} elsif (($gtc == 8) && ($gtv == 1)) {
	} elsif (($gtc == 9) && ($gtv == 0)) {
		print "EWS(/ODA) ";
	} elsif (($gtc == 9) && ($gtv == 1)) {
	} elsif (($gtc == 10) && ($gtv == 0)) {
		printf("AB=%i C=%i ", $x >> 4, $x & 0x1);
		printf("PTYN=%i,%i,%i,%i", $y >> 8, $y & 0xff, $z >> 8, $z & 0xff);
	} elsif (($gtc == 10) && ($gtv == 1)) {
	} elsif (($gtc == 11) && ($gtv == 0)) {
	} elsif (($gtc == 11) && ($gtv == 1)) {
	} elsif (($gtc == 12) && ($gtv == 0)) {
	} elsif (($gtc == 12) && ($gtv == 1)) {
	} elsif (($gtc == 13) && ($gtv == 0)) {
		print "RP(/ODA) ";
		printf("erpcs=%i STY=%i", $x >> 3, $x & 3);
		printf("erp=%4.4x,%4.4x", $y, $z);
	} elsif (($gtc == 13) && ($gtv == 1)) {
	} elsif (($gtc == 14) && ($gtv == 0)) {
		print "EON ";
		my $vc = $x & 0xf;
		printf ("TP(ON)=%i VC=%i ", $x >> 4, $vc);
		printf("PS(ON)=%i,%i ", $y >> 8, $y & 0xff) if ($vc == 0);
		printf("PS(ON)=%i,%i ", $y >> 8, $y & 0xff) if ($vc == 1);
		printf("PS(ON)=%i,%i ", $y >> 8, $y & 0xff) if ($vc == 2);
		printf("PS(ON)=%i,%i ", $y >> 8, $y & 0xff) if ($vc == 3);
		printf("AF(ON)=%i,%i ", $y >> 8, $y & 0xff) if ($vc == 4);
		printf("TF(TN)=%i MFFM(ON)=%i ", $y >> 8, $y & 0xff) if ($vc == 5);
		printf("TF(TN)=%i MFFM(ON)=%i ", $y >> 8, $y & 0xff) if ($vc == 6);
		printf("TF(TN)=%i MFFM(ON)=%i ", $y >> 8, $y & 0xff) if ($vc == 7);
		printf("TF(TN)=%i MFFM(ON)=%i ", $y >> 8, $y & 0xff) if ($vc == 8);
		printf("TF(TN)=%i MFAM(ON)=%i ", $y >> 8, $y & 0xff) if ($vc == 9);
		printf("unalloc ") if ($vc == 10);
		printf("unalloc ") if ($vc == 11);
		if ($vc == 12) {
			my $ils = ($y >> 12) & 1;
			printf("LA=%i EG=%i x=%i ILS=%i ", $y >> 15, ($y >> 14) & 1, ($y >> 13) & 1, $ils);
			if ($ils == 0) {
				printf("LSN=%3.3x ", $y & 0xfff);
			} else {
				printf("CI=%1.1x LI=%2.2x ", ($y >> 8) & 0xf, $y & 0xff);
			}
		}
		printf("PTY(ON)=%i res=%3.3x TA(ON)=%i ", ($y >> 11) & 0x1f, ($y >> 1) & 0x3ff, $y & 1) if ($vc == 13);
		printf("PIN(ON)=%i,%i,%i ", ($y >> 11) & 0x1f, ($y >> 6) & 0x1f, $y & 0x3f) if ($vc == 14);
		printf("res=%4.4x ", $y) if ($vc == 15);
		printf("PI(ON)=%4.4x", $z);
	} elsif (($gtc == 14) && ($gtv == 1)) {
		printf("TP(ON)=%i TA(ON)=%i unused=%i ", $x >> 4, ($x >> 3) & 1, $x & 7);
		printf("PI(ON)=%4.4x", $z);
	} elsif (($gtc == 15) && ($gtv == 0)) {
	} elsif (($gtc == 15) && ($gtv == 1)) {
		printf("TA=%i MS=%i DI=%i C=%i ", ($x >> 4) & 1, ($x >> 3) & 1, ($x >> 2) & 1, ($x & 3));
		printf("GT=%i", ($z >> 12) & 0xf);
		print "A " if ((($z >> 11) & 1) == 0);
		print "B " if ((($z >> 11) & 1) == 1);
		printf("TP=%i PTY=%i TA=%i MS=%i DI=%i C=%i", ($z >> 10) & 1, ($z >> 5) & 0x1f, ($z >> 4) & 1, ($z >> 3) & 1, ($z >> 2) & 1, ($z & 3));
	}
	&oda_a($gtc, $x, $y, $z) if ($gtv == 0);
	&oda_b($gtc, $x,     $z) if ($gtv == 1);
	printf("\n");
}
