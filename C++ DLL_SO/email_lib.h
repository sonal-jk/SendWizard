#pragma once

#ifndef EMAIL_LIB_H
#define EMAIL_LIB_H

#include <iostream>
#include <string>

using namespace std;

//allows the DLL functions to be called from C code or other languages that support C-style linking.
//Ensures C linkage/compatibility when using C++ code
#ifdef __cplusplus
extern "C" {
#endif

#ifdef BUILD_MY_DLL
   //tells the compiler that the function will be exported
   #define EMAIL_LIB __declspec(dllexport)
#else
   //tells the compiler that the function will be imported 
   #define EMAIL_LIB __declspec(dllimport)
#endif

//void sendEmail(const char* auth_header, const char* receiver, const char* subject, const char* body);
bool sendEmail(const char* auth_header, const char* receiver, const char* subject, const char* body);

bool scheduleEmail(const char* dateTime);


#ifdef __cplusplus
}
#endif

#endif // EMAIL_LIB_H


// __declspec gives control over whether a symbol is exported or imported when creating DLLs. This helps in managing the visibility and sharing of functions or variables across different parts of a program.