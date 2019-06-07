#use strict;
use File::Basename;

my $out_dir = $ARGV[2];

my $make_file = "../GCC/Makefile_ubin.src";
my $ubin_obj_list = "$out_dir/decoupling/ubin_obj_list_generator.txt";

my @input_array;
my @interim_array;
my @output_array;
my %file_path;
my $is_reading;
my $file_name;

#my $CHECK_DEP = $ARGV[0];
#print "CHECK_DEP = ".$CHECK_DEP."\n";

open(sFile, "<$make_file") or die "[ERROR] can't open $make_file!\n";
@input_array = <sFile>;
close(sFile);

$is_reading = 0;
foreach my $input_line (@input_array)
{
    if ($is_reading == 1)
    {
	if ($input_line !~ /\\[\s]+$/)
	{
	    $is_reading = 0;
	}
	if ($input_line =~ /([^\s]+)[\s]+/)
	{
	    push @interim_array, $1;
	}
	if ($input_line =~ /\$\((.*)\)/)
	{
	    $file_path{$1} = $1;
	}
    }
    else
    {
	if ($input_line =~ /^[\s]*\bUBIN_FILES\b[\s]*=[\s]*([^\s]+)[\s]+/ || 
	    $input_line =~ /^[\s]*\bUBIN_FILES\b[\s]*\+=[\s]*([^\s]+)[\s]+/ ||
            $input_line =~ /^[\s]*\bUBIN_CXX_FILES\b[\s]*=[\s]*([^\s]+)[\s]+/ ||
            $input_line =~ /^[\s]*\bUBIN_CXX_FILES\b[\s]*\+=[\s]*([^\s]+)[\s]+/ ||
            $input_line =~ /^[\s]*\bUBIN_LIBS\b[\s]*=[\s]*([^\s]+)[\s]+/ || 
	          $input_line =~ /^[\s]*\bUBIN_LIBS\b[\s]*\+=[\s]*([^\s]+)[\s]+/)
	{
	    $file_name = $1;
	    if ($input_line =~ /^[\s]*\bUBIN_FILES\b[\s]*=/ || 
                $input_line =~ /^[\s]*\bUBIN_CXX_FILES\b[\s]*=[\s]*([^\s]+)[\s]+/ ||
                $input_line =~ /^[\s]*\bUBIN_LIBS\b[\s]*=/)
	    {
	        @interim_array = ();
	    }				
	    if ($input_line =~ /(\\[\s]+)$/)
	    {
	        $is_reading = 1;
	    }					
	    if ($file_name =~ /\$\((.*)\)/)
	    {
	        $file_path{$1} = $1;
	    }
            #printf "put obj name in array\n";	
	    push @interim_array, $file_name;
	}
    }
}


foreach my $input_line (@input_array)
{
    if ($input_line =~ /^([\s]*\b[\w]+\b)[\s]*=[\s]*([^\s]+)[\s]+/)
    {
        if (exists $file_path{$1})
	{
	    $file_path{$1} = $2;
	}
    }
}

foreach my $interim_line (@interim_array)
{
    if ($interim_line =~ /^[\s]*\$\((.*)\)/)
    {
        my $replace_path = $1;
	$interim_line =~ s/\$\($replace_path\)/$file_path{$replace_path}/
    }
    push @output_array, $interim_line;
}


my $DIR = dirname($ubin_obj_list);
if(! -d "$DIR")
{
	system ("mkdir -p $DIR");
}
open(outputFile, ">$ubin_obj_list") or die "[ERROR] can't open $ubin_obj_list!\n";
foreach my $output_line (@output_array)
{
    $output_line =~ s/\.c/\.o/;
    $output_line =~ s/\.opp/\.o/;
    print outputFile "Build/"."$output_line"."\n";
    #printf $output_line."\n";
}
close(outputFile);
system ("perl ../decoupling/cross_reference_parser.pl $ARGV[0] $ARGV[1] $out_dir");
