#include "UITalk.h"
#include <QPushButton>
#include <QLabel>
#include <QWidget>
#include <QLayout>
#include <QTime>
#include <QScrollBar>
#include "windows.h"

void sleep(int secs)
{
	QTime dieTime = QTime::currentTime().addMSecs(secs);

	while (QTime::currentTime() < dieTime)
		QCoreApplication::processEvents(QEventLoop::AllEvents, 20);
}

UITalk::UITalk(QWidget *parent)
	: QWidget(parent)
	, m_spacer(NULL)
	, m_mainView(NULL)
	, m_Ver(NULL)
	, m_parent(NULL)
	, m_vecAudio(NULL)
	, m_bar(NULL)
{
	ui.setupUi(this);

	m_view = new QScrollArea;
	m_view->setFocusPolicy(Qt::NoFocus);
	m_view->setFrameStyle(QFrame::NoFrame);
	m_view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

	m_mainView = new QWidget();
	m_VerAll = new QVBoxLayout(m_mainView);

	m_Ver = new QVBoxLayout();
	m_VerAll->addLayout(m_Ver);

	m_view->setWidget(m_mainView);
	m_view->setWidgetResizable(true);

	ui.verticalLayout->addWidget(m_view);

	style(m_view);

	m_bar = m_view->verticalScrollBar();
	connect(m_bar, SIGNAL(rangeChanged(int, int)), this, SLOT(rangeChanged(int,int)));
}

UITalk::~UITalk()
{
	if (m_Ver)
	{
		delete m_Ver;
		m_Ver = NULL;
	}

	if (m_VerAll)
	{
		delete m_VerAll;
		m_VerAll = NULL;
	}

	if (m_mainView)
	{
		delete m_mainView;
		m_mainView = NULL;
	}

	if (m_view)
	{
		delete m_view;
		m_view = NULL;
	}
}

// ��������������Ϣ
void UITalk::InsertChat(QPixmap* pixmap, QString name, QString time, QString text, bool bTeacher)
{
	if (!pixmap)
		pixmap = &QPixmap("./images/teacherPhoto.png");

	QFont font;
	font.setPixelSize(13);
	font.setFamily(("΢���ź�"));

	QFont font1;
	font1.setPixelSize(12);
	font1.setFamily(("΢���ź�"));

	// ��һ�У�ͷ�����֡�ʱ�䣩
	QHBoxLayout* FirstRow = new QHBoxLayout();
	FirstRow->setObjectName("firstRow");

	CBtnPix* head = new CBtnPix(*pixmap, this);
	head->setFixedSize(24, 24);

	QLabel* LName = new QLabel();
	if (name.count()>7)
		name = name.mid(0, 7);
	LName->setText(name);
	LName->setFont(font);
	if (bTeacher)
		LName->setStyleSheet("color: rgb(190, 11, 11);"); //��ʦ������ɫ
	else
		LName->setStyleSheet("color: rgb(135, 195, 237);"); //ѧ��������ɫ
	LName->setFixedWidth(LName->fontMetrics().width(name));

	QLabel* LTime = new QLabel();
	LTime->setText(time);
	LTime->setFont(font1);
	LTime->setStyleSheet("color: rgb(153, 153, 153);");
	FirstRow->addWidget(head);
	FirstRow->addWidget(LName);
	FirstRow->addWidget(LTime);

	// �ڶ��У��������ݣ�
	QHBoxLayout* SecRow = new QHBoxLayout();
	SecRow->setContentsMargins(30, 0, 20, 0);
	if (isDigitStr(text))
	{
		AnimatedTextBrowser* Text = new AnimatedTextBrowser(this);
		Text->setFont(font);
		Text->append(text);
		Text->setStyleSheet("color: rgb(85, 85, 85);");
		Text->setFixedHeight(Text->document()->size().height());
		Text->autoHeight();
		connect(Text, SIGNAL(sig_scrollDown()), this, SLOT(slot_scrollDown()));
		SecRow->addWidget(Text);
	}
	else
	{
		QLabel* LText = new QLabel();
		LText->setWordWrap(true);
		LText->setText(text);
		LText->setFont(font);
		LText->setStyleSheet("color: rgb(85, 85, 85);");
		SecRow->addWidget(LText);
	}
	
	m_Ver->addLayout(FirstRow);
	m_Ver->addLayout(SecRow);

	// ��ӵ�������
	if (m_spacer == NULL)
	{
		m_spacer = new QSpacerItem(5, 5, QSizePolicy::Minimum, QSizePolicy::Expanding);
		m_Ver->addSpacerItem(m_spacer);
	}
	else
	{
		m_Ver->removeItem(m_spacer);
		delete m_spacer;
		m_spacer = NULL;
		m_spacer = new QSpacerItem(5, 5, QSizePolicy::Minimum, QSizePolicy::Expanding);
		m_Ver->addSpacerItem(m_spacer);
	}

// 	sleep(50);
// 	ScrollDown();
}

