#include "vpch.h"
#include "WorldDock.h"
#include "ActorTreeWidget.h"
#include <QTreeWidgetItem>
#include <qmenu.h>
#include "World.h"
#include <qboxlayout.h>
#include <qlineedit.h>
#include "Actors/IActorSystem.h"
#include "Actors/Actor.h"
#include "Camera.h"
#include "WorldEditor.h"
#include "Log.h"
#include "Editor.h"
#include "Input.h"

WorldDock::WorldDock() : QDockWidget("World")
{
	//Search bar
	actorSearchBar = new QLineEdit(this);
	actorSearchBar->setPlaceholderText("Search Actors...");
	connect(actorSearchBar, &QLineEdit::textChanged, this, &WorldDock::SearchActors);

	//Actor Tree widget
	actorTreeWidget = new ActorTreeWidget(this);
	actorTreeWidget->setColumnCount(1);
	actorTreeWidget->setHeaderLabels(QStringList("Actors"));
	actorTreeWidget->setSelectionMode(QAbstractItemView::SelectionMode::MultiSelection);
	//turn on sorting (click on column header to sort)
	actorTreeWidget->setSortingEnabled(true);
	actorTreeWidget->sortByColumn(0);

	actorTreeWidget->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(actorTreeWidget, &QTreeWidget::customContextMenuRequested, this, &WorldDock::ActorListContextMenu);

	connect(actorTreeWidget, &QTreeWidget::itemClicked, this, &WorldDock::ClickOnActorInList);
	connect(actorTreeWidget, &QTreeWidget::itemSelectionChanged, this, &WorldDock::ArrowSelectActorInList);
	connect(actorTreeWidget, &QTreeWidget::itemChanged, this, &WorldDock::ActorNameChanged);

	//Dock Layout
	auto vLayout = new QVBoxLayout(this);
	vLayout->addWidget(actorSearchBar);
	vLayout->addWidget(actorTreeWidget);

	auto worldWidget = new QWidget(this);
	worldWidget->setLayout(vLayout);

	setWidget(worldWidget);
}

void WorldDock::Tick()
{
	if (Input::GetAsyncKey(Keys::Ctrl))
	{
		actorListSelectionMode = QAbstractItemView::SelectionMode::MultiSelection;
	}
	else
	{
		actorListSelectionMode = QAbstractItemView::SelectionMode::SingleSelection;
	}

	actorTreeWidget->setSelectionMode(actorListSelectionMode);
}

void WorldDock::PopulateWorldActorList()
{
    actorTreeWidget->clear();

	//Need to block signals because calling functions on tree items makes the connect()ed event fire
	actorTreeWidget->blockSignals(true);

	//clear()s are here because these maps are added to in ActorSystem::Add() calls
	//but there's no way to refresh them before Deserialising data.
	world.actorNameMap.clear();
	world.actorUIDMap.clear();

	for (Actor* actor : world.GetAllActorsInWorld())
	{
		auto item = new QTreeWidgetItem(actorTreeWidget);
		item->setText(0, QString::fromStdString(actor->name));
		item->setFlags(item->flags() | Qt::ItemIsEditable);

		world.actorNameMap.emplace(actor->name, actor);
		world.actorUIDMap.emplace(actor->uid, actor);
	}

	actorTreeWidget->blockSignals(false);
}

void WorldDock::ClickOnActorInList(QTreeWidgetItem* item, int column)
{
	QString actorName = item->text(column);
	Actor* clickedActor = world.GetActorByName(actorName.toStdString());
	if (clickedActor)
	{
		worldEditor.pickedActor = clickedActor;

		if (actorListSelectionMode == QAbstractItemView::SelectionMode::MultiSelection)
		{
			worldEditor.pickedActors.insert(clickedActor);
		}
		else
		{
			worldEditor.pickedActors.clear();
		}

		editor->SetActorProps(clickedActor);
	}
}

void WorldDock::ArrowSelectActorInList()
{
	auto items = actorTreeWidget->selectedItems();
	if (!items.empty())
	{
		QString pickedActorName = items[0]->text(0);
		auto pickedActor = world.GetActorByName(pickedActorName.toStdString());
		worldEditor.pickedActor = pickedActor;
		editor->SetActorProps(pickedActor);
	}
}

void WorldDock::ActorNameChanged(QTreeWidgetItem* item, int column)
{
	QString newActorName = item->text(column);

	Actor* actor = worldEditor.pickedActor;
	assert(actor); //pickedActor should be set before this is hit in the other events

	if (!actor->SetName(newActorName.toStdString()))
	{
		Log("Could not change actor name from %s to %s. Name already exists.",
			actor->name.c_str(), newActorName.toStdString().c_str());

		//Reset item text
		item->setText(column, QString::fromStdString(actor->name));
	}
}

void WorldDock::ActorListContextMenu(const QPoint& pos)
{
	QPoint globalPos = actorTreeWidget->mapToGlobal(pos);

	QMenu actorListMenu;
	actorListMenu.addAction("Clear Selection", actorTreeWidget, &QTreeWidget::clearSelection);

	actorListMenu.exec(globalPos);
}

void WorldDock::SelectActorInList()
{
	actorTreeWidget->clearSelection();

	std::vector<std::string> actorNames;
	QList<QTreeWidgetItem*> listItems;

	for (auto actor : worldEditor.pickedActors)
	{
		std::string actorName = actor->name;
		auto foundItems = actorTreeWidget->findItems(QString::fromStdString(actorName), Qt::MatchExactly);

		//Names should be unique in list, even though findItems() returns a collection
		assert(foundItems.size() == 1);

		listItems.append(foundItems);
	}

	for (auto item : listItems)
	{
		actorTreeWidget->setItemSelected(item, true);
	}
}

void WorldDock::AddActorToList(Actor* actor)
{
	actorTreeWidget->blockSignals(true);

	auto item = new QTreeWidgetItem(actorTreeWidget);
	item->setText(0, QString::fromStdString(actor->name));
	item->setFlags(item->flags() | Qt::ItemIsEditable);

	world.actorNameMap.emplace(actor->name, actor);
	world.actorUIDMap.emplace(actor->uid, actor);

	actorTreeWidget->blockSignals(false);
}

void WorldDock::RemoveActorFromList()
{
	actorTreeWidget->blockSignals(true);

	QList<QTreeWidgetItem*> foundItems;
	for (auto actor : worldEditor.pickedActors)
	{
		std::string actorName = actor->name;
		foundItems = actorTreeWidget->findItems(QString::fromStdString(actorName), Qt::MatchExactly);
		assert(foundItems.size() == 1);

		world.actorNameMap.erase(actor->name);
		world.actorUIDMap.erase(actor->uid);
	}

	for (auto item : foundItems)
	{
		actorTreeWidget->removeItemWidget(item, 0);
	}

	actorTreeWidget->blockSignals(false);
}

//This is only working for single columns in the tree. If showing actor parenting through this list 
//later on, need to change this too in the item->text(int column) below.
void WorldDock::SearchActors()
{
	QString searchText = actorSearchBar->text().toLower();
	
	for (int i = 0; i < actorTreeWidget->topLevelItemCount(); i++)
	{
		QTreeWidgetItem* item = actorTreeWidget->topLevelItem(i);
		if (item->text(0).toLower().contains(searchText))
		{
			item->setHidden(false);
		}
		else
		{
			item->setHidden(true);
		}
	}
}