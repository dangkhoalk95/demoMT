#use strict;
use File::Basename;

my $CHECK_DEP = $ARGV[0];
my $PROJ_NAME = $ARGV[1];
my $out_dir = $ARGV[2];

my $map_file = "$out_dir/".$PROJ_NAME.".map";
#my $map_file = "test.txt";
my $symbol_file = "$out_dir/symbol_list.txt"; 
my %ubin_obj_list; 
my $ubin_obj_parse_result = "$out_dir/decoupling/ubin_obj_list_generator.txt";
my $keep_symbol_file = "$out_dir/decoupling/mbin_keep_sym.txt";
my $keep_symbol_file_output = "../GCC/mbin_keep_sym.txt";
my $keep_symbol_mak = "../GCC/mbin_keep_sym.mk";
my %keep_symbol_hash;

print "CHECK_DEP = ".$CHECK_DEP."\n";

#=========== main flow==========
printf "read map file \n";
my @ubinData;
open(mFile, "<$ubin_obj_parse_result") or die "[ERROR] can't open $ubin_obj_parse_result!\n";
@ubinData = <mFile>;
close(mFile);
foreach my $obj_name (@ubinData)
{
    $obj_name =~ s/([\s]+)$//;
    $ubin_obj_list{$obj_name} = $obj_name;
}

# read map file and symbol file
printf "read map file \n";
my @mapData;
open(mFile, "<$map_file") or die "[ERROR] can't open $map_file!\n";
@mapData = <mFile>;
close(mFile);

printf "read symbol file \n";
my @symbolData;
open(sFile, "<$symbol_file") or die "[ERROR] can't open $symbol_file!\n";
@symbolData = <sFile>;
close(sFile);

#delete outdated file
unlink "$out_dir/decoupling/m_reference_u_data.txt";
unlink "$out_dir/decoupling/m_reference_u_func.txt";
#unlink $keep_symbol_file;

#parse map flie
my $cross_ref_begin = 0;
my $symbol_to_parse;
my $symbol_to_keep;
my $symbol_def_file;
my $symbol_ref_file;
my $symbol_in_ubin;
my $ref_file_in_ubin;
my $symbol_type;
my $symbol_output_file;
foreach my $line (@mapData)
{
    if ($cross_ref_begin == 0)
    {
        if ($line =~ /Cross Reference Table/)
        {
    	    printf "cross reference table begin\n";
    	    $cross_ref_begin = 1;
    	}
    }
    else
    {
	if ($line =~ /(\w+\:\:)(\w+)(\(.*\))\s+([^\s]+\/\w+\.[o|a])/)
	{
            #printf "case 1:\n";
	    $symbol_to_parse = $1.$2.$3;
	    $symbol_to_keep = $2;
	    $symbol_type = get_symbol_type($symbol_to_keep, 1);

	    parse_def_file($4);
	}
	elsif ($line =~ /(\w+)(\(.*\))\s+([^\s]+\/\w+\.[o|a])/)
	{
            #printf "case 2:\n";
	    $symbol_to_parse = $1.$2;
	    $symbol_to_keep = $1;
	    $symbol_type = get_symbol_type($symbol_to_keep, 1);

	    parse_def_file($3);
	}
        elsif ($line =~ /(\w+)\s+([^\s]+\/\w+\.[o|a])/)
    	{
            #printf "case 3:\n";
    	      $symbol_to_parse = $1;
	          $symbol_to_keep = $1;
    	      $symbol_type = get_symbol_type($symbol_to_parse, 0);
    				
	          parse_def_file($2);
            #printf "symbol_def_file: ".$2."\n";
    	}
    	elsif ($line  =~ /^\s+([^\s]+\/\w+\.[o|a])/)
    	{
            #printf "case 4:\n";
	          $symbol_ref_file = $1;
            #printf "symbol_ref_file: ".$symbol_ref_file."\n";
	          #if (exists $ubin_obj_list{$symbol_ref_file})
	          
	          parse_ref_file($symbol_ref_file);
	          if ($ref_file_in_ubin == 1)
	          {
	              #printf "ref file in ubin\n";
	              if ($symbol_in_ubin == 1)
		            {
		                next;
		            }
	              else
		            {
		                #printf "keep some symbol 1\n";
		                if ($symbol_type == 1 || $symbol_type == 2)
		                {
		                    #printf "keep some symbol 2\n";
			                  $keep_symbol_hash{$symbol_to_keep} = $symbol_to_keep;
		                }
		            }
	          }
	          else
	          {
		            if ($symbol_in_ubin == 1)
		            {
                    #print "mbin ref ubin case\n";
		                if ($symbol_type == 1)
		                {	
		                    #printf "referenced symbol is data\n";
		    	              $symbol_output_file = "$out_dir/decoupling/m_reference_u_data.txt";
		                }
		                elsif ($symbol_type == 2)
	                  {
	                      #printf "referenced symbol is func\n";
			                  $symbol_output_file = "$out_dir/decoupling/m_reference_u_func.txt";
		                }
		                format_output_symbol_to_file($symbol_output_file);
	              }
		            else
		            {
		                next;
		            }
	          }
    	}
	else
	{
	    next;
	}
    }
}

