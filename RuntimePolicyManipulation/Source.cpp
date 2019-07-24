#define UNICODE
#define _UNICODE

#include <windows.h>
#include <ntsecapi.h>
#include <ntstatus.h>
#include <Sddl.h>

void InitLsaString(PLSA_UNICODE_STRING LsaString, LPCWSTR String)
{
	DWORD StringLength;

	if (String == NULL) {
		LsaString->Buffer = NULL;
		LsaString->Length = 0;
		LsaString->MaximumLength = 0;
		return;
	}

	StringLength = wcslen(String);
	LsaString->Buffer = (WCHAR*)malloc(StringLength * sizeof(WCHAR));
	LPWSTR success = lstrcpyW(LsaString->Buffer, String);
	LsaString->Length = (USHORT)StringLength * sizeof(WCHAR);
	LsaString->MaximumLength = (USHORT)(StringLength + 1) * sizeof(WCHAR);
}

NTSTATUS OpenPolicy(LPWSTR ServerName, DWORD DesiredAccess, PLSA_HANDLE PolicyHandle)
{
	LSA_OBJECT_ATTRIBUTES ObjectAttributes;
	LSA_UNICODE_STRING ServerString;
	PLSA_UNICODE_STRING Server = NULL;

	// 
	// Always initialize the object attributes to all zeroes.
	// 
	ZeroMemory(&ObjectAttributes, sizeof(ObjectAttributes));

	if (ServerName != NULL) {
		// 
		// Make a LSA_UNICODE_STRING out of the LPWSTR passed in
		// 
		InitLsaString(&ServerString, ServerName);
		Server = &ServerString;
	}

	// 
	// Attempt to open the policy.
	// 
	return LsaOpenPolicy(
		Server,
		&ObjectAttributes,
		DesiredAccess,
		PolicyHandle
	);
	free(ServerString.Buffer);
}

NTSTATUS SetPrivilegeOnAccount(LSA_HANDLE PolicyHandle, PSID AccountSid, LPCWSTR PrivilegeName, BOOL bEnable)
{
	LSA_UNICODE_STRING PrivilegeString;

	// 
	// Create a LSA_UNICODE_STRING for the privilege name.
	// 
	InitLsaString(&PrivilegeString, PrivilegeName);

	// 
	// grant or revoke the privilege, accordingly
	// 
	if (bEnable) {
		return LsaAddAccountRights(
			PolicyHandle,       // open policy handle
			AccountSid,         // target SID
			&PrivilegeString,   // privileges
			1                   // privilege count
		);
	}
	else {
		return LsaRemoveAccountRights(
			PolicyHandle,       // open policy handle
			AccountSid,         // target SID
			FALSE,              // do not disable all rights
			&PrivilegeString,   // privileges
			1                   // privilege count
		);
	}
}

int main()
{
	HANDLE hToken = NULL;

	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken))
	{
		MessageBox(0, L"failed OpenProcessToken", 0, 0);

		return -1;
	}

	DWORD dwBufferSize = 0;

	// Probe the buffer size reqired for PTOKEN_USER structure
	if (!GetTokenInformation(hToken, TokenUser, NULL, 0, &dwBufferSize) &&
		(GetLastError() != ERROR_INSUFFICIENT_BUFFER))
	{

		// Cleanup
		CloseHandle(hToken);
		hToken = NULL;

		MessageBox(0, L"failed GetTokenInformation buffer size", 0, 0);

		return -1;
	}

	PTOKEN_USER pTokenUser = (PTOKEN_USER)malloc(dwBufferSize);

	// Retrieve the token information in a TOKEN_USER structure
	if (!GetTokenInformation(
		hToken,
		TokenUser,
		pTokenUser,
		dwBufferSize,
		&dwBufferSize))
	{

		// Cleanup
		CloseHandle(hToken);
		hToken = NULL;

		MessageBox(0, L"failed GetTokenInformation", 0, 0);
		return -1;
	}

	// Print SID string
	LPWSTR strsid;
	ConvertSidToStringSid(pTokenUser->User.Sid, &strsid);

	// Cleanup
	CloseHandle(hToken);
	hToken = NULL;

	NTSTATUS status;
	LSA_HANDLE policyHandle;

	if (status = OpenPolicy(NULL, POLICY_CREATE_ACCOUNT | POLICY_LOOKUP_NAMES, &policyHandle))
	{
		MessageBox(0, L"failed OpenPolicy", 0, 0);
		return -1;
	}

	// Add new privelege to the account
	status = SetPrivilegeOnAccount(policyHandle, pTokenUser->User.Sid, SE_LOCK_MEMORY_NAME, TRUE);
	if (status != STATUS_SUCCESS)
	{
		MessageBox(0, L"failed SetPrivilegeOnAccount", 0, 0);
		return -1;
	}

	// Enable this priveledge for the current process
	hToken = NULL;
	TOKEN_PRIVILEGES tp;

	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY | TOKEN_ADJUST_PRIVILEGES, &hToken))
	{
		MessageBox(0, L"failed OpenProcessToken", 0, 0);
		return -1;
	}

	tp.PrivilegeCount = 1;
	tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

	if (!LookupPrivilegeValue(NULL, SE_LOCK_MEMORY_NAME, &tp.Privileges[0].Luid))
	{
		MessageBox(0, L"failed LookupPrivilegeValue", 0, 0);
		return -1;
	}

	BOOL result = AdjustTokenPrivileges(hToken, FALSE, &tp, 0, (PTOKEN_PRIVILEGES)NULL, 0);
	DWORD error = GetLastError();

	if (!result || (error != ERROR_SUCCESS))
	{
		MessageBox(0, L"failed AdjustTokenPrivileges", 0, 0);
		CloseHandle(hToken);
		return -1;
	}
	MessageBox(0, L"success AdjustTokenPrivileges", 0, 0);

	// Cleanup
	CloseHandle(hToken);
	hToken = NULL;

	//SIZE_T pageSize = GetLargePageMinimum();

	//// Finally allocate the memory
	//char* largeBuffer = (char*)VirtualAlloc(NULL, pageSize * 3, MEM_RESERVE | MEM_COMMIT | MEM_LARGE_PAGES, PAGE_READWRITE);
	//if (largeBuffer)
	//{
	//}
}