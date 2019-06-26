from __future__ import print_function  # for python 2.6+ support print("XXX", end="") feature
from collections import OrderedDict
import sys
import re
import os
import io
import fileinput

""" tag & label example
<Files>  -------------------------------------------------------------------> tag
    <Filter Name="my_module" Filter="c;cpp;cxx;h;hpp;hxx">  ----------------> tag with value
        <File RelativePath="hello_world1.c" />  ----------------------------> label with value
        <ProjectName>upditsim</ProjectName>     ----------------------------> quote with string
        <ProjectName Condition="test">upditsim</ProjectName>     -----------> quote with value & string
    </Filter>
    <ToolFiles />  ---------------------------------------------------------> label
</Files>
"""


class QUOTE:
    def __init__(self, quote="", string="", value=OrderedDict(), level=0):
        self.quote = quote
        self.string = string
        self.value = value
        self.level = level

    def print_quote(self):
        print("  " * self.level + "<" + self.quote, end="")
        for key in self.value:
            print(" " + key + "=\"" + self.value[key] + "\"", end="")
        print(">" + self.string + "</" + self.quote + ">\n", end="")

class LABEL:
    def __init__(self, label="", value=OrderedDict(), level=0):
        self.label = label
        self.value = value
        self.level = level

    def print_label(self):
        print("  " * self.level + "<" + self.label, end="")
        for key in self.value:
            print(" " + key + "=\"" + self.value[key] + "\"", end="")
        print(" />\n", end="")


class TAG:
    def __init__(self, tag="", value=OrderedDict(), level=0):
        self.tag = tag
        self.value = value
        self.level = level
        self.childList = []

    def add_child_tag(self, tag="", value=OrderedDict()):
        child = TAG(tag, value, self.level+1)
        self.childList.append(child)
        return child

    def add_child_quote(self, quote="", string="", value=OrderedDict()):
        child = QUOTE(quote, string, value, self.level+1)
        self.childList.append(child)
        return child

    def add_child_label(self, label="", value=OrderedDict()):
        child = LABEL(label, value, self.level+1)
        self.childList.append(child)
        return child

    def print_tag(self):
        print("  "*self.level + "<" + self.tag, end="")
        for key in self.value:
            print(" " + key + "=\"" + self.value[key] + "\"", end="")
        print(">\n", end="")
        for child in self.childList:
            if isinstance(child, TAG):
                child.print_tag()
            elif isinstance(child, QUOTE):
                child.print_quote()
            else:
                child.print_label()

        print("  " * self.level + "</" + self.tag + ">\n", end="")

# =========================================================================================================
if len(sys.argv) != 3:
    print("Usage:\n python ./vc_proj_generator_hello_world.py ./vc_proj_gen.cfg ./vc_proj.exe.vcxproj")
    exit(1)

cfg_file = sys.argv[1]
output_file = sys.argv[2]

cfg_file_dir = os.path.dirname(cfg_file)

# get config
fcfg_file = open(cfg_file, "r")
lines = fcfg_file.readlines()
fcfg_file.close()

reg = re.compile(r"(\w+)\s*=\s*(\S+)")
for line in lines:
    result = reg.match(line.rstrip('\n'))
    if result:
        exec(line)
        print(result.group(1) + " = " + eval(result.group(1)))

# get path replace list
if PATH_REPLACE_LIST != "":
    path_replace_list = (PATH_REPLACE_LIST.rstrip('\n')).split()
    path_search_list = path_replace_list[0::2]
    path_replace_list = path_replace_list[1::2]
	
# get path replace list for unit tests
if UT_PATH_REPLACE_LIST != "":
    ut_path_replace_list = (UT_PATH_REPLACE_LIST.rstrip('\n')).split()
    ut_path_search_list = ut_path_replace_list[0::2]
    ut_path_replace_list = ut_path_replace_list[1::2]

# get available include path prefix
available_inc_prefix = AVAILABLE_INCLUDE_PATH_PREFIX.split()

