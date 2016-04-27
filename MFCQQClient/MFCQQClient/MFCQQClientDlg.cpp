
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
    , login(this)
{
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
    pSock = NULL;
    m_connected = false;
}

CMFCQQClientDlg::~CMFCQQClientDlg()
{
    CString dataToSend = msg.join("", TYPE[Logout], userName, "", "", pwd);
    sendMsg(dataToSend, pSock);
    pSock->Close();
    delete pSock;
}

void CMFCQQClientDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
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
            modifyStatus("�����Ϸ�����");
            SetTimer(1, 3000, NULL); // ����������
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
        else if (msg.type == TYPE[Squezze]) {
            KillTimer(1);
            pSock->Close();
            delete pSock;
            pSock = NULL;
            MessageBox("������������һ����¼�����ڽ���ǿ������", "��ܰ��ʾ");
            showLoginDlg();
        }
        else if (msg.type == TYPE[OnlineState]) {
            if (msg.data == "1") {
                modifyStatus("���û����ߣ�����ֱ�ӷ�����Ϣ��");
            }
            else {
                modifyStatus("���û���ǰ�����ߣ����ɸ�������������Ϣ��");
            }
        }
        else if (msg.type == TYPE[Server_is_closed]) {
            KillTimer(1);
            modifyStatus("��������ֹͣ���У����Ժ�������������������");
            MessageBox("��������ֹͣ���У����Ժ�������������������");
            showLoginDlg();
        }
        else if (msg.type == TYPE[OfflineMsg]) {
            while (msg.data != "") {
                CString _from, _time, _content;
                int i = msg.data.Find(seperator);
                _from = msg.data.Left(i);
                msg.data = MyMsg::rightN(msg.data, i + 2);
                i = msg.data.Find(seperator);
                _time = msg.data.Left(i);
                msg.data = MyMsg::rightN(msg.data, i + 2);
                i = msg.data.Find(seperator);
                _content = msg.data.Left(i);
                msg.data = MyMsg::rightN(msg.data, i + 2);
                updateEvent(_from, _content, _time);
            }
            updateEvent("����������������������Ϣ����������", "", "");
        }
        else {
            updateEvent("δ֪��Ϣ", buffer);
        }
    }
    else {
        MessageBox(ClientSocket::getLastErrorStr(), "������ʾ", MB_ICONERROR);
    }
}

int CMFCQQClientDlg::sendMsg(const CString & data, ClientSocket * sock)
{
    if (sock->Send(data, data.GetLength() + 1) == SOCKET_ERROR) {
        KillTimer(1); //�ͷ��������ӳ����޷�������Ϣʱ�ر��������ķ���
        MessageBox("������Ϣʧ�ܣ�" + ClientSocket::getLastErrorStr(), "������ʾ", MB_ICONERROR);
        return SOCKET_ERROR;
    }
    return 0;
}

void CMFCQQClientDlg::updateEvent(const CString & title, const CString & content, const CString setTime)
{
    CString curData;
    GetDlgItemText(IDC_Rece, curData); //���ռ�IDC_Rece�е����ݷŵ�����curData��
    bool haveDate = (curData == ""); //����ʱ������Ϊ�գ����������
    CString str = getDateTime(haveDate) + " ";
    if (setTime != "-") {
        str = setTime + " ";
    }
    if (content == "") {
        str += title + "\r\n";
    }
    else {
        str += title + ": " + content + "\r\n";
    }
    //UpdateData(false); //��ΪҪ�������������һ�У�UpdateData��ʵ�ֲ��˵�
    //���������ľ�ʵ�֣���������ע�ͣ���ôÿ����Ϣ���������
    CEdit* pEvent = (CEdit*)GetDlgItem(IDC_Rece); //��ȡ��������һ���ؼ����ؼ�ID�������Լ�д
    int lastLine = pEvent->LineIndex(pEvent->GetLineCount() - 1); //��ȡ�༭�����һ���������ú����Ĳ�����������Ч������ֵ���±꣩������5������Ч���±���0~4
    pEvent->SetSel(lastLine + 1, lastLine + 2, 0); //ѡ��༭�����һ��
    pEvent->ReplaceSel(str); //�滻��ѡ��һ�е����ݣ�������һ����û�����ݵ�
}

void CMFCQQClientDlg::modifyStatus(const CString & status, bool _sleep)
{
    HWND h = CreateStatusWindow(WS_CHILD | WS_VISIBLE, status, m_hWnd, 0);
    if (_sleep) {
        Sleep(50);
    }
    ::SendMessage(h, SB_SETBKCOLOR, 0, RGB(0, 125, 205)); //ȫ�ֺ���,�򲻻����dlg��ĳ�Ա����������ȫ�ֵĳ�Ա����
}

