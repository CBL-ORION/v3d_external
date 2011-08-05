#include <iostream>
#include <fstream>
#include "custom_button_gui.h"

using namespace std;

static QString pluginRootPath = QObject::tr("/Users/xiaoh10/Applications/v3d/plugins");
static QString settingFilePath = QObject::tr("/Users/xiaoh10/.v3d_toolbox_layout");
static QList<CustomButtonSetting*> settingList;

void getAllFiles(QString dirname, QStringList & fileList)
{
	QDir dir(dirname);
	QStringList dirlist = dir.entryList(QDir::Dirs);
	if(dirlist.size() == 2) 
	{
		QStringList files = dir.entryList(QDir::Files);
		QStringList::iterator fit = files.begin();
		while(fit != files.end())
		{
			fileList.append(dir.absoluteFilePath(*fit));
			fit++;
		}
		return;
	}

	for(QStringList::iterator it = dirlist.begin(); it != dirlist.end(); it++)
	{
		//cout<<(*it).toStdString().c_str()<<endl;
		if(((*it) == ".") || ((*it) == "..")) continue;
		getAllFiles(dir.absoluteFilePath(*it), fileList);
	}
}

void getObjectList(QStringList & fileList,QList<QObject*> &objectList)
{
	if(fileList.empty()){objectList.empty(); return;}

	QStringList::iterator fit = fileList.begin();
	while(fit != fileList.end())
	{
		QPluginLoader loader(*fit);
		QObject * plugin = loader.instance();
		objectList.push_back(plugin);
		fit++;
	}
}

QString v3d_getInterfaceName(QObject *plugin)
{
    QString name;

    // Derived class must appear first, to be selected
    V3DSingleImageInterface2_1 *iFilter2_1 = qobject_cast<V3DSingleImageInterface2_1 *>(plugin);
    if (iFilter2_1 )  return (name = "V3DSingleImageInterface/2.1");

    // Base class must appear later, so derived class has a chance
    V3DSingleImageInterface *iFilter = qobject_cast<V3DSingleImageInterface *>(plugin);
    if (iFilter )  return (name = "V3DSingleImageInterface/1.0");

    V3DPluginInterface2_1 *iface2_1 = qobject_cast<V3DPluginInterface2_1 *>(plugin);
    if (iface2_1 )  return (name = "V3DPluginInterface/2.1");

    V3DPluginInterface2 *iface2 = qobject_cast<V3DPluginInterface2 *>(plugin);
    if (iface2 )  return (name = "V3DPluginInterface/2.0");

    V3DPluginInterface *iface = qobject_cast<V3DPluginInterface *>(plugin);
    if (iface )  return (name = "V3DPluginInterface/1.1");

    return name;
}

QStringList v3d_getInterfaceMenuList(QObject *plugin)
{
    QStringList qslist;

    V3DSingleImageInterface2_1 *iFilter2_1 = qobject_cast<V3DSingleImageInterface2_1 *>(plugin);
    if (iFilter2_1 )  return (qslist = iFilter2_1->menulist());

    V3DSingleImageInterface *iFilter = qobject_cast<V3DSingleImageInterface *>(plugin);
    if (iFilter )  return (qslist = iFilter->menulist());

    V3DPluginInterface *iface = qobject_cast<V3DPluginInterface *>(plugin);
    if (iface )  return (qslist = iface->menulist());

    V3DPluginInterface2 *iface2 = qobject_cast<V3DPluginInterface2 *>(plugin);
    if (iface2 )  return (qslist = iface2->menulist());

    V3DPluginInterface2_1 *iface2_1 = qobject_cast<V3DPluginInterface2_1 *>(plugin);
    if (iface2_1 )  return (qslist = iface2_1->menulist());

    return qslist;
}

