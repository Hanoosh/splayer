// This is a part of the Active Template Library.
// Copyright (C) Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Active Template Library Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Active Template Library product.

#ifndef __ATLSERR_H__
#define __ATLSERR_H__

#pragma once
#pragma pack(push,_ATL_PACKING)
namespace ATL{

#define VALIDATION_S_OK				0x00000000
#define VALIDATION_S_EMPTY			0x00000001
#define VALIDATION_E_PARAMNOTFOUND	0x00000002
#define VALIDATION_E_LENGTHMIN		0x80000083
#define VALIDATION_E_LENGTHMAX		0x80000084
#define VALIDATION_E_INVALIDLENGTH  0x80000080
#define VALIDATION_E_INVALIDPARAM	0x80000005
#define VALIDATION_E_FAIL			0x80000006

#define VALIDATION_SUCCEEDED(x) (((x == VALIDATION_S_OK) || (x == VALIDATION_S_EMPTY )))

typedef DWORD HTTP_CODE;

#define HTTP_ERROR(err, sub)			((HTTP_CODE)(DWORD_PTR)MAKELONG((WORD)err, (WORD)sub))
#define HTTP_ERROR_CODE(err)			((DWORD)LOWORD(err))
#define HTTP_SUBERROR_CODE(err)			((DWORD)HIWORD(err))
#define HTTP_SUCCESS					HTTP_ERROR(0, 0)

#define SUBERR_NONE								0
#define ISE_SUBERR_BADSRF						1
#define ISE_SUBERR_HNDLFAIL						2
#define ISE_SUBERR_SYSOBJFAIL					3
#define ISE_SUBERR_READFILEFAIL					4
#define ISE_SUBERR_LOADFILEFAIL                 6
#define ISE_SUBERR_LOADLIB						7
#define ISE_SUBERR_HANDLERIF					8
#define ISE_SUBERR_OUTOFMEM						9
#define ISE_SUBERR_UNEXPECTED					10
#define ISE_SUBERR_STENCIL_INVALIDFUNCOFFSET	11
#define ISE_SUBERR_STENCIL_MISMATCHWHILE		12
#define ISE_SUBERR_STENCIL_MISMATCHIF			13
#define ISE_SUBERR_STENCIL_UNEXPECTEDTYPE		14
#define ISE_SUBERR_STENCIL_INVALIDINDEX			15
#define ISE_SUBERR_STENCIL_INDEXOUTOFRANGE		16
#define ISE_SUBERR_STENCIL_PARSE_FAIL			17
#define ISE_SUBERR_STENCIL_LOAD_FAIL			18
#define ISE_SUBERR_HANDLER_NOT_FOUND			19
#define ISE_SUBERR_BAD_HANDLER_TAG				20
#define ISE_SUBERR_NO_HANDLER_TAG				21
#define ISE_SUBERR_LONGMETHODNAME				22
#define ISE_SUBERR_LONGHANDLERNAME				23
#define ISE_SUBERR_IMPERSONATIONFAILED			24
#define ISE_SUBERR_ISAPISTARTUPFAILED			25
#define ISE_SUBERR_SOAPNOSOAPACTION				26
#define SUBERR_NO_PROCESS						27
#define SUBERR_S_FALSE							28
#define SUBERR_ASYNC							29
#define SUBERR_ASYNC_DONE						30
#define SUBERR_ASYNC_NOFLUSH					31
#define SUBERR_ASYNC_NOFLUSH_DONE				32
#define SUBERR_NO_CACHE							33
#define DBG_SUBERR_ALREADY_DEBUGGING            34
#define DBG_SUBERR_NOT_DEBUGGING                35
#define DBG_SUBERR_INVALID_SESSION              36
#define DBG_SUBERR_BAD_ID                       37
#define DBG_SUBERR_COCREATE                     38
#define DBG_SUBERR_ATTACH                       39


#define HTTP_FAIL						HTTP_ERROR(500, SUBERR_NONE)
#define HTTP_SUCCESS_NO_PROCESS			HTTP_ERROR(200, SUBERR_NO_PROCESS)
#define HTTP_S_FALSE					HTTP_ERROR(HTTP_ERROR_CODE(HTTP_SUCCESS), SUBERR_S_FALSE)
#define HTTP_SUCCESS_ASYNC				HTTP_ERROR(200, SUBERR_ASYNC)
#define HTTP_SUCCESS_ASYNC_DONE         HTTP_ERROR(200, SUBERR_ASYNC_DONE)
#define HTTP_SUCCESS_ASYNC_NOFLUSH		HTTP_ERROR(200, SUBERR_ASYNC_NOFLUSH)
#define HTTP_SUCCESS_ASYNC_NOFLUSH_DONE HTTP_ERROR(200, SUBERR_ASYNC_NOFLUSH_DONE)
#define HTTP_SUCCESS_NO_CACHE           HTTP_ERROR(200, SUBERR_NO_CACHE)
#define HTTP_OK							HTTP_ERROR(200, SUBERR_NONE)
#define HTTP_CONTINUE					HTTP_ERROR(100, SUBERR_NONE)

#define HTTP_CREATED					HTTP_ERROR(201, SUBERR_NONE)
#define HTTP_ACCEPTED					HTTP_ERROR(202, SUBERR_NONE)
#define HTTP_NON_AUTHORITATIVE			HTTP_ERROR(203, SUBERR_NONE)
#define HTTP_NO_CONTENT					HTTP_ERROR(204, SUBERR_NONE)
#define HTTP_RESET_CONTENT				HTTP_ERROR(205, SUBERR_NONE)
#define HTTP_PARTIAL_CONTENT			HTTP_ERROR(206, SUBERR_NONE)

#define HTTP_MULTIPLE_CHOICES			HTTP_ERROR(300, SUBERR_NONE)
#define HTTP_MOVED_PERMANENTLY			HTTP_ERROR(301, SUBERR_NONE)
#define HTTP_FOUND						HTTP_ERROR(302, SUBERR_NONE)
#define HTTP_SEE_OTHER					HTTP_ERROR(303, SUBERR_NONE)
#define HTTP_NOT_MODIFIED				HTTP_ERROR(304, SUBERR_NONE)
#define HTTP_USE_PROXY					HTTP_ERROR(305, SUBERR_NONE)
#define HTTP_TEMPORARY_REDIRECT			HTTP_ERROR(307, SUBERR_NONE)

#define HTTP_BAD_REQUEST				HTTP_ERROR(400, SUBERR_NONE)
#define HTTP_UNAUTHORIZED				HTTP_ERROR(401, SUBERR_NONE)
#define HTTP_PAYMENT_REQUIRED			HTTP_ERROR(402, SUBERR_NONE)
#define HTTP_FORBIDDEN					HTTP_ERROR(403, SUBERR_NONE)
#define HTTP_NOT_FOUND					HTTP_ERROR(404, SUBERR_NONE)
#define HTTP_METHOD_NOT_ALLOWED			HTTP_ERROR(405, SUBERR_NONE)
#define HTTP_NOT_ACCEPTABLE				HTTP_ERROR(406, SUBERR_NONE)
#define HTTP_PROXY_AUTHENTICATION_REQUIRED	HTTP_ERROR(407, SUBERR_NONE)
#define HTTP_REQUEST_TIMEOUT			HTTP_ERROR(408, SUBERR_NONE)
#define HTTP_CONFLICT					HTTP_ERROR(409, SUBERR_NONE)
#define HTTP_GONE						HTTP_ERROR(410, SUBERR_NONE)
#define HTTP_LENGTH_REQUIRED			HTTP_ERROR(411, SUBERR_NONE)
#define HTTP_PRECONDITION_FAILED		HTTP_ERROR(412, SUBERR_NONE)
#define HTTP_REQUEST_ENTITY_TOO_LONG	HTTP_ERROR(413, SUBERR_NONE)
#define HTTP_REQUEST_URI_TOO_LONG		HTTP_ERROR(414, SUBERR_NONE)
#define HTTP_UNSUPPORTED_MEDIA_TYPE		HTTP_ERROR(415, SUBERR_NONE)
#define HTTP_RANGE_NOT_SATISFIABLE		HTTP_ERROR(416, SUBERR_NONE)
#define HTTP_EXPECTATION_FAILED			HTTP_ERROR(417, SUBERR_NONE)

#define HTTP_INTERNAL_SERVER_ERROR		HTTP_ERROR(500, SUBERR_NONE)
#define HTTP_NOT_IMPLEMENTED			HTTP_ERROR(501, SUBERR_NONE)
#define HTTP_BAD_GATEWAY				HTTP_ERROR(502, SUBERR_NONE)
#define HTTP_SERVICE_UNAVAILABLE		HTTP_ERROR(503, SUBERR_NONE)
#define HTTP_GATEWAY_TIMEOUT			HTTP_ERROR(504, SUBERR_NONE)
#define HTTP_VERSION_NOT_SUPPORTED		HTTP_ERROR(505, SUBERR_NONE)

inline bool IsAsyncStatus(HTTP_CODE hcStatus)
{
	return 
		hcStatus == HTTP_SUCCESS_ASYNC ||
		hcStatus == HTTP_SUCCESS_ASYNC_DONE ||
		hcStatus == HTTP_SUCCESS_ASYNC_NOFLUSH ||
		hcStatus == HTTP_SUCCESS_ASYNC_NOFLUSH_DONE;
}

inline bool IsAsyncContinueStatus(HTTP_CODE hcStatus)
{
	return 
		hcStatus == HTTP_SUCCESS_ASYNC ||
		hcStatus == HTTP_SUCCESS_ASYNC_NOFLUSH;
}

inline bool IsAsyncDoneStatus(HTTP_CODE hcStatus)
{
	return 
		hcStatus == HTTP_SUCCESS_ASYNC_DONE ||
		hcStatus == HTTP_SUCCESS_ASYNC_NOFLUSH_DONE;
}

inline bool IsAsyncFlushStatus(HTTP_CODE hcStatus)
{
	return 
		hcStatus == HTTP_SUCCESS_ASYNC ||
		hcStatus == HTTP_SUCCESS_ASYNC_DONE;
}

inline bool IsAsyncNoFlushStatus(HTTP_CODE hcStatus)
{
	return 
		hcStatus == HTTP_SUCCESS_ASYNC_NOFLUSH ||
		hcStatus == HTTP_SUCCESS_ASYNC_NOFLUSH_DONE;
}

ATL_NOINLINE inline HTTP_CODE AtlsHttpError(WORD wStatus, WORD wSubErr) throw()
{
	return HTTP_ERROR(wStatus, wSubErr);
}

}; // namespace ATL
#pragma pack(pop)

#endif // __ATLSERR_H__
