#pragma once

#include <QMainWindow>

/// MainWindow for the default ParaView application.
class ZelusViewMainWindow : public QMainWindow
{
	Q_OBJECT

public:
	ZelusViewMainWindow();
	~ZelusViewMainWindow() override;

protected:
	void dragEnterEvent(QDragEnterEvent* evt) override;
	void dropEvent(QDropEvent* evt) override;
	void showEvent(QShowEvent* evt) override;
	void closeEvent(QCloseEvent* evt) override;

protected Q_SLOTS:
	void handleMessage(const QString&, int);

private:
	Q_DISABLE_COPY(ZelusViewMainWindow)

		class Internals;
	Internals* mInternals;
};