QStringList v3d_getInterfaceFuncList(QObject *plugin)
{
	QStringList qslist;

	V3DPluginInterface *iface = qobject_cast<V3DPluginInterface *>(plugin);
	if (iface )  return (qslist = iface->funclist());

	V3DPluginInterface2 *iface2 = qobject_cast<V3DPluginInterface2 *>(plugin);
	if (iface2 )  return (qslist = iface2->funclist());

	V3DPluginInterface2_1 *iface2_1 = qobject_cast<V3DPluginInterface2_1 *>(plugin);
	if (iface2_1 )  return (qslist = iface2_1->funclist());

	return qslist;
}


CustomButtonSelectWidget::CustomButtonSelectWidget(V3DPluginCallback2 &callback, QWidget * parent, CustomButtonSetting* _cbs): QWidget(parent)
{
	if(!_cbs) new CustomButtonSetting();
	cbs = _cbs;

	toolBar = cbs->toolBar;

	connect(toolBar, SIGNAL(visibilityChanged(bool)), this, SLOT(saveToolBarState()));
	if(toolBar->parent() == 0)
	{
		QWidget * w = QApplication::activeWindow();
		QMainWindow * mw = qobject_cast<QMainWindow*>(w);
		if(mw) mw->addToolBar(cbs->position, toolBar);
	}

	toolButtonIcon.addPixmap(style()->standardPixmap(QStyle::SP_DirHomeIcon));
	toolBar->addAction(toolButtonIcon, tr("Add custom button"),this, SLOT(openMe()));
	toolBar->addSeparator();

	pluginIcon.addPixmap(style()->standardPixmap(QStyle::SP_DirHomeIcon));
	interfaceIcon.addPixmap(style()->standardPixmap(QStyle::SP_DirOpenIcon),
			QIcon::Normal, QIcon::On);
	interfaceIcon.addPixmap(style()->standardPixmap(QStyle::SP_DirClosedIcon),
			QIcon::Normal, QIcon::Off);
	menuIcon.addPixmap(style()->standardPixmap(QStyle::SP_FileIcon));
	funcIcon.addPixmap(style()->standardPixmap(QStyle::SP_MessageBoxInformation));

	tabWidget = new QTabWidget();
	pageTriView = new QWidget();
	page3dView = new QWidget();
	pagePlugin = new QWidget();

	pageTriViewLayout = new QGridLayout();

	pageTriView->setLayout(pageTriViewLayout);

	QStringList fileList;

	getAllFiles(pluginRootPath, fileList);

	pluginTreeWidget = new QTreeWidget();
	pluginTreeWidget->setColumnCount(2);

	QStringList headerStringList = QStringList() <<tr("Plugin Name")<<tr("Set Button Name");
	QTreeWidgetItem * headerItem = new QTreeWidgetItem(headerStringList);
	pluginTreeWidget->setHeaderItem(headerItem);

	pagePluginLayout = new QVBoxLayout();

	foreach(QString file, fileList)
	{
		QPluginLoader* loader = new QPluginLoader(file);
		pluginLoaderList.push_back(loader);

		QObject * plugin = loader->instance();

		if(plugin)
		{
			QTreeWidgetItem *pluginItem = new QTreeWidgetItem(pluginTreeWidget);
			QString pluginPath = file;
			pluginItem->setText(0, pluginPath.replace(0,pluginRootPath.size(),tr("")));
			pluginItem->setIcon(0, pluginIcon);

			QStringList menulist = v3d_getInterfaceMenuList(plugin);
			foreach(QString menu, menulist)
			{
				QTreeWidgetItem * menuItem = new QTreeWidgetItem(pluginItem);

				QCheckBox * checkbox = new QCheckBox(menu);
				checkbox->setChecked(false);
				pluginTreeWidget->setItemWidget(menuItem, 0, checkbox);
				pluginCheckboxList.push_back(checkbox);

				QLineEdit * editor = new QLineEdit(menu);
				pluginTreeWidget->setItemWidget(menuItem, 1, editor);
				editor->setDisabled(true);
				pluginEditorList.push_back(editor);

				CustomButton * qb = new CustomButton(0, editor->text(), toolBar);
				qb->button->setVisible(false);
				qb->slot_class = plugin;
				qb->bt = 2;
				qb->menu_name = menu;
				qb->callback = &callback;
				qb->parent = parent;
				qb->plugin_path = file;

				pluginCustomButtonList.push_back(qb);

				connect(checkbox, SIGNAL(clicked(bool)), editor, SLOT(setEnabled(bool)));
				connect(checkbox, SIGNAL(clicked(bool)), this, SLOT(setToolBarButton(bool)));
				connect(editor, SIGNAL(textChanged(const QString &)), qb, SLOT(setButtonText(const QString &)));
				if(cbs->preLoadPluginPathList.contains(file))
				{
					int i = cbs->preLoadPluginPathList.indexOf(file);
					editor->setText(cbs->preLoadPluginLabelList.at(i));
					editor->setEnabled(true);
					checkbox->setChecked(Qt::Checked);

					qb->button->setVisible(true);
					toolBar->addAction(qb->button);
					cbs->activePluginButtonList.push_back(qb);
				}
			}
		}
	}

	pluginTreeWidget->resizeColumnToContents(0);
	pagePluginLayout->addWidget(pluginTreeWidget);

	pagePlugin->setLayout(pagePluginLayout);

	tabWidget->addTab(pageTriView, tr("Tri View"));
	tabWidget->addTab(page3dView, tr("3D View"));
	tabWidget->addTab(pagePlugin, tr("Plugin"));
	tabWidget->setCurrentIndex(2);

	mainLayout = new QHBoxLayout();
	mainLayout->addWidget(tabWidget);

	setLayout(mainLayout);
}

