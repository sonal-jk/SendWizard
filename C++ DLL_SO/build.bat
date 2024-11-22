:: Run this script to create a .dll file in Windows
"++ -c -DBUILD_MY_DLL email_lib.cpp
g++ -email_lib -o email_lib.dll email_lib.o -Wl, --out-implib,libemail_lib.a"

:: DLL stands for Dynamic Link Library, a type of file used in Windows operating systems to store compiled code that can be used by multiple programs simultaneously. 
::A DLL file contains code, data, and resources that programs can call during runtime to perform specific functions or tasks, without having to have those functions coded directly into the application.