# get c file list
fc_files = open(cfg_file_dir + "/" + C_FILES, "r")
str_c_files = fc_files.read()
fc_files.close()
c_file_list = str_c_files.split()
for c_index, c_file in enumerate(c_file_list):
    found_match = False
    if PATH_REPLACE_LIST != "":
        for index, search_pattern in enumerate(path_search_list):
            c_file_new = re.sub(r"^"+search_pattern, path_replace_list[index], c_file)
            if c_file != c_file_new:
                found_match = True
                c_file_list[c_index] = (ROOT + c_file_new).replace("/", "\\")
                break
    if not found_match:
        c_file_list[c_index] = (ROOT + c_file).replace("/", "\\")

# get c defines, convert to vc format
fc_def = open(cfg_file_dir + "/" + C_DEFINES, "r")
str_c_def = fc_def.read()
fc_def.close()
str_c_def_vc = (((str_c_def.replace("-D", "")).replace(" ", ";")).replace("\"", "")).rstrip('\n')
if len(str_c_def_vc) > 0 and str_c_def_vc[-1:] != ";":
    str_c_def_vc += ";"

# get c include paths, filter out available include path, convert to vc format
fc_inc = open(cfg_file_dir + "/" + C_INCLUDES, "r")
str_c_inc = fc_inc.read()
fc_inc.close()
str_c_inc_list = (str_c_inc.rstrip('\n')).split()
str_c_inc_list_after_filter = []
if len(str_c_inc_list) > 0:
    for inc_path in str_c_inc_list:
        inc_path = re.sub(r"-I", "", inc_path)
        for available_inc in available_inc_prefix:
            if inc_path.startswith(available_inc):
                if PATH_REPLACE_LIST != "":
                    for index, search_pattern in enumerate(path_search_list):
                        inc_path_new = re.sub(r"^" + search_pattern, path_replace_list[index], inc_path)
                        if inc_path != inc_path_new:
                            inc_path = inc_path_new
                            break
                str_c_inc_list_after_filter.append(ROOT + inc_path)
                break
str_c_inc_vc = ";".join(str_c_inc_list_after_filter)
if len(str_c_inc_vc) > 0 and str_c_inc_vc[-1:] != ";":
    str_c_inc_vc += ";"
str_c_inc_vc = str_c_inc_vc.replace("/", "\\")

# dump final c file include path to file
fc_inc_vc = open(cfg_file_dir + "/" + re.sub(r"(.*)\.", r"\1_vc.", C_INCLUDES), "w")
fc_inc_vc.write("\n".join(["-I" + s for s in str_c_inc_list_after_filter]))
fc_inc_vc.close()

# UT: get c & cpp include paths, filter out available include path, convert to ifilegen format
fc_inc = open(cfg_file_dir + "/" + C_INCLUDES, "r")
str_files_inc = fc_inc.read()
fc_inc.close()
fcpp_files = open(cfg_file_dir + "/" + CPP_INCLUDES, "r")
str_files_inc = str_files_inc + fcpp_files.read()
fcpp_files.close()
str_c_inc_list = (str_files_inc.rstrip('\n')).split()
str_c_inc_list_after_filter = []
if len(str_c_inc_list) > 0:
    for inc_path in str_c_inc_list:
        inc_path = re.sub(r"-I", "", inc_path)
        for available_inc in available_inc_prefix:
            if inc_path.startswith(available_inc):
                if PATH_REPLACE_LIST != "":
                    for index, search_pattern in enumerate(ut_path_search_list):
                        inc_path_new = re.sub(r"^" + search_pattern, ut_path_replace_list[index], inc_path)
                        if inc_path != inc_path_new:
                            inc_path = inc_path_new
                            break
                str_c_inc_list_after_filter.append(ROOT + inc_path)
                break
ut_str_c_inc_vc = ";".join(str_c_inc_list_after_filter)
if len(ut_str_c_inc_vc) > 0 and ut_str_c_inc_vc[-1:] != ";":
    ut_str_c_inc_vc += ";"