// ��������������Ϣ
void UITalk::InsertAudioChat(QPixmap* pixmap, QString name, QString time, QString text, std::string path, std::string sid, std::string msgid,nim::IMMessage msg, bool bTeacher, bool bRead)
{
	if (!pixmap)
		pixmap = &QPixmap("./images/teacherPhoto.png");

	QFont font;
	font.setPixelSize(13);
	font.setFamily(("΢���ź�"));

	QFont font1;
	font1.setPixelSize(12);
	font1.setFamily(("΢���ź�"));

	// ��һ�У�ͷ�����֡�ʱ�䣩
	QHBoxLayout* FirstRow = new QHBoxLayout();
	FirstRow->setObjectName("firstRow");

	CBtnPix* head = new CBtnPix(*pixmap, this);
	head->setFixedSize(24, 24);

	QLabel* LName = new QLabel();
	if (name.count() > 7)
		name = name.mid(0, 7);
	LName->setText(name);
	LName->setFont(font);
	if (bTeacher)
		LName->setStyleSheet("color: rgb(190, 11, 11);"); //��ʦ������ɫ
	else
		LName->setStyleSheet("color: rgb(135, 195, 237);"); //ѧ��������ɫ
	LName->setFixedWidth(LName->fontMetrics().width(name));

	QLabel* LTime = new QLabel();
	LTime->setText(time);
	LTime->setFont(font1);
	LTime->setStyleSheet("color: rgb(153, 153, 153);");
	FirstRow->addWidget(head);
	FirstRow->addWidget(LName);
	FirstRow->addWidget(LTime);

	// �ڶ��У��������ݣ�
	QHBoxLayout* SecRow = new QHBoxLayout();
	SecRow->setContentsMargins(30, 0, 0, 0);
	CBtnAudio* pAudio = new CBtnAudio(path, sid,msgid,this,bRead);
	pAudio->setMsg(msg);
	pAudio->setFixedSize(80, 25);
	connect(pAudio, SIGNAL(sig_Audioclicked(std::string, std::string, std::string, bool)), this, SLOT(slot_Audioclicked(std::string, std::string, std::string, bool)));
	connect(pAudio, SIGNAL(sig_AudioLoadFail(nim::IMMessage)), this, SLOT(slot_AudioLoadFail(nim::IMMessage)));
	m_vecAudio.push_back(pAudio);
	SecRow->addWidget(pAudio);

	QLabel* LDur = new QLabel(); // ��ʾʱ��
	LDur->setText(text + "\"");
	LDur->setFont(font);
	LDur->setStyleSheet("color:rgb(153,153,153);}");
	SecRow->addWidget(LDur);
	// �Ӹ����ɣ��Ѱ�ť����
	QSpacerItem* spacer = new QSpacerItem(5, 5, QSizePolicy::Expanding, QSizePolicy::Minimum);
	SecRow->addSpacerItem(spacer);

	m_Ver->addLayout(FirstRow);
	m_Ver->addLayout(SecRow);

	// ��ӵ�������
	if (m_spacer == NULL)
	{
		m_spacer = new QSpacerItem(5, 5, QSizePolicy::Minimum, QSizePolicy::Expanding);
		m_Ver->addSpacerItem(m_spacer);
	}
	else
	{
		m_Ver->removeItem(m_spacer);
		m_spacer = NULL;
		m_spacer = new QSpacerItem(5, 5, QSizePolicy::Minimum, QSizePolicy::Expanding);
		m_Ver->addSpacerItem(m_spacer);
	}

	if (!bRead)
		nim::NOS::FetchMedia(msg, nim::NOS::DownloadMediaCallback(), nim::NOS::ProgressCallback());
// 	sleep(50);
// 	ScrollDown();
}

