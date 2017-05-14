#ifndef SHAPE_H
#define SHAPE_H

#include <QObject>
#include <QVector>
#include <QPen>

class QPainter;

class Shape : public QObject
{
    Q_OBJECT
public:
    explicit Shape(QObject *parent = 0);

    void append(const QPointF &point);

    void setPen(const QPen &pen);

	void paint(QPainter &painter, const QSize &size);

	void setLaserPen(bool bLaserPen);

	void setLaserPT(const QPointF &point);
signals:

public slots:

private:
    QVector< QPointF > mPointVec;

    QPen mPen;

	bool m_bLaserPen;
	QPointF m_pTLaser;
	QPixmap m_signPen;
};

#endif // SHAPE_H