ut_str_c_inc_vc = ut_str_c_inc_vc.replace("/", "\\")

# UT: dump final c file include path to file
fc_inc_vc = open(cfg_file_dir + "/" + 'inc_paths_db.txt', "w")
fc_inc_vc.write("\n".join(["-I" + s for s in str_c_inc_list_after_filter]))
fc_inc_vc.close()

# get cpp file list
fcpp_files = open(cfg_file_dir + "/" + CPP_FILES, "r")
str_cpp_files = fcpp_files.read()
fcpp_files.close()
cpp_file_list = str_cpp_files.split()
for cpp_index, cpp_file in enumerate(cpp_file_list):
    found_match = False
    if PATH_REPLACE_LIST != "":
        for index, search_pattern in enumerate(path_search_list):
            cpp_file_new = re.sub(r"^"+search_pattern, path_replace_list[index], cpp_file)
            if cpp_file != cpp_file_new:
                found_match = True
                cpp_file_list[cpp_index] = (ROOT + cpp_file_new).replace("/", "\\")
                break
    if not found_match:
        cpp_file_list[cpp_index] = (ROOT + cpp_file).replace("/", "\\")

# get cpp defines, convert to vc format
fcpp_def = open(cfg_file_dir + "/" + CPP_DEFINES, "r")
str_cpp_def = fcpp_def.read()
fcpp_def.close()
str_cpp_def_vc = (((str_cpp_def.replace("-D", "")).replace(" ", ";")).replace("\"", "")).rstrip('\n')
if len(str_cpp_def_vc) > 0 and str_cpp_def_vc[-1:] != ";":
    str_cpp_def_vc += ";"

# get cpp include paths, filter out available include path, convert to vc format
fcpp_inc = open(cfg_file_dir + "/" + CPP_INCLUDES, "r")
str_cpp_inc = fcpp_inc.read()
fcpp_inc.close()
str_cpp_inc_list = (str_cpp_inc.rstrip('\n')).split()
str_cpp_inc_list_after_filter = []
if len(str_cpp_inc_list) > 0:
    for inc_path in str_cpp_inc_list:
        inc_path = re.sub(r"-I", "", inc_path)
        for available_inc in available_inc_prefix:
            if inc_path.startswith(available_inc):
                if PATH_REPLACE_LIST != "":
                    for index, search_pattern in enumerate(path_search_list):
                        inc_path_new = re.sub(r"^" + search_pattern, path_replace_list[index], inc_path)
                        if inc_path != inc_path_new:
                            inc_path = inc_path_new
                            break
                str_cpp_inc_list_after_filter.append(ROOT + inc_path)
                break
str_cpp_inc_vc = ";".join(str_cpp_inc_list_after_filter)
if len(str_cpp_inc_vc) > 0 and str_cpp_inc_vc[-1:] != ";":
    str_cpp_inc_vc += ";"
str_cpp_inc_vc = str_cpp_inc_vc.replace("/", "\\")

# dump final cpp file include path to file
fcpp_inc_vc = open(cfg_file_dir + "/" + re.sub(r"(.*)\.", r"\1_vc.", CPP_INCLUDES), "w")
fcpp_inc_vc.write("\n".join(["-I" + s for s in str_cpp_inc_list_after_filter]))
fcpp_inc_vc.close()

# combine c & cpp file include files for unit test project
with open(cfg_file_dir + "/" + 'inc_files.txt', 'w') as file:
    filename = [(cfg_file_dir + "/" + re.sub(r"(.*)\.", r"\1_vc.", C_INCLUDES))]
    input_lines = fileinput.input(filename)
    file.writelines(input_lines)
    file.write('\n')
    filename = [(cfg_file_dir + "/" + re.sub(r"(.*)\.", r"\1_vc.", CPP_INCLUDES))]
    input_lines = fileinput.input(filename)
    file.writelines(input_lines)

