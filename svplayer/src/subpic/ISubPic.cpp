/* 
 *	Copyright (C) 2003-2006 Gabest
 *	http://www.gabest.org
 *
 *  This Program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *   
 *  This Program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *   
 *  You should have received a copy of the GNU General Public License
 *  along with GNU Make; see the file COPYING.  If not, write to
 *  the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA. 
 *  http://www.gnu.org/copyleft/gpl.html
 *
 */

#include "stdafx.h"
#include "ISubPic.h"
#include "..\DSUtil\DSUtil.h"

#include <afxtempl.h>
#include "..\apps\mplayerc\mplayerc.h"

#include "..\svplib\svplib.h"
//#define  LOGSUBRECT

#define SVP_LogMsg5  __noop
//
// ISubPicImpl
//

ISubPicImpl::ISubPicImpl() 
	: CUnknown(NAME("ISubPicImpl"), NULL)
	, m_rtStart(0), m_rtStop(0)
	, m_rcDirty(0, 0, 0, 0), m_maxsize(0, 0), m_size(0, 0), m_vidrect(0, 0, 0, 0)
	, m_VirtualTextureSize(0, 0), m_VirtualTextureTopLeft (0, 0)
{
}

STDMETHODIMP ISubPicImpl::NonDelegatingQueryInterface(REFIID riid, void** ppv)
{
	return 
		QI(ISubPic)
		__super::NonDelegatingQueryInterface(riid, ppv);
}

// ISubPic

STDMETHODIMP_(REFERENCE_TIME) ISubPicImpl::GetStart()
{
	return(m_rtStart);
}

STDMETHODIMP_(REFERENCE_TIME) ISubPicImpl::GetStop()
{
	return(m_rtStop);
}

STDMETHODIMP_(void) ISubPicImpl::SetStart(REFERENCE_TIME rtStart)
{
	m_rtStart = rtStart;
}

STDMETHODIMP_(void) ISubPicImpl::SetStop(REFERENCE_TIME rtStop)
{
	m_rtStop = rtStop;
}

STDMETHODIMP ISubPicImpl::CopyTo(ISubPic* pSubPic)
{
	if(!pSubPic)
		return E_POINTER;

	pSubPic->SetStart(m_rtStart);
	pSubPic->SetStop(m_rtStop);
	pSubPic->SetDirtyRect(m_rcDirty);
	pSubPic->SetSize(m_size, m_vidrect);
	pSubPic->SetVirtualTextureSize(m_VirtualTextureSize, m_VirtualTextureTopLeft);

	return S_OK;
}

STDMETHODIMP ISubPicImpl::GetDirtyRect(RECT* pDirtyRect)
{
	return pDirtyRect ? *pDirtyRect = m_rcDirty, S_OK : E_POINTER;
}
STDMETHODIMP ISubPicImpl::GetSourceAndDest(SIZE* pSize, RECT* pRcSource, RECT* pRcDest)
{
	CheckPointer (pRcSource, E_POINTER);
	CheckPointer (pRcDest,	 E_POINTER);

	if(m_size.cx > 0 && m_size.cy > 0)
	{
		CRect		rcTemp = m_rcDirty;

		// FIXME
		rcTemp.DeflateRect(1, 1);

		*pRcSource = rcTemp;
		
		

		CRect RcDest (rcTemp.left   * pSize->cx / m_VirtualTextureSize.cx,
						  rcTemp.top    * pSize->cy / m_VirtualTextureSize.cy,
						  rcTemp.right  * pSize->cx / m_VirtualTextureSize.cx,
						  rcTemp.bottom * pSize->cy / m_VirtualTextureSize.cy);
		LONG cy = min ( pSize->cx * 4/5 , pSize->cy );
		if(cy < pSize->cy ){
			cy = pSize->cy - cy;
			RcDest.DeflateRect( RcDest.Width() *  cy  / pSize->cy / 2 , RcDest.Height() *   cy  / pSize->cy / 2);
		}
		
		 *pRcDest = RcDest;
#ifdef LOGSUBRECT
		CString szLog ;
		szLog.Format(_T(" resize w %d %d h %d %d") , pSize->cx , m_VirtualTextureSize.cx ,pSize->cy , m_VirtualTextureSize.cy);
		SVP_LogMsg(szLog);
#endif
		return S_OK;
	}
	else
		return E_INVALIDARG;
}

STDMETHODIMP ISubPicImpl::SetDirtyRect(RECT* pDirtyRect)
{
	return pDirtyRect ? m_rcDirty = *pDirtyRect, S_OK : E_POINTER;
}

STDMETHODIMP ISubPicImpl::GetMaxSize(SIZE* pMaxSize)
{
	return pMaxSize ? *pMaxSize = m_maxsize, S_OK : E_POINTER;
}

