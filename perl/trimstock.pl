#!/usr/bin/env perl -w

BEGIN {
    use FindBin;
    use lib $FindBin::Bin;
    push @INC, $FindBin::Bin;
}

my $usage = "Usage: $0 <minscore> <Stockholm alignments...>";
my $s = shift;
defined($s) or die $usage;

while (<>) {
    my @f= ($_);
    while (<>) {
	push @f, $_;
	last if/^\s*\/\//;
    }
    my $a = "@f";
    if ($a =~ /\#=GF\s+SC\s+(\S+)/) {
	print @f if $1 > $s;
    }
}
