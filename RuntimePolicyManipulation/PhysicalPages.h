#pragma once
#include <windows.h>

#include <Sddl.h>
#include <assert.h>
#include <ntsecapi.h>
#include <ntstatus.h>
#include "ErrorMessages.h"
#include "SystemInfo.h"

// The AcquireLargePageAccess function and its sub functions consist of modified code from Andrey Moiseev's post on
// stack overflow.
//
// link: (https://stackoverflow.com/questions/42354504/enable-large-pages-in-windows-programmatically)
// license: (https://creativecommons.org/licenses/by-sa/3.0/)

inline namespace NativeMemory
{

        namespace Internal
        {
                // PLSA_UNICODE_STRING must be freed after a call to this function
                inline void InitLsaString(__out PLSA_UNICODE_STRING _dst, __in LPCWSTR _src)
                {
                        if (_src == NULL)
                        {
                                _dst->Buffer        = NULL;
                                _dst->Length        = 0;
                                _dst->MaximumLength = 0;
                                return;
                        }
                        USHORT strMemSize = (lstrlenW(_src) + 1ULL) * sizeof(WCHAR);
                        _dst->Buffer      = (WCHAR*)malloc(strMemSize);
                        assert(_dst->Buffer);
                        memcpy(_dst->Buffer, _src, strMemSize);
                        _dst->Length        = strMemSize - sizeof(WCHAR);
                        _dst->MaximumLength = strMemSize;
                }

                inline NTSTATUS OpenPolicy(LPWSTR ServerName, DWORD DesiredAccess, PLSA_HANDLE PolicyHandle)
                {
                        LSA_OBJECT_ATTRIBUTES ObjectAttributes;
                        LSA_UNICODE_STRING    ServerString;
                        PLSA_UNICODE_STRING   Server = NULL;

                        //
                        // Always initialize the object attributes to all zeroes.
                        //
                        ZeroMemory(&ObjectAttributes, sizeof(ObjectAttributes));

                        if (ServerName != NULL)
                        {
                                //
                                // Make a LSA_UNICODE_STRING out of the LPWSTR passed in
                                //
                                InitLsaString(&ServerString, ServerName);
                                Server = &ServerString;
                        }

                        //
                        // Attempt to open the policy.
                        //
                        return LsaOpenPolicy(Server, &ObjectAttributes, DesiredAccess, PolicyHandle);
                        free(ServerString.Buffer);
                }

                inline NTSTATUS SetPrivilegeOnAccount(LSA_HANDLE PolicyHandle,
                                                      PSID       AccountSid,
                                                      LPCWSTR    PrivilegeName,
                                                      BOOL       bEnable)
                {
                        LSA_UNICODE_STRING PrivilegeString;
                        NTSTATUS           result;
                        //
                        // Create a LSA_UNICODE_STRING for the privilege name.
                        //
                        InitLsaString(&PrivilegeString, PrivilegeName);

                        //
                        // grant or revoke the privilege, accordingly
                        //
                        if (bEnable)
                        {
                                result = LsaAddAccountRights(PolicyHandle,     // open policy handle
                                                             AccountSid,       // target SID
                                                             &PrivilegeString, // privileges
                                                             1                 // privilege count
                                );
                        }
                        else
                        {
                                result = LsaRemoveAccountRights(PolicyHandle,     // open policy handle
                                                                AccountSid,       // target SID
                                                                FALSE,            // do not disable all rights
                                                                &PrivilegeString, // privileges
                                                                1                 // privilege count
                                );
                        }
                        free(PrivilegeString.Buffer);
                        return result;
                }

        } // namespace Internal

        inline bool AcquireLargePageAccess()
        {
                using namespace NativeMemory::Internal;
                HANDLE hToken = NULL;

                if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken))
                {
                        DisplayDbgErrorW(L"OpenProcessToken");
                        return 0;
                }
                DWORD dwBufferSize = 0;
                // Probe the buffer size reqired for PTOKEN_USER structure
                if (!GetTokenInformation(hToken, TokenUser, NULL, 0, &dwBufferSize) &&
                    (GetLastError() != ERROR_INSUFFICIENT_BUFFER))
                {

                        // Cleanup
                        CloseHandle(hToken);
                        hToken = NULL;
                        DisplayDbgErrorW(, L"Probe GetTokenInformation");
                        return 0;
                }

                PTOKEN_USER pTokenUser = (PTOKEN_USER)malloc(dwBufferSize);
                auto        cleanup    = [&]() {
                        free(pTokenUser);
                        CloseHandle(hToken);
                        hToken = 0;
                };
                // Retrieve the token information in a TOKEN_USER structure
                if (!GetTokenInformation(hToken, TokenUser, pTokenUser, dwBufferSize, &dwBufferSize))
                {
                        cleanup();
                        DisplayDbgErrorW(, L"GetTokenInformation");
                        return 0;
                }

                // Print SID string
                LPWSTR strsid;
                ConvertSidToStringSid(pTokenUser->User.Sid, &strsid);
                // Cleanup
                CloseHandle(hToken);
                hToken = NULL;

                NTSTATUS   status;
                LSA_HANDLE policyHandle;

                if (status = OpenPolicy(NULL, POLICY_CREATE_ACCOUNT | POLICY_LOOKUP_NAMES, &policyHandle))
                {
                        cleanup();
                        DisplayDbgErrorW(, L"OpenPolicy");
                        return 0;
                }

                // Add new privelege to the account
                status = SetPrivilegeOnAccount(policyHandle, pTokenUser->User.Sid, SE_LOCK_MEMORY_NAME, TRUE);
                if (status != STATUS_SUCCESS)
                {
                        cleanup();
                        DisplayDbgErrorW(, L"SetPrivilegeOnAccount");
                        return 0;
                }

                // Enable this priveledge for the current process
                hToken = NULL;
                TOKEN_PRIVILEGES tp;

                if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY | TOKEN_ADJUST_PRIVILEGES, &hToken))
                {
                        cleanup();
                        DisplayDbgErrorW(, L"OpenProcessToken");
                        return 0;
                }

                tp.PrivilegeCount           = 1;
                tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

                if (!LookupPrivilegeValue(NULL, SE_LOCK_MEMORY_NAME, &tp.Privileges[0].Luid))
                {
                        cleanup();
                        DisplayDbgErrorW(, L"LookupPrivilegeValue");
                        return 0;
                }

                BOOL  result = AdjustTokenPrivileges(hToken, FALSE, &tp, 0, (PTOKEN_PRIVILEGES)NULL, 0);
                DWORD error  = GetLastError();

                if (!result || (error != ERROR_SUCCESS))
                {
                        cleanup();
                        DisplayDbgErrorW(L"Restart Computer", L"AdjustTokenPrivileges");
                        return 0;
                }

                // Cleanup
                cleanup();
                return 1;
        }


} // namespace NativeMemory