STDMETHODIMP ISubPicImpl::SetSize(SIZE size, RECT vidrect)
{
	m_size = size;
	m_vidrect = vidrect;

	if(m_size.cx > m_maxsize.cx)
	{
		m_size.cy = MulDiv(m_size.cy, m_maxsize.cx, m_size.cx);
		m_size.cx = m_maxsize.cx;
	}

	if(m_size.cy > m_maxsize.cy)
	{
		m_size.cx = MulDiv(m_size.cx, m_maxsize.cy, m_size.cy);
		m_size.cy = m_maxsize.cy;
	}

	if(m_size.cx != size.cx || m_size.cy != size.cy)
	{
		m_vidrect.top = MulDiv(m_vidrect.top, m_size.cx, size.cx);
		m_vidrect.bottom = MulDiv(m_vidrect.bottom, m_size.cx, size.cx);
		m_vidrect.left = MulDiv(m_vidrect.left, m_size.cy, size.cy);
		m_vidrect.right = MulDiv(m_vidrect.right, m_size.cy, size.cy);
	}
	m_VirtualTextureSize = m_size;

	return S_OK;
}

STDMETHODIMP ISubPicImpl::SetVirtualTextureSize (const SIZE pSize, const POINT pTopLeft)
{
	m_VirtualTextureSize.SetSize (pSize.cx, pSize.cy);
	m_VirtualTextureTopLeft.SetPoint (pTopLeft.x, pTopLeft.y);
	
	return S_OK;
}

//
// ISubPicAllocatorImpl
//

ISubPicAllocatorImpl::ISubPicAllocatorImpl(SIZE cursize, bool fDynamicWriteOnly, bool fPow2Textures)
	: CUnknown(NAME("ISubPicAllocatorImpl"), NULL)
	, m_cursize(cursize)
	, m_fDynamicWriteOnly(fDynamicWriteOnly)
	, m_fPow2Textures(fPow2Textures)
{
	m_curvidrect = CRect(CPoint(0,0), m_cursize);
	
}

STDMETHODIMP ISubPicAllocatorImpl::NonDelegatingQueryInterface(REFIID riid, void** ppv)
{
	return 
		QI(ISubPicAllocator)
		__super::NonDelegatingQueryInterface(riid, ppv);
}

// ISubPicAllocator
STDMETHODIMP ISubPicAllocatorImpl::Lock(){
	m_pLock.Lock();
	return  S_OK;
}
STDMETHODIMP ISubPicAllocatorImpl::Unlock(){
	m_pLock.Unlock();
	return  S_OK ;
}
STDMETHODIMP ISubPicAllocatorImpl::SetCurSize(SIZE cursize)
{
	m_cursize = cursize; 
	return S_OK;
}

STDMETHODIMP ISubPicAllocatorImpl::SetCurVidRect(RECT curvidrect)
{
	m_curvidrect = curvidrect; 
	return S_OK;
}

STDMETHODIMP ISubPicAllocatorImpl::GetStatic(ISubPic** ppSubPic)
{
	if(!ppSubPic)
		return E_POINTER;

	if(!m_pStatic)
	{
		if(!Alloc(true, &m_pStatic) || !m_pStatic) 
			return E_OUTOFMEMORY;
	}

	m_pStatic->SetSize(m_cursize, m_curvidrect);

	(*ppSubPic = m_pStatic)->AddRef();

	return S_OK;
}

STDMETHODIMP ISubPicAllocatorImpl::AllocDynamic(ISubPic** ppSubPic)
{
	if(!ppSubPic)
		return E_POINTER;

	if(!Alloc(false, ppSubPic) || !*ppSubPic)
		return E_OUTOFMEMORY;

	(*ppSubPic)->SetSize(m_cursize, m_curvidrect);

	return S_OK;
}

STDMETHODIMP_(bool) ISubPicAllocatorImpl::IsDynamicWriteOnly()
{
	return(m_fDynamicWriteOnly);
}

STDMETHODIMP ISubPicAllocatorImpl::ChangeDevice(IUnknown* pDev)
{
	m_pStatic = NULL;
	return S_OK;
}


//
// ISubPicProviderImpl
//

ISubPicProviderImpl::ISubPicProviderImpl(CCritSec* pLock)
	: CUnknown(NAME("ISubPicProviderImpl"), NULL)
	, m_pLock(pLock)
{
}

ISubPicProviderImpl::~ISubPicProviderImpl()
{
}

STDMETHODIMP ISubPicProviderImpl::NonDelegatingQueryInterface(REFIID riid, void** ppv)
{
	return
		QI(ISubPicProvider)
		__super::NonDelegatingQueryInterface(riid, ppv);
}

// ISubPicProvider

STDMETHODIMP ISubPicProviderImpl::Lock()
{
	return m_pLock ? m_pLock->Lock(), S_OK : E_FAIL;
}

STDMETHODIMP ISubPicProviderImpl::Unlock()
{
	return m_pLock ? m_pLock->Unlock(), S_OK : E_FAIL;
}

//
// ISubPicQueueImpl
//

