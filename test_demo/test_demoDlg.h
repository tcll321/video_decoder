
// test_demoDlg.h : ͷ�ļ�
//

#pragma once
#include "afxwin.h"
#include <vector>
#include "LocalPlayer.h"
#include "RealPlayer.h"

// Ctest_demoDlg �Ի���
class Ctest_demoDlg : public CDialogEx
{
// ����
public:
	Ctest_demoDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TEST_DEMO_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CComboBox m_cmbChannelCount;
	CEdit m_edtUrl;
	afx_msg void OnBnClickedBtnSelectfile();
	afx_msg void OnBnClickedBtnStart();
	afx_msg void OnBnClickedBtnStop();
	afx_msg void OnCbnSelchangeComboChannelcnt();

private:
	int m_curSplit;
};