# combine c & cpp defines files for unit test project
with open(cfg_file_dir + "/" + 'def_files.txt', 'w') as file:
    filename = [(cfg_file_dir + "/" + CPP_DEFINES)]
    input_lines = fileinput.input(filename)
    file.writelines(input_lines)
    file.write('\n')
    filename = [(cfg_file_dir + "/" + SIGDB_DEFINES)]
    input_lines = fileinput.input(filename)
    file.writelines(input_lines)

# get additional include path
additional_lib_dir = ";".join(ADDITIONAL_LIB_DIR.split())
if len(additional_lib_dir) > 0 and additional_lib_dir[-1:] != ";":
    additional_lib_dir += ";"
additional_lib_dir = additional_lib_dir.replace("/", "\\")

# define module to it's src list
module_to_src_list = OrderedDict([("my_module", c_file_list + cpp_file_list)])

# generate visual studio project structure
warn_as_error = "false"
if "TREAT_WARINGS_AS_ERRORS" in os.environ:
    warn_as_error = r"$(TREAT_WARINGS_AS_ERRORS)"

vs_process_num = "8"
if "VS2013_PROCESSOR_NUM" in os.environ:
    vs_process_num = r"$(VS2013_PROCESSOR_NUM)"

pre_build_event_cmd = " "
if "PRE_BUILD_EVENT" in os.environ:
    pre_build_event_cmd = r"$(PRE_BUILD_EVENT)"

proj_guid = "{39E6D2D2-1FDC-4861-A82B-C7C4AEBF8FC2}"

vs_proj = TAG("Project", OrderedDict([("DefaultTargets", "Build"),
                                      ("ToolsVersion", "12.0"),
                                      ("xmlns", "http://schemas.microsoft.com/developer/msbuild/2003")]))

item_group = vs_proj.add_child_tag("ItemGroup", OrderedDict([("Label", "ProjectConfigurations")]))

proj_debug_config = item_group.add_child_tag("ProjectConfiguration", OrderedDict([("Include", "Debug|Win32")]))
proj_debug_config.add_child_quote("Configuration", "Debug")
proj_debug_config.add_child_quote("Platform", "Win32")

proj_release_config = item_group.add_child_tag("ProjectConfiguration", OrderedDict([("Include", "Release|Win32")]))
proj_release_config.add_child_quote("Configuration", "Release")
proj_release_config.add_child_quote("Platform", "Win32")

property_group1 = vs_proj.add_child_tag("PropertyGroup", OrderedDict([("Label", "Globals")]))
property_group1.add_child_label("RootNamespace")
property_group1.add_child_quote("ProjectGuid", proj_guid)
property_group1.add_child_quote("ProjectName", PROJ_NAME)
property_group1.add_child_quote("Keyword", "Win32Proj")

vs_proj.add_child_label("Import", OrderedDict([("Project", r"$(VCTargetsPath)\Microsoft.Cpp.Default.props")]))

property_group2 = vs_proj.add_child_tag("PropertyGroup", OrderedDict([("Condition", "'$(Configuration)|$(Platform)'=='Release|Win32'"),
                                                                      ("Label", "Configuration")]))
property_group2.add_child_quote("ConfigurationType", "Application")
property_group2.add_child_quote("CharacterSet", "MultiByte")
property_group2.add_child_quote("PlatformToolset", "v120")

property_group3 = vs_proj.add_child_tag("PropertyGroup", OrderedDict([("Condition", "'$(Configuration)|$(Platform)'=='Debug|Win32'"),
                                                                      ("Label", "Configuration")]))
property_group3.add_child_quote("ConfigurationType", "Application")
property_group3.add_child_quote("CharacterSet", "MultiByte")
property_group3.add_child_quote("PlatformToolset", "v120")

vs_proj.add_child_label("Import", OrderedDict([("Project", r"$(VCTargetsPath)\Microsoft.Cpp.props")]))

vs_proj.add_child_tag("ImportGroup", OrderedDict([("Label", "ExtensionSettings")]))

