
// X86DisasmEngineDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "X86DisasmEngine.h"
#include "X86DisasmEngineDlg.h"
#include "afxdialogex.h"


#include "inteldef.h"
#include "inteldis.h"
//////////////////////////////////////////////////////////////////////////
#define FILEBUGSIZE 1000
//////////////////////////////////////////////////////////////////////////

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx {
public:
    CAboutDlg();

    // 对话框数据
#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_ABOUTBOX };
#endif

protected:
    virtual void DoDataExchange(CDataExchange *pDX);    // DDX/DDV 支持

    // 实现
protected:
    DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX) {
}

void CAboutDlg::DoDataExchange(CDataExchange *pDX) {
    CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CX86DisasmEngineDlg 对话框



CX86DisasmEngineDlg::CX86DisasmEngineDlg(CWnd *pParent /*=nullptr*/)
    : CDialogEx(IDD_X86DISASMENGINE_DIALOG, pParent) {
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CX86DisasmEngineDlg::DoDataExchange(CDataExchange *pDX) {
    CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CX86DisasmEngineDlg, CDialogEx)
    ON_WM_SYSCOMMAND()
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    ON_BN_CLICKED(IDC_CHFILE, &CX86DisasmEngineDlg::OnBnClickedChfile)
    ON_WM_DROPFILES()
END_MESSAGE_MAP()


// CX86DisasmEngineDlg 消息处理程序

int m_nLineCount;

BOOL CX86DisasmEngineDlg::OnInitDialog() {
    CDialogEx::OnInitDialog();
    // 将“关于...”菜单项添加到系统菜单中。
    // IDM_ABOUTBOX 必须在系统命令范围内。
    ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
    ASSERT(IDM_ABOUTBOX < 0xF000);
    CMenu *pSysMenu = GetSystemMenu(FALSE);
    if (pSysMenu != nullptr) {
        BOOL bNameValid;
        CString strAboutMenu;
        bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
        ASSERT(bNameValid);
        if (!strAboutMenu.IsEmpty()) {
            pSysMenu->AppendMenu(MF_SEPARATOR);
            pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
        }
    }
    // 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
    //  执行此操作
    SetIcon(m_hIcon, TRUE);			// 设置大图标
    SetIcon(m_hIcon, FALSE);		// 设置小图标
    // TODO: 在此添加额外的初始化代码
    TEXTMETRIC tm;
    CRect rect;
    GetDlgItem(IDC_EDIT2)->GetClientRect(&rect);
    CDC *pdc = GetDlgItem(IDC_EDIT2)->GetDC();
    ::GetTextMetrics(pdc->m_hDC, &tm);
    GetDlgItem(IDC_EDIT2)->ReleaseDC(pdc);
    m_nLineCount = rect.bottom / (tm.tmHeight - 1.5);
    return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CX86DisasmEngineDlg::OnSysCommand(UINT nID, LPARAM lParam) {
    if ((nID & 0xFFF0) == IDM_ABOUTBOX) {
        CAboutDlg dlgAbout;
        dlgAbout.DoModal();
    } else {
        CDialogEx::OnSysCommand(nID, lParam);
    }
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CX86DisasmEngineDlg::OnPaint() {
    if (IsIconic()) {
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
    } else {
        CDialogEx::OnPaint();
    }
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CX86DisasmEngineDlg::OnQueryDragIcon() {
    return static_cast<HCURSOR>(m_hIcon);
}

CString filepath;
int g_nAddrOfEP = 0;

PBYTE GetFileOepBuf(CString pFileName) {
    DWORD dwReadByte = 0;
    PBYTE pFileBuf = NULL;
    BOOL bRet = FALSE;
    STARTUPINFO tagStartupInfo = { 0 };
    PROCESS_INFORMATION tagProcessInfoRmation = { 0 };
    DEBUG_EVENT tagDebugEvent = { 0 };
    pFileBuf = (PBYTE)malloc(sizeof(BYTE) * FILEBUGSIZE);
    tagStartupInfo.cb = sizeof(STARTUPINFO);
    bRet = CreateProcess(pFileName, NULL, NULL, NULL, FALSE,
                         DEBUG_ONLY_THIS_PROCESS, NULL, NULL,
                         &tagStartupInfo, &tagProcessInfoRmation);
    if (bRet == FALSE) {
        goto EXIT_FUN;
    }
    bRet = WaitForDebugEvent(&tagDebugEvent, INFINITE);
    if (bRet == 0) {
        goto EXIT_FUN;
    }
    g_nAddrOfEP = (int)tagDebugEvent.u.CreateProcessInfo.lpStartAddress;
    if (ReadProcessMemory(tagProcessInfoRmation.hProcess,
                          (LPVOID)g_nAddrOfEP,
                          pFileBuf, FILEBUGSIZE, (SIZE_T*)&dwReadByte) && dwReadByte == FILEBUGSIZE) {
        return pFileBuf;
    }
EXIT_FUN:
    if (tagProcessInfoRmation.hThread != NULL) {
        CloseHandle(tagProcessInfoRmation.hThread);
    }
    if (tagProcessInfoRmation.hProcess != NULL) {
        CloseHandle(tagProcessInfoRmation.hProcess);
    }
    return NULL;
}

bool fileExist(CString fp) {
    CFile file;
    CFileException ex;
    if (file.Open(fp, CFile::modeRead | CFile::shareDenyWrite | CFile::typeBinary, &ex) == 0) return false;
    return true;
}

wchar_t *ctow(char *sText) {
    DWORD dwNum = MultiByteToWideChar(CP_ACP, 0, sText, -1, NULL, 0);//把第五个参数设成NULL的到宽字符串的长度包括结尾符
    wchar_t *pwText = NULL;
    pwText = new wchar_t[dwNum];
    if (!pwText) {
        delete[]pwText;
        pwText = NULL;
    }
    unsigned nLen = MultiByteToWideChar(CP_ACP, 0, sText, -1, pwText, dwNum + 10);
    if (nLen >= 0) {
        pwText[nLen] = 0;
    }
    return pwText;
}


CString doDisassemble(bool UiShow) {
    PBYTE pFileBuf = NULL;
    INSTRUCTION_INFORMATION tagInstInfo = { 0 };
    DIS_CPU tagDispCpu = { 0 };
    char szAsmString[0x80] = { 0 };
    BYTE szOpCode[0x10] = { 0 };
    int nCodeLen = 0;
    CString result = L"";
    //获取文件入口Buff
    if (!fileExist(filepath)) AfxMessageBox(L"File does not exist!");
    else {
        pFileBuf = GetFileOepBuf(filepath);
        if (pFileBuf == NULL) return L"";
        while (TRUE) {
            memset(szAsmString, 0, sizeof(szAsmString));
            memcpy(szOpCode, pFileBuf, sizeof(szOpCode));
            tagInstInfo.pAsmString = szAsmString;
            if (OnDisassembly(&tagInstInfo, nCodeLen, szOpCode,
                              g_nAddrOfEP, MODE_32BIT, &tagDispCpu) == TRUE) {
                char f[2048];
                sprintf(f, "%08X  %-30s   Length:%d ", g_nAddrOfEP, szAsmString, nCodeLen);
                result += CString(ctow(f));
                if(UiShow) result += L"\r\n";
                else result += L"\n";
            } else break;
            pFileBuf = pFileBuf + nCodeLen;
            g_nAddrOfEP += nCodeLen;
            nCodeLen = 0;
        }
    }
    return result;
}

void CX86DisasmEngineDlg::ZGDisassemble() {
    int nButtonPressed = 0;
    TaskDialog(AfxGetMainWnd()->m_hWnd, AfxGetInstanceHandle(),
               L"What's your choice?",
               L"Disassemble to text box or to file?",
               L"Click yes to disassemble to text box, or no to disassemble to file",
               TDCBF_OK_BUTTON | TDCBF_CANCEL_BUTTON,
               TD_INFORMATION_ICON,
               &nButtonPressed);
    if (IDOK == nButtonPressed) {
        CString ed = doDisassemble(true);
        if (ed == L"") {
            TaskDialog(AfxGetMainWnd()->m_hWnd, AfxGetInstanceHandle(),
                       L"Error",
                       L"Cannot disassemble this file due to internal error!",
                       NULL,
                       TDCBF_OK_BUTTON,
                       TD_ERROR_ICON,
                       NULL);
            return;
        }
        SetDlgItemText(IDC_EDIT2, ed);
        int nLine = ((CEdit *)GetDlgItem(IDC_EDIT2))->GetLineCount();
        if (nLine > m_nLineCount) GetDlgItem(IDC_EDIT2)->ShowScrollBar(SB_VERT, TRUE);
        else GetDlgItem(IDC_EDIT2)->ShowScrollBar(SB_VERT, FALSE);
    } else if (IDCANCEL == nButtonPressed) {
        BOOL isOpen = FALSE;//是否打开(TRUE为打开对话框，FALSE为保存对话框) ? ?
        CString filter = L"文本文件(*.txt)|*.txt|所有类型(*.*)|*.*||";//文件过虑的类型 ? ?
        CFileDialog openFileDlg(isOpen, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, filter, NULL);
        INT_PTR result = openFileDlg.DoModal();//是否选中文件，或取消了选择
        if (result == IDOK) {
            CString sfp = openFileDlg.GetPathName();
            switch (openFileDlg.m_ofn.nFilterIndex) {
            case 1:
                sfp += ".txt";
                break;
            default:
                break;
            }
            CFile file;
            file.Open(sfp, CFile::modeCreate | CFile::modeWrite | CFile::modeNoTruncate, NULL);
            CString res = doDisassemble(false);
            if (res == L"") {
                TaskDialog(AfxGetMainWnd()->m_hWnd, AfxGetInstanceHandle(),
                           L"Error",
                           L"Cannot disassemble this file due to internal error!",
                           NULL,
                           TDCBF_OK_BUTTON,
                           TD_ERROR_ICON,
                           NULL);
                return;
            }
            file.Write(res, res.GetLength());
            file.Close();
        }
    }
}

void CX86DisasmEngineDlg::OnBnClickedChfile() {
    // TODO: 在此添加控件通知处理程序代码
    BOOL isOpen = TRUE;//是否打开(TRUE为打开对话框，FALSE为保存对话框) ? ?
    CString filter = L"可执行文件(*.exe)|*.exe|所有类型(*.*)|*.*||";//文件过虑的类型 ? ?
    CFileDialog openFileDlg(isOpen, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, filter, NULL);
    INT_PTR result = openFileDlg.DoModal();//是否选中文件，或取消了选择
    if (result == IDOK) {
        filepath = openFileDlg.GetPathName();
        CEdit *pBoxOne = (CEdit *)GetDlgItem(IDC_EDIT1);
        pBoxOne->SetWindowText(filepath);
        ZGDisassemble();
    }
}

void CX86DisasmEngineDlg::OnDropFiles(HDROP hDropInfo) {
    // TODO: 在此添加消息处理程序代码和/或调用默认值
    int DropCount = DragQueryFile(hDropInfo, -1, NULL, 0);//取得被拖动文件的数目
    if (DropCount > 1) AfxMessageBox(L"You can only place one file in one time!");
    else {
        WCHAR wcStr[MAX_PATH];
        DragQueryFile(hDropInfo, 0, wcStr, MAX_PATH);//获得拖曳的第i个文件的文件名
        filepath = CString(wcStr);
        CEdit *pBoxOne = (CEdit *)GetDlgItem(IDC_EDIT1);
        pBoxOne->SetWindowText(filepath);
    }
    DragFinish(hDropInfo);
    ZGDisassemble();
    CDialogEx::OnDropFiles(hDropInfo);
}
