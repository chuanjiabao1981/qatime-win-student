#ifndef PALETTE_H
#define PALETTE_H

#include <QWidget>
#include <QStack>
#include <QPen>

namespace Ui {
class Palette;
}

class Shape;

class Palette : public QWidget
{
    Q_OBJECT

public:
    explicit Palette(QWidget *parent = 0);
    ~Palette();

    void setPenColor(const QColor &color);		//���û�����ɫ
	QColor getPenColor();						//��ȡ��ǰ������ɫ

	void revocation();							//������һ�β���
	void cleanUp();								//��հװ�

	void setIsDraw(bool isDraw);				//�����Ƿ���Ҫ��
	void RecData(const std::string& data);		//��������
	std::list<std::string> BoardStringTokenize(const char* input, const char* delimitor);	//��������

	void DrawUndo();							//�Է�����
	int  colorConvert(QColor color);			//��������

	enum DrawOpType
	{
		DrawOpStart = 1,
		DrawOpMove,
		DrawOpEnd,
		DrawOpUndo,
		DrawOpPktId,
		DrawOpClear,
		DrawOpClearCb,
	};

	enum MultiBoardOpType
	{
		kMultiBoardOpUnKnow = -1,
		kMultiBoardOpStart = 1,
		kMultiBoardOpMove = 2,
		kMultiBoardOpEnd = 3,
		kMultiBoardOpUndo = 4,
		kMultiBoardOpPktId = 5,
		kMultiBoardOpClear = 6,
		kMultiBoardOpClearCb = 7,
		kMultiBoardOpSyncQuery = 8,
		kMultiBoardOpSync = 9,
		kMultiBoardOpSyncPrep = 10,
		kMultiBoardOpSyncPrepCb = 11,
		kMultiBoardOpSign = 12,
		kMultiBoardOpSignEnd = 13,
		kMultiBoardOpDocInfo = 14, //"14:id���ĵ�id��,page_num(��ǰҳ����1��ʼ����),page_count����ҳ����,type(״̬֪ͨ��0����ҳ������1);" "14:8c17c252-1276-4817-92ce-688576b8df4c,2,4,1;"
	};

private:
	void setMouseStyle(bool isDraw);			//�������ָ����ʽ

protected:
    void paintEvent(QPaintEvent *);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

signals:
	void PicData(QString);
private slots:
	void slot_onCountTimeout();
private:
    Ui::Palette *ui;

    QStack< Shape * > mShapeStack;
	QStack< Shape * > mSycnShapeStack;			//���յ�������ʾ

    bool mMouseIsPress;							//����Ƿ���
	bool mIsDraw;								//�Ƿ���Ҫ��
    QPen mPen;
	QTimer*	 m_timer;
};

#endif // PALETTE_H
