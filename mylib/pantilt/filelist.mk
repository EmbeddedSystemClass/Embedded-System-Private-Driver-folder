###################################################
# LIBRARY SOURCES
###################################################

# Set mylib folder path.
# IMPORTANT NOTE: change MYLIBTEMPLATE_PATH to your Driver name e.g. JOYSTICK_MYLIBPATH
# e.g. JOYSTICK_MYLIBPATH=/holme/csse3010/mylib/joystick
#MYLIBTEMPLATE_PATH=path_to_mylib_driver_folder

PANTILT_MYLIBPATH=/home/csse3010/csse3010/mylib/pantilt

# Set folder path with header files to include.
CFLAGS += -I$(PANTILT_MYLIBPATH)

# List all c files locations that must be included (use space as separate e.g. LIBSRCS += path_to/file1.c path_to/file2.c)
LIBSRCS += $(PANTILT_MYLIBPATH)/s4527438_hal_pantilt.c
LIBSRCS += $(PANTILT_MYLIBPATH)/s4527438_os_pantilt.c
LIBSRCS += $(PANTILT_MYLIBPATH)/s4527438_cli_pantilt.c
