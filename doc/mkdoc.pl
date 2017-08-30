#!/usr/bin/perl

use strict;
use warnings;

package HTML::Element;

sub new
{
	my ($class, $buffer_ref, $tag, %attributes) = @_;

	my $self = bless
	{
		buffer_ref => $buffer_ref,
		stack => []
	}, $class;

	$self->push_tag($tag, %attributes);

	return $self
}

sub contents
{
	return ${$_[0]->{buffer_ref}}
}

sub write
{
	my ($self, $data) = @_;

	${$self->{buffer_ref}} .= $data
}

sub write_markup
{
	my ($self, $markup) = @_;

	$markup =~ s/^\t+//mgo;

	$self->write($markup)
}

sub push_tag
{
	my ($self, $tag, %attributes) = @_;

	$self->write("<$tag\n");

	while (my ($key, $value) = each %attributes)
	{
		$self->write("$key=\"$value\"\n")
	}

	$self->write('>');

	push @{$self->{stack}}, $tag
}

sub open_tag
{
	my ($self, $tag, %attributes) = @_;

	return HTML::Element->new($self->{buffer_ref}, $tag, %attributes)
}

sub close_tag
{
	my ($self) = @_;

	while (my $tag = pop @{$self->{stack}})
	{
		$self->write("</$tag\n>")
	}
}

sub DESTROY
{
	my ($self) = @_;

	$self->close_tag()
}

package HTML::Page;

our @ISA = qw(HTML::Element);

sub new
{
	my ($class) = @_;

	my $buffer = "<!DOCTYPE html>\n";

	return $class->SUPER::new(\$buffer, 'html', lang => "en")
}

sub print_page
{
	my ($self, $stream) = @_;

	$self->close_tag();

	$stream = *STDOUT unless $stream;

	binmode $stream;
	binmode $stream, ':utf8';

	print $stream ${$self->{buffer_ref}};
	print $stream "\n"
}

package main;

my $top_srcdir = $ARGV[0] or die "Usage: $0 \$top_srcdir\n";


my $readme_pathname = "$top_srcdir/README";

open README, '<', $readme_pathname or die "Cannot open $readme_pathname\: $!";

my $index_page = HTML::Page->new();

$index_page->open_tag('head')->open_tag('title')->write('The B Library');

$index_page->push_tag('body');
$index_page->push_tag('pre');

while (<README>)
{
	s/</&lt;/g;
	s/>/&gt;/g;

	$index_page->write($_)
}

close README;

mkdir 'html';

my $index_pathname = 'html/index.html';

open INDEX, '>', $index_pathname or die "Cannot open $index_pathname\: $!";

$index_page->print_page(*INDEX);

close INDEX
