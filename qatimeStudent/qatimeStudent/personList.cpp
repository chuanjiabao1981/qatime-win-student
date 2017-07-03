
#include "personlist.h"
#include <QAction>
#include <QIcon>
#include "UIMessageBox.h"
#pragma execution_character_set("utf-8")
personList::personList(QWidget *parent) :
    QListWidget(parent)
	, currentItem(NULL)
	, buddyFirst(NULL)
	, buddySec(NULL)
	, buddyLast(NULL)
	, backImage(NULL)
	, m_vecBuddy(NULL)
{
    setFocusPolicy(Qt::NoFocus);       // ȥ��itemѡ��ʱ�����߱߿�
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);//ˮƽ�������ر�
    initMenu();	

	timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), this, SLOT(timeOutSet()));
}
//��ʼ���˵�
void personList::initMenu()
{
    //��ʼ����
    blankMenu = new QMenu();
    groupMenu = new QMenu();
    personMenu = new QMenu();
    groupNameEdit=new QLineEdit();
	QAction *rename = new QAction("������", this);
	QAction *addBuddy = new QAction("��Ӻ���", this);
//    QAction *addGroup = new QAction("��ӷ���", this);
//    QAction *delGroup = new QAction("ɾ������", this);    
//    QAction *delBuddy = new QAction("ɾ������", this);
    //���ã�
    groupNameEdit->setParent(this);  //���ø���
    groupNameEdit->hide(); //���ó�ʼʱ����
    groupNameEdit->setPlaceholderText("δ����");//���ó�ʼʱ������
    //���֣�
	blankMenu->addAction(addBuddy);
    groupMenu->addAction(rename);
    groupMenu->addAction(addBuddy);
 //   personMenu->addAction(delBuddy);
//   groupMenu->addAction(delGroup);
    //��Ϣ�ۣ�
    connect(groupNameEdit,SIGNAL(editingFinished()),this,SLOT(slotRenameEditFshed()));
	connect(rename, SIGNAL(triggered()), this, SLOT(slotRename()));
	connect(addBuddy, SIGNAL(triggered()), this, SLOT(slotAddBuddy()));
//    connect(addGroup,SIGNAL(triggered()),this,SLOT(slotAddGroup()));
//    connect(delGroup,SIGNAL(triggered()),this,SLOT(slotDelGroup()));  
//    connect(delBuddy,SIGNAL(triggered()),this,SLOT(slotDelBuddy()));

}
//������¼�
void personList::mousePressEvent(QMouseEvent *event)
{
    QListWidget::mousePressEvent(event); // ��������û���mousePressEvent��item��select����첻��Ӧ,���ø��࣬��QSS��Ч����ΪQSS���ڸ���QListWidget����������Ӵ��ڣ��������ϲ㴰�ڣ��Ǹ����ڸ������ϵģ��������ڸ����ڲ�����Ϣ
    //��ֹһ���������������item�������������item��հ״�ʱ��ָ����item��currentItem����������item
//     if(groupNameEdit->isVisible() && !(groupNameEdit->rect().contains(event->pos())))
//     {
//         if(groupNameEdit->text()!=NULL)
//             currentItem->setText(groupNameEdit->text());
//         groupNameEdit->setText("");
//         groupNameEdit->hide();
//     }
//     currentItem = this->itemAt(mapFromGlobal(QCursor::pos()));//���λ�õ�Item�������Ҽ��������ȡ
//     if(event->button()==Qt::LeftButton && currentItem!=NULL && currentItem==groupMap.value(currentItem))//����������������ǵ��������
//     {
//         if(isHideMap.value(currentItem))                                  //�����ǰ�����أ�����ʾ
//         {
//             foreach(QListWidgetItem* subItem, groupMap.keys(currentItem))//������Ķ�Ӧ�����������ͺ��ѣ�
//                 if(subItem!=currentItem)                                 //�������Ļ������д���
//                 {
//                     subItem->setHidden(false);                            //����ȫ����ʾ
//                 }
//             isHideMap.insert(currentItem,false);                          //���ø���Ϊ��ʾ״̬
//             currentItem->setIcon(QIcon(":/arrowDown"));
//         }
//         else                                                             //������ǰ����ʾ��������
//         {
//             foreach(QListWidgetItem* subItem, groupMap.keys(currentItem))//������Ķ�Ӧ�����������ͺ��ѣ�
//                 if(subItem!=currentItem)                                 //�������Ļ������д���
//                 {
//                     subItem->setHidden(true);                            //����ȫ������
//                 }
//              isHideMap.insert(currentItem,true);                          //���ø���Ϊ����״̬
//              currentItem->setIcon(QIcon(":/arrowRight"));
//         }
//     }
}
//�˵��¼���Ϊ����ʾ�˵����������Ҽ���Ӧ��������¼�mousePressEvent������contextMenuEvent
void personList::contextMenuEvent(QContextMenuEvent *event)
{
    QListWidget::contextMenuEvent(event);           //���û����¼�
//     if(currentItem==NULL)                           //�����������ǿհ״�
//     {
//         blankMenu->exec(QCursor::pos());
//         return;
//     }
//     if(currentItem==groupMap.value(currentItem))    // ��������������
//         groupMenu->exec(QCursor::pos());
//     else                                            //�����������Ǻ���
//         personMenu->exec(QCursor::pos());
}
//�����
void personList::slotAddGroup()
{
//     QListWidgetItem *newItem=new QListWidgetItem(QIcon(":/arrowRight"),QString("δ����"));    //����һ��Item
//     newItem->setSizeHint(QSize(this->width(),25));//���ÿ�ȡ��߶�
//     this->addItem(newItem);         //�ӵ�QListWidget��
//     groupMap.insert(newItem,newItem);//�ӵ�����groupMap�key��value��Ϊ��
//     isHideMap.insert(newItem,true);  //���ø�������״̬
//     groupNameEdit->raise();
//     groupNameEdit->setText(QString("δ����")); //����Ĭ������
//     groupNameEdit->selectAll();        //����ȫѡ
//     groupNameEdit->setGeometry(this->visualItemRect(newItem).left()+15,this->visualItemRect(newItem).top()+1,this->visualItemRect(newItem).width(),this->visualItemRect(newItem).height()-2);//���ֵ�λ��
//     groupNameEdit->show();              //��ʾ
//     groupNameEdit->setFocus();          //��ȡ����
//     currentItem = newItem;	   // ��ΪҪ��group���������Ե�ǰ��currentItem��Ϊ��group
}
//ɾ����
void personList::slotDelGroup()
{
//     foreach(QListWidgetItem* item, groupMap.keys(currentItem))  //������������к��Ѻ��������
//     {
//         groupMap.remove(item);   //�Ƴ�
//         delete item;   //ɾ��
//     }
//     isHideMap.remove(currentItem); //�Ƴ�
}
//������
void personList::slotRename()
{
    groupNameEdit->raise();
    groupNameEdit->setGeometry(this->visualItemRect(currentItem).left()+15,this->visualItemRect(currentItem).top()+1,this->visualItemRect(currentItem).width(),this->visualItemRect(currentItem).height()-2);//���ֵ�λ��
    groupNameEdit->setText(currentItem->text());   //��ȡ����������
    groupNameEdit->show();                        //��ʾ
    groupNameEdit->selectAll();                   //ȫѡ
    groupNameEdit->setFocus();                        //��ȡ����
}

