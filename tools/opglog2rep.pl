#! /usr/bin/perl -w
# $Id: opglog2rep.pl,v 1.1 2001/11/21 17:53:36 antos Exp $

if ($#ARGV != 2 || $ARGV[0] eq "--help") {
  print "\nThis is report maker for OPATGEN log files.\n";
  print "Written by David Antos, part of the OPATGEN suite.\n";
  print "(c) 2001, David Antos. This is free software.\n";
  print "This is a hacker's tool ;-)\n";
  print "Call me with just 3 parameters:\n";
  print "  log       log file base name\n";
  print "  start     starting level number\n";
  print "  out       output for the report\n";
  print "I'll then take the files log.start, log.(start+1), etc. till they exist\n";
  print "and write to the out file their last 3 lines containing the summary report\n";
  print "provided the word list has been hyphenated in each pass ;)\n";
  exit 0;
}

my $log = $ARGV[0];
my $level = $ARGV[1];
my $out = $ARGV[2];

open(OUT, ">$out") or die("File $out can't be opened for writing\n");

while (-f $log.".".$level) {
  $file = $log.".".$level;
  
  print OUT "Level $level\n";
  
  open (L, "tail -n3 ".$file."| ");
  while ($l = <L>) {
    print OUT $l;
  }
  close L;
  
  $level++;
}
