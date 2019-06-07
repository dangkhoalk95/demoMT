OS_VERSION := $(shell uname)
ifneq ($(filter MINGW%,$(OS_VERSION)),)
  $(DRV_CHIP_PATH)_EXTRA    := -j1
  $(MID_MBEDTLS_PATH)_EXTRA := -j1
  $(MID_NBIOT_MD_CUSTOM_PATH)_EXTRA := -j1
  IFILEGEN := ifilegen2
else
  IFILEGEN := linux/ifilegen
endif

ifeq ($(MAKELEVEL),0)
M :=
else
M := -
endif

include $(SOURCE_DIR)/middleware/MTK/verno/module.mk

.PHONY: merge_lib copy_lib cleanlog genvcxproj gen_sig_db


cleanlog:
ifeq ($(TARGET_PATH),)
	rm -f $(OUTPATH)/*.log
else
	@echo "trigger by build.sh, skip cleanlog"
endif


$(TARGET_LIB).a: $(C_OBJS) $(CXX_OBJS) $(S_OBJS)
	@echo Gen $(TARGET_LIB).a
	@echo Gen $(TARGET_LIB).a >>$(BUILD_LOG)
	$(Q)if [ -e "$(OUTPATH)/$@" ]; then rm -f "$(OUTPATH)/$@"; fi
	$(Q)if [ -e "$(OUTPATH)/lib/$@" ]; then rm -f "$(OUTPATH)/lib/$@"; fi
	$(Q)$(M)$(AR) -r $(OUTPATH)/$@ $(C_OBJS) $(CXX_OBJS) $(S_OBJS) >>$(BUILD_LOG) 2>>$(ERR_LOG); \
	if [ "$$?" != "0" ]; then \
		echo "MODULE BUILD $@ FAIL"; \
		echo "MODULE BUILD $@ FAIL" >> $(BUILD_LOG); \
		exit 1;\
	else \
		echo "MODULE BUILD $@ PASS"; \
		echo "MODULE BUILD $@ PASS" >> $(BUILD_LOG); \
	fi;

$(BUILD_DIR)/%.o: $(SOURCE_DIR)/%.c
	@mkdir -p $(dir $@)
	@echo Build... $$(basename $@)
	@echo Build... $@ >> $(BUILD_LOG)
	@if [ -e "$@" ]; then rm -f "$@"; fi
	@if [ -n "$(OVERRIDE_CFLAGS)" ]; then \
		echo $(CC) $(OVERRIDE_CFLAGS) $@ >> $(BUILD_LOG); \
		$(CC) $(OVERRIDE_CFLAGS) -c $< -o $@ 2>>$(ERR_LOG); \
	else \
		echo $(CC) $(CFLAGS) $@ >> $(BUILD_LOG); \
		$(CC) $(CFLAGS) -c $< -o $@ 2>>$(ERR_LOG); \
	fi; \
	if [ "$$?" != "0" ]; then \
		echo "Build... $$(basename $@) FAIL"; \
		echo "Build... $@ FAIL" >> $(BUILD_LOG); \
	else \
		echo "Build... $$(basename $@) PASS"; \
		echo "Build... $@ PASS" >> $(BUILD_LOG); \
	fi;

$(BUILD_DIR)/%.d: $(SOURCE_DIR)/%.c
	@mkdir -p $(dir $@)
	@set -e; rm -f $@; \
	export D_FILE="$@"; \
	export B_NAME=`echo $$D_FILE | sed 's/\.d//g'`; \
	if [ -n "$(OVERRIDE_CFLAGS)" ]; then \
		$(CC) -MM $(OVERRIDE_CFLAGS) $< > $@.$$$$; \
	else \
		$(CC) -MM $(CFLAGS) $< > $@.$$$$; \
	fi; \
	sed 's@\(.*\)\.o@'"$$B_NAME\.o $$B_NAME\.d"'@g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

$(BUILD_DIR)/%.o: $(SOURCE_DIR)/%.cpp
	@mkdir -p $(dir $@)
	@echo Build... $$(basename $@)
	@echo Build... $@ >> $(BUILD_LOG)
	@if [ -e "$@" ]; then rm -f "$@"; fi
	@if [ -n "$(OVERRIDE_CFLAGS)" ]; then \
		echo $(CXX) $(OVERRIDE_CFLAGS) $@ >> $(BUILD_LOG); \
		$(CXX) $(OVERRIDE_CFLAGS) -c $< -o $@ 2>>$(ERR_LOG); \
	else \
		echo $(CXX) $(CXXFLAGS) $@ >> $(BUILD_LOG); \
		$(CXX) $(CXXFLAGS) -c $< -o $@ 2>>$(ERR_LOG); \
	fi; \
	if [ "$$?" != "0" ]; then \
		echo "Build... $$(basename $@) FAIL"; \
		echo "Build... $@ FAIL" >> $(BUILD_LOG); \
	else \
		echo "Build... $$(basename $@) PASS"; \
		echo "Build... $@ PASS" >> $(BUILD_LOG); \
	fi;

$(BUILD_DIR)/%.d: $(SOURCE_DIR)/%.cpp
	@mkdir -p $(dir $@)
	@set -e; rm -f $@; \
	export D_FILE="$@"; \
	export B_NAME=`echo $$D_FILE | sed 's/\.d//g'`; \
	if [ -n "$(OVERRIDE_CFLAGS)" ]; then \
		$(CXX) -MM $(OVERRIDE_CFLAGS) $< > $@.$$$$; \
	else \
		$(CXX) -MM $(CXXFLAGS) $< > $@.$$$$; \
	fi; \
	sed 's@\(.*\)\.o@'"$$B_NAME\.o $$B_NAME\.d"'@g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

$(BUILD_DIR)/%.o: $(SOURCE_DIR)/%.s
	@mkdir -p $(dir $@)
	@echo Build... $$(basename $@)
	@echo Build... $@ >> $(BUILD_LOG)
	@if [ -e "$@" ]; then rm -f "$@"; fi
	@if [ -n "$(OVERRIDE_CFLAGS)" ]; then \
		$(CC) $(OVERRIDE_CFLAGS) -c $< -o $@; \
	else \
		$(CC) $(CFLAGS) -c $< -o $@; \
	fi; \
	if [ "$$?" != "0" ]; then \
		echo "Build... $$(basename $@) FAIL"; \
		echo "Build... $@ FAIL" >> $(BUILD_LOG); \
	else \
		echo "Build... $$(basename $@) PASS"; \
		echo "Build... $@ PASS" >> $(BUILD_LOG); \
	fi;

genvcxproj:
	@echo "Gen VC Project..."
	@rm -rf $(OUTPATH)/vc_proj
	@mkdir -p $(OUTPATH)/vc_proj
	@echo $(sort $(C_FILES)) > $(OUTPATH)/vc_proj/c_files.txt
	@echo $(filter -D%, $(VC_CFLAGS)) > $(OUTPATH)/vc_proj/c_def.txt
	@echo $(filter -D%, $(SIGDB_CFLAGS)) > $(OUTPATH)/vc_proj/c_def_db.txt
	@echo $(addprefix -I, $(subst $(realpath $(SOURCE_DIR))/,,$(realpath $(subst -I,,$(filter -I%, $(CFLAGS)))))) > $(OUTPATH)/vc_proj/c_inc.txt
	@echo $(sort $(CXX_FILES)) > $(OUTPATH)/vc_proj/cpp_files.txt
	@echo $(filter -D%, $(CPPFLAGS)) > $(OUTPATH)/vc_proj/cpp_def.txt
	@echo $(addprefix -I, $(subst $(realpath $(SOURCE_DIR))/,,$(realpath $(subst -I,,$(filter -I%, $(CPPFLAGS)))))) > $(OUTPATH)/vc_proj/cpp_inc.txt
	@echo C_FILES = '"c_files.txt"' > $(OUTPATH)/vc_proj/vc_proj_gen.cfg
	@echo C_DEFINES = '"c_def.txt"' >> $(OUTPATH)/vc_proj/vc_proj_gen.cfg
	@echo C_INCLUDES = '"c_inc.txt"' >> $(OUTPATH)/vc_proj/vc_proj_gen.cfg
	@echo CPP_FILES = '"cpp_files.txt"' >> $(OUTPATH)/vc_proj/vc_proj_gen.cfg
	@echo CPP_DEFINES = '"cpp_def.txt"' >> $(OUTPATH)/vc_proj/vc_proj_gen.cfg
	@echo CPP_INCLUDES = '"cpp_inc.txt"' >> $(OUTPATH)/vc_proj/vc_proj_gen.cfg
	@echo SIGDB_DEFINES = '"c_def_db.txt"' >> $(OUTPATH)/vc_proj/vc_proj_gen.cfg
	@echo PROJ_NAME = '"$(PROJ_NAME)"' >> $(OUTPATH)/vc_proj/vc_proj_gen.cfg
	@echo EXTRA_DEFINES = '"$(VC_EXTRA_DEFINES)"' >> $(OUTPATH)/vc_proj/vc_proj_gen.cfg
	@echo AVAILABLE_INCLUDE_PATH_PREFIX = '"$(VC_AVAILABLE_INCLUDE_PATH_PREFIX)"' >> $(OUTPATH)/vc_proj/vc_proj_gen.cfg
	@echo ROOT = '"$(VC_ROOT)"' >> $(OUTPATH)/vc_proj/vc_proj_gen.cfg
	@echo PATH_REPLACE_LIST = '"$(VC_PATH_REPLACE_LIST)"' >> $(OUTPATH)/vc_proj/vc_proj_gen.cfg
	@echo UT_PATH_REPLACE_LIST = '"$(UT_PATH_REPLACE_LIST)"' >> $(OUTPATH)/vc_proj/vc_proj_gen.cfg
	@echo ADDITIONAL_LIB_DIR = '"$(VC_ADDITIONAL_LIB_DIR)"' >> $(OUTPATH)/vc_proj/vc_proj_gen.cfg
	@echo ADDITIONAL_DEP = '"$(VC_ADDITIONAL_DEP)"' >> $(OUTPATH)/vc_proj/vc_proj_gen.cfg
	# vc_proj_generator.py is now run as part og the Borg unit test profiles

ifneq ($(MTK_NBIOT_USE_WIN32),y)
ifeq ($(filter clean gen_sig_db prebuilt%,$(MAKECMDGOALS)),)
-include $(C_OBJS:.o=.d)
-include $(CXX_OBJS:.o=.d)
endif
endif