void personList::initFronUi()
{
	buddyFirst = new personListBuddy();
	buddyFirst->initFirst();		
	QListWidgetItem *newItem = new QListWidgetItem();
	newItem->setSizeHint(QSize(300, 30));
	this->insertItem(0, newItem);
	this->setItemWidget(newItem, buddyFirst);
	newItem->setHidden(false);	
}
void personList::initSecUi(int iWidth)
{
	int SecWidth = 287;
	SecWidth += iWidth;
	buddySec = new personListBuddy();	
	buddySec->initFindPeople(SecWidth);
	connect(buddySec, SIGNAL(signalFindName(const QString)), this, SLOT(findName(const QString)));
	connect(buddySec, SIGNAL(signalFindNameNULL(const QString)), this, SLOT(findName(const QString)));	
	QListWidgetItem *newItem = new QListWidgetItem();
	newItem->setSizeHint(QSize(30, 30));
	this->insertItem(1 , newItem);
	this->setItemWidget(newItem, buddySec);	
	newItem->setHidden(false);
	buddySec->secLinEdit->raise();
	newItem->setBackgroundColor(QColor(153,153,153));
	buddySec->resize(buddySec->width() + iWidth, buddySec->height());

	int LastWidth = SecWidth - 15;
	buddyLast = new personListBuddy();
	buddyLast->initNotFind(LastWidth);
	connect(buddyLast, SIGNAL(signalStopAllTalk(bool, QString)), this, SLOT(stopAllTalk(bool, QString)));
	QListWidgetItem *newItemLast = new QListWidgetItem();
	newItemLast->setSizeHint(QSize(30, 30));
	this->insertItem(2, newItemLast);
	this->setItemWidget(newItemLast, buddyLast);
	newItemLast->setHidden(false);
	buddyLast->resize(buddyLast->width() + iWidth, buddyLast->height());
}
void personList::setStrdentNumbers(int num)
{
//	buddyFirst->setOlineNum(num, 0);
}
QLabel* personList::addStrdent(QString imagesUrl,QString stuName,QString ID, int iWidth)
{
	int buddyBtnWidth = 272;
	buddyBtnWidth += iWidth;
	personListBuddy *buddy = new personListBuddy();
	QLabel* head = buddy->initUi(imagesUrl, stuName, ID, buddyBtnWidth);
	connect(buddy, SIGNAL(emitRadioChange(int, QString, QString)), this, SLOT(chickChage(int, QString, QString)));
	QList<QListWidgetItem*> tem = groupMap.keys(buddy);
	QListWidgetItem *newItem = new QListWidgetItem();       
	newItem->setSizeHint(QSize(30, 30));
	int num = row(currentItem) + tem.count();
	this->insertItem(num + 4, newItem);
	this->setItemWidget(newItem, buddy);
	groupMap.insert(newItem, buddy);
	allStudents.insert(stuName, newItem);
	IdStudents.insert(ID, newItem);
	newItem->setHidden(false);
	int test = buddy->width();
	buddy->resize(buddy->width() + iWidth, buddy->height());
	return head;
}
void personList::slotAddBuddy()
{  
//    buddy->sign->setText("��ͨ�׵����ԣ�����̵ļ�����");   //���ø���ǩ��
//     QList<QListWidgetItem*> tem = groupMap.keys(currentItem);//��ǰ���Ӧ��������鱾��ͺ��ѣ����Ƹ�tem
//     //�ؼ�����
//     QListWidgetItem *newItem = new QListWidgetItem();       //����һ��newItem
// 	newItem->setSizeHint(QSize(30, 30));
//     this->insertItem(row(currentItem)+tem.count(),newItem); //����newItem���뵽����
//     this->setItemWidget(newItem, buddy); //��buddy������newItem
//     groupMap.insert(newItem,currentItem);   //�ӽ�������keyΪ���ѣ�valueΪ��
//     if(isHideMap.value(currentItem))          //������������أ���ӽ�ȥ�ĺ�������Ϊ����
//         newItem->setHidden(true);
//     else                                      //���򣬸ú�������Ϊ��ʾ
//         newItem->setHidden(false);
}
//ɾ������
void personList::slotDelBuddy()
{
    groupMap.remove(currentItem);  //�Ƴ��ú���
    delete currentItem;            //ɾ��
}
//���Һ���
void personList::findName(const QString name)
{
	if (name.isEmpty())
	{
		for (int i = 0; i < allStudents.size(); i++)
		{			
			allStudents.values().at(i)->setHidden(false);			
		}
// 		buddyLast->notFind->setText("�������������ң�");
// 		buddyLast->notFind->show();
// 		timer->start(3000);
		buddyLast->firstButton->show();
	}
	else
	{
		int num = 0;
		for (int i = 0; i < allStudents.size(); i++)
		{
			if (allStudents.keys().at(i) != name)
			{
				allStudents.values().at(i)->setHidden(true);
				num++;
			}
		}
		if (num == allStudents.size())
		{
			for (int i = 0; i < allStudents.size(); i++)
			{
				allStudents.values().at(i)->setHidden(true);
			}
			buddyLast->notFind->setText("û���ҵ���ѧ����");
			buddyLast->notFind->show();
			buddyLast->firstButton->hide();
			timer->start(3000);
		}
		else
		{
			buddyLast->notFind->hide();
		}
	}
	
}
//���������
void personList::slotRenameEditFshed()
{
    if(groupNameEdit->text()!=NULL)      //����������༭��Ϊ��
        currentItem->setText(groupNameEdit->text());  //��������
    groupNameEdit->setText("");
    groupNameEdit->hide();  //�����������༭��
}
//����ѧ��
void personList::chickChage(int b, QString ID, QString name)
{
	emit signalChickChage(b, ID, name);
}
//�������е�ѧ��
void personList::stopAllTalk(bool b, QString ID)
{
	for (int i = 0; i < groupMap.size();i++)
	{
		groupMap.values().at(i)->setMute(b);
	}
}