import_group1 = vs_proj.add_child_tag("ImportGroup", OrderedDict([("Condition", "'$(Configuration)|$(Platform)'=='Release|Win32'"),
                                                                  ("Label", "PropertySheets")]))

import_group1.add_child_label("Import", OrderedDict([("Project", r"$(UserRootDir)\Microsoft.Cpp.$(Platform).user.props"),
                                                     ("Condition", "exists('$(UserRootDir)\Microsoft.Cpp.$(Platform).user.props')"),
                                                     ("Label", "LocalAppDataPlatform")]))
import_group1.add_child_label("Import", OrderedDict([("Project", r"$(VCTargetsPath)Microsoft.CPP.UpgradeFromVC70.props")]))

import_group2 = vs_proj.add_child_tag("ImportGroup", OrderedDict([("Condition", "'$(Configuration)|$(Platform)'=='Debug|Win32'"),
                                                                  ("Label", "PropertySheets")]))

import_group2.add_child_label("Import", OrderedDict([("Project", r"$(UserRootDir)\Microsoft.Cpp.$(Platform).user.props"),
                                                     ("Condition", "exists('$(UserRootDir)\Microsoft.Cpp.$(Platform).user.props')"),
                                                     ("Label", "LocalAppDataPlatform")]))
import_group2.add_child_label("Import", OrderedDict([("Project", r"$(VCTargetsPath)Microsoft.CPP.UpgradeFromVC70.props")]))

vs_proj.add_child_label("PropertyGroup", OrderedDict([("Label", "UserMacros")]))

property_group4 = vs_proj.add_child_tag("PropertyGroup")
property_group4.add_child_quote("_ProjectFileVersion", "10.0.40219.1")

property_group4.add_child_quote("OutDir", r"$(ProjectDir)", OrderedDict([("Condition", "'$(Configuration)|$(Platform)'=='Debug|Win32'")]))
property_group4.add_child_quote("IntDir", r"$(ProjectDir)", OrderedDict([("Condition", "'$(Configuration)|$(Platform)'=='Debug|Win32'")]))
property_group4.add_child_quote("LinkIncremental", "false", OrderedDict([("Condition", "'$(Configuration)|$(Platform)'=='Debug|Win32'")]))
property_group4.add_child_quote("OutDir", r"$(ProjectDir)", OrderedDict([("Condition", "'$(Configuration)|$(Platform)'=='Release|Win32'")]))
property_group4.add_child_quote("IntDir", r"$(ProjectDir)", OrderedDict([("Condition", "'$(Configuration)|$(Platform)'=='Release|Win32'")]))
property_group4.add_child_quote("LinkIncremental", "false", OrderedDict([("Condition", "'$(Configuration)|$(Platform)'=='Release|Win32'")]))

