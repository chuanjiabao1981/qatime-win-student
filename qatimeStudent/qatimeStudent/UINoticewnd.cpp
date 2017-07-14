#include "UINoticewnd.h"
#include <QPainter>
#include <QMouseEvent>
#include <QObject>
#include <QScrollBar>
#include "UITextEdit.h"

UINoticeWnd::UINoticeWnd(QWidget *parent)
	: QWidget(parent)
	, m_mainAllView(NULL)
	, m_VerAll(NULL)
	, m_spacer(NULL)
	, m_veclabel(NULL)
	, m_parent(NULL)
{
	ui.setupUi(this);

	setAutoFillBackground(true);
	QPalette p = palette();
	p.setColor(QPalette::Window, QColor("white"));
	setPalette(p);

	m_mainAllView = new QWidget();
	ui.scrollArea->setWidget(m_mainAllView);
	m_VerAll = new QVBoxLayout(m_mainAllView);
	m_VerAll->setSpacing(0);
	m_VerAll->setContentsMargins(5, 5, 5, 5);
	ui.scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	m_mainAllView->show();
	 
	style(ui.scrollArea);

	QFont font = ui.label->font();
	font.setPixelSize(13);
	ui.label->setFont(font);

	ui.noDate_label->setStyleSheet("border-image:url(./images/notice.png)");
}

UINoticeWnd::~UINoticeWnd()
{

}

void UINoticeWnd::paintEvent(QPaintEvent *event)
{
	QPainterPath path;
	path.setFillRule(Qt::WindingFill);
	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing, true);

	QColor color(5, 157, 210);
	for (int i = 0; i < 4; i++)
	{
		path.setFillRule(Qt::WindingFill);

		path.addRect(0, 0, this->width(),  this->height() );

		painter.setPen(color);
		painter.drawPath(path);
	}
}

void UINoticeWnd::focusOutEvent(QFocusEvent* e)
{
	QPoint pt = mapFromGlobal(QCursor::pos());
	pt.setY(pt.y() + geometry().y());
	QRect rc = this->geometry();
	if (rc.contains(pt))
	{
		setFocus();
		return;
	}

	if (m_parent->IsHasFoucs(UIWindowSet::NoticeWnd))
		return;

	m_parent->GetNoticeBtn()->setStyleSheet("border-image: url(./images/notice_nor.png);");
	this->hide();
}

void UINoticeWnd::AddNotic(QString text, QString time)
{
	ui.label->setVisible(true);
	ui.scrollArea->setVisible(true);
	ui.noDate_label->setVisible(false);

	QFont font;
	font.setPixelSize(13);
	font.setFamily(("微软雅黑"));

	UITextEdit* LText = new UITextEdit();
	LText->setMaximumWidth(375);
//	LText->setWordWrap(true);
	LText->setFont(font);
	LText->setText(text);

	QLabel* LTime = new QLabel();
	LTime->setFont(font);
	LTime->setAlignment(Qt::AlignRight);
	LTime->setText("发布时间：" + time);
	LTime->setStyleSheet("color:rgb(153,153,153)");
	LTime->setMaximumWidth(375);

	QLabel* LSpec = new QLabel();
	LSpec->setFixedHeight(20);

	m_VerAll->addWidget(LText);
	m_VerAll->addWidget(LTime);
	m_VerAll->addWidget(LSpec);

	m_vecedit.push_back(LText);
	m_veclabel.push_back(LTime);
	m_veclabel.push_back(LSpec);

	// 添加到布局里
	if (m_spacer == NULL)
	{
		m_spacer = new QSpacerItem(5, 5, QSizePolicy::Minimum, QSizePolicy::Expanding);
		m_VerAll->addSpacerItem(m_spacer);
	}
	else
	{
		m_VerAll->removeItem(m_spacer);
		m_spacer = NULL;
		m_spacer = new QSpacerItem(5, 5, QSizePolicy::Minimum, QSizePolicy::Expanding);
		m_VerAll->addSpacerItem(m_spacer);
	}

	setFocus();
}

void UINoticeWnd::DeleteNotice()
{
	if (m_veclabel.size()>0)
	{
		std::vector<QLabel*>::iterator it;
		for (it = m_veclabel.begin(); it != m_veclabel.end(); it++)
		{
			QLabel* label = *it;
			m_VerAll->removeWidget(label);
			delete label;
			label = NULL;
		}
	}
	
	m_veclabel.clear();

	if (m_vecedit.size() > 0)
	{
		std::vector<QTextEdit*>::iterator it;
		for (it = m_vecedit.begin(); it != m_vecedit.end(); it++)
		{
			QTextEdit* label = *it;
			m_VerAll->removeWidget(label);
			delete label;
			label = NULL;
		}
	}

	m_vecedit.clear();
}

void UINoticeWnd::style(QScrollArea *style)
{
	if (style)
	{
		// 设置滚动条样式
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

void UINoticeWnd::setParentBtn(UIWindowSet* parent)
{
	m_parent = parent;
}

void UINoticeWnd::initDate()
{
	ui.label->setVisible(false);
	ui.scrollArea->setVisible(false);
	ui.noDate_label->setVisible(true);
}