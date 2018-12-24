
// test_demoDlg.h : 头文件
//

#pragma once
#include "afxwin.h"
#include <vector>
#include "LocalPlayer.h"
#include "RealPlayer.h"

// Ctest_demoDlg 对话框
class Ctest_demoDlg : public CDialogEx
{
// 构造
public:
	Ctest_demoDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TEST_DEMO_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
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
