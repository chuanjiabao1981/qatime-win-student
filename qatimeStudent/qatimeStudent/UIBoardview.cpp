#include "UIBoardview.h"

UIBoardView::UIBoardView(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	setStyleSheet("border-image:url(./images/AuxiliaryBtn.png);");
}

UIBoardView::~UIBoardView()
{

}