ISubPicQueueImpl::ISubPicQueueImpl(ISubPicAllocator* pAllocator, HRESULT* phr) 
	: CUnknown(NAME("ISubPicQueueImpl"), NULL)
	, m_pAllocator(pAllocator)
	, m_rtNow(0)
	, m_fps(25.0)
{
	if(phr) *phr = S_OK;

	if(!m_pAllocator)
	{
		if(phr) *phr = E_FAIL;
		return;
	}
}

ISubPicQueueImpl::~ISubPicQueueImpl()
{
}

STDMETHODIMP ISubPicQueueImpl::NonDelegatingQueryInterface(REFIID riid, void** ppv)
{
	return 
		QI(ISubPicQueue)
		__super::NonDelegatingQueryInterface(riid, ppv);
}

// ISubPicQueue

STDMETHODIMP ISubPicQueueImpl::SetSubPicProvider(ISubPicProvider* pSubPicProvider)
{
	CAutoLock cAutoLock(&m_csSubPicProvider);

//	if(m_pSubPicProvider != pSubPicProvider)
	{
		m_pSubPicProvider = pSubPicProvider;

		Invalidate();
	}

	return S_OK;
}

STDMETHODIMP ISubPicQueueImpl::GetSubPicProvider(ISubPicProvider** pSubPicProvider)
{
	if(!pSubPicProvider)
		return E_POINTER;

	CAutoLock cAutoLock(&m_csSubPicProvider);

	if(m_pSubPicProvider)
		(*pSubPicProvider = m_pSubPicProvider)->AddRef();

	return !!*pSubPicProvider ? S_OK : E_FAIL;
}

STDMETHODIMP ISubPicQueueImpl::SetFPS(double fps)
{
	m_fps = fps;

	return S_OK;
}

STDMETHODIMP ISubPicQueueImpl::SetTime(REFERENCE_TIME rtNow)
{
	m_rtNow = rtNow;

	return S_OK;
}

// private

HRESULT ISubPicQueueImpl::RenderTo(ISubPic* pSubPic, REFERENCE_TIME rtStart, REFERENCE_TIME rtStop, double fps)
{
	HRESULT hr = E_FAIL;

	if(!pSubPic)
		return hr;

	CComPtr<ISubPicProvider> pSubPicProvider;
	if(FAILED(GetSubPicProvider(&pSubPicProvider)) || !pSubPicProvider)
		return hr;

	if(FAILED(pSubPicProvider->Lock()))
		return hr;

	SubPicDesc spd;
	if(SUCCEEDED(pSubPic->ClearDirtyRect(0xFF000000))
	&& SUCCEEDED(pSubPic->Lock(spd)))
	{
		CRect r(0,0,0,0);
		hr = pSubPicProvider->Render(spd, (rtStart+rtStop)/2, fps, r);

		pSubPic->SetStart(rtStart);
		pSubPic->SetStop(rtStop);

		pSubPic->Unlock(r);
	}

	pSubPicProvider->Unlock();

	return hr;
}

//
// CSubPicQueue
//

CSubPicQueue::CSubPicQueue(int nMaxSubPic, ISubPicAllocator* pAllocator, HRESULT* phr) 
	: ISubPicQueueImpl(pAllocator, phr)
	, m_nMaxSubPic(nMaxSubPic)
	, m_rtQueueStart(0)
{
	if(phr && FAILED(*phr))
		return;

	if(m_nMaxSubPic < 1)
		{if(phr) *phr = E_INVALIDARG; return;}

	m_fBreakBuffering = false;
	for(int i = 0; i < EVENT_COUNT; i++) 
		m_ThreadEvents[i] = CreateEvent(NULL, FALSE, FALSE, NULL);
	CAMThread::Create();
}

CSubPicQueue::~CSubPicQueue()
{
	m_fBreakBuffering = true;
	SetEvent(m_ThreadEvents[EVENT_EXIT]);
	CAMThread::Close();
	for(int i = 0; i < EVENT_COUNT; i++) 
		CloseHandle(m_ThreadEvents[i]);
}

// ISubPicQueue

STDMETHODIMP CSubPicQueue::SetFPS(double fps)
{
	HRESULT hr = __super::SetFPS(fps);
	if(FAILED(hr)) return hr;

	SetEvent(m_ThreadEvents[EVENT_TIME]);

	return S_OK;
}

STDMETHODIMP CSubPicQueue::SetTime(REFERENCE_TIME rtNow)
{
	SVP_LogMsg5(L"SetTime2 %f", double(rtNow));
	HRESULT hr = __super::SetTime(rtNow);
	if(FAILED(hr)) return hr;

	SetEvent(m_ThreadEvents[EVENT_TIME]);

	return S_OK;
}

STDMETHODIMP CSubPicQueue::Invalidate(REFERENCE_TIME rtInvalidate)
{
	{
//		CAutoLock cQueueLock(&m_csQueueLock);
//		RemoveAll();

		m_rtInvalidate = rtInvalidate;
		m_fBreakBuffering = true;
		SetEvent(m_ThreadEvents[EVENT_TIME]);
	}

	return S_OK;
}

