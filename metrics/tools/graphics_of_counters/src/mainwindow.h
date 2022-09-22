#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <qt5/QtWidgets/QMainWindow>
#include <QThread>
#include "points_generator.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
		Q_OBJECT

	public:
		MainWindow(QWidget *parent = nullptr);
		~MainWindow();
	signals:
		void start();
		void pause();
		void stop();
	private slots:
		void btnPressed();
		void addData(PointsGenerator::PlotData data);
		void generatorDeleted();
		void generatorStarted();
		void generatorPaused();
	private: //methods
		void setDefaultBtnState();

		void startThread();
		void stopThread();

		void updateMaxValueDiff();
	private: //properties
		Ui::MainWindow *ui;
		QThread points_generator_thread;

		QVector<uint64_t> mav_value_diff;
};
#endif // MAINWINDOW_H
