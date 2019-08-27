
// face-testDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "face-test.h"
#include "face-testDlg.h"
#include "afxdialogex.h"

#include <cstdint>
#include <fstream>
#include <iostream>
#include <string>

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <opencv2/imgcodecs.hpp>

#include "face_identification.h"
#include "recognizer.h"
#include "face_detection.h"
#include "face_alignment.h"

using namespace std;
using namespace seeta;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


wstring string2wstring(string str)  
{  
    wstring result;  
    //获取缓冲区大小，并申请空间，缓冲区大小按字符计算  
    int len = MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.size(), NULL, 0);  
    TCHAR* buffer = new TCHAR[len + 1];  
    //多字节编码转换成宽字节编码  
    MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.size(), buffer, len);  
    buffer[len] = '\0';             //添加字符串结尾  
    //删除缓冲区并返回值  
    result.append(buffer);  
    delete[] buffer;  
    return result;  
}  
  
//将wstring转换成string  
string wstring2string(wstring wstr)  
{  
    string result;  
    //获取缓冲区大小，并申请空间，缓冲区大小事按字节计算的  
    int len = WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), wstr.size(), NULL, 0, NULL, NULL);  
    char* buffer = new char[len + 1];  
    //宽字节编码转换成多字节编码  
    WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), wstr.size(), buffer, len, NULL, NULL);  
    buffer[len] = '\0';  
    //删除缓冲区并返回值  
    result.append(buffer);  
    delete[] buffer;  
    return result;  
}

string CStringToString(CString cstr)

{
	string result(cstr.GetLength(),'e');
	for (int i=0;i<cstr.GetLength();i++)
	{
		result[i]=(char)cstr[i];

	}
	return result;

}
std::string GetModuleDir()
{
    char szPath[MAX_PATH];
    ::GetModuleFileNameA(NULL, szPath, MAX_PATH);

    std::string aRet = szPath;
    size_t pos = aRet.find_last_of("\\");
    aRet = aRet.substr(0, pos + 1);

    return aRet;
}

CfacetestDlg::CfacetestDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CfacetestDlg::IDD, pParent)
	, m_sIdentifyNo(_T(""))
	, m_sResult(_T(""))
	, m_sPhotoPath1(_T("test001.jpg"))
	, m_sPhotoPath2(_T("test002.jpg"))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CfacetestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_IDENTIFY, m_sIdentifyNo);
	DDX_Text(pDX, IDC_EDIT_RESULT, m_sResult);
	DDX_Text(pDX, IDC_EDIT_PH001, m_sPhotoPath1);
	DDX_Text(pDX, IDC_EDIT_PH002, m_sPhotoPath2);
}

 BOOL CfacetestDlg::OnInitDialog ()
 {
	 BOOL ret = CDialogEx::OnInitDialog();
	 return ret;
 }

BEGIN_MESSAGE_MAP(CfacetestDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_DETECTION, &CfacetestDlg::OnBnClickedButtonDetection)
	ON_BN_CLICKED(IDC_BUTTON_ALIGN, &CfacetestDlg::OnBnClickedButtonAlign)
	ON_BN_CLICKED(IDC_BUTTON_IDENTIFY, &CfacetestDlg::OnBnClickedButtonIdentify)
END_MESSAGE_MAP()


// CfacetestDlg 消息处理程序
// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CfacetestDlg::OnPaint()
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
HCURSOR CfacetestDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CfacetestDlg::OnBnClickedButtonDetection()
{
	std::string moduleDir  = GetModuleDir();
	std::string img_path = moduleDir + "data\\detection\\yang.png";
	std::string model_path = moduleDir + "model\\seeta_fd_frontal_v1.0.bin";
	seeta::FaceDetection detector(model_path.c_str());

	detector.SetMinFaceSize(40);
	detector.SetScoreThresh(2.f);
	detector.SetImagePyramidScaleFactor(0.8f);
	detector.SetWindowStep(4, 4);

	cv::Mat img = cv::imread(img_path, cv::IMREAD_UNCHANGED);
	cv::Mat img_gray;

	if (img.channels() != 1)
		cv::cvtColor(img, img_gray, cv::COLOR_BGR2GRAY);
	else
		img_gray = img;

	seeta::ImageData img_data;
	img_data.data = img_gray.data;
	img_data.width = img_gray.cols;
	img_data.height = img_gray.rows;
	img_data.num_channels = 1;

	long t0 = cv::getTickCount();
	std::vector<seeta::FaceInfo> faces = detector.Detect(img_data);
	long t1 = cv::getTickCount();
	double secs = (t1 - t0)/cv::getTickFrequency();

	cout << "Detections takes " << secs << " seconds " << endl;
#ifdef USE_OPENMP
	cout << "OpenMP is used." << endl;
#else
	cout << "OpenMP is not used. " << endl;
#endif

#ifdef USE_SSE
	cout << "SSE is used." << endl;
#else
	cout << "SSE is not used." << endl;
#endif

	cout << "Image size (wxh): " << img_data.width << "x" 
		<< img_data.height << endl;

	cv::Rect face_rect;
	int32_t num_face = static_cast<int32_t>(faces.size());

	for (int32_t i = 0; i < num_face; i++) {
		face_rect.x = faces[i].bbox.x;
		face_rect.y = faces[i].bbox.y;
		face_rect.width = faces[i].bbox.width;
		face_rect.height = faces[i].bbox.height;

		cv::rectangle(img, face_rect, CV_RGB(0, 0, 255), 4, 8, 0);
	}

	cv::namedWindow("Test", cv::WINDOW_AUTOSIZE);
	cv::imshow("Test", img);
	cv::waitKey(0);
	cv::destroyAllWindows();
}


