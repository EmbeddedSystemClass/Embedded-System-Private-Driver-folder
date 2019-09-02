###################################################
# LIBRARY SOURCES
###################################################

# Set mylib folder path.
# IMPORTANT NOTE: change MYLIBTEMPLATE_PATH to your Driver name e.g. JOYSTICK_MYLIBPATH
# e.g. JOYSTICK_MYLIBPATH=/holme/csse3010/mylib/joystick
#MYLIBTEMPLATE_PATH=path_to_mylib_driver_folder

HAMMING_MYLIBPATH=/home/csse3010/csse3010/mylib/hamming

# Set folder path with header files to include.
CFLAGS += -I$(HAMMING_MYLIBPATH)

# List all c files locations that must be included (use space as separate e.g. LIBSRCS += path_to/file1.c path_to/file2.c)
LIBSRCS += $(HAMMING_MYLIBPATH)/s4527438_lib_hamming.c