void personList::timeOutSet()
{
	buddyLast->notFind->setText(" ");
	timer->stop();
}

void personList::setOlineNum(int olineNum, int allNum)
{
	buddyFirst->setOlineNum(olineNum, allNum);
}

void personList::cleanStudents(int iWidth)
{
	for (int i = 0; i < groupMap.size(); i++)
	{
		if (groupMap.keys().at(i))
		{
			delete (groupMap.keys().at(i));
			delete (groupMap.values().at(i));
		}		
	}	
	groupMap.clear();
	allStudents.clear();
	IdStudents.clear();
	this->clear();
	initSecUi(iWidth);
}

personListBuddy* personList::findID(const QString id)
{
	personListBuddy* Buddy = NULL;

	QListWidgetItem* item = NULL;
	item = IdStudents.value(id, NULL);
	if (item)
	{
		Buddy = groupMap.value(item, NULL);
		return Buddy;
	}
	else
	{
		return Buddy;
	}
}

// ������Ļ��ȣ���ťҲ��Ҫ�ı�λ��
void  personList::setAllWidth(int iWidth)
{
	if (iWidth == 0)
		return;

	int iCount = this->count();
	if (iCount == 0)
		return;

	personListBuddy* Buddy = NULL;
	for (int i = 2; i < iCount;i++)
	{
		QListWidgetItem* pItem = this->item(i);
		if (pItem)
		{
			Buddy = *groupMap.find(pItem);
			if (Buddy)
			{
				QRect rc;
				if (Buddy->button)
				{
					rc = Buddy->button->geometry();
					Buddy->button->move(rc.left() + iWidth, rc.top());
					Buddy->setFixedWidth(Buddy->width() + iWidth);
				}
			}
		}
	}

	if (buddySec)
	{
		buddySec->secLinEdit->setFixedWidth(buddySec->secLinEdit->width() + iWidth);
		QRect rc;
		rc = buddySec->secFindButton->geometry();
		buddySec->secFindButton->move(rc.left() + iWidth, rc.top());
		buddySec->setFixedWidth(buddySec->width() + iWidth);
	}

	if (buddyLast)
	{
		QRect rc;
		rc = buddyLast->firstButton->geometry();
		buddyLast->firstButton->move(rc.left() + iWidth, rc.top());
		buddyLast->setFixedWidth(buddyLast->width() + iWidth);
	}
}

