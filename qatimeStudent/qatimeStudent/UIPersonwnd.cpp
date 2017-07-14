#include "UIPersonwnd.h"
#include <QHBoxLayout>
#include <QPainter>
#include <QScrollBar>

UIPersonWnd::UIPersonWnd(QWidget *parent)
	: QWidget(parent)
	, m_spacer(NULL)
	, m_vecLayout(NULL)
	, m_btn(NULL)
{
	ui.setupUi(this);
	style(ui.scrollArea);

	setAutoFillBackground(true);;
	QPalette p = palette();
	p.setColor(QPalette::Window, QColor("white"));
	setPalette(p);
}

UIPersonWnd::~UIPersonWnd()
{

}

void UIPersonWnd::AddPerson(std::vector<personListBuddy*> vecBuddy, QString strID, bool b1v1)
{
	QFont font;
	font.setFamily(QString::fromUtf8("\345\276\256\350\275\257\351\233\205\351\273\221"));
	font.setPixelSize(13);

	if (vecBuddy.size() > 0 )
	{
		std::vector<personListBuddy*>::iterator it;
		for (it = vecBuddy.begin(); it != vecBuddy.end(); it++)
		{
			personListBuddy* buddy = *it;
			if (buddy)
			{
				QHBoxLayout* SecRow = new QHBoxLayout();
				SecRow->setContentsMargins(0, 0, 0, 0);

				QPixmap* pix = NULL;
				pix= (QPixmap*)buddy->head->pixmap();
				QLabel* LPic = new QLabel();
				LPic->setFixedSize(30, 30);
				LPic->setPixmap(pix->scaled(30, 30));

				QLabel* LText = new QLabel();
				LText->setMaximumWidth(85);
				LText->setFont(font);
				LText->setText(buddy->name->text());
				if (!b1v1)
				{
					if (buddy->m_ID == strID)
						LText->setStyleSheet("color:rgb(255,0,0)");
				}
				
				QLabel* LStyle = new QLabel();
				LStyle->setFixedSize(30, 30);
				LStyle->setFont(font);
				if (b1v1)
				{
					if (buddy->m_ID == strID)
					{
						LStyle->setText("学生");
						LStyle->setStyleSheet("color:rgb(100,100,100)");
					}
					else{
						LStyle->setText("老师");
						LStyle->setStyleSheet("color:rgb(255,0,0)");
					}
				}
				else
				{
					if (buddy->m_ID == strID)
					{
						LStyle->setText("老师");
						LStyle->setStyleSheet("color:rgb(255,0,0)");
					}
					else{
						LStyle->setText("学生");
						LStyle->setStyleSheet("color:rgb(100,100,100)");
					}
				}

				// 加省略号
				int fontSize = LText->fontMetrics().width(buddy->name->text());//获取之前设置的字符串的像素大小
				if (fontSize >= LText->width()) //与label自身相比较
				{
					QString  tmpStr = LText->fontMetrics().elidedText(buddy->name->text(), Qt::ElideRight, LText->width());
					LText->setText(tmpStr);
					LText->setToolTip(buddy->name->text());
				}

				SecRow->addWidget(LPic);
				SecRow->addWidget(LText);
				SecRow->addWidget(LStyle);
				if (b1v1)
				{
					if (buddy->m_ID != strID)
						ui.verticalLayout->insertLayout(0, SecRow);
					else
						ui.verticalLayout->addLayout(SecRow);
				}
				else
				{
					if (buddy->m_ID == strID)
						ui.verticalLayout->insertLayout(0, SecRow);
					else
						ui.verticalLayout->addLayout(SecRow);
				}

				if (m_spacer == NULL)
				{
					m_spacer = new QSpacerItem(5, 5, QSizePolicy::Minimum, QSizePolicy::Expanding);
					ui.verticalLayout->addSpacerItem(m_spacer);
				}
				else
				{
					ui.verticalLayout->removeItem(m_spacer);
					m_spacer = NULL;
					m_spacer = new QSpacerItem(5, 5, QSizePolicy::Minimum, QSizePolicy::Expanding);
					ui.verticalLayout->addSpacerItem(m_spacer);
				}

				m_vecLayout.push_back(LPic);
				m_vecLayout.push_back(LText);
				m_vecLayout.push_back(LStyle);
			}

		}
	}

	setFocus();
}

void UIPersonWnd::paintEvent(QPaintEvent *event)
{
	QPainterPath path;
	path.setFillRule(Qt::WindingFill);
	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing, true);

	QColor color(5, 157, 210);
	for (int i = 0; i < 4; i++)
	{
		path.setFillRule(Qt::WindingFill);

		path.addRect(0, 0, this->width(), this->height());

		painter.setPen(color);
		painter.drawPath(path);
	}
}

void UIPersonWnd::focusOutEvent(QFocusEvent* e)
{
	QPoint pt = mapFromGlobal(QCursor::pos());
	pt.setX(pt.x() + geometry().x());
	pt.setY(pt.y() + geometry().y());
	QRect rc = this->geometry();
	if (rc.contains(pt))
	{
		setFocus();
		return;
	}

	if (m_parent->IsHasFoucs(UIWindowSet::PersonWnd))
		return;

	m_parent->GetPersonBtn()->setStyleSheet("border-image: url(./images/personBtn_nor.png);");
	this->hide();
}

void UIPersonWnd::DeletePerson()
{
	if (m_vecLayout.size()>0)
	{
		std::vector<QLabel*>::iterator it;
		for (it = m_vecLayout.begin(); it != m_vecLayout.end(); it++)
		{
			QLabel* layout = *it;
			if (layout)
			{
				delete layout;
				layout = NULL;
			}
			
		}
	}

	m_vecLayout.clear();
}

void UIPersonWnd::style(QScrollArea *style)
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

void UIPersonWnd::setParentBtn(UIWindowSet* parent)
{
	m_parent = parent;
}