STDMETHODIMP_(bool) CSubPicQueue::LookupSubPic(REFERENCE_TIME rtNow, ISubPic** ppSubPic)
{
	if(!ppSubPic)
		return(false);

	*ppSubPic = NULL;

	CAutoLock cQueueLock(&m_csQueueLock);

	POSITION pos = GetHeadPosition();
	while(pos)
	{
		CComPtr<ISubPic> pSubPic = GetNext(pos);
		if(pSubPic->GetStart() <= rtNow && rtNow < pSubPic->GetStop())
		{
			*ppSubPic = pSubPic.Detach();
			break;
		}
	}

	return(!!*ppSubPic);
}

STDMETHODIMP CSubPicQueue::GetStats(int& nSubPics, REFERENCE_TIME& rtNow, REFERENCE_TIME& rtStart, REFERENCE_TIME& rtStop)
{
	CAutoLock cQueueLock(&m_csQueueLock);

	nSubPics = GetCount();
	rtNow = m_rtNow;
	rtStart = m_rtQueueStart;
	rtStop = GetCount() > 0 ? GetTail()->GetStop() : rtStart;

	return S_OK;
}

STDMETHODIMP CSubPicQueue::GetStats(int nSubPic, REFERENCE_TIME& rtStart, REFERENCE_TIME& rtStop)
{
	CAutoLock cQueueLock(&m_csQueueLock);

	rtStart = rtStop = -1;

	if(nSubPic >= 0 && nSubPic < (int)GetCount())
	{
		if(POSITION pos = FindIndex(nSubPic))
		{
			rtStart = GetAt(pos)->GetStart();
			rtStop = GetAt(pos)->GetStop();
		}
	}
	else
	{
		return E_INVALIDARG;
	}

	return S_OK;
}

// private

REFERENCE_TIME CSubPicQueue::UpdateQueue()
{
	CAutoLock cQueueLock(&m_csQueueLock);

	REFERENCE_TIME rtNow = m_rtNow;

	if(rtNow < m_rtQueueStart)
	{
		RemoveAll();
	}
	else
	{
		while(GetCount() > 0 && rtNow >= GetHead()->GetStop())
			RemoveHead();
	}

	m_rtQueueStart = rtNow;

	if(GetCount() > 0)
		rtNow = GetTail()->GetStop();

	return(rtNow);
}

void CSubPicQueue::AppendQueue(ISubPic* pSubPic)
{
	CAutoLock cQueueLock(&m_csQueueLock);

	AddTail(pSubPic);
}

// overrides

DWORD CSubPicQueue::ThreadProc()
{	
	SetThreadPriority(m_hThread, THREAD_PRIORITY_NORMAL/*THREAD_PRIORITY_BELOW_NORMAL*/);

	while((WaitForMultipleObjects(EVENT_COUNT, m_ThreadEvents, FALSE, INFINITE) - WAIT_OBJECT_0) == EVENT_TIME)
	{
		double fps = m_fps;
		REFERENCE_TIME rtNow = UpdateQueue();

		int nMaxSubPic = m_nMaxSubPic;

		CComPtr<ISubPicProvider> pSubPicProvider;
		if(SUCCEEDED(GetSubPicProvider(&pSubPicProvider)) && pSubPicProvider
		&& SUCCEEDED(pSubPicProvider->Lock()) && SUCCEEDED(m_pAllocator->Lock()))
		{
			for(POSITION pos = pSubPicProvider->GetStartPosition(rtNow, fps); 
				pos && !m_fBreakBuffering && GetCount() < (size_t)nMaxSubPic; 
				pos = pSubPicProvider->GetNext(pos))
			{
				REFERENCE_TIME rtStart = pSubPicProvider->GetStart(pos, fps);
				REFERENCE_TIME rtStop = pSubPicProvider->GetStop(pos, fps);

				if(m_rtNow >= rtStart)
				{
//						m_fBufferUnderrun = true;
					if(m_rtNow >= rtStop) continue;
				}

				if(rtStart >= m_rtNow + 60*10000000i64) // we are already one minute ahead, this should be enough
					break;

				if(rtNow < rtStop)
				{
					SIZE	MaxTextureSize, VirtualSize;
					POINT	VirtualTopLeft;
					HRESULT	hr2;
					if (SUCCEEDED (hr2 = pSubPicProvider->GetTextureSize(pos, MaxTextureSize, VirtualSize, VirtualTopLeft)))
						m_pAllocator->SetMaxTextureSize(MaxTextureSize);

					CComPtr<ISubPic> pStatic;
					if(FAILED(m_pAllocator->GetStatic(&pStatic)))
						break;

					HRESULT hr = RenderTo(pStatic, rtStart, rtStop, fps);

					if(FAILED(hr))
						break;

					if(S_OK != hr) // subpic was probably empty
						continue;

					CComPtr<ISubPic> pDynamic;
					if(FAILED(m_pAllocator->AllocDynamic(&pDynamic))
					|| FAILED(pStatic->CopyTo(pDynamic)))
						break;

					if (SUCCEEDED (hr2))
						pDynamic->SetVirtualTextureSize (VirtualSize, VirtualTopLeft);

					AppendQueue(pDynamic);
				}
			}

			pSubPicProvider->Unlock();
			m_pAllocator->Unlock();
		}

		if(m_fBreakBuffering)
		{
			CAutoLock cQueueLock(&m_csQueueLock);

			REFERENCE_TIME rtInvalidate = m_rtInvalidate;

			while(GetCount() && GetTail()->GetStop() > rtInvalidate)
			{
				if(GetTail()->GetStart() < rtInvalidate) GetTail()->SetStop(rtInvalidate);
				else RemoveTail();
			}

			m_fBreakBuffering = false;
		}
	}

	return(0);
}