void CfacetestDlg::OnBnClickedButtonAlign()
{
	

	std::string moduleDir  = GetModuleDir();
	std::string dataDir = moduleDir + "data\\alignment\\";

	std::string model_path1 = moduleDir + "model\\seeta_fd_frontal_v1.0.bin";
	seeta::FaceDetection detector(model_path1.c_str());
	detector.SetMinFaceSize(40);
	detector.SetScoreThresh(2.f);
	detector.SetImagePyramidScaleFactor(0.8f);
	detector.SetWindowStep(4, 4);

	// Initialize face alignment model 
	std::string model_path2 = moduleDir + "model\\seeta_fa_v1.1.bin";
	seeta::FaceAlignment point_detector(model_path2.c_str());

	//load image
	IplImage *img_grayscale = NULL;
	img_grayscale = cvLoadImage((dataDir + "image_0001.png").c_str(), 0);
	if (img_grayscale == NULL)
	{
		return;
	}

	IplImage *img_color = cvLoadImage((dataDir + "image_0001.png").c_str(), 1);
	int pts_num = 5;
	int im_width = img_grayscale->width;
	int im_height = img_grayscale->height;
	unsigned char* data = new unsigned char[im_width * im_height];
	unsigned char* data_ptr = data;
	unsigned char* image_data_ptr = (unsigned char*)img_grayscale->imageData;
	int h = 0;
	for (h = 0; h < im_height; h++) {
		memcpy(data_ptr, image_data_ptr, im_width);
		data_ptr += im_width;
		image_data_ptr += img_grayscale->widthStep;
	}

	seeta::ImageData image_data;
	image_data.data = data;
	image_data.width = im_width;
	image_data.height = im_height;
	image_data.num_channels = 1;

	// Detect faces
	std::vector<seeta::FaceInfo> faces = detector.Detect(image_data);
	int32_t face_num = static_cast<int32_t>(faces.size());

	if (face_num == 0)
	{
		delete[]data;
		cvReleaseImage(&img_grayscale);
		cvReleaseImage(&img_color);
		return ;
	}

	// Detect 5 facial landmarks
	seeta::FacialLandmark points[5];
	point_detector.PointDetectLandmarks(image_data, faces[0], points);

	// Visualize the results
	cvRectangle(img_color, cvPoint(faces[0].bbox.x, faces[0].bbox.y), cvPoint(faces[0].bbox.x + faces[0].bbox.width - 1, faces[0].bbox.y + faces[0].bbox.height - 1), CV_RGB(255, 0, 0));
	for (int i = 0; i<pts_num; i++)
	{
		cvCircle(img_color, cvPoint(points[i].x, points[i].y), 2, CV_RGB(0, 255, 0), CV_FILLED);
	}

	std::string saveImgPath = moduleDir + "result.jpg";
	cvSaveImage(saveImgPath.c_str(), img_color);

	cv::Mat img = cv::imread(saveImgPath, cv::IMREAD_UNCHANGED);
	cv::namedWindow("Test", cv::WINDOW_AUTOSIZE);
	cv::imshow("Test", img);
	cv::waitKey(0);
	cv::destroyAllWindows();

	// Release memory
	cvReleaseImage(&img_color);
	cvReleaseImage(&img_grayscale);
	delete[]data;
}


