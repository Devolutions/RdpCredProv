
#pragma once
#include "common.h"
#include <direct.h>
#include <windows.h>

#pragma warning(push)
#pragma warning(disable : 4995)
#include <shlwapi.h>
#pragma warning(pop)

HRESULT FieldDescriptorCoAllocCopy(const CREDENTIAL_PROVIDER_FIELD_DESCRIPTOR& rcpfd, CREDENTIAL_PROVIDER_FIELD_DESCRIPTOR** ppcpfd);

HRESULT FieldDescriptorCopy(const CREDENTIAL_PROVIDER_FIELD_DESCRIPTOR& rcpfd, CREDENTIAL_PROVIDER_FIELD_DESCRIPTOR* pcpfd);

HRESULT UnicodeStringInitWithString(PWSTR pwz, UNICODE_STRING* pus);

HRESULT KerbInteractiveUnlockLogonInit(PWSTR pwzDomain, PWSTR pwzUsername, PWSTR pwzPassword,
				       CREDENTIAL_PROVIDER_USAGE_SCENARIO cpus, KERB_INTERACTIVE_UNLOCK_LOGON* pkiul);

HRESULT KerbInteractiveUnlockLogonPack(const KERB_INTERACTIVE_UNLOCK_LOGON& rkiulIn, BYTE** prgb, DWORD* pcb);

void KerbInteractiveUnlockLogonUnpackInPlace(KERB_INTERACTIVE_UNLOCK_LOGON* pkiul);

HRESULT RetrieveNegotiateAuthPackage(ULONG* pulAuthPackage);

HRESULT ProtectIfNecessaryAndCopyPassword(PWSTR pwzPassword, CREDENTIAL_PROVIDER_USAGE_SCENARIO cpus, PWSTR* ppwzProtectedPassword);

int ConvertToUnicode(UINT CodePage, DWORD dwFlags, LPCSTR lpMultiByteStr,
		int cbMultiByte, LPWSTR* lpWideCharStr, int cchWideChar);

int ConvertFromUnicode(UINT CodePage, DWORD dwFlags, LPCWSTR lpWideCharStr, int cchWideChar,
		LPSTR* lpMultiByteStr, int cbMultiByte, LPCSTR lpDefaultChar, LPBOOL lpUsedDefaultChar);

class CLogFile
{
public:
	CLogFile();
	~CLogFile();

	void OpenFile(const char* filename);
	void CloseFile();

	void Write(const char* pszFormat, ...);

	bool m_enabled = false;

private:
	FILE* m_pLogFile = NULL;
	CRITICAL_SECTION m_cs;
};