//
// CSubPicQueueNoThread
//

CSubPicQueueNoThread::CSubPicQueueNoThread(ISubPicAllocator* pAllocator, HRESULT* phr) 
	: ISubPicQueueImpl(pAllocator, phr)
{
}

CSubPicQueueNoThread::~CSubPicQueueNoThread()
{
}

// ISubPicQueue

STDMETHODIMP CSubPicQueueNoThread::Invalidate(REFERENCE_TIME rtInvalidate)
{
	CAutoLock cQueueLock(&m_csLock);

	m_pSubPic = NULL;

	return S_OK;
}

STDMETHODIMP_(bool) CSubPicQueueNoThread::LookupSubPic(REFERENCE_TIME rtNow, ISubPic** ppSubPic)
{
	if(!ppSubPic)
		return(false);

	*ppSubPic = NULL;

	CComPtr<ISubPic> pSubPic;

	{
		CAutoLock cAutoLock(&m_csLock);

		if(!m_pSubPic)
		{
			if(FAILED(m_pAllocator->AllocDynamic(&m_pSubPic)))
				return(false);
		}

		pSubPic = m_pSubPic;
	}

	if(pSubPic->GetStart() <= rtNow && rtNow < pSubPic->GetStop())
	{
		(*ppSubPic = pSubPic)->AddRef();
	}
	else
	{
		CComPtr<ISubPicProvider> pSubPicProvider;
		if(SUCCEEDED(GetSubPicProvider(&pSubPicProvider)) && pSubPicProvider
		&& SUCCEEDED(pSubPicProvider->Lock()))
		{
			double fps = m_fps;

			if(POSITION pos = pSubPicProvider->GetStartPosition(rtNow, fps))
			{
				REFERENCE_TIME rtStart = pSubPicProvider->GetStart(pos, fps);
				REFERENCE_TIME rtStop = pSubPicProvider->GetStop(pos, fps);

				if(pSubPicProvider->IsAnimated(pos))
				{
					rtStart = rtNow;
					rtStop = rtNow+1;
				}

				if(rtStart <= rtNow && rtNow < rtStop)
				{
					SIZE	MaxTextureSize, VirtualSize;
					POINT	VirtualTopLeft;
					HRESULT	hr2;
					if (SUCCEEDED (hr2 = pSubPicProvider->GetTextureSize(pos, MaxTextureSize, VirtualSize, VirtualTopLeft)))
						m_pAllocator->SetMaxTextureSize(MaxTextureSize);
					
					if(m_pAllocator->IsDynamicWriteOnly())
					{
						CComPtr<ISubPic> pStatic;
						if(SUCCEEDED(m_pAllocator->GetStatic(&pStatic))
						&& SUCCEEDED(RenderTo(pStatic, rtStart, rtStop, fps))
						&& SUCCEEDED(pStatic->CopyTo(pSubPic)))
							(*ppSubPic = pSubPic)->AddRef();
					}
					else
					{
						if(SUCCEEDED(RenderTo(m_pSubPic, rtStart, rtStop, fps)))
							(*ppSubPic = pSubPic)->AddRef();
					}
					if (SUCCEEDED(hr2))
						pSubPic->SetVirtualTextureSize (VirtualSize, VirtualTopLeft);
				}
			}

			pSubPicProvider->Unlock();

			if(*ppSubPic)
			{
				CAutoLock cAutoLock(&m_csLock);

				m_pSubPic = *ppSubPic;
			}
		}
	}

	return(!!*ppSubPic);
}

STDMETHODIMP CSubPicQueueNoThread::GetStats(int& nSubPics, REFERENCE_TIME& rtNow, REFERENCE_TIME& rtStart, REFERENCE_TIME& rtStop)
{
	CAutoLock cAutoLock(&m_csLock);

	nSubPics = 0;
	rtNow = m_rtNow;
	rtStart = rtStop = 0;

	if(m_pSubPic)
	{
		nSubPics = 1;
		rtStart = m_pSubPic->GetStart();
		rtStop = m_pSubPic->GetStop();
	}

	return S_OK;
}

STDMETHODIMP CSubPicQueueNoThread::GetStats(int nSubPic, REFERENCE_TIME& rtStart, REFERENCE_TIME& rtStop)
{
	CAutoLock cAutoLock(&m_csLock);

	if(!m_pSubPic || nSubPic != 0)
		return E_INVALIDARG;

	rtStart = m_pSubPic->GetStart();
	rtStop = m_pSubPic->GetStop();

	return S_OK;
}