void CfacetestDlg::OnBnClickedButtonIdentify()
{
	UpdateData();

	std::string moduleDir  = GetModuleDir();

	std::string DATA_DIR = moduleDir + "data\\";
    std::string MODEL_DIR = moduleDir + "model\\";

	std::string model_path1 = moduleDir + "model\\seeta_fd_frontal_v1.0.bin";
	seeta::FaceDetection detector(model_path1.c_str());
	detector.SetMinFaceSize(40);
	detector.SetScoreThresh(2.f);
	detector.SetImagePyramidScaleFactor(0.8f);
	detector.SetWindowStep(4, 4);

	// Initialize face alignment model 
	std::string model_path2 = moduleDir + "model\\seeta_fa_v1.1.bin";
	seeta::FaceAlignment point_detector(model_path2.c_str());

	// Initialize face Identification model  FaceIdentification\model
	FaceIdentification face_recognizer((MODEL_DIR + "seeta_fr_v1.0.bin").c_str());
	std::string test_dir = DATA_DIR + "identify\\";

	//load image
	std::string photopath1 = CStringToString(m_sPhotoPath1);
	cv::Mat gallery_img_color = cv::imread(test_dir + photopath1, 1);
	cv::Mat gallery_img_gray;
	cv::cvtColor(gallery_img_color, gallery_img_gray, CV_BGR2GRAY);

	std::string photopath2 = CStringToString(m_sPhotoPath2);
	cv::Mat probe_img_color = cv::imread(test_dir + photopath2, 1);
	cv::Mat probe_img_gray;
	cv::cvtColor(probe_img_color, probe_img_gray, CV_BGR2GRAY);

	ImageData gallery_img_data_color(gallery_img_color.cols, gallery_img_color.rows, gallery_img_color.channels());
	gallery_img_data_color.data = gallery_img_color.data;

	ImageData gallery_img_data_gray(gallery_img_gray.cols, gallery_img_gray.rows, gallery_img_gray.channels());
	gallery_img_data_gray.data = gallery_img_gray.data;

	ImageData probe_img_data_color(probe_img_color.cols, probe_img_color.rows, probe_img_color.channels());
	probe_img_data_color.data = probe_img_color.data;

	ImageData probe_img_data_gray(probe_img_gray.cols, probe_img_gray.rows, probe_img_gray.channels());
	probe_img_data_gray.data = probe_img_gray.data;

	// Detect faces
	std::vector<seeta::FaceInfo> gallery_faces = detector.Detect(gallery_img_data_gray);
	int32_t gallery_face_num = static_cast<int32_t>(gallery_faces.size());

	std::vector<seeta::FaceInfo> probe_faces = detector.Detect(probe_img_data_gray);
	int32_t probe_face_num = static_cast<int32_t>(probe_faces.size());

	if (gallery_face_num == 0 || probe_face_num==0)
	{
		std::cout << "Faces are not detected.";
		return ;
	}

	// Detect 5 facial landmarks
	seeta::FacialLandmark gallery_points[5];
	point_detector.PointDetectLandmarks(gallery_img_data_gray, gallery_faces[0], gallery_points);

	seeta::FacialLandmark probe_points[5];
	point_detector.PointDetectLandmarks(probe_img_data_gray, probe_faces[0], probe_points);

	for (int i = 0; i<5; i++)
	{
		cv::circle(gallery_img_color, cv::Point(gallery_points[i].x, gallery_points[i].y), 2,
			CV_RGB(0, 255, 0));
		cv::circle(probe_img_color, cv::Point(probe_points[i].x, probe_points[i].y), 2,
			CV_RGB(0, 255, 0));
	}
	cv::imwrite("gallery_point_result.jpg", gallery_img_color);
	cv::imwrite("probe_point_result.jpg", probe_img_color);

	// Extract face identity feature
	float gallery_fea[2048];
	float probe_fea[2048];
	face_recognizer.ExtractFeatureWithCrop(gallery_img_data_color, gallery_points, gallery_fea);
	face_recognizer.ExtractFeatureWithCrop(probe_img_data_color, probe_points, probe_fea);

	// Caculate similarity of two faces
	float sim = face_recognizer.CalcSimilarity(gallery_fea, probe_fea);

	m_sIdentifyNo.Format(L"%f", sim);
	if(sim > 0.7)
	{
		m_sResult = L"我感觉是同一个人呢";
	}
	else
	{
		m_sResult = L"我感觉是不同人呢";
	}
	UpdateData(FALSE);
}
