#include "palette.h"
#include "ui_palette.h"

#include <QMoveEvent>
#include <QPainter>
#include <QDebug>
#include <QColorDialog>
#include <QTimer>
#include "shape.h"
#include "windows.h"
#include <QThread>
#include <QDateTime>

#define  COLOR_VALUE_BLACK QColor(0, 0, 0)
#define  COLOR_VALUE_RED QColor(204, 0, 0)
#define  COLOR_VALUE_YELLOW QColor(255, 204, 0)
#define  COLOR_VALUE_BLUE QColor(0, 153, 255)
#define  COLOR_VALUE_GREEN QColor(0, 153, 0)
#define  COLOR_VALUE_PURPLE QColor(255, 51, 255)

Palette::Palette(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Palette)
	, m_timer(NULL)
	, m_LaserShape(NULL)
	, m_OnlyTime(0)
{
    ui->setupUi(this);
	setMouseTracking(true);
    mMouseIsPress = false;

    mPen.setColor(Qt::black);
    mPen.setWidth(2);

	m_timer = new QTimer(this);
	connect(m_timer, SIGNAL(timeout()), this, SLOT(slot_onCountTimeout()));

	if (m_LaserShape == NULL)
	{
		m_LaserShape = new Shape();
		m_LaserShape->setLaserPen(true);
		mLaserShapeStack.append(m_LaserShape);
		mSycnShapeStack.append(m_LaserShape);
	}
}

void Palette::slot_onCountTimeout()
{
	
}

Palette::~Palette()
{
    delete ui;

    foreach(Shape *shape, mShapeStack)
    {
        delete shape;
    }

	foreach(Shape *shape, mSycnShapeStack)
	{
		delete shape;
	}
}

void Palette::setPenColor(const QColor &color)
{
    mPen.setColor(color);
}

QColor Palette::getPenColor()
{
	return mPen.color();
}

// 撤销
void Palette::revocation()
{
	if (mShapeStack.isEmpty())
	{
		return;
	}
	delete mShapeStack.last();
	mShapeStack.removeLast();

	QString strInfo;
	strInfo.append(QString("%1:%2,%3,%4;").arg(QString::number(DrawOpUndo)).arg("").arg("").arg(""));
	emit PicData(strInfo);
	update();
}

// 清空
void Palette::cleanUp()
{
	foreach(Shape *shape, mShapeStack)
	{
		delete shape;
	}
	mShapeStack.clear();

	foreach(Shape *shape, mSycnShapeStack)
	{
		if (shape != m_LaserShape)
		{
			delete shape;
		}	
	}
	mSycnShapeStack.clear();
	update();
}

void Palette::setIsDraw(bool isDraw)
{
	mIsDraw = isDraw;
	setMouseStyle(mIsDraw);
}

void Palette::setMouseStyle(bool isDraw)
{
	if (isDraw)
	{
		QCursor cursor;
		QPixmap pixmap("./images/pen.png");
		cursor = QCursor(pixmap, 0, pixmap.height());
		setCursor(cursor);
	}
	else
	{
		QCursor cursor;
		cursor = QCursor(Qt::ArrowCursor);
		setCursor(cursor);
	}
}

void Palette::paintEvent(QPaintEvent *)
{
	if (this->isVisible())
	{
		QPainter paint(this);
		paint.setBrush(Qt::white);
		paint.setPen(Qt::white);   //绘制画板区域
		QSize lsize = size();
		paint.drawRect(0, 0, lsize.width(), lsize.height());

		foreach(Shape *shape, mShapeStack)
		{
			shape->paint(paint, lsize);
		}

		foreach(Shape *shape, mSycnShapeStack)
		{
			shape->paint(paint, lsize);
		}

		foreach(Shape *shape, mLaserShapeStack)
		{
			shape->paint(paint, lsize);
		}
	}
}