//
// ISubPicAllocatorPresenterImpl
//

ISubPicAllocatorPresenterImpl::ISubPicAllocatorPresenterImpl(HWND hWnd, HRESULT& hr)
	: CUnknown(NAME("ISubPicAllocatorPresenterImpl"), NULL)
	, m_hWnd(hWnd)
	, m_NativeVideoSize(0, 0), m_AspectRatio(0, 0)
	, m_VideoRect(0, 0, 0, 0), m_WindowRect(0, 0, 0, 0)
	, m_fps(25.0)
	, m_lSubtitleDelay(0), m_lSubtitleDelay2(0)
	, m_pSubPicQueue(NULL) , m_pSubPicQueue2(NULL)
	, m_last_2ndSubBaseLineUp(-1), m_last_2ndSubBaseLineDown(1000000)
	, m_last_2ndSubBaseLineUp2(-1), m_last_2ndSubBaseLineDown2(1000000)
	, m_last_2sub_relative(-1)
	, m_force_pos_counter(0)
{
    if(!IsWindow(m_hWnd)) {hr = E_INVALIDARG; return;}
	GetWindowRect(m_hWnd, &m_WindowRect);
	SetVideoAngle(Vector(), false);
	hr = S_OK;
}

ISubPicAllocatorPresenterImpl::~ISubPicAllocatorPresenterImpl()
{
}

STDMETHODIMP ISubPicAllocatorPresenterImpl::NonDelegatingQueryInterface(REFIID riid, void** ppv)
{
	//SVP_LogMsg5(L"ISubPic %s" , CStringFromGUID(riid));
	return 
		QI(ISubPicAllocatorPresenter)
		QI(ISubPicAllocatorPresenterRender)
		QI(ISubPicAllocatorPresenter2)
		__super::NonDelegatingQueryInterface(riid, ppv);
}

