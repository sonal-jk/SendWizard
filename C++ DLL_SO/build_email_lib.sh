#!/bin/bash
#Run this script to create a .so file in linux

# Define file names
SOURCE_FILE="email_lib.cpp"
OBJECT_FILE="email_lib.o"
SO_FILE="libemail_lib.so"

# Remove any existing object files to prevent conflicts
rm -f $OBJECT_FILE

# Compile the source file to an object file with -fPIC for shared library
g++ -fPIC -c $SOURCE_FILE -o $OBJECT_FILE
if [ $? -ne 0 ]; then
    echo "Compilation failed."
    exit 1
fi

# Link the object file into a shared library (.so file)
#Added the -lcurl in path to link it with .so file otherwise you won't be able to use the functionalities
g++ -shared -o $SO_FILE $OBJECT_FILE -lcurl
if [ $? -ne 0 ]; then
    echo "Linking failed."
    exit 1
fi

# Output success message
echo "Shared library created successfully:"
echo "  Shared Library: $SO_FILE"

#the -fPIC flag, which is necessary for creating shared libraries on Linux
# The object file must be compiled with the -fPIC option to generate position-independent code.
# If you previously compiled email_lib.cpp without -fPIC, the resulting object file may not work correctly. Make sure to remove any old object files before recompiling.
#Without -fPIC, the code would assume fixed memory addresses for functions and variables, making it non-relocatable.
#With -fPIC, the generated code uses relative addressing rather than absolute memory addresses, ensuring that the library can be loaded anywhere in memory.
#.so stands for Shared Object. These are shared libraries used on Unix-based systems (like Linux). They are similar to .dll (Dynamic Link Library) files on Windows.
#Shared libraries allow code to be modular and reusable, reducing memory usage since multiple programs can share the same library at runtime.