void Palette::mousePressEvent(QMouseEvent *event)
{
	setFocus();

	if (!mIsDraw)
	{
		return;
	}
    switch (event->button()) {
    case Qt::LeftButton:
    {
        mMouseIsPress = true;
        Shape *shape = new Shape();
        mShapeStack.append(shape);

		QPoint point = event->pos();
		QSize lSize = size();
		QPointF pointf(point.x() * 1.0 / lSize.width(), point.y() * 1.0 / lSize.height());

		shape->append(pointf);
        shape->setPen(mPen);
        update();

		float x = (float)(point.x() * 1.0 / lSize.width());
		float y = (float)(point.y() * 1.0 / lSize.height());
		QString ptX = QString("%1").arg(x);				//	x的相对坐标
		QString ptY = QString("%1").arg(y);				//  y的相对坐标
		QString opType = QString::number(DrawOpStart);	//  消息类型
		DWORD clr = colorConvert(getPenColor());
		QString sClr = QString::number(clr);			//  颜色类型
		QString strInfo;
		strInfo.append(QString("%1:%2,%3,%4;").arg(opType).arg(ptX).arg(ptY).arg(sClr));
		emit PicData(strInfo);
    }
        break;
    default:
        break;
    }	
}

void Palette::mouseMoveEvent(QMouseEvent *event)
{
	if (!mIsDraw)
	{
		return;
	}

    if(mMouseIsPress)
    {
		QPoint point = event->pos();
		QSize lSize = size();
		QPointF pointf(point.x() * 1.0 / lSize.width(), point.y() * 1.0 / lSize.height());
		mShapeStack.last()->append(pointf);
        update();

		float x = (float)(point.x() * 1.0 / lSize.width());
		float y = (float)(point.y() * 1.0 / lSize.height());
		QString ptX = QString("%1").arg(x);				//	x的相对坐标
		QString ptY = QString("%1").arg(y);				//  y的相对坐标
		QString opType = QString::number(DrawOpMove);	//  消息类型
		DWORD clr = colorConvert(getPenColor());
		QString sClr = QString::number(clr);			//  颜色类型
		QString strInfo;
		strInfo.append(QString("%1:%2,%3,%4;").arg(opType).arg(ptX).arg(ptY).arg(sClr));
		emit PicData(strInfo);
    }
}

void Palette::mouseReleaseEvent(QMouseEvent *event)
{
	if (!mIsDraw)
	{
		return;
	}
    if(Qt::LeftButton == event->button())
    {
        mMouseIsPress = false;
        update();

		QPoint point = event->pos();
		QSize lSize = size();
		float x = (float)(point.x() * 1.0 / lSize.width());
		float y = (float)(point.y() * 1.0 / lSize.height());
		QString ptX = QString("%1").arg(x);				//	x的相对坐标
		QString ptY = QString("%1").arg(y);				//  y的相对坐标
		QString opType = QString::number(DrawOpEnd);	//  消息类型
		DWORD clr = colorConvert(getPenColor());
		QString sClr = QString::number(clr);			//  颜色类型
		QString strInfo;
		strInfo.append(QString("%1:%2,%3,%4;").arg(opType).arg(ptX).arg(ptY).arg(sClr));
		emit PicData(strInfo);
    }
}

void Palette::RecData(const std::string& data)
{
	float x;
	float y;
	DWORD clr_;
	int draw_op_type_;

	QString strData = QString::fromStdString(data);

	if (strData.mid(0, 1).toInt() == kMultiBoardOpSync)
	{
		RecSyncData(strData);
		return;
	}
	else if(strData.mid(0, 2).toInt() == 16)
	{
		QStringList list = strData.split(";");
		foreach(const QString& p, list)
		{
			QStringList param_list = p.split(":");
			QStringList pointInfo = param_list.last().split(",");

			QString parentid = pointInfo.at(0);
			qint64 timetag_ = QDateTime::currentMSecsSinceEpoch();
			QString msg = QString("%1:%2,%3,%4,%5;").arg(17).arg(timetag_).arg(parentid).arg("InteractiveReson").arg("read");
			emit PicData(msg);
			return;
		}
	}
		

	QStringList list = strData.split(";");
	foreach(const QString& p, list)
	{
		QStringList param_list = p.split(":");
		draw_op_type_ = param_list.first().toInt();
		QStringList pointInfo = param_list.last().split(",");
		
		if (pointInfo.length() == 3)
		{
			x = pointInfo.at(0).toFloat();
			y = pointInfo.at(1).toFloat();
			clr_ = pointInfo.at(2).toULong();
		}
		else
			continue;

		switch (draw_op_type_)
		{
		case kMultiBoardOpStart:
		case kMultiBoardOpMove:
		case kMultiBoardOpEnd:
		{
			QPointF pt(x, y);
			if (mSycnShapeStack.size() > 0 && draw_op_type_ != kMultiBoardOpStart)
			{
				mSycnShapeStack.last()->append(pt);
				update();
			}
			else
			{
				Shape *shape = new Shape();
				mSycnShapeStack.append(shape);
				shape->append(pt);
				QPen pen;
				pen.setWidth(2);
				pen.setColor(QColor(QRgb(clr_)));
				shape->setPen(pen);
				update();
			}
			break;
		}
		case kMultiBoardOpSign:
			m_LaserShape->setLaserPT(QPointF(x, y));
			update();
			return;
		case kMultiBoardOpSignEnd:
			m_LaserShape->setLaserPT(QPointF(0, 0));
			update();
			return;
		case kMultiBoardOpClear:
		{
			cleanUp();
			return;
		}
		break;
		case  kMultiBoardOpUndo:
			DrawUndo();
			return;
		}
	}
}