item_def_group1 = vs_proj.add_child_tag("ItemDefinitionGroup", OrderedDict([("Condition", "'$(Configuration)|$(Platform)'=='Debug|Win32'")]))
cl_compile = item_def_group1.add_child_tag("ClCompile")
cl_compile.add_child_quote("WarningLevel", "Level3")
cl_compile.add_child_tag("PrecompiledHeader")
cl_compile.add_child_quote("StructMemberAlignment", "1Byte")
cl_compile.add_child_quote("RuntimeTypeInfo", "true")
cl_compile.add_child_quote("RuntimeLibrary", "MultiThreadedDebug")
cl_compile.add_child_quote("PreprocessorDefinitions", str_c_def_vc + str_cpp_def_vc + EXTRA_DEFINES + "%(PreprocessorDefinitions)")
cl_compile.add_child_quote("Optimization", "Disabled")
cl_compile.add_child_quote("MinimalRebuild", "false")
cl_compile.add_child_quote("ExceptionHandling", "Sync")
cl_compile.add_child_quote("DebugInformationFormat", "ProgramDatabase")
cl_compile.add_child_quote("BasicRuntimeChecks", "EnableFastChecks")
cl_compile.add_child_quote("AdditionalIncludeDirectories", str_c_inc_vc + str_cpp_inc_vc + "%(AdditionalIncludeDirectories)")
cl_compile.add_child_quote("EnableEnhancedInstructionSet", "StreamingSIMDExtensions2")
cl_compile.add_child_quote("FunctionLevelLinking", "true")
cl_compile.add_child_quote("SuppressStartupBanner", "true")
cl_compile.add_child_quote("WholeProgramOptimization", "false")
cl_compile.add_child_quote("InlineFunctionExpansion", "Default")
cl_compile.add_child_quote("IntrinsicFunctions", "false")
cl_compile.add_child_quote("AdditionalOptions", "/c /FS /MP %(AdditionalOptions)")
cl_compile.add_child_quote("MultiProcessorCompilation", "true")
cl_compile.add_child_quote("ProcessorNumber", vs_process_num)
cl_compile.add_child_quote("TreatWarningAsError", warn_as_error)
link = item_def_group1.add_child_tag("Link")
link.add_child_quote("TargetMachine", "NotSet")
link.add_child_quote("SubSystem", "Console")
link.add_child_quote("ProgramDatabaseFile", r"$(ProjectName).pdb")
link.add_child_quote("OutputFile", r"$(ProjectDir)$(ProjectName).exe")
link.add_child_quote("GenerateMapFile", "true")
link.add_child_quote("GenerateDebugInformation", "true")
link.add_child_quote("AdditionalLibraryDirectories", additional_lib_dir + "%(AdditionalLibraryDirectories)")
link.add_child_quote("AdditionalDependencies", ADDITIONAL_DEP.replace(" ", ";") + ";%(AdditionalDependencies)")
link.add_child_quote("StackReserveSize", "2500000")
link.add_child_quote("EnableCOMDATFolding", "false")
link.add_child_quote("OptimizeReferences", "false")
link.add_child_quote("TreatLinkerWarningAsErrors", warn_as_error)
PreBuildEvent = item_def_group1.add_child_tag("PreBuildEvent")
PreBuildEvent.add_child_quote("Command", pre_build_event_cmd)

item_def_group2 = vs_proj.add_child_tag("ItemDefinitionGroup", OrderedDict([("Condition", "'$(Configuration)|$(Platform)'=='Release|Win32'")]))
cl_compile = item_def_group2.add_child_tag("ClCompile")
cl_compile.add_child_quote("WarningLevel", "Level3")
cl_compile.add_child_tag("PrecompiledHeader")
cl_compile.add_child_quote("StructMemberAlignment", "1Byte")
cl_compile.add_child_quote("RuntimeTypeInfo", "true")
cl_compile.add_child_quote("RuntimeLibrary", "MultiThreadedDebug")
cl_compile.add_child_quote("PreprocessorDefinitions", str_c_def_vc + str_cpp_def_vc + EXTRA_DEFINES + "%(PreprocessorDefinitions)")
cl_compile.add_child_quote("Optimization", "Disabled")
cl_compile.add_child_quote("MinimalRebuild", "true")
cl_compile.add_child_quote("ExceptionHandling", "Sync")
cl_compile.add_child_quote("DebugInformationFormat", "ProgramDatabase")
cl_compile.add_child_quote("BasicRuntimeChecks", "EnableFastChecks")
cl_compile.add_child_quote("AdditionalIncludeDirectories", str_c_inc_vc + str_cpp_inc_vc + "%(AdditionalIncludeDirectories)")
cl_compile.add_child_quote("EnableEnhancedInstructionSet", "StreamingSIMDExtensions2")
cl_compile.add_child_quote("FunctionLevelLinking", "true")
cl_compile.add_child_quote("SuppressStartupBanner", "true")
cl_compile.add_child_quote("AdditionalOptions", "/c /FS /MP %(AdditionalOptions)")
cl_compile.add_child_quote("MultiProcessorCompilation", "true")
cl_compile.add_child_quote("ProcessorNumber", vs_process_num)
cl_compile.add_child_quote("TreatWarningAsError", warn_as_error)
link = item_def_group2.add_child_tag("Link")
link.add_child_quote("TargetMachine", "NotSet")
link.add_child_quote("SubSystem", "Console")
link.add_child_quote("ProgramDatabaseFile", r"$(OutDir)$(ProjectName).pdb")
link.add_child_quote("OutputFile", r"$(ProjectDir)$(ProjectName).exe")
link.add_child_quote("GenerateMapFile", "true")
link.add_child_quote("GenerateDebugInformation", "true")
link.add_child_quote("AdditionalLibraryDirectories", additional_lib_dir + "%(AdditionalLibraryDirectories)")
link.add_child_quote("AdditionalDependencies", ADDITIONAL_DEP.replace(" ", ";") + ";%(AdditionalDependencies)")
link.add_child_quote("StackReserveSize", "2500000")
link.add_child_quote("EnableCOMDATFolding", "false")
link.add_child_quote("OptimizeReferences", "false")
link.add_child_quote("TreatLinkerWarningAsErrors", warn_as_error)
PreBuildEvent = item_def_group2.add_child_tag("PreBuildEvent")
PreBuildEvent.add_child_quote("Command", pre_build_event_cmd)

