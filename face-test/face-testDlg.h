
// face-testDlg.h : ͷ�ļ�
//

#pragma once


// CfacetestDlg �Ի���
class CfacetestDlg : public CDialogEx
{
// ����
public:
	CfacetestDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_FACETEST_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��
	virtual BOOL OnInitDialog ();

// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonDetection();
	afx_msg void OnBnClickedButtonAlign();
	afx_msg void OnBnClickedButtonIdentify();

private:
	CString m_sIdentifyNo;
	CString m_sResult;
	CString m_sPhotoPath1;
	CString m_sPhotoPath2;
};