void Palette::DrawUndo()
{
	if (mSycnShapeStack.isEmpty())
	{
		return;
	}

	if (mSycnShapeStack.last() != m_LaserShape)
	{
		delete mSycnShapeStack.last();
		mSycnShapeStack.removeLast();
		update();
	}
}

// 解析数据
std::list<std::string> Palette::BoardStringTokenize(const char* input, const char* delimitor)
{
	std::list<std::string> ret_list;
	std::string data = input;
	int pos = data.find(delimitor);
	while (pos != -1)
	{
		ret_list.push_back(data.substr(0, pos));
		data = data.substr(pos + 1);
		pos = data.find(delimitor);
	}
	ret_list.push_back(data);
	return ret_list;
}

int Palette::colorConvert(QColor color)
{
	int clr;
	if (color == COLOR_VALUE_BLACK)
		clr = 0x000000;
	else if (color == COLOR_VALUE_RED)
		clr = 0xcc0000;
	else if (color == COLOR_VALUE_YELLOW)
		clr = 0xffcc00;
	else if (color == COLOR_VALUE_BLUE)
		clr = 0x228bf8;
	else if (color == COLOR_VALUE_GREEN)
		clr = 0x009900;
	else if (color == COLOR_VALUE_PURPLE)
		clr = 0xff33ff;

	return clr;
}

void Palette::sendSyncQuery()
{
	qint64 cdate = QDateTime::currentMSecsSinceEpoch();
	m_OnlyTime = cdate;
	QThread::sleep(1);
	QString strInfo;
	strInfo.append(QString("%1:%2,%3,%4;").arg(kMultiBoardOpSyncQuery).arg(m_OnlyTime).arg("0.3").arg("1478787"));
	emit PicData(strInfo);
}

void Palette::RecSyncData(QString SyncData)
{
	float x;
	float y;
	DWORD clr_;
	int draw_op_type_;

	QStringList list = SyncData.split(";");
	foreach(const QString& p, list)
	{
		QStringList param_list = p.split(":");
		draw_op_type_ = param_list.first().toInt();
		QStringList pointInfo = param_list.last().split(",");

		if (pointInfo.length() == 3)
		{
			x = pointInfo.at(0).toFloat();
			y = pointInfo.at(1).toFloat();
			clr_ = pointInfo.at(2).toULong();
		}
		else
			continue;

		switch (draw_op_type_)
		{
		case kMultiBoardOpStart:
		case kMultiBoardOpMove:
		case kMultiBoardOpEnd:
		{
			QPointF pt(x, y);
			if (mShapeStack.size() > 0 && draw_op_type_ != kMultiBoardOpStart)
			{
				mShapeStack.last()->append(pt);
				update();
			}
			else
			{
				Shape *shape = new Shape();
				mShapeStack.append(shape);
				shape->append(pt);
				QPen pen;
				pen.setWidth(2);
				pen.setColor(QColor(QRgb(clr_)));
				shape->setPen(pen);
				update();
			}
			break;
		}
		}
	}
}