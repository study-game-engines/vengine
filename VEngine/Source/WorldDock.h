#pragma once

#include <qdockwidget.h>
#include <qlistwidget.h>
#include <qlineedit.h>
#include <qstringlist.h>
#include <unordered_map>

class ActorSystem;

class WorldDock : public QDockWidget
{
public: 
	WorldDock(const char* title);
	void Tick();

    void PopulateActorSystemList();
    void PopulateWorldList();
    void ClickOnActor(class QTreeWidgetItem* listItem);
    void ClickOnActorSystem(class QListWidgetItem* listItem);
    void SearchWorldList();

    QListWidget* actorSystemList;
    QLineEdit* worldSearch;
    QStringList worldStringList;
};
