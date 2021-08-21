#pragma once
#include <qdockwidget.h>

class QTreeWidget;
class QTreeWidgetItem;

struct WorldDock : public QDockWidget
{
	QTreeWidget* actorTreeWidget;

	WorldDock();
	void PopulateWorldActorList();
	void ClickOnActorInList(QTreeWidgetItem* item, int column);
	void ActorNameChanged(QTreeWidgetItem* item, int column);
};
