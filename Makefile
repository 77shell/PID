
# ARCH		?= x86
# SRC_ROOT	:= ../../../..
# include_paths	:= -iquote$(SRC_ROOT)/Inc -iquote$(SRC_ROOT)/Inc/ctrl-mode
# STATIC_LINK	:= y
MAKEFLAGS	+= -j$(shell nproc)
target 		:= pid

# ifeq ($(ARCH), am335x)
# include $(SRC_ROOT)/am335x/Makefile.inc
# include_paths	+= -I$(SRC_ROOT)/am335x/usr/local/include
# lib_paths	+= -L$(SRC_ROOT)/am335x/usr/local/lib
# strip_prog	= $(STRIP)
# else
# include $(SRC_ROOT)/$(ARCH)/Makefile.inc
# include_paths	+= -I$(SRC_ROOT)/usr/include
# lib_paths	+= -L$(SRC_ROOT)/usr/lib -L$(SRC_ROOT)/usr/local/lib
# STORAGE_PATH	:= "."
# endif

# include_paths	+= \
# 		-iquote$(SRC_ROOT)/Inc \
# 		-I$(SRC_ROOT)/usr/include \
# 		-I/home/max/src/Catch2/single_include

LIB_DIRS_NAME := ""
# adc ip65csu/db serial can imbus ip65csu/lcd ip65csu/ipc/webserver ipc rms imbus/slave
ifneq ($(LIB_DIRS_NAME), "")
ifeq ($(STATIC_LINK), y)
lib_paths 	+= $(foreach name,$(LIB_DIRS_NAME),-L$(SRC_ROOT)/$(name))
else
lib_paths 	+= $(foreach name,$(LIB_DIRS_NAME),-L$(SRC_ROOT)/$(name)) \
	-Wl,-rpath,$(LIB_INSTALL_DIR)
endif
endif #ifneq empty directory list

LIBS_NAME := ""
# serial can imbus db gdbm lcd ipc2webserver ipc imbuslave
ifneq ($(LIBS_NAME), "")
ifeq ($(STATIC_LINK), y)
LIBS := -Wl,-static,--start-group \
	$(foreach lib,$(LIBS_NAME),-l$(lib)) \
	-Wl,--end-group,-dy

else
LIBS := -Wl,--start-group \
	$(foreach lib,$(LIBS_NAME),-l$(lib)) \
	-Wl,--end-group
endif
endif #ifneq empty library list

CPPFLAGS 	+= $(include_paths)

pid-objs := \
	PID.o \
	PID_Test.o

objects := \
	$(pid-objs)


.PHONY: all
all: CXXFLAGS += -O0 -ggdb
all: $(objects)
#	$(call make_libs)
	$(CC) $(lib_paths) -fsanitize=address -fsanitize=leak -fstack-protector -pthread -o$(target) $(objects) $(LIBS)

.PHONY: clean
clean:
#	$(call make_libs, clean)
	rm $(target) $(objects) -rf

define make_libs
	@ for i in $(LIB_DIRS_NAME); \
	do \
		CAN_INTERFACE='vcan0' $(MAKE) MAKEFLAGS=-j`nproc` --directory=$(SRC_ROOT)/$$i $1; \
	done
endef

.PHONY: TAGS
TAGS:
	rm -rf TAGS
	find . -regextype posix-egrep -iregex '.*\.(cpp|h|c)' > search.log
	perl filter.pl search.log | xargs etags -a
	rm -rf search.log
