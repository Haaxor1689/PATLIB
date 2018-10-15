#! /usr/bin/perl -w
# $Id: opgwrap.pl,v 1.2 2001/11/21 17:53:36 antos Exp $

if ($#ARGV != 4 || $ARGV[0] eq "--help") {
  print "\nThis is OPGwrapper; wrapper for OPATGEN\n";
  print "Written by David Antos, part of the OPATGEN suite.\n";
  print "(c) 2001, David Antos. This is free software.\n";
  print "This is a hacker's tool ;-)\n";
  print "Call me with just 5 parameters:\n";
  print "  extra     arguments for OPATGEN (possibly empty)\n";
  print "  dic       name of the DICtionary file\n";
  print "  pat       the PATtern base filename\n";
  print "  tra       the TRAnslate file\n";
  print "  log       log file base name\n";
  print "\n";
  print "Then I read the standard input and first I read the starting level number.\n";
  print "Following lines contain values for pat_start, pat_finish, good_wt,\n";
  print "bad_wt, and thresh for the starting level and following ones.\n";
  print "Then I call OPATGEN with patterns to read in from previous level\n";
  print "and to write to current one, until the end of STDIN.\n";
  print "The transcription of the session is written to file log.level\n";
  print "\n";
  print "For example: opgwrap \"\" d p t l\n";
  print "with input 1<CR>3 4 1 5 10<CR>\n";
  print "starts opatgen d p.0 p.1 t with level range 1, 1; length range 3, 4;\n";
  print "and parameters 1, 5, 10. Then it hyphenates the word list.\n";
  print "The session is logged into l.1\n";
  exit 0;
}

my $extra = $ARGV[0];
my $dic = $ARGV[1];
my $pat = $ARGV[2];
my $tra = $ARGV[3];
my $log = $ARGV[4];

# The startlevel and level definitions are on standard input

# Let's read the starting level, we'll increase the number
my $curlevel = <STDIN>;
chomp $curlevel;

while ($levelpars = <STDIN>) {
  chomp $levelpars;

  # The command line is opatgen extra-switches dic pat.n-1 pat.n tra and
  # all goes copied to log.n
  $commandline  = "opatgen ".$extra." ".$dic." ";
  $commandline .= $pat.".".($curlevel-1)." ";
  $commandline .= $pat.".".$curlevel." ";
  $commandline .= $tra." ";
  $commandline .= " | tee ".$log.".".$curlevel;
  
  print "WRAPPER: Running command \n  ".$commandline."\n";

  # The interactive parameters are level, level, parameters, y for
  # hyphenate word list
  $levelpars = $curlevel." ".$curlevel." ".$levelpars." y";

  print "  with parameters ".$levelpars."\n\n";

  open (OPG, '| '.$commandline)
    or die "WRAPPER ERROR: Cannot run $commandline\n";
  print OPG $levelpars."\n";
  close OPG;

  $curlevel++; # The next level
}
