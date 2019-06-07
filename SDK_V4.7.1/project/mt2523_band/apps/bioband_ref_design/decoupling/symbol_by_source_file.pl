#use strict;
use File::Basename;

my $out_dir = $ARGV[0];

my %def_file_hash;
my %ref_file_hash;
my %def_symbol_hash;

my @symbolData;

my $cross_file_func = "$out_dir/decoupling/m_reference_u_func.txt";
my $cross_file_data = "$out_dir/decoupling/m_reference_u_data.txt";
my $symbol_list_of_file = "$out_dir/decoupling/symbol_list_of_file.txt";
my $all_symbols = "$out_dir/decoupling/all_symbols.txt";

#my $cross_file_func = "test.txt";
#my $cross_file_data = "m_reference_u_data.txt";
#my $symbol_list_of_file = "symbol_list_of_file.txt";
#my $all_symbols = "all_symbols.txt";

my @all_symbols_array;
my $output_length = 40;

my $result = "kernel/FreeRTOS/Source/tasks.c";

cross_reference_final_parse($cross_file_data);
cross_reference_final_parse($cross_file_func);
output_symbol_to_file();
#unlink "$out_dir/decoupling/m_reference_u_data.txt";
#unlink "$out_dir/decoupling/m_reference_u_func.txt";
#unlink "$out_dir/decoupling/ubin_obj_list_generator.txt";

#system ("perl ../decoupling/template_generator.pl");

sub cross_reference_final_parse()
{
    my $file_to_parse = $_[0];
    if (open(sFile, "<$file_to_parse"))
    {
        @symbolData = <sFile>;
	close(sFile);
	foreach my $line (@symbolData)
	{
	    if ($line =~ /([^\s]+)\s+([^\s]+)\s+([^\s]+.*)\n$/)
	    {
		my $def_file = $1;
		my $ref_file = $2;
		my $def_symbol = $3;
		my $symbol_def_path;
		my $space_length;
                if ($def_symbol =~ /__gnu_lto_v1/)
                {
                    next;
                }					
		if ($def_file =~ /$out_dir\/([^\s]+\/\w+)\.o/)
		{
		    $symbol_def_path = $1.".o";
		    $space_length = $output_length - length($def_symbol);
		    while ($space_length > 0)							
		    {
		        $symbol_def_path = " ".$symbol_def_path;
			$space_length--;
		    }
		    $symbol_def_path = $def_symbol.$symbol_def_path;
		    push_unique_item_in_array(\@all_symbols_array, $symbol_def_path)
		}					
		push_unique_item_in_array(\@{$def_file_hash{$def_file}}, $def_file);
		push_unique_item_in_array(\@{$ref_file_hash{$def_file}}, $ref_file);
                #printf "the symbol to push: ".$def_symbol."\n";
		push_unique_item_in_array(\@{$def_symbol_hash{$def_file}}, $def_symbol);
                #printf "the file to belong: ".$def_file."\n";
	    }
	}
    }
    else
    {
	printf "No such reference case.".$file_to_parse."!\n";
    }
}

sub push_unique_item_in_array()
{
    my $data_array;
    my $data_item;
    ($data_array, $data_item) = @_;
    foreach my $array_member (@{$data_array})
    {
        #printf "compare one array member: ".$array_member."\n";
        #printf "compared data item: ".$data_item."\n";
        if ($data_item =~ /\Q$array_member\E/ && $array_member =~ /\Q$data_item\E/)
	{
            #printf "found matched one\n";
	    return;
	}
    }
    push @{$data_array}, $data_item;
}


sub output_symbol_to_file()
{
    my $item_count;
    my $DIR = dirname($symbol_list_of_file);
    if(! -d "$DIR")
    {
        system ("mkdir -p $DIR");
    }
    open(outputFile, ">$symbol_list_of_file") or die "[ERROR]can't open $symbol_list_of_file!\n";
    while (my ($hash_key, $hash_value) = each %def_file_hash)
    {
        if ($hash_key =~ /$out_dir\/([^\s]+\/)(\w+)\.o/)
	{
	    print outputFile "File name:       ".$2.".c\n";
	    print outputFile "File path:       ".$1."\n";	
	    $item_count = @{$def_symbol_hash{$hash_key}};
	    foreach my $item (@{$def_symbol_hash{$hash_key}})
	    {
	        if ($item_count == @{$def_symbol_hash{$hash_key}})
		{
		    print outputFile "Defined symbol:  ".$item."\n";
		    $item_count--;
		}
		else
		{
		    print outputFile "                 ".$item."\n";
		}
	    }	
	    $item_count = @{$ref_file_hash{$hash_key}};
	    foreach my $item (@{$ref_file_hash{$hash_key}})
	    {
		if ($item =~ /$out_dir\/([^\s]+\/\w+)\.o/)
		{
		    if ($item_count == @{$ref_file_hash{$hash_key}})
		    {
		        print outputFile "Reference file:  ".$1.".c\n";
			$item_count--;
		    }
		    else
		    {
			print outputFile "                 ".$1.".c\n";
		    }
		}
	    }
	    print outputFile "\n\n";
	}
    }
    close(outputFile);

    my $DIR = dirname($all_symbols);
    if(! -d "$DIR")
    {
        system ("mkdir -p $DIR");
    }	
    open(outputFile, ">$all_symbols") or die "[ERROR] can't open $all_symbols!\n";
    foreach my $symbol (@all_symbols_array)
    {
        print outputFile $symbol."\n";
    }
    close(outputFile);
}
