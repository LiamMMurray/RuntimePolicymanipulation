#include "ErrorMessages.h"

void __internal_display_getlasterror(std::wstring file_name,
                            std::wstring function_name,
                            std::wstring line_number,
                            std::wstring getlasterror,
                            std::wstring optional_msg,
                            std::wstring optional_title)
{
        std::wstring msg = L"file:\t\t" + file_name + L"\nfunction:\t\t" + function_name + L"\nline:\t\t" + line_number +
                           L"\nGetLastError:\t" + getlasterror + L"\n\n" + optional_msg;
        MessageBox(0, msg.c_str(), optional_title.c_str(), 0);
}