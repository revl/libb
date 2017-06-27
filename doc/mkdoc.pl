#!/usr/bin/perl

use strict;
use warnings;

my $top_srcdir = $ARGV[0] or die "Usage: $0 \$top_srcdir\n";


my $readme_pathname = "$top_srcdir/README";

open README, '<', $readme_pathname or die "Cannot open $readme_pathname\: $!";

mkdir 'html';

my $index_pathname = 'html/index.html';

open INDEX, '>', $index_pathname or die "Cannot open $index_pathname\: $!";

print INDEX <<HTML;
<!DOCTYPE html>
<html
lang="en"
><head
><title
>The B Library</title
></head
><body
><pre>
HTML

while (<README>)
{
	s/</&lt;/g;
	s/>/&gt;/g;
	print INDEX $_
}

print INDEX <<HTML;
</pre
></body
></html
>
HTML

close INDEX;

close README
