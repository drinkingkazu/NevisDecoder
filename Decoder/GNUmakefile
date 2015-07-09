#
# This is an example GNUmakefile for my packages
#

# specific names for this package
DICT  = NevisDecoder_DecoderCint
SHLIB = libNevisDecoder_Decoder.so
SOURCES = $(filter-out $(DICT).cxx, $(wildcard *.cxx))
FMWK_HEADERS = LinkDef.h $(DICT).h
HEADERS = $(filter-out $(FMWK_HEADERS), $(wildcard *.h))
OBJECTS = $(SOURCES:.cxx=.o)

# include options for this package
INCFLAGS  = -I.                       #Include itself
INCFLAGS += $(shell larlite-config --includes)
INCFLAGS += -I$(LARLITE_USERDEVDIR)/NevisDecoder

# platform-specific options
OSNAME          = $(shell uname -s)
HOST            = $(shell uname -n)
OSNAMEMODE      = $(OSNAME)

LDFLAGS += $(shell larlite-config --libs)
LDFLAGS += -L$(LARLITE_USERDEVDIR) -lNevisDecoder_DecoderAlgo -lNevisDecoder_DecoderBase
# call kernel specific compiler setup
include $(LARLITE_BASEDIR)/Makefile/Makefile.${OSNAME}

# call the common GNUmakefile
include $(LARLITE_BASEDIR)/Makefile/GNUmakefile.CORE
