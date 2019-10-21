#############################################################################
#
# Project Makefile
#
# (c) Wouter van Ooijen (www.voti.nl) 2016
#
# This file is in the public domain.
#
#############################################################################

# source files in this project (main.cpp is automatically assumed)
SOURCES :=

# header files in this project
HEADERS := tasks.hpp version.h util.hpp

# other places to look for files for this project
SEARCH  :=

# other files to create (eg. main.lst)
RESULTS :=

DEFINES := -DSPARKYSPARKY_VERSION_GIT="\"$(shell git describe --abbrev=7 --dirty --always --tags)\""

# set RELATIVE to the next higher directory
# and defer to the appropriate Makefile.* there
RELATIVE := ..
include $(RELATIVE)/Makefile.due