// ����֪ͨ��Ϣ��
void UITalk::InsertNotice(QString text)
{
	QFont font;
	font.setPixelSize(13);
	font.setFamily(("΢���ź�"));

	// ��һ�У���Ϣ��
	QHBoxLayout* FirstRow = new QHBoxLayout();
	QLabel* LNotice = new QLabel();
	LNotice->setText(text);
	LNotice->setFont(font);
	LNotice->setStyleSheet("color: rgb(85, 85, 85);"); //ѧ��������ɫ
	LNotice->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
	LNotice->setFixedHeight(35);
	FirstRow->addWidget(LNotice);

	m_Ver->addLayout(FirstRow);

	// ��ӵ�������
	if (m_spacer == NULL)
	{
		m_spacer = new QSpacerItem(5, 5, QSizePolicy::Minimum, QSizePolicy::Expanding);
		m_Ver->addSpacerItem(m_spacer);
	}
	else
	{
		m_Ver->removeItem(m_spacer);
		m_spacer = NULL;
		m_spacer = new QSpacerItem(5, 5, QSizePolicy::Minimum, QSizePolicy::Expanding);
		m_Ver->addSpacerItem(m_spacer);
	}

// 	sleep(50);
// 	ScrollDown();
}

// ����ͼƬ������Ϣ
void UITalk::InsertPic(QPixmap* pixmap, QString name, QString time, QString url, QString sMsgID, bool bTeacher)
{
	if (!pixmap)
		pixmap = &QPixmap("./images/teacherPhoto.png");

	QFont font;
	font.setPixelSize(13);
	font.setFamily(("΢���ź�"));

	QFont font1;
	font1.setPixelSize(12);
	font1.setFamily(("΢���ź�"));

	// ��һ�У�ͷ�����֡�ʱ�䣩
	QHBoxLayout* FirstRow = new QHBoxLayout();

	CBtnPix* head = new CBtnPix(*pixmap, this);
	head->setFixedSize(24, 24);

	QLabel* LName = new QLabel();
	if (name.count() > 7)
		name = name.mid(0, 7);
	LName->setText(name);
	LName->setFont(font);
	if (bTeacher)
		LName->setStyleSheet("color: rgb(190, 11, 11);"); //��ʦ������ɫ
	else
		LName->setStyleSheet("color: rgb(135, 195, 237);"); //ѧ��������ɫ
	LName->setFixedWidth(LName->fontMetrics().width(name));

	QLabel* LTime = new QLabel();
	LTime->setText(time);
	LTime->setFont(font1);
	LTime->setStyleSheet("color: rgb(153, 153, 153);");
	FirstRow->addWidget(head);
	FirstRow->addWidget(LName);
	FirstRow->addWidget(LTime);

	// �ڶ��У��������ݣ�
	QHBoxLayout* SecRow1 = new QHBoxLayout();
	SecRow1->setContentsMargins(30, 0, 0, 0);

	QVBoxLayout* SecRow = new QVBoxLayout();
//	SecRow->setContentsMargins(30, 0, 0, 0);

	// ��ȡ����
	QFile theFile(url);
	theFile.open(QIODevice::ReadOnly);
	QByteArray ba = theFile.readAll();

	// ����QPixmap�����ļ�
	QPixmap img;
	img.loadFromData(ba);
	CBtnPix* pBtn = new CBtnPix(url, img, this);
	if (pBtn)
	{
		connect(pBtn, SIGNAL(sig_idclicked(QString, QPixmap, bool)), this, SLOT(slot_btnclicked(QString, QPixmap, bool)));
		connect(pBtn, SIGNAL(sig_faildclicked(CBtnPix*)), this, SLOT(slot_faildclicked(CBtnPix*)));

		QPushButton* pImgProcess = new QPushButton();
		pImgProcess->setFixedSize(0, 0);
		pImgProcess->setStyleSheet("QPushButton{border-image:url(./images/process.png);");
		pBtn->SetImgProcess(pImgProcess);
		pBtn->SetMsgID(sMsgID);

		SecRow->addWidget(pBtn);
		SecRow->addWidget(pImgProcess);
		
		m_vecImgProcess.push_back(pBtn);
	}

	QSpacerItem* spacer = new QSpacerItem(5, 5, QSizePolicy::Expanding, QSizePolicy::Minimum);
	SecRow1->addLayout(SecRow);
	SecRow1->addItem(spacer);

	m_Ver->addLayout(FirstRow);
	m_Ver->addLayout(SecRow1);

	// ��ӵ�������
	if (m_spacer == NULL)
	{
		m_spacer = new QSpacerItem(5, 5, QSizePolicy::Minimum, QSizePolicy::Expanding);
		m_Ver->addSpacerItem(m_spacer);
	}
	else
	{
		m_Ver->removeItem(m_spacer);
		m_spacer = NULL;
		m_spacer = new QSpacerItem(5, 5, QSizePolicy::Minimum, QSizePolicy::Expanding);
		m_Ver->addSpacerItem(m_spacer);
	}

// 	sleep(50);
// 	ScrollDown();
}

