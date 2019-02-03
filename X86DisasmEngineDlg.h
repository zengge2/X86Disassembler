
// X86DisasmEngineDlg.h: 头文件
//

#pragma once


// CX86DisasmEngineDlg 对话框
class CX86DisasmEngineDlg : public CDialogEx
{
// 构造
public:
	CX86DisasmEngineDlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_X86DISASMENGINE_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedChfile();
	afx_msg void OnDropFiles(HDROP hDropInfo);
	void ZGDisassemble();
};
