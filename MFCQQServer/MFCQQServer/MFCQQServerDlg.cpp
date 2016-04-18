
// MFCQQServerDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "MFCQQServer.h"
#include "MFCQQServerDlg.h"
#include "afxdialogex.h"
#include "tlhelp32.h"

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


// CMFCQQServerDlg �Ի���
bool findProcessByName(const CString &name) {
    PROCESSENTRY32 pe32 = { sizeof(pe32) };
    HANDLE hp = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    for (BOOL find = Process32First(hp, &pe32); find != 0; find = Process32Next(hp, &pe32)) {
        if (name == pe32.szExeFile)
            return true;
    }
    return false;//���������˻�û�з��أ�������δ�ҵ��ý��̣����Է���false
}


CMFCQQServerDlg::CMFCQQServerDlg(CWnd* pParent /*=NULL*/)
    : CDialogEx(IDD_MFCQQSERVER_DIALOG, pParent)
    , sendData(_T(""))
{
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
    listenSocket = NULL;
    m_port = 22783;
    if (!findProcessByName("mysqld.exe")) {
        ShellExecute(0, "open", "mysqld", 0, 0, SW_HIDE);
    }
    conn = mysql_init(0);
    if (mysql_real_connect(conn, "localhost", "root", "123456", "test", 0, 0, 0) != 0) {
        mysql_query(conn, "set names 'GBK'");
        if (mysql_query(conn, "select userName,passwd from UserInfo") == 0) {
            MYSQL_RES *res = mysql_store_result(conn);
            MYSQL_ROW row;
            userList = "";
            while ((row = mysql_fetch_row(res)) != NULL) {
                userInfoMap[row[0]] = row[1];
                userList += row[0] + (CString)";";
            }
            mysql_free_result(res);
        }
        else
            MessageBox(mysql_error(conn), "Error on MySQL:", MB_ICONERROR);
    }
    else {
        MessageBox(mysql_error(conn), "Error on MySQL:", MB_ICONERROR);
        exit(1);
    }
}

void CMFCQQServerDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_SendData, sendData);
}

void CMFCQQServerDlg::addClient()
{
    ServerSocket* pSocket = new ServerSocket(this);
    listenSocket->Accept(*pSocket);
    pSocket->AsyncSelect(FD_READ | FD_WRITE | FD_CLOSE);
}

void CMFCQQServerDlg::receData(ServerSocket* sock)
{
    char buffer[10000];
    if (sock->Receive(buffer, sizeof(buffer)) != SOCKET_ERROR) {
        msg.load(buffer);
        if (msg.type == TYPE[Login]) {
            if (isUserInfoValid(msg.userId, msg.pw)) {
                if (userSockMap.find(msg.userId) == userSockMap.end()) { //˵����ʱ��δ������û���socket������������������¼
                    CString dataToSend = msg.join(userList, TYPE[UserList], msg.userId);
                    sendMsg(dataToSend, sock);
                    userSockMap[msg.userId] = sock;
                    updateEvent(msg.userId + "������", "");
                }else{
                    CString dataToSend = msg.join(userList, TYPE[UserList], msg.userId);
                    sendMsg(dataToSend, sock);
                    dataToSend = msg.join("", TYPE[Sequze], msg.userId);
                    sendMsg(dataToSend, userSockMap[msg.userId]);
                    userSockMap[msg.userId]->Close();
                    delete userSockMap[msg.userId];
                    userSockMap[msg.userId] = sock;
                    updateEvent(msg.userId + "������һ����¼", "");
                }
            }
            else {
                CString dataToSend = msg.join("", TYPE[LoginFail], msg.userId);
                sendMsg(dataToSend, sock);
            }
        }
        else if (msg.type == TYPE[Logout]) {
            auto it = userSockMap.find(msg.userId);
            if (it != userSockMap.end()) {
                userSockMap[msg.userId]->Close();
                delete userSockMap[msg.userId];
                userSockMap.erase(it);
                updateEvent(msg.userId + "������", "");
            }
        }
        else if (msg.type == TYPE[ChatMsg]) {
            if (msg.toUser == "������") {
                updateEvent(msg.userId, msg.data);
            }
            else {
                auto it = userSockMap.find(msg.toUser);
                if (it != userSockMap.end()) {
                    CString dataToSend = msg.join(msg.data, TYPE[ChatMsg], msg.toUser, msg.userId); //���͵����� ��Ϣ���� ��Ϣ����˭ ��������˭
                    sendMsg(dataToSend, userSockMap[msg.toUser]);
                    updateEvent(msg.userId + "��" + msg.toUser, msg.data);
                }
                else {
                    CString dataToSend = msg.join(msg.toUser + "������,���Ժ�����", TYPE[ChatMsg], msg.userId, "������");
                    sendMsg(dataToSend, userSockMap[msg.userId]);
                    updateEvent(msg.userId + "��" + msg.toUser, msg.data + "������ʧ�ܣ����û��ݲ����ߣ���");
                }
            }
        }
        else if (msg.type == TYPE[Register]) {
            if (userInfoMap.find(msg.userId) == userInfoMap.end()) {
                CString str = "insert into userinfo(userName,passwd) values('" + msg.userId + "','" + msg.pw + "')";
                mysql_query(conn,str);
                userInfoMap[msg.userId] = msg.pw; //��map������һ������û���������Դ���map��
                userList += msg.userId + ";"; //���û��б�������һ���û���
                for (auto &elem:userSockMap) {
                    CString dataToSend = msg.join(msg.userId, TYPE[AddUserList], elem.first); //��ʽ����Ϣ���� ��Ϣ���� ˭���յ���������Է���������˵������˭���յ����������Կͻ��ˣ���˭�ڷ��ͣ��������� ȥ���� ������
                    sendMsg(dataToSend, elem.second);
                }
            }
        }
        else {
            updateEvent("δ֪��Ϣ", buffer);
        }
    }
}

