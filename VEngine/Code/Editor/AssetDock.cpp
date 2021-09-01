#include "AssetDock.h"
#include <filesystem>
#include <qdockwidget.h>
#include <qfilesystemmodel.h>
#include <qtreeview.h>
#include <qlistwidget.h>
#include <QDesktopServices>
#include <qboxlayout.h>
#include <QUrl>
#include "FileSystem.h"
#include "Actors/MeshActor.h"
#include "Editor.h"
#include "WorldEditor.h"

AssetDock::AssetDock() : QDockWidget("Assets")
{
    fileSystemModel = new QFileSystemModel();
    fileSystemModel->setRootPath(QDir::currentPath());

    //Only show folders. Don't show parent folders (../ and ./)
    fileSystemModel->setFilter(QDir::Dirs | QDir::NoDotAndDotDot);

    assetTreeView = new QTreeView();
    assetTreeView->setModel(fileSystemModel);
    assetTreeView->setRootIndex(fileSystemModel->index(QDir::currentPath()));
    assetTreeView->setMaximumWidth(250);
    connect(assetTreeView, &QTreeView::clicked, this, &AssetDock::AssetFolderClicked);

    //Hide all columns except for 'Name' (Other columns mirror Win32 usuals. Date, Type, etc.)
    //Start on i = 1 as 'Name' is first column.

    for (int i = 1; i < fileSystemModel->columnCount(); i++)
    {
        assetTreeView->hideColumn(i);
    }

    assetIcons = new QListWidget();
    assetIcons->setIconSize(QSize(75, 75));
    assetIcons->setViewMode(QListView::ViewMode::IconMode);
    connect(assetIcons, &QListWidget::clicked, this, &AssetDock::AssetItemClicked);

    QHBoxLayout* assetHBox = new QHBoxLayout();
    assetHBox->addWidget(assetTreeView, Qt::AlignLeft);
    assetHBox->addWidget(assetIcons, Qt::AlignRight);

    QWidget* assetWidget = new QWidget();
    assetWidget->setLayout(assetHBox);

    setWidget(assetWidget);

    QFileSystemModel* fileModel = new QFileSystemModel();
    fileModel->setRootPath(QDir::currentPath());
}

void AssetDock::AssetItemClicked()
{
    QModelIndex index = assetTreeView->currentIndex();
    QString path = fileSystemModel->filePath(index);

    QString assetName = assetIcons->currentItem()->text();
    QString fullPath = path + "/" + assetName;

    auto fileExtension = std::filesystem::path(fullPath.toStdString()).extension();
    auto extension = fileExtension.c_str();

    //Load world
    if (std::wcscmp(extension, L".sav") == 0) //Map files
    {
        fileSystem.LoadWorld(fullPath.toStdString().c_str());
    }
    else if (std::wcscmp(extension, L".fbx") == 0) //FBX files
    {
        MeshFileClicked(assetName.toStdString());
    }
    else if (std::wcscmp(extension, L".at") == 0) //Actor Template
    {
        ActorTemplateClicked(fullPath.toStdString());
    }
    else
    {
        //Opens up default system program from filename.
        QDesktopServices::openUrl(QUrl::fromLocalFile(fullPath));
    }
}

void AssetDock::AssetFolderClicked()
{
    QModelIndex index = assetTreeView->currentIndex();
    QString path = fileSystemModel->filePath(index);

    QDir directory(path);
    QStringList list = directory.entryList(QDir::NoDotAndDotDot | QDir::AllEntries);

    assetIcons->clear();

    QPixmap iconImage = QPixmap("Icons/play_icon.png");
    QIcon icon = QIcon(iconImage);

    for (int i = 0; i < list.count(); i++)
    {
        QListWidgetItem* item = new QListWidgetItem(icon, list[i]);
        item->setSizeHint(QSize(100, 100));
        assetIcons->addItem(item);
    }
}

void AssetDock::MeshFileClicked(std::string meshFilename)
{
    //Set spawner system as MeshActor
    worldEditor.spawnSystem = &MeshActor::system;
    MeshActor::spawnMeshFilename = meshFilename;
}

void AssetDock::ActorTemplateClicked(std::string actorTemplateFilename)
{
    std::filebuf fb;
    fb.open(actorTemplateFilename.c_str(), std::ios_base::in);
    std::istream is(&fb);

    std::string actorSystemName;
    is >> actorSystemName;

    auto actorSystemIt = actorSystemCache.nameToSystemMap->find(actorSystemName);
    worldEditor.spawnSystem = actorSystemIt->second;
    actorSystemIt->second->actorTemplateFilename = actorTemplateFilename;
}