// ����ͼƬ������Ϣ
void UITalk::InsertPicUrl(QPixmap* pixmap, QString name, QString time, QString url_, QString sMsgID, bool bTeacher)
{
	if (!pixmap)
		pixmap = &QPixmap("./images/teacherPhoto.png");

	QFont font;
	font.setPixelSize(13);
	font.setFamily(("΢���ź�"));

	QFont font1;
	font1.setPixelSize(12);
	font1.setFamily(("΢���ź�"));

	// ��һ�У�ͷ�����֡�ʱ�䣩
	QHBoxLayout* FirstRow = new QHBoxLayout();

	CBtnPix* head = new CBtnPix(*pixmap, this);
	head->setFixedSize(24, 24);

	QLabel* LName = new QLabel();
	if (name.count() > 7)
		name = name.mid(0, 7);
	LName->setText(name);
	LName->setFont(font);
	if (bTeacher)
		LName->setStyleSheet("color: rgb(190, 11, 11);"); //��ʦ������ɫ
	else
		LName->setStyleSheet("color: rgb(135, 195, 237);"); //ѧ��������ɫ
	LName->setFixedWidth(LName->fontMetrics().width(name));

	QLabel* LTime = new QLabel();
	LTime->setText(time);
	LTime->setFont(font1);
	LTime->setStyleSheet("color: rgb(153, 153, 153);");
	FirstRow->addWidget(head);
	FirstRow->addWidget(LName);
	FirstRow->addWidget(LTime);

	// �ڶ��У��������ݣ�
	QHBoxLayout* SecRow1 = new QHBoxLayout();
	SecRow1->setContentsMargins(30, 0, 0, 0);

	QVBoxLayout* SecRow = new QVBoxLayout();
	//	SecRow->setContentsMargins(30, 0, 0, 0);

	// ��ȡ����
	QUrl url(url_);
	QNetworkAccessManager manager;
	QEventLoop loop;

	QNetworkReply *reply = manager.get(QNetworkRequest(url));
	//���������������ɺ��˳����¼�ѭ�� 
	QObject::connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
	//�������¼�ѭ�� 
	loop.exec();
	QByteArray jpegData = reply->readAll();
	QPixmap pixmapload;

	// ���سɹ�����ʾ
	pixmapload.loadFromData(jpegData);
	CBtnPix* pBtn = new CBtnPix(url_, pixmapload, this);
	if (pBtn)
	{
		connect(pBtn, SIGNAL(sig_idclicked(QString, QPixmap, bool)), this, SLOT(slot_btnclicked(QString, QPixmap, bool)));
		connect(pBtn, SIGNAL(sig_faildclicked(CBtnPix*)), this, SLOT(slot_faildclicked(CBtnPix*)));

		QPushButton* pImgProcess = new QPushButton();
		pImgProcess->setFixedSize(0, 0);
		pImgProcess->setStyleSheet("QPushButton{border-image:url(./images/process.png);");
		pBtn->SetImgProcess(pImgProcess);
		pBtn->SetMsgID(sMsgID);

		SecRow->addWidget(pBtn);
		SecRow->addWidget(pImgProcess);

		m_vecImgProcess.push_back(pBtn);
	}

	QSpacerItem* spacer = new QSpacerItem(5, 5, QSizePolicy::Expanding, QSizePolicy::Minimum);
	SecRow1->addLayout(SecRow);
	SecRow1->addItem(spacer);

	m_Ver->addLayout(FirstRow);
	m_Ver->addLayout(SecRow1);

	// ��ӵ�������
	if (m_spacer == NULL)
	{
		m_spacer = new QSpacerItem(5, 5, QSizePolicy::Minimum, QSizePolicy::Expanding);
		m_Ver->addSpacerItem(m_spacer);
	}
	else
	{
		m_Ver->removeItem(m_spacer);
		m_spacer = NULL;
		m_spacer = new QSpacerItem(5, 5, QSizePolicy::Minimum, QSizePolicy::Expanding);
		m_Ver->addSpacerItem(m_spacer);
	}

// 	sleep(50);
// 	ScrollDown();
}

