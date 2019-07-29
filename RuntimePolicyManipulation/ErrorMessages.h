#pragma once
#include <windows.h>
#include <string>

#define __FILENAMEW__ (wcsrchr(__FILEW__, '\\') ? wcsrchr(__FILEW__, '\\') + 1 : __FILEW__)

#ifdef _DEBUG
#define DisplayDbgErrorW(OPTIONAL_MSG, OPTIONAL_TITLE) DisplayErrorW(OPTIONAL_MSG, OPTIONAL_TITLE)
#else
DisplayDbgErrorW()
#endif

#define DisplayErrorW(OPTIONAL_MSG, OPTIONAL_TITLE)                                                     \
        {                                                                                               \
                std::wstring line_number    = std::to_wstring(__LINE__ - 1);                            \
                std::wstring file_name      = std::wstring{__FILENAMEW__};                              \
                std::wstring function_name  = std::wstring{__FUNCTIONW__};                              \
                std::wstring getlasterror   = std::to_wstring(GetLastError());                          \
                std::wstring optional_msg   = std::wstring{OPTIONAL_MSG};                               \
                std::wstring optional_title = std::wstring{OPTIONAL_TITLE};                             \
                __internal_display_getlasterror(                                                        \
                    file_name, function_name, line_number, getlasterror, optional_msg, optional_title); \
        }
#define CDisplayErrorW(EXPRESSION, OPTIONAL_MSG, OPTIONAL_TITLE)                                                \
        {                                                                                                       \
                if (EXPRESSION)                                                                                 \
                {                                                                                               \
                        std::wstring line_number    = std::to_wstring(__LINE__ - 1);                            \
                        std::wstring file_name      = std::wstring{__FILENAMEW__};                              \
                        std::wstring function_name  = std::wstring{__FUNCTIONW__};                              \
                        std::wstring getlasterror   = std::to_wstring(GetLastError());                          \
                        std::wstring optional_msg   = std::wstring{OPTIONAL_MSG};                               \
                        std::wstring optional_title = std::wstring{OPTIONAL_TITLE};                             \
                        __internal_display_getlasterror(                                                        \
                            file_name, function_name, line_number, getlasterror, optional_msg, optional_title); \
                }                                                                                               \
        }

void __internal_display_getlasterror(std::wstring file_name,
                                     std::wstring function_name,
                                     std::wstring line_number,
                                     std::wstring getlasterror,
                                     std::wstring optional_msg,
                                     std::wstring optional_title);
