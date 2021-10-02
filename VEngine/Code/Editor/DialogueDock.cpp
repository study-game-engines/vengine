#include "DialogueDock.h"
#include <fstream>
#include <qtreewidget.h>
#include <qstandarditemmodel.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qfiledialog.h>
#include <qcombobox.h>
#include <qjsonobject.h>
#include <qjsondocument.h>
#include "World.h"
#include "Actors/Actor.h"

//columns for QTreeWidget
const int lineColumn = 0;
const int gotoColumn = 1;
const int actorColumn = 2;
const int intuitionColumn = 3;
const int textColumn = 4;

DialogueDock::DialogueDock() : QDockWidget("Dialogue")
{
	//TREE WIDGET
	dialogueTree = new QTreeWidget(); 
	dialogueTree->setColumnCount(5);
	dialogueTree->setHeaderLabels({ "Line", "Goto", "Actor", "Intuition", "Text"});


	//BUTTONS
	auto insertDialogueLineButton = new QPushButton("Insert Line");
	connect(insertDialogueLineButton, &QPushButton::clicked, this, &DialogueDock::AddEmptyDialogueLine);

	auto deleteLineButton = new QPushButton("Delete");
	connect(deleteLineButton, &QPushButton::clicked, this, &DialogueDock::DeleteLine);

	auto saveButton = new QPushButton("Save");
	saveButton->setFixedWidth(50);
	connect(saveButton, &QPushButton::clicked, this, &DialogueDock::SaveDialogueToFile);

	auto loadButton = new QPushButton("Load");
	loadButton->setFixedWidth(50);
	connect(loadButton, &QPushButton::clicked, this, &DialogueDock::LoadDialogueFile);

	//BUTTONS HORIZONTAL LAYOUT
	auto hLayout = new QHBoxLayout();
	hLayout->addWidget(insertDialogueLineButton);
	hLayout->addWidget(deleteLineButton);
	hLayout->addWidget(saveButton);
	hLayout->addWidget(loadButton);

	//WIDGET LAYOUT
	auto vLayout = new QVBoxLayout();
	vLayout->addLayout(hLayout);
	vLayout->addWidget(dialogueTree);

	auto dialogueWidget = new QWidget();
	dialogueWidget->setLayout(vLayout);

	setWidget(dialogueWidget);
}

void DialogueDock::PopulateTreeItem(QTreeWidgetItem* item)
{
	int index = dialogueTree->indexOfTopLevelItem(item);
	item->setText(0, QString::number(index));
	item->setFlags(item->flags() | Qt::ItemIsEditable);

	//'Next' is the default progression
	item->setText(gotoColumn, "next");

	//Grab all actors in world and add their names to a combobox
	auto actorComboBox = new QComboBox(this);
	auto actorsInWorld = world.GetAllActorsInWorld();
	for (auto actor : actorsInWorld)
	{
		QString actorName = QString::fromStdString(actor->name);
		actorComboBox->addItem(actorName);
	}

	dialogueTree->setItemWidget(item, actorColumn, actorComboBox);
}

void DialogueDock::AddEmptyDialogueLine()
{
	auto selectedLine = dialogueTree->currentItem();
	auto item = new QTreeWidgetItem(dialogueTree, selectedLine);
	PopulateTreeItem(item);

	//refresh all the line values
	QTreeWidgetItemIterator it(dialogueTree);
	int lineIndex = 0;
	while (*it)
	{
		(*it)->setText(lineColumn, QString::number(lineIndex));
		lineIndex++;
		it++;
	}
}

void DialogueDock::DeleteLine()
{
	auto selectedLine = dialogueTree->currentItem();
	int index = dialogueTree->indexOfTopLevelItem(selectedLine);
	dialogueTree->takeTopLevelItem(index);
}

void DialogueDock::SaveDialogueToFile()
{
	QFileDialog saveDialog;
	QString saveName = saveDialog.getSaveFileName(this, "Save Dialogue", "Dialogues/");

	std::ofstream os;
	os.open(saveName.toStdString(), std::ios_base::out);

	QTreeWidgetItemIterator it(dialogueTree);
	while (*it) 
	{
		auto lineText = (*it)->text(lineColumn);
		auto gotoText = (*it)->text(gotoColumn);

		//Have to do a bit more to get the text from the combobox
		QComboBox* actorComboBox = (QComboBox*)dialogueTree->itemWidget(*it, actorColumn);

		auto intuitionText = (*it)->text(intuitionColumn);
		auto text = (*it)->text(textColumn);

		os << lineText.toStdString() << "\n";
		os << gotoText.toStdString() << "\n";
		os << actorComboBox->currentText().toStdString() << "\n";
		os << intuitionText.toStdString() << "\n";
		os << text.toStdString() << "\n";

		it++;
	}

	os.close();
}

void DialogueDock::LoadDialogueFile()
{
	QFileDialog loadDialog;
	QString loadName = loadDialog.getOpenFileName(this, "Open Dialogue", "Dialogues/");

	std::ifstream is;
	is.open(loadName.toStdString(), std::ios_base::in);

	dialogueTree->clear();


	while (!is.eof())
	{
		//Get all text
		std::string lineText;
		std::string gotoText;
		std::string actorText;
		std::string intuitionText;
		std::string text;

		char line[1024];

		is.getline(line, 1024); 
		lineText.assign(line);
		if (lineText.empty())
		{
			break;
		}
		
		is.getline(line, 1024); 
		gotoText.assign(line);
		
		is.getline(line, 1024); 
		actorText.assign(line);

		is.getline(line, 1024);
		intuitionText.assign(line);

		is.getline(line, 1024);
		text.assign(line);		
		
		//Populate widget items
		auto item = new QTreeWidgetItem(dialogueTree);
		PopulateTreeItem(item);

		item->setText(lineColumn, QString::fromStdString(lineText));
		item->setText(gotoColumn, QString::fromStdString(gotoText));
		item->setText(intuitionColumn, QString::fromStdString(intuitionText));
		item->setText(textColumn, QString::fromStdString(text));

		//Find the matching existing entry in the combobox and set it per the index
		QComboBox* actorComboBox = (QComboBox*)dialogueTree->itemWidget(item, actorColumn);

		//findText() returns -1 if nothing is found and will place an empty entry in the combobox.
		//Have to be careful here on the findText() as well. QStrings work a bit funny with '\n' and '\r' I'm guessing.
		QString actorStr = QString::fromStdString(actorText);
		int foundComboEntryIndex = actorComboBox->findText(actorStr);
		actorComboBox->setCurrentIndex(foundComboEntryIndex);
	}

	is.close();
}