void ISubPicAllocatorPresenterImpl::AlphaBltSubPic(CSize size, SubPicDesc* pTarget)
{
	size.cy -= AfxGetMyApp()->GetBottomSubOffset();

	CComPtr<ISubPic> pSubPic;
	CComPtr<ISubPic> pSubPic2;
	BOOL bltSub1 = false, bltSub2 = false;
	CRect rcSource1, rcSource2, rcDest1, rcDest2;
#ifdef LOGSUBRECT
	CString szD1, szD2;
#endif
	if(m_pSubPicQueue->LookupSubPic(m_rtNow, &pSubPic))
	{
		if (SUCCEEDED (pSubPic->GetSourceAndDest(&size, rcSource1, rcDest1))){
			//pSubPic->AlphaBlt(rcSource, rcDest, pTarget);
			bltSub1 = true;
		}
#ifdef LOGSUBRECT
		UINT iTotalLenSec = (UINT)( (INT64) m_rtNow / 10000000 );
		szD1.Format(_T("  sub1 size %d %d   , source %d %d %d %d , dest  %d %d %d %d , time %d:%02d") , 
			size.cx, size.cy, rcSource1.top, rcSource1.right, rcSource1.bottom, rcSource1.left, 
			 rcDest1.top, rcDest1.right, rcDest1.bottom, rcDest1.left,(int)iTotalLenSec/60, iTotalLenSec % 60 );
#endif

	}
	
	if(m_pSubPicQueue2->LookupSubPic(m_rtNow2, &pSubPic2))
	{
		if (SUCCEEDED (pSubPic2->GetSourceAndDest(&size, rcSource2, rcDest2))){
			bltSub2 = true;
		}
#ifdef LOGSUBRECT
		UINT iTotalLenSec = (UINT)( (INT64) m_rtNow2 / 10000000 );
		szD2.Format(_T("  sub2 size %d %d   , source %d %d %d %d , dest  %d %d %d %d , time %d:%02d") , 
			size.cx, size.cy, rcSource2.top, rcSource2.right, rcSource2.bottom, rcSource2.left, 
			rcDest2.top, rcDest2.right, rcDest2.bottom, rcDest2.left,(int)iTotalLenSec/60, iTotalLenSec % 60 );
#endif			
	}

	if(bltSub2 && rcDest2.top < 0)
		rcDest2.MoveToY(0);
	if(bltSub1 && rcDest1.top < 0)
		rcDest1.MoveToY(0);

	BOOL bForcePosBy2 =  (m_last_2ndSubBaseLineDown2 < 1000000) || (m_last_2ndSubBaseLineUp2 >= 0) ;
	BOOL bForcePosBy1 =  (m_last_2ndSubBaseLineDown < 1000000) || (m_last_2ndSubBaseLineUp >= 0);
	BOOL bForcePos =  bForcePosBy1 || bForcePosBy2 ;
	if(bltSub1 && bltSub2 || (bForcePos && (bltSub1 || bltSub2 ) )){
		//avoid overlap
		CRect rectInter;
		if(rectInter.IntersectRect(rcDest1, rcDest2) || bForcePos ){
			
			//there is overlap
			CPoint cent1 = rcDest1.CenterPoint(); // sub1 center
			CPoint cent2 = rcDest2.CenterPoint(); // sub2 center
			CPoint vcent( size.cx /2 , size.cy /2)  ; //video center
			int i_targetY = -1;

			//which one is closer to border?
			if  ( abs(cent1.y - vcent.y) > abs(cent2.y - vcent.y) && pSubPic2 ){ 
				//rcDest1 is outer(fixed) , move rcDest2
				
				if(cent2.y > cent1.y && pSubPic || (!pSubPic && m_last_2sub_relative == 1 ) ){ //sub2 is under sub1 and there are on top area of sreen
					m_last_2ndSubBaseLineUp =  max(m_last_2ndSubBaseLineUp ,  rcDest1.bottom);
					i_targetY = m_last_2ndSubBaseLineUp;
					m_last_2sub_relative = 1;
					  //moving down
				}else{
					if(rcDest1.top)
					{
						m_last_2ndSubBaseLineDown = min(m_last_2ndSubBaseLineDown, rcDest1.top );
						m_last_2sub_relative = 2;
					}

					i_targetY = m_last_2ndSubBaseLineDown  - rcDest2.Height()  ;
					
					 //moving up
				}
				if(i_targetY >= 0 && i_targetY < 1000000){
					rcDest2.MoveToY(i_targetY);
					if(pSubPic ){
						if(m_last_2sub_relative == 1)
						{
							rcDest1.MoveToY(max(0, i_targetY - rcDest1.Height()));
						}else{
							rcDest1.MoveToY(i_targetY + rcDest2.Height());
						}
					}
				}
			}else{
				//rcDest2 is outer(fixed) , move rcDest1
				if(cent1.y > cent2.y && pSubPic2 || (!pSubPic2 && m_last_2sub_relative == 3)){ //sub1 is under sub2 and there are on top area of sreen
					if(pSubPic2 ){
						m_last_2sub_relative = 3;	
						m_last_2ndSubBaseLineUp2 =  max( m_last_2ndSubBaseLineUp2 , rcDest2.bottom);  //moving down
					}
					i_targetY = m_last_2ndSubBaseLineUp2 ;
					
				}else{
					if(pSubPic2){
						m_last_2ndSubBaseLineDown2 = min(m_last_2ndSubBaseLineDown2, rcDest2.top );
						m_last_2sub_relative = 4;	
					}
					i_targetY = m_last_2ndSubBaseLineDown2  - rcDest1.Height() ; //moving up
					
				}
				if(i_targetY >= 0 && i_targetY < 1000000){
					rcDest1.MoveToY(i_targetY);
					if(pSubPic2){
						if(m_last_2sub_relative == 3)
						{
							rcDest2.MoveToY(max(0, i_targetY - rcDest2.Height()));
						}else{
							rcDest2.MoveToY(i_targetY + rcDest1.Height());
						}
					}
				}
			}

			m_last_pSubPic = pSubPic;
			m_last_pSubPic2 = pSubPic2;

		}
	}
	if(bltSub1)
		pSubPic->AlphaBlt(rcSource1, rcDest1, pTarget);
	if(bltSub2){
		pSubPic2->AlphaBlt(rcSource2, rcDest2, pTarget);
	}

	if( !bltSub1 && !bltSub2 && bForcePos){
		m_force_pos_counter++;
		if(m_force_pos_counter > 10){
			ResSetForcePos();
		}
		
	}

	
#ifdef LOGSUBRECT
	if(bltSub1 || bltSub2){
		SVP_LogMsg(szD1 + szD2);
	}
#endif
}

// ISubPicAllocatorPresenter

STDMETHODIMP_(SIZE) ISubPicAllocatorPresenterImpl::GetVideoSize(bool fCorrectAR)
{
	CSize VideoSize(m_NativeVideoSize);

	if(fCorrectAR && m_AspectRatio.cx > 0 && m_AspectRatio.cy > 0)
		VideoSize.cx = VideoSize.cy*m_AspectRatio.cx/m_AspectRatio.cy;

	return(VideoSize);
}

