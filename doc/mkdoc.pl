#!/usr/bin/perl

# B: Common C++ library
# Copyright (C) 2002-2007, 2016-2020 Damon Revoe
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

	my $self = bless {buffer_ref => $buffer_ref, stack => []}, $class;

	$self->push_tag($tag, %attributes);

	return $self
}

sub contents
{
	return ${$_[0]->{buffer_ref}}
}

sub write
{
	my ($self, $markup) = @_;

	${$self->{buffer_ref}} .= $markup
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

sub write_text
{
	my ($self, $text) = @_;

	$text =~ s/</&lt;/g;
	$text =~ s/>/&gt;/g;

	${$self->{buffer_ref}} .= $text
}

sub DESTROY
{
	my ($self) = @_;

	$self->close_tag()
}

# }}} (package HTML::Element)

package HTML::Page; # {{{

our @ISA = qw(HTML::Element);

use File::Basename;
use File::Path;

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

sub save
{
	my ($self, $pathname) = @_;

	mkpath(dirname($pathname));

	open FILE, '>', $pathname or die "$pathname\: $!";

	$self->print_page(*FILE);

	close FILE
}

# }}} (package HTML::Page)

package CppHeader; # {{{

sub new
{
	my ($class, $relative_pathname, $file_contents) = @_;

	unless ($relative_pathname =~ m/.h$/so)
	{
		warn "$relative_pathname - skipped: not a header file\n";
		return undef
	}

	unless ($file_contents =~ s,^/\*.*Copyright.*?\*/\s+,,so)
	{
		warn "$relative_pathname - skipped: no copyright statement\n";
		return undef
	}

	my $self = bless
	{
		relative_pathname => $relative_pathname,
		contents => $file_contents
	}, $class;

	return $self
}

# }}} (package CppHeader)

package main; # {{{

use File::Basename;
use File::Find;
use File::Spec;

die "Usage: $0 OUTPUT_DIR\n" if @ARGV != 1;

my ($output_dir) = @ARGV;

my $top_srcdir = dirname($0);
die if basename($top_srcdir) ne 'doc';
$top_srcdir = dirname($top_srcdir);

my $include_dir = File::Spec->catdir($top_srcdir, 'include', 'b');

my @headers;

sub load_header
{
	return unless -f;

	my $relative_pathname =
		File::Spec->abs2rel($File::Find::name, $include_dir);

	open my $fh, '<', $File::Find::name or die;
	local $/ = undef;
	my $contents = <$fh>;
	close $fh;

	my $header = CppHeader->new($relative_pathname, $contents);

	push @headers, $header if $header
}

find({wanted => \&load_header, no_chdir => 1}, $include_dir);

foreach my $cpp_header (@headers)
{
	my $pathname = $cpp_header->{relative_pathname};
	my $header_page = HTML::Page->new('The B Library - ' . $pathname);

	$header_page->push_tag('body');
	$header_page->push_tag('pre');
	$header_page->write_text($cpp_header->{contents});

	$header_page->save(File::Spec->catfile($output_dir, "$pathname.html"))
}

my $readme_pathname = File::Spec->catfile($top_srcdir, 'README.md');

open README, '<', $readme_pathname or die "Cannot open $readme_pathname\: $!";

my $index_page = HTML::Page->new('The B Library');

$index_page->push_tag('body');
$index_page->push_tag('pre');

while (<README>)
{
	$index_page->write_text($_)
}

close README;

$index_page->save(File::Spec->catfile($output_dir, 'index.html'));

# }}}

# vim600:foldmethod=marker
