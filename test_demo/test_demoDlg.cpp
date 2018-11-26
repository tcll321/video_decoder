
// test_demoDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "test_demo.h"
#include "test_demoDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// Ctest_demoDlg 对话框



Ctest_demoDlg::Ctest_demoDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_TEST_DEMO_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void Ctest_demoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_CHANNELCNT, m_cmbChannelCount);
	DDX_Control(pDX, IDC_EDIT_URL, m_edtUrl);
}

BEGIN_MESSAGE_MAP(Ctest_demoDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTN_SELECTFILE, &Ctest_demoDlg::OnBnClickedBtnSelectfile)
	ON_BN_CLICKED(IDC_BTN_START, &Ctest_demoDlg::OnBnClickedBtnStart)
	ON_BN_CLICKED(IDC_BTN_STOP, &Ctest_demoDlg::OnBnClickedBtnStop)
	ON_CBN_SELCHANGE(IDC_COMBO_CHANNELCNT, &Ctest_demoDlg::OnCbnSelchangeComboChannelcnt)
END_MESSAGE_MAP()


// Ctest_demoDlg 消息处理程序

BOOL Ctest_demoDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	m_cmbChannelCount.InsertString(0, "1");
	m_cmbChannelCount.InsertString(1, "4");
	m_cmbChannelCount.InsertString(2, "9");
	m_cmbChannelCount.InsertString(3, "12");
	m_cmbChannelCount.InsertString(4, "16");
	m_cmbChannelCount.InsertString(5, "25");

	m_edtUrl.SetWindowTextA("D:\\test.h264");
	m_cmbChannelCount.SetCurSel(0);
	OnCbnSelchangeComboChannelcnt();
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void Ctest_demoDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR Ctest_demoDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void Ctest_demoDlg::OnBnClickedBtnSelectfile()
{
	// TODO: 在此添加控件通知处理程序代码
	CFileDialog fDlg(TRUE);
	if (fDlg.DoModal() == IDOK)
	{
		CString strFileName;
		strFileName = fDlg.GetPathName();
		m_edtUrl.SetWindowTextA(strFileName);
	}
}


void Ctest_demoDlg::OnBnClickedBtnStart()
{
	// TODO: 在此添加控件通知处理程序代码
	CPlayer* pPlay = new CLocalPlayer("d:\\test.h264", 0);
	pPlay->Play();
	pPlay->CreatePlayer(0);
}


void Ctest_demoDlg::OnBnClickedBtnStop()
{
	// TODO: 在此添加控件通知处理程序代码
}


void Ctest_demoDlg::OnCbnSelchangeComboChannelcnt()
{
	// TODO: 在此添加控件通知处理程序代码
	CString strWndCount;
	m_cmbChannelCount.GetLBText(m_cmbChannelCount.GetCurSel(), strWndCount);
	m_curSplit = atoi(strWndCount);
}
