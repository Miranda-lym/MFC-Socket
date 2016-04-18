
// MFCQQClientDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "MFCQQClient.h"
#include "MFCQQClientDlg.h"
#include "afxdialogex.h"
#include "LoginDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
    CAboutDlg();

    // �Ի�������
#ifdef AFX_DESIGN_TIME
    enum {
        IDD = IDD_ABOUTBOX
    };
#endif

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
    DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CMFCQQClientDlg �Ի���



CMFCQQClientDlg::CMFCQQClientDlg(CWnd* pParent /*=NULL*/)
    : CDialogEx(IDD_MFCQQCLIENT_DIALOG, pParent)
    , m_send(_T(""))
    , login(this)
{
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
    pSock = NULL;
    m_connected = false;
}

CMFCQQClientDlg::~CMFCQQClientDlg()
{
    CString dataToSend = msg.join("", TYPE[Logout], userName, "", "", pwd);
    pSock->Send(dataToSend, dataToSend.GetLength() + 1);//��Ϣ���ݣ���Ϣ����
    pSock->Close();
    delete pSock;
}

void CMFCQQClientDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_Send, m_send);
    DDX_Control(pDX, IDC_msgTo, m_cbMsgTo);
}

void CMFCQQClientDlg::receData()
{
    char buffer[10000];
    if (pSock->Receive(buffer, sizeof(buffer)) != SOCKET_ERROR) {
        msg.load(buffer);
        if (msg.type == TYPE[UserList]) { //��ʾ��¼�ɹ�
            m_connected = true;
            m_cbMsgTo.ResetContent(); //��������б���������
            for (int i; (i = msg.data.Find(";")) != -1;) { //���Ҳ������ˣ����û��б����
                CString user = msg.data.Left(i); //ȡ���i���ַ���ɵ��Ӵ�
                if (user != login.userName) {
                    m_cbMsgTo.AddString(user); //���û�����ӵ������б�
                }
                msg.data = MyMsg::rightN(msg.data, i + 1);
            }
            m_cbMsgTo.SetCurSel(0);
            updateEvent("�����Ϸ�����", msg.data);
        }
        else if (msg.type == TYPE[LoginFail]) {
            login.loginFail = true;
        }
        else if (msg.type == TYPE[ChatMsg]) {
            updateEvent(msg.fromUser, msg.data);
        }
        else if (msg.type == TYPE[AddUserList]) {
            m_cbMsgTo.AddString(msg.data); //����һ���û���Ϣ
        }
        else if (msg.type == TYPE[Sequze]) {
            pSock->Close();
            delete pSock;
            pSock = NULL;
            MessageBox("������������һ����¼�����ڽ���ǿ������", "��ܰ��ʾ");
            showLoginDlg();
        }
        else {
            updateEvent("δ֪��Ϣ", buffer);
        }
    }
}

void CMFCQQClientDlg::updateEvent(const CString & title, const CString & content)
{
    CString str;
    if (content == "") {
        str = title + "\r\n";
    }
    else {
        str = title + ": " + content + "\r\n";
    }
    //UpdateData(false); //��ΪҪ�������������һ�У�UpdateData��ʵ�ֲ��˵�
    //���������ľ�ʵ�֣���������ע�ͣ���ôÿ����Ϣ���������
    CEdit* pEvent = (CEdit*)GetDlgItem(IDC_Rece); //��ȡ��������һ���ؼ����ؼ�ID�������Լ�д
    int lastLine = pEvent->LineIndex(pEvent->GetLineCount() - 1); //��ȡ�༭�����һ���������ú����Ĳ�����������Ч������ֵ���±꣩������5������Ч���±���0~4
    pEvent->SetSel(lastLine + 1, lastLine + 2, 0); //ѡ��༭�����һ��
    pEvent->ReplaceSel(str); //�滻��ѡ��һ�е����ݣ�������һ����û�����ݵ�
}

BEGIN_MESSAGE_MAP(CMFCQQClientDlg, CDialogEx)
    ON_WM_SYSCOMMAND()
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    ON_BN_CLICKED(ID_ConnectServer, &CMFCQQClientDlg::OnBnClickedLogout)
    ON_BN_CLICKED(IDC_SendMessage, &CMFCQQClientDlg::OnBnClickedSendMessage)
    ON_WM_TIMER()