// ���ͼƬ��ȡͼƬ����url
void UITalk::slot_btnclicked(QString imgPath, QPixmap pixmap, bool b)
{
	imgPath.replace("/", "\\");
	TCHAR path[MAX_PATH] = { 0 };
	TCHAR Dir[MAX_PATH] = { 0 };
	GetSystemDirectory(Dir, MAX_PATH);
	lstrcat(path, Dir);
	lstrcat(path, L"\\rundll32.exe");

	QString sDir = QString::fromStdWString(Dir);;
	QString strParm;
	strParm.append(QString("%1\\shimgvw.dll imageview_fullscreen %2").arg(sDir).arg(imgPath));
	LPCTSTR lpstrParm = (LPCTSTR)strParm.utf16();
	HINSTANCE hNewExe = ShellExecute(NULL, L"Open", path, lpstrParm, NULL, SW_SHOW);
	if ((DWORD)hNewExe <= 32)
	{
		int error = (DWORD)hNewExe;
	}
}

// �������������Ϣ
void UITalk::InsertEmoji(QPixmap* pixmap, QString name, QString time, QString text, bool bTeacher)
{
	if (!pixmap)
		pixmap = &QPixmap("./images/teacherPhoto.png");

	QFont font;
	font.setPixelSize(13);
	font.setFamily(("΢���ź�"));

	QFont font1;
	font1.setPixelSize(12);
	font1.setFamily(("΢���ź�"));

	// ��һ�У�ͷ�����֡�ʱ�䣩
	QHBoxLayout* FirstRow = new QHBoxLayout();

	CBtnPix* head = new CBtnPix(*pixmap, this);
	head->setFixedSize(24, 24);

	QLabel* LName = new QLabel();
	if (name.count() > 7)
		name = name.mid(0, 7);
	LName->setText(name);
	LName->setFont(font);
	if (bTeacher)
		LName->setStyleSheet("color: rgb(190, 11, 11);"); //��ʦ������ɫ
	else
		LName->setStyleSheet("color: rgb(135, 195, 237);"); //ѧ��������ɫ
	LName->setFixedWidth(LName->fontMetrics().width(name));

	QLabel* LTime = new QLabel();
	LTime->setText(time);
	LTime->setFont(font1);
	LTime->setStyleSheet("color: rgb(153, 153, 153);");
	FirstRow->addWidget(head);
	FirstRow->addWidget(LName);
	FirstRow->addWidget(LTime);

	// �ڶ��У��������ݣ�
	QHBoxLayout* SecRow = new QHBoxLayout();
	SecRow->setContentsMargins(30, 0, 20, 0);

	AnimatedTextBrowserA* Anim = new AnimatedTextBrowserA(true,this);
	Anim->setOpenLinks(true);
	Anim->setMinimumWidth(245);
	Anim->setFont(font);
	SecRow->addWidget(Anim);
	ParseFace(Anim, text);
	Anim->autoHeight();
	connect(Anim, SIGNAL(sig_scrollDown()), this, SLOT(slot_scrollDown()));

	m_Ver->addLayout(FirstRow);
	m_Ver->addLayout(SecRow);

	// ��ӵ�������
	if (m_spacer == NULL)
	{
		m_spacer = new QSpacerItem(5, 5, QSizePolicy::Minimum, QSizePolicy::Expanding);
		m_Ver->addSpacerItem(m_spacer);
	}
	else
	{
		m_Ver->removeItem(m_spacer);
		m_spacer = NULL;
		m_spacer = new QSpacerItem(5, 5, QSizePolicy::Minimum, QSizePolicy::Expanding);
		m_Ver->addSpacerItem(m_spacer);
	}
	
// 	sleep(50);
// 	ScrollDown();
}

