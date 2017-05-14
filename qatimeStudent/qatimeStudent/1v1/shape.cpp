#include "shape.h"

#include <QPainter>
#include <QDebug>

Shape::Shape(QObject *parent) : QObject(parent)
{
	m_bLaserPen = false;
	m_signPen = QPixmap("./images/pensign.png");
}

void Shape::append(const QPointF &point)
{
    mPointVec.append(point);
}

void Shape::setPen(const QPen &pen)
{
    mPen = pen;
}

void Shape::paint(QPainter &painter, const QSize &size)
{
	int width = size.width();
	int height = size.height();

    painter.setPen(mPen);
    int vecsize = mPointVec.size();
	if (m_bLaserPen)
	{
		painter.drawPixmap(QPointF(m_pTLaser.x()*width, m_pTLaser.y()*height-m_signPen.height()), m_signPen);
	}
	else if (vecsize <= 0)
    {
        return;
    }
	else if (vecsize == 1)
    {
        painter.drawPoint(mPointVec.first());
    }
    else
    {
		for (int i = 0; i < vecsize - 1; i++)
        {
			const QPointF &pointf1 = mPointVec.at(i);
			const QPointF &pointf2 = mPointVec.at(i + 1);
			painter.drawLine(pointf1.x() * width, pointf1.y() * height, pointf2.x() * width, pointf2.y() * height);
        }
    }
}

void Shape::setLaserPen(bool bLaserPen)
{
	m_bLaserPen = bLaserPen;
}

void Shape::setLaserPT(const QPointF &point)
{
	m_pTLaser = point;
}