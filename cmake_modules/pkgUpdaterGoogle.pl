#!/usr/bin/perl

######
# Author: Ding-Yi Chen <dchen@redhat.com>
# License: BSD
#
# Idea from send2goog, but without dumping config at $HOME

use strict;
use Getopt::Long;
use LWP::UserAgent;
use utf8;
binmode(STDIN, ':encoding(utf8)');
binmode(STDOUT, ':encoding(utf8)');
binmode(STDERR, ':encoding(utf8)');

my $ua = new LWP::UserAgent;					# web browser object
our ($user, $pass, $project, $summary, $filePattern);				# we use "our" so we can refer to them as $$_ in a loop

my (@labels, $batch);

###
# most of the code revolves around getting/setting config options
# if you want to just look at the upload part, search for "upload part" below
###

GetOptions("user=s"=>\$user, "pass=s"=>\$pass, "summary=s"=>\$summary,
    "label=s"=>\@labels, "filePattern=s"=>\$filePattern) || usage();

$project = shift @ARGV if (@ARGV > 1);
my $file = shift @ARGV;
usage("Must specify file") if !$file;
my $fkey = $file;
$fkey =~ s/.*\///;
$fkey =~ s/\b\d+\b/#/g;

for (qw(project user pass summary filePattern)) {
    no strict 'refs';
    no warnings 'uninitialized';
    $$_ = prompt($_ ) if !$$_;					# prompt for value
}

if (!@labels) {								# not in args?
    print("NO LABEL");
    while (my $lab = prompt("label") ){
	push @labels, $lab;
    }
}


for (qw(project user pass summary filePattern)) {
    no strict 'refs';
    usage("Need $_") if (!$$_);					# die unless all vars are defined
}

###
# Deprecated present package
my $downloadlist = "http://code.google.com/p/ibus/downloads/list";
my $res=$ua->get($downloadlist);
if ($res->is_success) {
    my @matches = ($res->decoded_content =~ m/name="($filePattern)" title="Download"/g);
}else {
     die $res->status_line;
}


@labels = map {('label', $_)} @labels;					# put the word 'label' before each
my @labelsWithDeprecated = (@labels, "label", "Deprecated");

my $url = "https://$user:$pass\@$project.googlecode.com/files";
# yep, that's it, one function does it all
my $res = $ua->post($url,
	Content_Type => 'form-data',
	Content => [ summary=>$summary,
		     filename=>[$file],
		     @labelsWithDeprecated
		   ]
);
process_web_result/./////;

###
# finally the "upload part"
###
my $res = $ua->post($url,
    Content_Type => 'form-data',
    Content => [ summary=>$summary,
    filename=>[$file],
    @labels
    ]
);
process_web_result;

# show the results and explicitly call exit with success/fail

sub process_web_result {
    if ($res->is_success) {
	print $res->content, "\n";
    } else {
	print STDERR $res->status_line, "\n";
	exit 1;
    }
}


sub prompt {
    print "Enter ". $_[0] . ":" ;			# show the prompt
    my $r = <STDIN>;
    chomp($r);
    return $r;
}

sub usage {
    my $msg = "$_[0]\n\n" if $_[0];		# put the reason on the first line, if any

    no warnings 'uninitialized';

    die <<EOF;
${msg}USAGE: $0 [options] <project> <file>

Sends file specified by <file> to google code.

Options are:
 -f[ilePattern] file pattern for searching old released files.
 -l[abel]     label (can specify more than 1 time)
 -u[ser]      username
 -p[ass]      password
 -s[ummary]     file summary/description
EOF
}



