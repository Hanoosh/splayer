// DlgChkUpdater.cpp : implementation file
//

#include "stdafx.h"
#include "mplayerc.h"
#include "DlgChkUpdater.h"
#include "../../svplib/svplib.h"
#include "MainFrm.h"
// CDlgChkUpdater dialog

IMPLEMENT_DYNAMIC(CDlgChkUpdater, CDialog)

CDlgChkUpdater::CDlgChkUpdater(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgChkUpdater::IDD, pParent)
{

}

CDlgChkUpdater::~CDlgChkUpdater()
{
}

void CDlgChkUpdater::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROGRESS1, cprog_chker);
	DDX_Control(pDX, IDC_STATIC1, cs_stat);
	DDX_Control(pDX, IDCANCEL, cb_close);
}


BEGIN_MESSAGE_MAP(CDlgChkUpdater, CDialog)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDCANCEL, &CDlgChkUpdater::OnBnClickedCancel)
END_MESSAGE_MAP()


// CDlgChkUpdater message handlers

BOOL CDlgChkUpdater::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	SetTimer(IDT_CHECK_TICK, 600, NULL);
	cprog_chker.SetRange(0, 1000);
	cprog_chker.SetPos(0);
	m_lostPos = 0;
	moreTick = 0;
	CMainFrame* pFrame = ((CMainFrame*)GetParentFrame());
	if(pFrame){
		SVP_CheckUpdaterExe(&pFrame->m_bCheckingUpdater, 1);	
	}else{
		SVP_CheckUpdaterExe(&mChk, 1);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgChkUpdater::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	if(IDT_CHECK_TICK == nIDEvent){
		if(m_lostPos < 500){
			m_lostPos += 80;
		}else if(m_lostPos < 800){
			m_lostPos += 20;
		}else if(m_lostPos < 900){
			m_lostPos += 10;
		}else if(m_lostPos < 960){
			m_lostPos += 4;
		}else{
			m_lostPos += 1;
		}
		if(moreTick == 0){
			CMainFrame* pFrame = ((CMainFrame*)GetParentFrame());
			if(pFrame){
				mChk = pFrame->m_bCheckingUpdater;
			}
			if(!mChk){
				moreTick = 10;
				
			}
		}

		if(moreTick == 1){
			cprog_chker.ShowWindow(SW_HIDE);
			cs_stat.ShowWindow(SW_SHOW);
			cb_close.SetWindowText(_T("关闭"));
			KillTimer(IDT_CHECK_TICK);
		}
		if(moreTick > 1 ){
			moreTick --;
		}
		cprog_chker.SetPos(m_lostPos);

		HANDLE hMutex = OpenMutex(MUTEX_ALL_ACCESS,	FALSE, _T("SVPLAYER_REAL_UPDATER"));
		if (hMutex ) {
			HWND hWndPrevious = ::FindWindow( NULL , _T("射手影音自动更新程序"));// ::GetDesktopWindow(),GW_CHILD);
			while( ::IsWindow(hWndPrevious)){
				//if (::GetProp(hWndPrevious, "WOWSocks")){
				//if (!::IsWindowVisible(hWndPrevious) ){
				::ShowWindow(hWndPrevious,SW_SHOW);
				::ShowWindow(hWndPrevious,SW_RESTORE);
				::SetForegroundWindow(hWndPrevious);
				::GetLastActivePopup(hWndPrevious);
			
				//}
				//}
				break;
				hWndPrevious = ::GetWindow(hWndPrevious,GW_HWNDNEXT);
			}
			KillTimer(IDT_CHECK_TICK);
			OnOK();

		}

	}

	CDialog::OnTimer(nIDEvent);
}

void CDlgChkUpdater::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	OnCancel();
}
