#!/usr/bin/perl

# B: Common C++ library
# Copyright (C) 2002-2007, 2016-2019 Damon Revoe
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#

use strict;
use warnings;

package HTML::Element; # {{{

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

# }}}

package HTML::Page; # {{{

our @ISA = qw(HTML::Element);

sub new
{
	my ($class, $title) = @_;

	my $buffer = "<!DOCTYPE html>\n";

	my $self = $class->SUPER::new(\$buffer, 'html', lang => "en");

	$self->open_tag('head')->open_tag('title')->write($title);

	return $self
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

# }}}

package main;

die "Usage: $0 \$top_srcdir doc/html\n" if @ARGV != 2;

my ($top_srcdir, $output_dir) = @ARGV;

my $readme_pathname = "$top_srcdir/README";

open README, '<', $readme_pathname or die "Cannot open $readme_pathname\: $!";

my $index_page = HTML::Page->new('The B Library');

$index_page->push_tag('body');
$index_page->push_tag('pre');

while (<README>)
{
	s/</&lt;/g;
	s/>/&gt;/g;

	$index_page->write($_)
}

close README;

my $index_pathname = "$output_dir/index.html";

open INDEX, '>', $index_pathname or die "$index_pathname\: $!";

$index_page->print_page(*INDEX);

close INDEX

# vim600:foldmethod=marker
