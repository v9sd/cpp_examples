#include "mainwindow.h"
#include "./ui_mainwindow.h"




MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
	, ui(new Ui::MainWindow)
{
	ui->setupUi(this);

	std::chrono::steady_clock::time_point start_point;
	double start_point_double = start_point.time_since_epoch().count();
	ui->customPlot->xAxis->setLabel("time");
	ui->customPlot->xAxis->setTicks(true);
	ui->customPlot->yAxis->setLabel("value");
	ui->customPlot->yAxis->setTicks(true);
	ui->customPlot->xAxis->setRange(start_point_double, start_point_double+1);
	ui->customPlot->yAxis->setRange(start_point_double, start_point_double+1);

	QObject::connect(ui->playButton, &QPushButton::clicked, this, &MainWindow::btnPressed, Qt::DirectConnection);
	QObject::connect(ui->pauseButton, &QPushButton::clicked, this, &MainWindow::btnPressed, Qt::DirectConnection);
	QObject::connect(ui->stopButton, &QPushButton::clicked, this, &MainWindow::btnPressed, Qt::DirectConnection);
	setDefaultBtnState();
	thread()->setPriority(QThread::Priority::HighPriority);
}

void MainWindow::setDefaultBtnState(){

	ui->playButton->setEnabled(true);
	ui->pauseButton->setEnabled(false);
	ui->stopButton->setEnabled(false);
}

MainWindow::~MainWindow()
{
	points_generator_thread.quit();
	points_generator_thread.wait();
	delete ui;
}

void MainWindow::btnPressed(){
	QObject * btn_sender = QObject::sender();
	ui->playButton->setEnabled(false);
	ui->pauseButton->setEnabled(false);
	ui->stopButton->setEnabled(false);
	if(static_cast<const QObject*>(ui->playButton) == btn_sender){
		if(!points_generator_thread.isRunning()){
			PointsGenerator * data_generator = new PointsGenerator(100);
			data_generator->moveToThread(&points_generator_thread);

			QObject::connect(this, &MainWindow::start, data_generator, &PointsGenerator::start, Qt::QueuedConnection);
			QObject::connect(this, &MainWindow::stop, data_generator, &PointsGenerator::stop, Qt::QueuedConnection);
			QObject::connect(this, &MainWindow::pause, data_generator, &PointsGenerator::pause, Qt::QueuedConnection);

			QObject::connect(data_generator, &PointsGenerator::paused, this, &MainWindow::generatorPaused, Qt::QueuedConnection);
			QObject::connect(data_generator, &PointsGenerator::started, this, &MainWindow::generatorStarted, Qt::QueuedConnection);
			QObject::connect(data_generator, &PointsGenerator::stoped, &points_generator_thread, &QThread::quit);

			QObject::connect(data_generator, &PointsGenerator::newPoint, this, &MainWindow::addData, Qt::QueuedConnection);

			QObject::connect(&points_generator_thread, &QThread::finished, data_generator, &QObject::deleteLater);
			QObject::connect(data_generator, &QObject::destroyed, this, &MainWindow::generatorDeleted, Qt::QueuedConnection);

			points_generator_thread.start();
		}
		emit start();
	}else if(static_cast<const QObject*>(ui->pauseButton) == btn_sender){
		if(points_generator_thread.isRunning()){
			emit pause();
		}
	}
	if(static_cast<const QObject*>(ui->stopButton) == btn_sender){
		if(points_generator_thread.isRunning()){
			emit stop();
		}
	}
}

void MainWindow::addData(PointsGenerator::PlotData data){
	for(int i = 0; i < data.size(); ++i){
		QCPGraph * graph = ui->customPlot->graph(i);
		if(graph == nullptr){
			graph = ui->customPlot->addGraph();
			graph->setLineStyle(QCPGraph::lsLine);
			graph->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssNone, 5));
			graph->setPen(QColor(qSin(i*1+1.2)*80+80, qSin(i*0.3+0)*80+80, qSin(i*0.3+1.5)*80+80));
		}
		graph->addData(data[i].first,data[i].second);
		graph->rescaleAxes(true);
	}
	ui->customPlot->replot();
}

void MainWindow::generatorDeleted(){
	points_generator_thread.wait();
	for(int i =0; i < ui->customPlot->graphCount(); ++i){
		ui->customPlot->graph(i)->setData({},{});
	}

//	ui->customPlot->xAxis->setRange(0, 1);
//	ui->customPlot->yAxis->setRange(0, 1);
	ui->customPlot->replot();
	setDefaultBtnState();
}

void MainWindow::generatorStarted(){
	ui->playButton->setEnabled(false);
	ui->pauseButton->setEnabled(true);
	ui->stopButton->setEnabled(true);
}

void MainWindow::generatorPaused(){
	ui->playButton->setEnabled(true);
	ui->pauseButton->setEnabled(false);
	ui->stopButton->setEnabled(true);
}
