#!/usr/bin/perl
use warnings;
use strict;
use Getopt::Long;
use Pod::Usage;

my $n = 0;
my $q;
my $max = 0;
my (@char_hex, @char, @count);
my $help;
my $man;
my $no_zero;

GetOptions("no-zero" => \$no_zero, 'help|?' => \$help, man => \$man) or pod2usage(2);
pod2usage(1) if $help;
pod2usage(-exitval => 0, -verbose => 2) if $man;
pod2usage("$0: No files given.")  if ((@ARGV == 0) && (-t STDIN));

if (!defined $ARGV[0]) {
  while (<STDIN>) {
    chomp;
    # Handle special case of space character (0x20   count)
    if (/^(0x20)\s+(\d+)$/) {
      $char_hex[$n] = $1;
      $char[$n] = ' ';
      $count[$n] = $2;
    } else {
      ($char_hex[$n], $char[$n], $count[$n]) = split /\s+/, $_, 3;
    }

    if ($max < $count[$n]) {
      $max = $count[$n];
    }
    $n++;
  }

  for ($q = 1; $max/$q >= 6000; $q++) { }

  for (my $i = 0; $i < $n; $i++) {
    if ($no_zero && ($count[$i]/$q) < 1) {
      next;
    }
    printf "%s %u\n", $char[$i], $count[$i]/$q;
  }
}
else {
  my @err;

  my $sum = 0;
  open FH, "<".$ARGV[0];
  while (<FH>) {
    my ($char, $count) = split /\s+/, $_, 2;
    $sum += $count;
  }
  close FH;

  my $sum2 = 0;
  while (<STDIN>) {
    chomp;
    # Handle special case of space character (0x20   count)
    if (/^(0x20)\s+(\d+)$/) {
      $char_hex[$n] = $1;
      $char[$n] = ' ';
      $count[$n] = $2;
    } else {
      ($char_hex[$n], $char[$n], $count[$n]) = split /\s+/, $_, 3;
    }

    $sum2 += $count[$n];
    $n++;
  }
  for (my $i = 0; $i < $n; $i++) {
    my $x = $count[$i]*$sum/$sum2;
    $count[$i] = int $x;
    $err[$i] = $count[$i] - $x;
  }
  $sum2 = 0;
  for (my $i = 0; $i < $n; $i++) {
    $sum2 += $count[$i];
  }

  while ($sum2 != $sum) {
    $max = 0;
    for (my $i = 0; $i < $n; $i++) {
      if ($err[$i]*($sum - $sum2) > $err[$max]*($sum - $sum2)) {
        $max = $i;
      }
    }
    $count[$max] += $sum2 > $sum ? -1 : 1;
    $err[$max] -= $sum2 > $sum ? -1 : 1;
    $sum2 += $sum2 > $sum ? -1 : 1;
  }

  for (my $i = 0; $i < $n; $i++) {
    if ($no_zero && ($count[$i]) < 1) {
      next;
    }
    printf "%s %u\n", $char[$i], $count[$i];
  }
}

__END__
=head1  normalize.pl

normalize.pl - Useful for producing CHARSET.base files, since the weights must fit
               into unsigned short int:

=head1 SYNOPSIS

normalize.pl [options] [EXISTING_COUNT] < [RAW_COUNT]

 Options:
   -help            brief help message
   -no-zero         Skip characters with zero counts

=head1 OPTIONS

=over 8

=item B<-help>

Print a brief help message and exits.

=item B<-man>

Prints the manual page and exits.

=back

=head1 DESCRIPTION

B<This program> will read the given input file(s) and do something
useful with the contents thereof.

=cut

