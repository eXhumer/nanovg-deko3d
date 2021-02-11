ifndef DEVKITPRO
$(error "Please set DEVKITPRO in your environment. export DEVKITPRO=<path to>/devkitpro")
else
include $(DEVKITPRO)/libnx/switch_rules
endif

LIB			:=	lib
TARGET		:=	libnanovg
SOURCES		:=	source source/framework
INCLUDES	:=	include

ARCH	:=	-march=armv8-a -mtune=cortex-a57 -mtp=soft -fPIE

DEFINES	:=	-D__SWITCH__

CFLAGS	:=	-Wall -O3 -ffunction-sections \
			$(ARCH) $(DEFINES)

CXXFLAGS	:= -std=gnu++2a -fno-exceptions -fno-rtti

ASFLAGS	:=	$(ARCH)

LIBDIRS	:= $(PORTLIBS) $(LIBNX)

OUTPUT	:=	$(LIB)/$(TARGET)
LIB_OUTPUT	:=	$(OUTPUT).a

CFILES		:=	$(foreach SOURCE,$(SOURCES),$(wildcard $(SOURCE)/*.c))
CPPFILES	:=	$(foreach SOURCE,$(SOURCES),$(wildcard $(SOURCE)/*.cpp))

ifeq ($(strip $(CPPFILES)),)
	LD	:=	$(CC)
else
	LD	:=	$(CXX)
endif

OFILES 	:=	$(CPPFILES:.cpp=.o) $(CFILES:.c=.o)
DEPENDS	:=	$(OFILES:.o=.d)

INCFLAGS	:=	$(foreach INCLUDE,$(INCLUDES),-I$(INCLUDE)) \
				$(foreach LIBDIR,$(LIBDIRS),-I$(LIBDIR)/include)

.PHONY: library clean

library: $(LIB_OUTPUT)
$(LIB_OUTPUT): $(LIB) $(OFILES)

$(LIB):
	[ -d $@ ] || mkdir -p $@

%.o:	%.c
	$(CC) -MMD -MP -MF $(@:%.o=%.d) $(CFLAGS) $(INCFLAGS) -c $< -o $@

%.o:	%.cpp
	$(CXX) -MMD -MP -MF $(@:%.o=%.d) $(CFLAGS) $(INCFLAGS) $(CXXFLAGS) -c $< -o $@

$(LIB_OUTPUT):
	$(AR) -rc $(LIB_OUTPUT) $(OFILES)

clean:
	rm -rf $(ROMFS_FOLDERS) $(OFILES) $(DEPENDS) $(LIB_OUTPUT)

-include $(DEPENDS)