CustomButtonSelectWidget::~CustomButtonSelectWidget()
{
	if(!pluginLoaderList.empty())
	{
		foreach(QPluginLoader * loader, pluginLoaderList)
		{
			loader->unload();
		}
	}

	delete cbs;
}

CustomButton * CustomButtonSelectWidget::getButton(QCheckBox* checkbox)
{
	int i = -1;
	if((i = triViewCheckboxList.indexOf(checkbox))!= -1)
	{
	}
	else if((i = view3dCheckboxList.indexOf(checkbox))!= -1)
	{
	}
	else if((i = pluginCheckboxList.indexOf(checkbox))!= -1)
	{
		return pluginCustomButtonList.at(i);
	}
	return 0;
}

CustomButton * CustomButtonSelectWidget::getButton(QAction* action)
{
	int i = -1;
	return 0;
}

void CustomButtonSelectWidget::setToolBarButton(bool state)
{
	QCheckBox * checkbox = dynamic_cast<QCheckBox*>(sender());
	CustomButton* cb = getButton(checkbox);
	if(cb==0) QMessageBox::information(0,"","0");
	if(state && !cb->button->isVisible())
	{
		cb->button->setVisible(true);
		toolBar->addAction(cb->button);
		cbs->activePluginButtonList.push_back(cb);
	}
	else if(!state && cb->button->isVisible()) 
	{
		cb->button->setVisible(false);
		int i = -1;
		if((i = cbs->activePluginButtonList.indexOf(cb)) != -1)
			cbs->activePluginButtonList.removeAt(i);
	}
	saveToolBarState();
}

void CustomButtonSelectWidget::openMe()
{
	if(isVisible()) 
		setHidden(true);
	else
		show();
}

void CustomButtonSelectWidget::closeEvent(QCloseEvent *event)
{
	if(toolBar && !toolBar->isVisible()) toolBar->show();
}
void CustomButtonSelectWidget::saveToolBarState()
{
	saveToolBarSettings();
}

