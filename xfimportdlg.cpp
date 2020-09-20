#include "xfimportdlg.h"
#include "ui_xfimportdlg.h"
#include <QFileSystemModel>
#include <QHeaderView>
#include <QListWidgetItem>
#include <QScrollBar>
#include <QFileIconProvider>


class xfFileIconProvider:public QFileIconProvider
{
public:
    xfFileIconProvider():QFileIconProvider(){};

    virtual QIcon
    icon(QFileIconProvider::IconType type) const override {return QIcon();}
    virtual QIcon
    icon(const QFileInfo &info) const override {return QIcon();}
};

xfImportDlg::xfImportDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::xfImportDlg)
{
    ui->setupUi(this);
    setWindowFlag(Qt::FramelessWindowHint,true);
    QFileSystemModel *model = new QFileSystemModel();
    model->setIconProvider(new xfFileIconProvider());
    model->setRootPath("/");
    model->setFilter(QDir::AllDirs|QDir::NoDotAndDotDot);
    ui->pDirTreeWdgt->setModel(model);
    //ui->pDirTreeWdgt->setRootIndex(model->index(QDir::currentPath()));
    ui->pDirTreeWdgt->header()->setFont(ui->pDirTreeWdgt->font());
    ui->pDirTreeWdgt->setRootIsDecorated(false);
    ui->pDirTreeWdgt->setColumnHidden(1,true);
    ui->pDirTreeWdgt->setColumnHidden(2,true);
    ui->pDirTreeWdgt->setColumnHidden(3,true);
    ui->pDirTreeWdgt->setColumnHidden(4,true);
    ui->pDirTreeWdgt->setColumnHidden(5,true);
    ui->pDirTreeWdgt->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->pDirTreeWdgt->setUniformRowHeights(true);
    ui->pImportTB->setEnabled(false);

    ui->pDirTreeWdgt->verticalScrollBar()->setStyleSheet(
    QString::fromUtf8("QScrollBar:vertical {"
                      "background: #ddd;"
                      "border: 2px solid #fff;"
                      "border-radius: 5px;"
                      "width: 40px;"
                      "margin: 7px;"
                      "}"
                      "QScrollBar::handle:vertical {"
                      "min-height: 30px;"
                      "background: #636668;"
                      "border: 0px solid;"
                      "border-radius: 5px;"
                    "}"
                    "QScrollBar::add-line:vertical {"
                    "    height: 0px;"
                    "    subcontrol-position: bottom;"
                    "    subcontrol-origin: margin;"
                                  "border-radius: 5px;"
                      "margin: 7px;"
                    "}"
                    "QScrollBar::sub-line:vertical {"
                    "    height: 0px;"
                    "    subcontrol-position: top;"
                    "    subcontrol-origin: margin;"
                                  "border-radius: 5px;"
                      "margin: 7px;"
                    "}"
       ));

    ui->pFileListWdgt->verticalScrollBar()->setStyleSheet(ui->pDirTreeWdgt->verticalScrollBar()->styleSheet());

    connect(ui->pDirTreeWdgt,SIGNAL(activated(const QModelIndex &)),this,SLOT(listTiffFilesInCurrentDir(const QModelIndex&)));
    connect(ui->pFileListWdgt,SIGNAL(itemPressed(QListWidgetItem *)),this,SLOT(itemActivated(QListWidgetItem *)));
}

xfImportDlg::~xfImportDlg()
{
    delete ui;
}

void xfImportDlg::setCurrentFile(const QString &fname)
{
    QFileSystemModel *model = dynamic_cast<QFileSystemModel*>(ui->pDirTreeWdgt->model());
    ui->pDirTreeWdgt->expand(model->index(fname));
    ui->pDirTreeWdgt->setCurrentIndex(model->index(fname));
    ui->pDirTreeWdgt->scrollTo(model->index(fname));
}

void xfImportDlg::listTiffFilesInCurrentDir(const QModelIndex& index)
{
    QFileSystemModel *pModel = dynamic_cast<QFileSystemModel*>(ui->pDirTreeWdgt->model());
    QFileInfo info = pModel->fileInfo(index);

    ui->pFileListWdgt->clear();
    QDir d(info.absoluteFilePath());
    d.setFilter(QDir::Files);
    d.setNameFilters(QStringList() << "*.tiff" << "*.tif" << "*.TIF" << "*.TIFF");

    QFileInfoList list=d.entryInfoList();
    for (QFileInfoList::iterator it=list.begin();it!=list.end();++it)
    {
        if (it->isFile())
        {
            QListWidgetItem *pItem = new QListWidgetItem(it->fileName());
            pItem->setData(Qt::UserRole,it->absoluteFilePath());
            ui->pFileListWdgt->addItem(pItem);
        }
    }

    ui->pImportTB->setEnabled(ui->pFileListWdgt->selectedItems().count()>0);

}

void xfImportDlg::accept()
{
    if (ui->pFileListWdgt->selectedItems().count()>0)
    {
        QListWidgetItem *pItem = ui->pFileListWdgt->selectedItems().at(0);
        QString fileName = pItem->data(Qt::UserRole).toString();
        emit selectedTIFFile(fileName);
    }

    QDialog::accept();
}

void xfImportDlg::itemActivated(QListWidgetItem *item)
{
    ui->pImportTB->setEnabled(item!=nullptr);
}

void xfImportDlg::show()
{
    ui->pFileListWdgt->clear();
    ui->pImportTB->setEnabled(false);
}