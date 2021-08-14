#pragma once
#include <QtWidgets/QMainWindow>

struct EditorMainWindow : public QMainWindow
{
	struct RenderViewWidget* renderView;
	struct PropertiesDock* propertiesDock;
	struct AssetDock* assetDock;
	struct WorldDock* worldDock;
	struct ToolbarDock* toolbarDock;
	struct LogDock* logDock;

	bool fullScreenOn = false;

	EditorMainWindow();
	void closeEvent(QCloseEvent* closeEvent) override;
	bool nativeEvent(const QByteArray& eventType, void* message, long* result) override;
	void HideAllDocks();
};