// ��������Buddy���
void  personList::setBuddyWidth(int iWidth)
{
	int iCount = this->count();
	if (iCount == 0)
		return;

	personListBuddy* Buddy = NULL;
	for (int i = 2; i < iCount; i++)
	{
		QListWidgetItem* pItem = this->item(i);
		if (pItem)
		{
			Buddy = *groupMap.find(pItem);
			if (Buddy)
			{
				Buddy->setFixedWidth(iWidth);
			}
		}
	}

	if (buddySec)
		buddySec->setFixedWidth(iWidth);

	if (buddyLast)
		buddyLast->setFixedWidth(iWidth);
}

// �й���������С���
void  personList::setZoomWidth(int iWidth)
{
	int iCount = this->count();
	if (iCount == 0)
		return;

	personListBuddy* Buddy = NULL;
	for (int i = 2; i < iCount; i++)
	{
		QListWidgetItem* pItem = this->item(i);
		if (pItem)
		{
			Buddy = *groupMap.find(pItem);
			if (Buddy)
			{
				QRect rc;
				if (Buddy->button)
				{
					if (!Buddy->IsZoom())
					{
						rc = Buddy->button->geometry();
						Buddy->button->move(rc.left() + iWidth, rc.top());
						Buddy->setFixedWidth(Buddy->width() + iWidth);
						Buddy->SetZoom(true);
					}
				}
			}
		}
	}

	if (buddySec)
	{
		if (!buddySec->IsZoom())
		{
			buddySec->secLinEdit->setFixedWidth(buddySec->secLinEdit->width() + iWidth);
			QRect rc;
			rc = buddySec->secFindButton->geometry();
			buddySec->secFindButton->move(rc.left() + iWidth, rc.top());
			buddySec->setFixedWidth(buddySec->width() + iWidth);
			buddySec->SetZoom(true);
		}
	}

	if (buddyLast)
	{
		if (!buddyLast->IsZoom())
		{
			QRect rc;
			rc = buddyLast->firstButton->geometry();
			buddyLast->firstButton->move(rc.left() + iWidth, rc.top());
			buddyLast->setFixedWidth(buddyLast->width() + iWidth);
			buddyLast->SetZoom(true);
		}
	}
}

// ��ȡ��Ա
std::vector<personListBuddy*>  personList::GetBuddy()
{
	std::vector<personListBuddy*>	vecBuddy;

	int iCount = this->count();
	if (iCount == 0)
		return vecBuddy;

	personListBuddy* Buddy = NULL;
	for (int i = 0; i < iCount; i++)
	{
		QListWidgetItem* pItem = this->item(i);
		if (pItem)
		{
			Buddy = *groupMap.find(pItem);
			if (Buddy)
			{
				vecBuddy.push_back(Buddy);
			}
		}
	}

	return vecBuddy;
}