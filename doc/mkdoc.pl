#!/usr/bin/perl

use strict;
use warnings;

package Page;

sub new
{
	my ($class) = @_;

	my $self = bless {}, $class;

	$self->discard_data();

	return $self
}

sub discard_data
{
	my ($self) = @_;

	my $buffer = '';

	@$self{qw(buffer)} = (\$buffer)
}

sub contents
{
	return ${$_[0]->{buffer}}
}

sub write
{
	my ($self, $data) = @_;

	${$self->{buffer}} .= $data
}

sub write_markup
{
	my ($self, $markup) = @_;

	$markup =~ s/^\t+//mgo;

	$self->write($markup)
}

sub send
{
	my ($self, $stream) = @_;

	$stream = *STDOUT unless $stream;

	binmode $stream;

	binmode $stream, ':utf8';

	print $stream ${$self->{buffer}}
}

sub start_tag
{
	my ($self, $tag, %attributes) = @_;

	$self->write("<$tag\n");

	while (my ($key, $value) = each %attributes)
	{
		$self->write("$key=\"$value\"\n")
	}

	$self->write('>');

	return bless {buffer => $self->{buffer}, tag => $tag, outer => $self},
		'HTMLElement'
}

package HTMLElement;

our @ISA = qw(Page);

sub DESTROY
{
	my ($self) = @_;

	$self->write("</$self->{tag}\n>")
}


package main;

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
