
// test_demoDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "test_demo.h"
#include "test_demoDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// Ctest_demoDlg �Ի���



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


// Ctest_demoDlg ��Ϣ�������

BOOL Ctest_demoDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	m_cmbChannelCount.InsertString(0, "1");
	m_cmbChannelCount.InsertString(1, "4");
	m_cmbChannelCount.InsertString(2, "9");
	m_cmbChannelCount.InsertString(3, "12");
	m_cmbChannelCount.InsertString(4, "16");
	m_cmbChannelCount.InsertString(5, "25");

	m_edtUrl.SetWindowTextA("D:\\test.h264");
	m_cmbChannelCount.SetCurSel(0);
	OnCbnSelchangeComboChannelcnt();
	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void Ctest_demoDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR Ctest_demoDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void Ctest_demoDlg::OnBnClickedBtnSelectfile()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
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
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CPlayer* pPlay = new CLocalPlayer("d:\\test.h264", 0);
	pPlay->Play();
	pPlay->CreatePlayer(0);
}


void Ctest_demoDlg::OnBnClickedBtnStop()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
}


void Ctest_demoDlg::OnCbnSelchangeComboChannelcnt()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString strWndCount;
	m_cmbChannelCount.GetLBText(m_cmbChannelCount.GetCurSel(), strWndCount);
	m_curSplit = atoi(strWndCount);
}