END_MESSAGE_MAP()


// CMFCQQClientDlg ��Ϣ�������

BOOL CMFCQQClientDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    // ��������...���˵�����ӵ�ϵͳ�˵��С�

    // IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
    ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
    ASSERT(IDM_ABOUTBOX < 0xF000);

    CMenu* pSysMenu = GetSystemMenu(FALSE);
    if (pSysMenu != NULL) {
        BOOL bNameValid;
        CString strAboutMenu;
        bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
        ASSERT(bNameValid);
        if (!strAboutMenu.IsEmpty()) {
            pSysMenu->AppendMenu(MF_SEPARATOR);
            pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
        }
    }

    // ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
    //  ִ�д˲���
    SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
    SetIcon(m_hIcon, FALSE);		// ����Сͼ��

    // TODO: �ڴ���Ӷ���ĳ�ʼ������
    showLoginDlg();
    GetDlgItem(IDC_Send)->SetFocus(); //���ý����ڷ��ͱ༭�򣬼�IDΪIDC_Send
    return FALSE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CMFCQQClientDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
    if ((nID & 0xFFF0) == IDM_ABOUTBOX) {
        CAboutDlg dlgAbout;
        dlgAbout.DoModal();
    }
    else {
        CDialogEx::OnSysCommand(nID, lParam);
    }
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CMFCQQClientDlg::OnPaint()
{
    if (IsIconic()) {
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
    else {
        CDialogEx::OnPaint();
    }
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CMFCQQClientDlg::OnQueryDragIcon()
{
    return static_cast<HCURSOR>(m_hIcon);
}


void CMFCQQClientDlg::showLoginDlg()
{
    SetDlgItemText(IDC_Rece, ""); //δ�󶨱���������ͨ���ؼ���ID���������
    m_send = ""; //�Ѱ󶨱�����ͨ��������ı�����������գ�����UpdateData()
    UpdateData(FALSE);
    ShowWindow(SW_HIDE);
    if (login.DoModal() != IDOK) {
        exit(1);
    }
    ShowWindow(SW_SHOW);
    userName = login.userName;
    pwd = login.pwd;
    SetWindowText("�ͻ��� - " + userName);
}


void CMFCQQClientDlg::OnBnClickedLogout()
{
    CString dataToSend = msg.join("", TYPE[Logout], userName, "", "", pwd);
    pSock->Send(dataToSend, dataToSend.GetLength() + 1);//��Ϣ���ݣ���Ϣ����
    pSock->Close();
    delete pSock;
    pSock = NULL;
    showLoginDlg();
}


void CMFCQQClientDlg::OnBnClickedSendMessage()
{
    if (m_connected) {
        UpdateData(true);
        if (m_send == "") {
            MessageBox("������Ҫ���͵���Ϣ", "��ܰ��ʾ");
            return;
        }
        CString toUser;
        m_cbMsgTo.GetLBText(m_cbMsgTo.GetCurSel(), toUser);
        CString dataToSend = msg.join(m_send, TYPE[ChatMsg], userName, "", toUser);
        if (pSock->Send(dataToSend, dataToSend.GetLength() + 1) == SOCKET_ERROR) {
            MessageBox("����ʧ��", "��ܰ��ʾ");
        }
        else {
            CString buffer = m_send;
            m_send = "";
            UpdateData(false);
            updateEvent("��", buffer);
        }
    }
    else {
        MessageBox("���������Ϸ�����", "��ܰ��ʾ");
    }
}


void CMFCQQClientDlg::OnOK()
{
    OnBnClickedSendMessage();
}


void CMFCQQClientDlg::OnTimer(UINT_PTR nIDEvent)
{
    // TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
    switch (nIDEvent) {
        case 0: //��¼��ʱ�ļ��
            KillTimer(0);
            MessageBox("��¼��ʱ", "��ܰ��ʾ");
            showLoginDlg();
            break;
    }
    CDialogEx::OnTimer(nIDEvent);
}
