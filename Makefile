# Project Name
TARGET = Sammy

# Sources
CPP_SOURCES = sammy.cpp sam/sam.cpp sam/reciter.cpp sam/render.cpp sam/processframes.cpp sam/createtransitions.cpp

# Library Locations
LIBDAISY_DIR = ../../libDaisy
DAISYSP_DIR = ../../DaisySP

# Core location, and generic Makefile.
SYSTEM_FILES_DIR = $(LIBDAISY_DIR)/core
include $(SYSTEM_FILES_DIR)/Makefile