void UITalk::ParseFace(AnimatedTextBrowserA* Anim, QString qContect)
{
	Anim->append("");
	QString qFace = "";			// ����
	bool    bHas = false;		// �жϵ�ǰ�Ƿ��б���
	int iCount = qContect.size();
	for (int i = 0; i < iCount; i++)
	{
		QString str = qContect.at(i);
		if (str == "[")
		{
			QString sBlock = qContect.mid(i, 4);
			// ����������ȫ�ȶ�
			if (sBlock == "[em_")
				bHas = true;
		}

		if (bHas)
			qFace += str;

		if (str == "]" && bHas)
		{
			QString strFace = BuildFaceToUrl(qFace);
			Anim->insertHtml("<img src='" + strFace + "'/>");  //   �˴���test �� url
			Anim->addAnimation(QUrl(strFace), strFace);  //���һ������.	

			qFace = "";
			bHas = false;
			continue;
		}

		if (!bHas)
		{
			stringToHtml(str,QColor(85,85,85));
			Anim->insertHtml(str);
		}
	}
}

QString UITalk::BuildFaceToUrl(QString qFace)
{
	qFace.replace("[", "");
	qFace.replace("]", "");

	QString qFacePath = "./images/em.gif";
	qFacePath.replace("em", qFace);
	return qFacePath;
}

void UITalk::stringToHtml(QString &str, QColor crl)
{
	QByteArray array;
	array.append(crl.red());
	array.append(crl.green());
	array.append(crl.blue());
	QString strC(array.toHex());
	str = QString("<span style=\" color:#%1;\">%2</span>").arg(strC).arg(str);
}

// ��������ײ�
void UITalk::ScrollDown()
{
	if (m_view)
		m_view->verticalScrollBar()->setValue(m_view->verticalScrollBar()->maximum());
}

void UITalk::PicProcess(double iProcess)
{
	if (m_vecImgProcess.size() > 0)
	{
		std::vector<CBtnPix*>::iterator it;
		it = m_vecImgProcess.begin();
		CBtnPix* img = *it;

		int iPicProcess = 0;
		if (iProcess != 1)
			iPicProcess = 120 * iProcess;
		else
			m_vecImgProcess.erase(it);

		QPushButton* imgProcess = img->GetImgProcess();
		if (imgProcess)
		{
			imgProcess->setFixedSize(iPicProcess, 3);
		}
	}
}

void UITalk::SendStatus(QString msgid)
{
	if (m_vecImgProcess.size() > 0)
	{
		std::vector<CBtnPix*>::iterator it;
		for (it = m_vecImgProcess.begin(); it != m_vecImgProcess.end(); it++)
		{
			CBtnPix* img = *it;
			if (img->GetMsgID() == msgid)
			{
				img->AddFailBtn();

				m_vecImgProcess.erase(it);
				return;
			}

		}
	}
}

void UITalk::setMainWindow(UIChatRoom* parent)
{
	m_parent = parent;
}

void UITalk::slot_faildclicked(CBtnPix* img)
{
	img->DeleteFailBtn();

	QString msgId;
	if (m_parent)
		m_parent->SendImage(img->GetUrl().toStdWString(), msgId, img->GetMsgID());

	m_vecImgProcess.push_back(img);
}

