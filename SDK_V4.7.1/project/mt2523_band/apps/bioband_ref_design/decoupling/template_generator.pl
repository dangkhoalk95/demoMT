use File::Copy;
my $fota_jump_table = "../decoupling/template/fota_jump_table.c";
my $fota_common = "../decoupling/template/fota_common.h";
my $wrapper_header;
my $wrapper_source;
my $first_output;

my $all_symbol_file = "../decoupling/all_symbols.txt";
my $file_relation_file = "../decoupling/symbol_list_of_file.txt";
my $symbol_file = "../GCC/Build/symbol_list.txt"; 

my @symbolData;
open(inputFile, "<$symbol_file") or die "[ERROR] can't open $symbol_file!\n";
@symbolData = <inputFile>;
close(inputFile);

my $origin_directory = "\.\.\/decoupling\/template\/origin\/";
my $template_dir = "\.\.\/decoupling\/template\/";
my $bak_directory = "\.\.\/decoupling\/template\/bak\/";
if (opendir(DIR, $bak_directory))
{
    while (my $file_name = readdir(DIR))
    {
	      unlink "\.\.\/decoupling\/template\/bak\/".$file_name;
    }
}
else
{
	  mkdir $bak_directory;
}
if (opendir(DIR, $template_dir))
{
	  while (my $file_name = readdir(DIR))
    {
    	  my $file_full_path = $template_dir.$file_name;
        if (-f $file_full_path)
    	  {
    	      if (!-d $origin_directory)
            {
            	  mkdir $origin_directory;
            }
            move($file_full_path , $origin_directory);
        }
    }
}


#auto gen jump table
my @allSymbols;
open(inputFile, "<$all_symbol_file") or die "[ERROR] can't open $all_symbol_file!\n";
@allSymbols = <inputFile>;
close(inputFile);

open(outputFile, ">$fota_jump_table") or die "[ERROR] can't open $fota_jump_table!\n";
print outputFile "\#include \"debug\.h\"\n";
print outputFile "\#ifdef WRAPPER_CONTENT_ENABLE\n\n\n";
print outputFile "\/\* include header files here \*\/\n\n\n";
print outputFile "\/\* jump table definition \*\/\n";
print outputFile "const void\* ubin_table\[\] __attribute__ \(\(section \(\"\.ubin_table\"\)\)\) \=\n";
print outputFile "\{\n";
$first_output = 1;
foreach my $symbol (@allSymbols)
{
    if ($symbol =~ /(\b\w+\b)+[\s]+[^\s]+/)
    {
        if ($first_output == 1)
        {
            print outputFile "    \&".$1;
            $first_output = 0;
        }
        else
        {
            print outputFile "\,\n    \&".$1;
        }
    }
}
print outputFile "\n\}\;\n";
print outputFile "\n\n\n\#endif";
close(outputFile);

#auto gen common header
open(outputFile, ">$fota_common") or die "[ERROR] can't open $fota_common!\n";
print outputFile "\#include \"debug\.h\"\n";
print outputFile "\#ifdef WRAPPER_CONTENT_ENABLE\n\n\n";
print outputFile "\#define UBIN_TABLE_ADDR    \(0x80000000 + 0x8000\)\n\n";
print outputFile "typedef enum\n";
print outputFile "\{\n";
$first_output = 1;
foreach my $symbol (@allSymbols)
{
    if ($symbol =~ /(\b\w+\b)+[\s]+[^\s]+/)
    {
        if ($first_output == 1)
        {
            print outputFile "    ".$1."_wrapper_idx";
            $first_output = 0;
        }
        else
        {
            print outputFile "\,\n    ".$1."_wrapper_idx";
        }
    }
}
print outputFile "\n\}func_symbol_enum\;\n\n";
print outputFile "\#define SYM_WRAPPER_DECLARE\(var\, type\)   \(\(\*\(\*\(type\)var\#\#_sym_wrapper\)\)\)\n";
print outputFile "\n\n\n\#endif";
close(outputFile);

#auto gen wrapper header
my @fileRelationArray;
my $wrapper_generating;
my @tempSymbolArray;
open(inputFile, "<$file_relation_file") or die "[ERROR] can't open $file_relation_file!\n";
@fileRelationArray = <inputFile>;
close(inputFile);
$wrapper_generating = 0;
foreach my $line (@fileRelationArray)
{
    if ($wrapper_generating == 1)
    {
        if ($line =~ /Reference file:/)
        {
            $wrapper_generating = 0;
            close(outputFile_c);
            print outputFile_h "\n\n\/\* global variable wrapper \*\/\n";
            foreach my $tempSymbol (@tempSymbolArray)
            {
                print outputFile_h "#define ".$tempSymbol."  SYM_WRAPPER\(".$tempSymbol."\, \/\* ToDo: data type \*\/\)\n";
            }
            close(outputFile_h);
            $tempSymbol = ()
        }
        elsif ($line =~ /(\b\w+\b)/)
        {
            output_one_symbol($1);
        }
    }
    else
    {
        if ($line =~ /File name\:\s+(\b\w+\b)/)
        {
            $wrapper_header = "\.\.\/decoupling\/template\/".$1."_wrapper\.h";
            $wrapper_source = "\.\.\/decoupling\/template\/".$1."_wrapper\.c";
            open(outputFile_h, ">$wrapper_header") or die "[ERROR] can't open $wrapper_header!\n";
            print outputFile_h "\/\* include header files here \*\/\n\n\n";
            print outputFile_h "\/\* global variable declaration \*\/\n";
            open(outputFile_c, ">$wrapper_source") or die "[ERROR] can't open $wrapper_source!\n";
            print outputFile_c "\#include \"fota_common.h\"\n";
            print outputFile_c "\#include \"".$1."_wrapper\.h\"\n\n";
            print outputFile_c "\/\* global variable wrapper \*\/\n";
        }
        elsif ($line =~ /Defined symbol:\s+(\b\w+\b)/)
        {
            $wrapper_generating = 1;
            output_one_symbol($1);
        }
    }
}


if (opendir(DIR, $template_dir))
{
	  while (my $file_name = readdir(DIR))
    {
    	  my $file_full_path = $template_dir.$file_name;
    	  if (-f $file_full_path && -d $origin_directory)
    	  {
    	  	  rename $file_full_path, $file_full_path."\.bak";
            move($file_full_path."\.bak", $bak_directory);
        }
    }
}
if (opendir(DIR, $origin_directory))
{
	  while (my $file_name = readdir(DIR))
    {
    	  my $file_full_path = $origin_directory.$file_name;
    	  if (-f $file_full_path)
    	  {
            move($file_full_path, $template_dir);
        }
    }
}
rmdir $origin_directory;

sub output_one_symbol()
{
    my $symbol = $_[0];
    if (get_symbol_type($symbol) == 1)
    {
        push @tempSymbolArray, $symbol;
        print outputFile_h "extern void             \*".$symbol."_sym_wrapper\;\n";
        print outputFile_c "void \*".$symbol."_sym_wrapper \= \(void\*\)UBIN_TABLE_ADDR \+ ".$symbol."_wrapper_idx\;\n";    
    }
}

#subroutine to judge a symbol is a variable or a function
sub get_symbol_type()
{
    my $symbol = $_[0];
    foreach my $line (@symbolData)
    {
            if ($line =~ /[^A-Z]+([A-Z]+)\s+.+\s+\b$symbol\b\s+/)
	    {
	        if ($1 =~ /OBJECT/)
	        {
		    return 1;
	        }
	        elsif ($1 =~ /FUNC/)
	        {
		    return 2;
	        }
	        return 0;
	    }
    }
    return 3;
}