BEGIN_MESSAGE_MAP(CMFCQQClientDlg, CDialogEx)
    ON_WM_SYSCOMMAND()
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    ON_BN_CLICKED(ID_ConnectServer, &CMFCQQClientDlg::OnBnClickedLogout)
    ON_BN_CLICKED(IDC_SendMessage, &CMFCQQClientDlg::OnBnClickedSendMessage)
    ON_WM_TIMER()
    ON_CBN_SELCHANGE(IDC_msgTo, &CMFCQQClientDlg::OnSelChangeMsgTo)
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
    SetDlgItemText(IDC_Send, ""); //δ�󶨱���������ͨ���ؼ���ID���������
    ShowWindow(SW_HIDE);
    if (login.DoModal() != IDOK) {
        exit(1);
    }
    ShowWindow(SW_SHOW);
    userName = login.userName;
    pwd = login.pwd;
    SetWindowText("�ͻ��� - " + userName);
}

CString CMFCQQClientDlg::getDateTime(bool haveDate)
{
    if (haveDate) {
        return CTime::GetCurrentTime().Format("%Y-%m-%d %H:%M:%S");
    }
    else {
        return CTime::GetCurrentTime().Format("%H:%M:%S");
    }
}


void CMFCQQClientDlg::OnBnClickedLogout()
{
    KillTimer(1); //����ע�������Դ�ʱ�ر��������ķ��ͣ����򽫻ᵼ��3���ڱ���
    CString dataToSend = msg.join("", TYPE[Logout], userName, "", "", pwd);
    sendMsg(dataToSend, pSock);
    pSock->Close();
    delete pSock;
    pSock = NULL;
    showLoginDlg();
}


void CMFCQQClientDlg::OnBnClickedSendMessage()
{
    if (m_connected) {
        CString send;
        GetDlgItemText(IDC_Send, send);
        if (send == "") {
            MessageBox("������Ҫ���͵���Ϣ", "��ܰ��ʾ");
            return;
        }
        CString toUser;
        m_cbMsgTo.GetLBText(m_cbMsgTo.GetCurSel(), toUser);
        CString dataToSend = msg.join(send, TYPE[ChatMsg], userName, "", toUser);
        if (sendMsg(dataToSend, pSock) != SOCKET_ERROR) {
            updateEvent("��", send);
            SetDlgItemText(IDC_Send, "");
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
        case 1:
            CString dataToSend = msg.join("", TYPE[I_am_online], userName);
            sendMsg(dataToSend, pSock);
            break;
    }
    CDialogEx::OnTimer(nIDEvent);
}


void CMFCQQClientDlg::OnSelChangeMsgTo()
{
    CString who; //ѡ��userList�е��ĸ��û�
    m_cbMsgTo.GetLBText(m_cbMsgTo.GetCurSel(), who);
    //��֯��Ϣ�����͸�����������ѯ���û��Ƿ�������
    CString dataToSend = msg.join(who, TYPE[OnlineState], userName);
    sendMsg(dataToSend, pSock);
}


BOOL CMFCQQClientDlg::PreTranslateMessage(MSG* pMsg)
{
    int cont_ID = GetWindowLong(pMsg->hwnd, GWL_ID); //��ȡ��Ӧ�ؼ���Ϣ��ID
    if (cont_ID == IDC_Send) { //�ж��Ƿ���Ҫ����Ŀؼ���ID
        if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN) { //�����¼�����Ϊenter��
            if (GetKeyState(VK_CONTROL) & 0x80) { //���Ѿ�����enter��ǰ�����ְ�����ctrl��������0x80�Ƕ����а����ļ�⣩
                //��ӻ���
                CString str;
                GetDlgItemText(IDC_Send, str);
                SetDlgItemText(IDC_Send, str + "\r\n");
                //���ù��λ�ã�������������һ��λ��
                CEdit* pEdit = (CEdit*)GetDlgItem(IDC_Send);
                int len = str.GetLength() + 2;
                pEdit->SetSel(len - 1, len); //SetSel()����ѡ�У��������ֱ����ѡ�е���ʼλ�ú�����λ��
            }
            else {
                OnBnClickedSendMessage();
            }
        }
    }
    return CDialogEx::PreTranslateMessage(pMsg); //Ĭ�ϴ���ʽ
}