for module in module_to_src_list:
    module_item_group = vs_proj.add_child_tag("ItemGroup")
    for src in module_to_src_list[module]:
        module_item_group.add_child_label("ClCompile", OrderedDict([("Include", src)]))

vs_proj.add_child_label("Import", OrderedDict([("Project", r"$(VCTargetsPath)\Microsoft.Cpp.targets")]))

vs_proj.add_child_tag("ImportGroup", OrderedDict([("Label", "ExtensionTargets")]))

# redirect print to file
f_output_file = open(output_file, "w")
sys.stdout = f_output_file

# print header
print('<?xml version="1.0" encoding="utf-8"?>')

# print project tag
vs_proj.print_tag()
f_output_file.close()

# write solution file
proj_file_path, proj_file_name = os.path.split(output_file)
solution_file = proj_file_path + "/" + proj_file_name.replace(".vcxproj", ".sln")
f_solution_file = io.open(solution_file, "w", newline="\r\n")
sys.stdout = f_solution_file
print(u"Microsoft Visual Studio Solution File, Format Version 12.00")
print(u"# Visual Studio Express 2013 for Windows Desktop")
print(u"VisualStudioVersion = 12.0.31101.0")
print(u"MinimumVisualStudioVersion = 10.0.40219.1")
print(u"Project(\"{8BC9CEB8-8B4A-11D0-8D11-00A0C91BC942}\") = \"" + PROJ_NAME + "\", \"" + proj_file_name + "\", \"" + proj_guid + "\"")
print(u"EndProject")
print(u"Global")
print(u"\tGlobalSection(SolutionConfigurationPlatforms) = preSolution")
print(u"\t\tDebug|Win32 = Debug|Win32")
print(u"\t\tRelease|Win32 = Release|Win32")
print(u"\tEndGlobalSection")
print(u"\tGlobalSection(ProjectConfigurationPlatforms) = postSolution")
print(u"\t\t" + proj_guid + ".Debug|Win32.ActiveCfg = Debug|Win32")
print(u"\t\t" + proj_guid + ".Debug|Win32.Build.0 = Debug|Win32")
print(u"\t\t" + proj_guid + ".Release|Win32.ActiveCfg = Release|Win32")
print(u"\t\t" + proj_guid + ".Release|Win32.Build.0 = Release|Win32")
print(u"\tEndGlobalSection")
print(u"\tGlobalSection(SolutionProperties) = preSolution")
print(u"\t\tHideSolutionNode = FALSE")
print(u"\tEndGlobalSection")
print(u"EndGlobal")
f_solution_file.close()

# switch standard output back
sys.stdout = sys.__stdout__

print("done")