bool CMFCQQServerDlg::isUserInfoValid(const CString & user, const CString & pwd)
{
    //std::map<CString, CString>::iterator i = userInfoMap.find(user);
    auto i = userInfoMap.find(user);
    if (i != userInfoMap.end())
        return userInfoMap[user] == pwd;
    return false;
}

void CMFCQQServerDlg::sendMsg(const CString & data, ServerSocket * sock)
{
    if (sock->Send(data, data.GetLength() + 1) != SOCKET_ERROR) { //�����ͳɹ�
        sendData = "";
        UpdateData(false);
    }
}

void CMFCQQServerDlg::updateEvent(const CString & title, const CString & content)
{
    CString str;
    if (content == "") {
        str = title + "\r\n";
    }
    else {
        str = title + ": " + content + "\r\n";
    }
    CEdit* pEvent = (CEdit*)GetDlgItem(IDC_RECE_DATA); //��ȡ��������һ���ؼ����ؼ�ID�������Լ�д
    int lastLine = pEvent->LineIndex(pEvent->GetLineCount() - 1); //��ȡ�༭�����һ���������ú����Ĳ�����������Ч������ֵ���±꣩������5������Ч���±���0~4
    pEvent->SetSel(lastLine + 1, lastLine + 2, 0); //ѡ��༭�����һ��
    pEvent->ReplaceSel(str); //�滻��ѡ��һ�е����ݣ�������һ����û�����ݵ�
}

BEGIN_MESSAGE_MAP(CMFCQQServerDlg, CDialogEx)
    ON_WM_SYSCOMMAND()
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    ON_BN_CLICKED(ID_OpenServer, &CMFCQQServerDlg::OnBnClickedOpenserver)
    ON_BN_CLICKED(IDC_SendMsg, &CMFCQQServerDlg::OnBnClickedSendMsg)
END_MESSAGE_MAP()


// CMFCQQServerDlg ��Ϣ�������

BOOL CMFCQQServerDlg::OnInitDialog()
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
    OnBnClickedOpenserver();
    return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CMFCQQServerDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CMFCQQServerDlg::OnPaint()
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
HCURSOR CMFCQQServerDlg::OnQueryDragIcon()
{
    return static_cast<HCURSOR>(m_hIcon);
}

void CMFCQQServerDlg::OnBnClickedOpenserver()
{
    // TODO: �ڴ���ӿؼ�֪ͨ����������
    if (listenSocket != NULL) {
        listenSocket->Close();
        delete listenSocket;
        listenSocket = NULL;
    }
    static bool firstOpen = 1;
    listenSocket = new ServerSocket(this);
    if (!listenSocket->Create(m_port, SOCK_STREAM)) {
        if (!firstOpen) {
            MessageBox("�����׽���ʧ��", "��ܰ��ʾ");
        }
        firstOpen = 0;
        return;
    }
    if (listenSocket->Listen(UserNumMax)) {
        GetDlgItem(ID_OpenServer)->EnableWindow(0);
        if (!firstOpen)
            MessageBox("�����ɹ�", "��ܰ��ʾ");
        else
            firstOpen = 0;
    }
    else
        if (!firstOpen)
            MessageBox("����ʧ�ܣ�������", "��ܰ��ʾ");
        else
            firstOpen = 0;
}


void CMFCQQServerDlg::OnBnClickedSendMsg()
{
    // TODO: �ڴ���ӿؼ�֪ͨ����������
    UpdateData(true);
    if (sendData == "") {
        MessageBox("����������Ϣ", "��ܰ��ʾ");
        return;
    }
    for (auto &elem : userSockMap) {
        sendMsg(sendData, elem.second);
    }
    //lastClientSocket->Send(sendData, sendData.GetLength()+1);

}