void UITalk::slot_scrollDown()
{
//	ScrollDown();
}

// �ж��Ƿ���������10�����ֻ���Ӣ��
bool UITalk::isDigitStr(QString str)
{
	int nCountChina = 0;
	int nCount = str.count();
	for (int i = 0; i < nCount; i++)
	{
		QChar cha = str.at(i);
		ushort uni = cha.unicode();
		if (uni >= 0x4E00 && uni <=0x9FA5)	//����ַ�������  
		{
			nCountChina=0;
		}
		else
		{
			nCountChina++;
			if (nCountChina == 10)
				return true;
		}
	}

	return false;
}

void UITalk::style(QScrollArea *style)
{
	if (style)
	{
		// ���ù�������ʽ
		style->verticalScrollBar()->setStyleSheet("QScrollBar:vertical"
			"{"
			"width:8px;"
			"background:rgba(0,0,0,0%);"
			"margin:0px,0px,0px,0px;"
			"padding-top:9px;"
			"padding-bottom:9px;"
			"}"
			"QScrollBar::handle:vertical"
			"{"
			"width:8px;"
			"background:rgba(0,0,0,25%);"
			" border-radius:4px;"
			"min-height:20;"
			"}"
			"QScrollBar::handle:vertical:hover"
			"{"
			"width:8px;"
			"background:rgba(0,0,0,50%);"
			" border-radius:4px;"
			"min-height:20;"
			"}"
			"QScrollBar::add-line:vertical"
			"{"
			"height:9px;width:8px;"
			"border-image:url(:/images/a/3.png);"
			"subcontrol-position:bottom;"
			"}"
			"QScrollBar::sub-line:vertical"
			"{"
			"height:9px;width:8px;"
			"border-image:url(:/images/a/1.png);"
			"subcontrol-position:top;"
			"}"
			"QScrollBar::add-line:vertical:hover"
			"{"
			"height:9px;width:8px;"
			"border-image:url(:/images/a/4.png);"
			"subcontrol-position:bottom;"
			"}"
			"QScrollBar::sub-line:vertical:hover"
			"{"
			"height:9px;width:8px;"
			"border-image:url(:/images/a/2.png);"
			"subcontrol-position:top;"
			"}"
			"QScrollBar::add-page:vertical,QScrollBar::sub-page:vertical"
			"{"
			"background:rgba(0,0,0,10%);"
			"border-radius:4px;"
			"}"
			);
	}
}

void UITalk::slot_Audioclicked(std::string path, std::string sid, std::string msgid, bool isPlay)
{
	if (m_parent)
		m_parent->OnPlayAudio(path, sid, msgid, isPlay);
}

void UITalk::slot_AudioLoadFail(nim::IMMessage msg)
{
	nim::NOS::FetchMedia(msg, nim::NOS::DownloadMediaCallback(), nim::NOS::ProgressCallback());
}

void UITalk::stopAudio(char* msgid)
{
	QString strMsgid = QString(QLatin1String(msgid));
	QString sMsgid = strMsgid.mid(0, 32);

	if (m_vecAudio.size() > 0)
	{
		std::vector<CBtnAudio*>::iterator it;
		for (it = m_vecAudio.begin(); it != m_vecAudio.end(); it++)
		{
			CBtnAudio* img = *it;
			if (img->GetMsgID() == sMsgid)
			{
				img->stopPlay();
				return;
			}

		}
	}
}

void UITalk::rangeChanged(int min, int max)
{
	ScrollDown();
}

void UITalk::SetAudioStatus(char* msgid, bool bSuc)
{
	if (!bSuc)
	{
		QString strMsgid = QString(QLatin1String(msgid));
		if (m_vecAudio.size() > 0)
		{
			std::vector<CBtnAudio*>::iterator it;
			for (it = m_vecAudio.begin(); it != m_vecAudio.end(); it++)
			{
				CBtnAudio* img = *it;
				if (img->GetMsgID() == strMsgid)
				{
					img->LoadFail();
					return;
				}
			}
		}
	}
}