bool CustomButton::run()
{
	if(slot_class==0) return false;
	if(bt == 0) // Triview
	{
		(((TriviewControl*)slot_class)->*(TriviewFunc)slot_func)();
		return true;
	}
	else if(bt == 1) // View3d
	{
		(((View3DControl*)slot_class)->*(View3DFunc)slot_func)();
		return true;
	}
	else if(bt == 2) // Plugin do menu
	{
		QObject * plugin = slot_class;
		V3DSingleImageInterface2_1 *iFilter2_1 = qobject_cast<V3DSingleImageInterface2_1 *>(plugin);
		if (iFilter2_1 )
		{
		}

		V3DSingleImageInterface *iFilter = qobject_cast<V3DSingleImageInterface *>(plugin);
		if (iFilter )
		{
		}

		V3DPluginInterface2_1 *iface2_1 = qobject_cast<V3DPluginInterface2_1 *>(plugin);
		if (iface2_1 )
		{
			iface2_1->domenu(menu_name, *callback, parent);
		}

		V3DPluginInterface2 *iface2 = qobject_cast<V3DPluginInterface2 *>(plugin);
		if (iface2 )
		{
			iface2->domenu(menu_name, *callback,parent);
		}

		V3DPluginInterface *iface = qobject_cast<V3DPluginInterface *>(plugin);
		if (iface )
		{
			iface->domenu(menu_name, *callback, parent);
		}

		return true;
	}
}

bool saveToolBarSettings() 
{
	ofstream ofs(settingFilePath.toStdString().c_str());
	if(ofs.fail())
	{
		ofs.close();
		return false;
	}

	foreach(CustomButtonSetting* cbs, settingList)
	{
		if(cbs->toolBar->isVisible())
		{
			ofs<<"ToolBar"<<endl;
			ofs<<"\t"<<cbs->toolBarTitle.toStdString()<<endl;
			ofs<<"\t"<<(int)(cbs->position)<<endl;
			foreach(CustomButton* cb, cbs->activeTriViewButtonList)
			{
			}
			foreach(CustomButton* cb, cbs->activeView3dButtonList)
			{
			}
			foreach(CustomButton* cb, cbs->activePluginButtonList)
			{
				ofs<<"PluginButton"<<endl;
				ofs<<"\t"<<cb->plugin_path.toStdString()<<endl;
				ofs<<"\t"<<cb->button->text().toStdString()<<endl;
			}
		}
	}
	ofs.close();
	return true;
}

bool loadToolBarSettings()
{
	ifstream ifs(settingFilePath.toStdString().c_str());
	if(ifs.fail())
	{
		ifs.close();
		return false;
	}
	CustomButtonSetting * cbs = 0;
	while(ifs.good())
	{
		string str;
		ifs>>str;
		if(str == "ToolBar")
		{
			char title[1000]; ifs.ignore(1000, '\t'); ifs.getline(title, 1000);
			int position; ifs.ignore(1000,'\t');ifs >> position;
			cout<<"title = \""<<title<<"\""<<endl;
			cout<<"position = "<<position<<endl;

			cbs = new CustomButtonSetting(QString(title));
			cbs->position = (Qt::ToolBarArea)position;
			settingList.push_back(cbs);
		}
		else if(str == "TriViewButton")
		{
		}
		else if(str == "View3dButton")
		{
		}
		else if(str == "PluginButton")
		{
			char path[1000]; ifs.ignore(1000,'\t'); ifs.getline(path, 1000);
			char label[1000]; ifs.ignore(1000,'\t'); ifs.getline(label,1000);
			cout<<"path = \""<<path<<"\""<<endl;
			cout<<"label = \""<<label<<"\""<<endl;
			if(cbs)
			{
				cbs->preLoadPluginPathList.push_back(QString(path).trimmed());
				cbs->preLoadPluginLabelList.push_back(QString(label).trimmed());
			}
		}
	}
	ifs.close();
	return true;
}
QList<CustomButtonSetting*>& getToolBarSettingList()
{
	return settingList;
}
void setToolBarSettingList(QList<CustomButtonSetting*> & _settingList)
{
	settingList = _settingList;
}