STDMETHODIMP_(void) ISubPicAllocatorPresenterImpl::SetPosition(RECT w, RECT v)
{
	bool fWindowPosChanged = !!(m_WindowRect != w);
	bool fWindowSizeChanged = !!(m_WindowRect.Size() != CRect(w).Size());

	m_WindowRect = w;

	bool fVideoRectChanged = !!(m_VideoRect != v);

	m_VideoRect = v;

	if(fWindowSizeChanged || fVideoRectChanged)
	{
		if(m_pAllocator)
		{
			m_pAllocator->SetCurSize(m_WindowRect.Size());
			m_pAllocator->SetCurVidRect(m_VideoRect);
		}

		if(m_pAllocator2)
		{
			m_pAllocator2->SetCurSize(m_WindowRect.Size());
			m_pAllocator2->SetCurVidRect(m_VideoRect);
		}
		if(m_pSubPicQueue)
		{
			m_pSubPicQueue->Invalidate();
		}
		if(m_pSubPicQueue2)
		{
			m_pSubPicQueue2->Invalidate();
		}
	}

	//CString szLog;
	//szLog.Format(_T("WVSize %d %d %d %d "), m_WindowRect.Width(), m_WindowRect.Height(), m_VideoRect.Width(), m_VideoRect.Height());
	//SVP_LogMsg(szLog);

	if(fWindowPosChanged || fVideoRectChanged)
		Paint(fWindowSizeChanged || fVideoRectChanged);

	ResSetForcePos();
}
void ISubPicAllocatorPresenterImpl::ResSetForcePos(int s)
{
	m_last_2ndSubBaseLineUp = (-1); 
	m_last_2ndSubBaseLineDown = (1000000);
	m_last_2ndSubBaseLineUp2 = (-1); 
	m_last_2ndSubBaseLineDown2 = (1000000);
	m_last_2sub_relative = (-1);
	m_force_pos_counter = 0;

}
STDMETHODIMP_(void) ISubPicAllocatorPresenterImpl::SetTime(REFERENCE_TIME rtNow)
{
/*
	if(m_rtNow <= rtNow && rtNow <= m_rtNow + 1000000)
		return;
*/
	m_rtNow = rtNow - m_lSubtitleDelay;
	m_rtNow2 = rtNow - m_lSubtitleDelay2;

	if(m_pSubPicQueue)
	{
		m_pSubPicQueue->SetTime(m_rtNow);
	}
	if(m_pSubPicQueue2)
	{
		m_pSubPicQueue2->SetTime(m_rtNow2);
	}
}

STDMETHODIMP_(void) ISubPicAllocatorPresenterImpl::SetSubtitleDelay(int delay_ms)
{
	m_lSubtitleDelay = delay_ms*10000;
}

STDMETHODIMP_(int) ISubPicAllocatorPresenterImpl::GetSubtitleDelay()
{
	return (m_lSubtitleDelay/10000);
}
STDMETHODIMP_(void) ISubPicAllocatorPresenterImpl::SetSubtitleDelay2(int delay_ms)
{
	m_lSubtitleDelay2 = delay_ms*10000;
}

STDMETHODIMP_(int) ISubPicAllocatorPresenterImpl::GetSubtitleDelay2()
{
	return (m_lSubtitleDelay2/10000);
}
STDMETHODIMP_(double) ISubPicAllocatorPresenterImpl::GetFPS()
{
	return(m_fps);
}

STDMETHODIMP_(void) ISubPicAllocatorPresenterImpl::SetSubPicProvider(ISubPicProvider* pSubPicProvider)
{
	m_SubPicProvider = pSubPicProvider;

	ResSetForcePos();

	if(m_pSubPicQueue)
		m_pSubPicQueue->SetSubPicProvider(pSubPicProvider);
}
STDMETHODIMP_(void) ISubPicAllocatorPresenterImpl::SetSubPicProvider2(ISubPicProvider* pSubPicProvider)
{
	m_SubPicProvider2 = pSubPicProvider;

	ResSetForcePos();

	if(m_pSubPicQueue2)
		m_pSubPicQueue2->SetSubPicProvider(pSubPicProvider);
}

STDMETHODIMP_(void) ISubPicAllocatorPresenterImpl::Invalidate(REFERENCE_TIME rtInvalidate)
{

	 ResSetForcePos();

	if(m_pSubPicQueue)
		m_pSubPicQueue->Invalidate(rtInvalidate);

	if(m_pSubPicQueue2)
		m_pSubPicQueue2->Invalidate(rtInvalidate);
}

#include <math.h>

void ISubPicAllocatorPresenterImpl::Transform(CRect r, Vector v[4])
{
	v[0] = Vector(r.left, r.top, 0);
	v[1] = Vector(r.right, r.top, 0);
	v[2] = Vector(r.left, r.bottom, 0);
	v[3] = Vector(r.right, r.bottom, 0);

	Vector center(r.CenterPoint().x, r.CenterPoint().y, 0);
	int l = (int)(Vector(r.Size().cx, r.Size().cy, 0).Length()*1.5f)+1;

	for(int i = 0; i < 4; i++)
	{
		v[i] = m_xform << (v[i] - center);
		v[i].z = v[i].z / l + 0.5f;
		v[i].x /= v[i].z*2;
		v[i].y /= v[i].z*2;
		//CString szLog;
		//szLog.Format(_T("%f %f %f %f %f %f"), l + 0.5f ,  v[i].z*2 , v[i].z*2 , v[i].x , v[i].y , v[i].z);
		//SVP_LogMsg(szLog);
		v[i] += center;
	}
	
}

STDMETHODIMP ISubPicAllocatorPresenterImpl::SetVideoAngle(Vector v, bool fRepaint)
{
	m_xform = XForm(Ray(Vector(0, 0, 0), v), Vector(1, 1, 1), false);
	if(fRepaint) Paint(true);
	return S_OK;
}
