#!/usr/bin/perl
use strict;

sub shuffle {
	my $array = shift;
	my $i;
	for ($i = @$array; --$i; ) {
		my $j = int rand ($i + 1);
		next if $i == $j;
		@$array[$i,$j] = @$array[$j,$i];
	}
}

# Splits an input seedFile into count subfiles.
# subfiles are created in /tmp and the subfiles have
# filenames created by appending _n to the seedFile name 

# Usage: split.p seedFile count

my $cmd;
my $fname = $ARGV[0];
my $splitCount = $ARGV[1];
printf "file = %s\n", $fname;
printf "count = %s\n", $splitCount;


open FH, $fname or die "Can't open $fname";

my $firstLine = <FH>;	
my ($seedDim, $count) = split /\s+/, $firstLine;
printf "sdiim = %s\n", $seedDim;
printf "count = %s\n", $count;

my @lines = <FH>; 
shuffle(\@lines);

my ($i, $j);

my @subFileLines;

for ($j = 0; $j < $splitCount; $j++)
{
	$subFileLines[$j] = [];
}

my $lineCount = $#lines;
for ($i = 0; $i <= $lineCount ; $i++)
{
	$j = $i % $splitCount;
        push @{$subFileLines[$j]} , shift @lines;
}	

$cmd = sprintf "\/bin\/rm -f /tmp/%s_\*", $fname;
my $status = system ($cmd);
die "$cmd exited with error: $?" unless $status == 0;
for ($j = 0; $j < $splitCount; $j++)
{
	my @array = @{$subFileLines[$j]};
	shuffle(\@array);
	printf "Array Size = %d\n", $#array + 1;
        my $outFileName = sprintf "/tmp/%s_%05d",$fname, $j+1;
	open(OUTPUT, ">$outFileName");
	printf OUTPUT "%s %d\n", $seedDim, $#array+1;
	my $line;
	foreach $line (@array)
	{
		printf OUTPUT "%d\n", $line;
	}
	close (OUTPUT);
}

open (OUTPUT, ">test.sh");
for ($j = 0; $j < $splitCount; $j++)
{
	printf OUTPUT "./th2 \$1 /tmp/%s_%d > a\$1_%d &\n", 
		$fname,  $j+1, $j+1;
}	
printf OUTPUT "wait\n";

close (OUTPUT);

