#ifndef UIBOARDVIEW_H
#define UIBOARDVIEW_H

#include <QWidget>
#include "ui_UIBoardview.h"

class UIBoardView : public QWidget
{
	Q_OBJECT

public:
	UIBoardView(QWidget *parent = 0);
	~UIBoardView();

private:
	Ui::UIBoardView ui;
};

#endif // UIBOARDVIEW_H