if (!($CHECK_DEP =~ /prebuild/))
{
    print "keep symbol enter.\n";
    my $DIR = dirname($keep_symbol_file);
    if(! -d "$DIR")
    {
        system ("mkdir -p $DIR");
    }
    open(outputFile, ">$keep_symbol_file") or die "[ERROR] can't open $keep_symbol_file!\n";
    open(outputFile_ks, ">$keep_symbol_mak") or die "[ERROR] can't open $keep_symbol_file!\n";
    foreach my $keeped_symbol (keys %keep_symbol_hash)
    {
        print outputFile $keeped_symbol."\n";
        print outputFile_ks "LDFLAGS += -u ".$keeped_symbol."\r\n";
    }
    close(outputFile);
    close(outputFile_ks);
    system ("cp $keep_symbol_file $keep_symbol_file_output");
}

system ("perl ../decoupling/symbol_by_source_file.pl $out_dir");
#=========== sub flow==========

#subroutine to judge a symbol is a variable or a function
sub get_symbol_type()
{
    my $symbol = $_[0];
    my $is_c_plus_plus = $_[1];
    foreach my $line (@symbolData)
    {
            if ($is_c_plus_plus == 0 && $line =~ /[^A-Z]+([A-Z]+)\s+.+\s+\b$symbol\b\s+/
                ||$line =~ /[^A-Z]+([A-Z]+)\s+.+\s+\w*$symbol\w*\s+/)
	    {
	        if ($1 =~ /OBJECT/)
	        {
	        #printf "symbol is object\n";
		    return 1;
	        }
	        elsif ($1 =~ /FUNC/)
	        {
	        #printf "symbol is func\n";
		    return 2;
	        }
	        return 0;
	    }
    }
    return 3;
}

sub parse_def_file()
{
    $symbol_def_file = $_[0];
    $symbol_in_ubin = 0;
    #print "symbol def file is: ".$symbol_def_file."\n";
    

    
    foreach my $ubin_obj_item (keys %ubin_obj_list)
    {    
        #printf "ubin item:  ".$ubin_obj_item."\n";
        if ($symbol_def_file =~ /$ubin_obj_item/)
        {
            #print "symbol def in ubin, source type\n";
            $symbol_in_ubin = 1;
        }
        elsif ($ubin_obj_item =~ /$symbol_def_file/)
        {
            #print "symbol def in ubin, lib type\n";
            $symbol_in_ubin = 1;
        }
    }
}

sub parse_ref_file()
{
    $symbol_ref_file = $_[0];
    $ref_file_in_ubin = 0; 
    foreach my $ubin_obj_item (keys %ubin_obj_list)
    {    
        if ($symbol_ref_file =~ /$ubin_obj_item/)
        {
            #printf "symbol ref in ubin, source type\n";
            $ref_file_in_ubin = 1;
        }
        elsif ($ubin_obj_item =~ /$symbol_ref_file/)
        {
            #printf "symbol ref in ubin, lib type\n";
            $ref_file_in_ubin = 1;
        }
    }
}

sub format_output_symbol_to_file()
{
    my $output_file_type = $_[0];
    #printf "symbol def file: ".$symbol_def_file."\n";
    #printf "symbol ref file: ".$symbol_ref_file."\n";
    #printf "symbol to parse: ".$symbol_to_parse."\n";
    my $DIR = dirname($output_file_type);
    if(!-d "$DIR")
    {
        system ("mkdir -p $DIR");
    }
    open(outputFile, ">>$output_file_type") or open(outputFile, ">$output_file_type");
    print outputFile $symbol_def_file."\t\t".$symbol_ref_file."\t\t".$symbol_to_parse."\n";
    close(outputFile);
}
