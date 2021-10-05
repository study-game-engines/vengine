#include "AssetDock.h"
#include <qmenu.h>
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
#include "PropertiesDock.h"
#include "Render/TextureSystem.h"
#include "Render/Material.h"

namespace Icons
{
    QPixmap* play;
    QPixmap* font;
    QPixmap* icon;
    QPixmap* lib;
    QPixmap* mesh;
    QPixmap* world;
    QPixmap* code;
    QPixmap* material;
    QPixmap* dialogue;
}

AssetDock::AssetDock() : QDockWidget("Assets")
{
    this->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, &QWidget::customContextMenuRequested, this, &AssetDock::ShowContextMenu);

    //Set icons
    Icons::play = new QPixmap("Icons/play_icon.png");
    Icons::font = new QPixmap("Icons/font_icon.png");
    Icons::icon = new QPixmap("Icons/icon_icon.png");
    Icons::lib = new QPixmap("Icons/lib_icon.png");
    Icons::mesh = new QPixmap("Icons/mesh_icon.png");
    Icons::world = new QPixmap("Icons/world_icon.png");
    Icons::code = new QPixmap("Icons/code_icon.png");
    Icons::material = new QPixmap("Icons/material_icon.png");
    Icons::dialogue = new QPixmap("Icons/dialogue_icon.png");

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
    connect(assetIcons, &QListWidget::doubleClicked, this, &AssetDock::OpenAssetItemInDefaultProgram);

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
        fileSystem.LoadWorld(assetName.toStdString());
    }   
    if (std::wcscmp(extension, L".mt") == 0) //Material files
    {
        MaterialFileClicked(fullPath.toStdString());
    }
    else if (std::wcscmp(extension, L".fbx") == 0) //FBX files
    {
        MeshFileClicked(assetName.toStdString());
    }
    else if (std::wcscmp(extension, L".jpg") == 0 ||
        std::wcscmp(extension, L".png") == 0)
    {
        TextureFileClicked(assetName.toStdWString());
    }
}

void AssetDock::OpenAssetItemInDefaultProgram()
{
    QModelIndex index = assetTreeView->currentIndex();
    QString path = fileSystemModel->filePath(index);

    QString assetName = assetIcons->currentItem()->text();
    QString fullPath = path + "/" + assetName;

    auto fileExtension = std::filesystem::path(fullPath.toStdString()).extension();
    auto extension = fileExtension.c_str();

    //Opens up default system program from filename.
    QDesktopServices::openUrl(QUrl::fromLocalFile(fullPath));
}

void AssetDock::AssetFolderClicked()
{
    QModelIndex index = assetTreeView->currentIndex();
    QString path = fileSystemModel->filePath(index);

    QDir directory(path);
    QStringList list = directory.entryList(QDir::NoDotAndDotDot | QDir::AllEntries);

    assetIcons->clear();

    auto fileExtension = std::filesystem::path(path.toStdString()).extension();
    auto extension = fileExtension.c_str();

    for (int i = 0; i < list.count(); i++)
    {
        QIcon icon;

        QString str = list[i];

        if (str.contains(".ttf"))
        {
            icon = *Icons::font;
        }
        else if (str.contains(".fbx"))
        {
            icon = *Icons::mesh;
        }
        else if (str.contains(".lib") || str.contains(".dll"))
        {
            icon = *Icons::lib;
        }
        else if (str.contains(".sav"))
        {
            icon = *Icons::world;
        }
        else if (str.contains(".h") || str.contains(".cpp"))
        {
            icon = *Icons::code;
        } 
        else if (str.contains(".png") || str.contains(".jpg"))
        {
            icon = *Icons::icon;
        }       
        else if (str.contains(".mt"))
        {
            icon = *Icons::material;
        }         
        else if (str.contains(".dialog"))
        {
            icon = *Icons::dialogue;
        }     
        else
        {
            icon = *Icons::play;
        }

        QListWidgetItem* item = new QListWidgetItem(icon, list[i]);
        item->setSizeHint(QSize(100, 100));
        assetIcons->addItem(item);
    }
}

void AssetDock::ShowContextMenu(const QPoint& point)
{
    QMenu contextMenu("Context menu", this);

    QAction materialAction("New Material", this);
    connect(&materialAction, &QAction::triggered, this, &AssetDock::CreateMaterialFile);
    contextMenu.addAction(&materialAction);

    contextMenu.exec(mapToGlobal(point));
}

void AssetDock::MeshFileClicked(const std::string meshFilename)
{
    //Set spawner system as MeshActor
    worldEditor.spawnSystem = &MeshActor::system;
    MeshActor::spawnMeshFilename = meshFilename;
}

void AssetDock::TextureFileClicked(const std::wstring textureFilename)
{
    textureSystem.selectedTextureInEditor = textureFilename;
}

void AssetDock::MaterialFileClicked(const std::string materialFilename)
{
    editor->OpenMaterialEditor(materialFilename);
}

void AssetDock::CreateMaterialFile()
{
    Serialiser s("Materials/testmaterial.mt", OpenMode::Out);
    Material mat = Material("test.png", "DefaultShader.hlsl");
    s.Serialise(mat.GetProps());
}
