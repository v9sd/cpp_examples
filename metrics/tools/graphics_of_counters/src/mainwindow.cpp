#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QPair>

#include <non_accurate_counter.h>
#include <accurate_counter.h>


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
	updateMaxValueDiff();

	qRegisterMetaType<PointsGenerator::PlotData>("PointsGenerator::PlotData");


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

			std::chrono::steady_clock::time_point current_time = std::chrono::steady_clock::now();
			std::chrono::steady_clock::duration interval = std::chrono::seconds(1);

			PointsGenerator * data_generator = new PointsGenerator({std::shared_ptr<PointsGenerator::AbstractCounter>(
										new PointsGenerator::CurrentCounter<AccurateCounter>(interval, current_time)),
									       std::shared_ptr<PointsGenerator::AbstractCounter>(
										new PointsGenerator::CurrentCounter<NonAccurateCounter>(interval, current_time))});
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

	const QPair<double, double> range_default = {std::numeric_limits<double>::max(), std::numeric_limits<double>::min()};

	QPair<double, double> range_value = range_default;
	QPair<double, double> range_key   = range_default;
	for(int i = 0; i < data.graphics_data.size(); ++i){
		QCPGraph * graph = ui->customPlot->graph(i);
		if(graph == nullptr){
			graph = ui->customPlot->addGraph();
			graph->setLineStyle(QCPGraph::lsLine);
			graph->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssNone, 5));
			QPen pen;
			pen.setStyle(Qt::SolidLine);
			pen.setWidth(3);
			pen.setColor(QColor(qSin(i)*80+80, qSin(i+M_PI)*80+80, qSin(i*0.3+1.5)*80+80));
			graph->setPen(pen);

		}
		graph->addData(data.graphics_data[i].key, data.graphics_data[i].value);
		bool found_range;
		QCPRange range = graph->data()->valueRange(found_range);
		if(found_range){
			range_value.first = std::min(range.lower, range_value.first);
			range_value.second = std::max(range.upper, range_value.second);
		}
		range = graph->data()->keyRange(found_range);
		if(found_range){
			range_key.first = std::min(range.lower, range_key.first);
			range_key.second = std::max(range.upper, range_key.second);
		}

		graph->rescaleAxes(true);
	}

	if(range_key != range_default){
		ui->customPlot->xAxis->setRange(range_key.first, range_key.second);
	}
	if(range_value != range_default){
		ui->customPlot->yAxis->setRange(range_value.first, range_value.second);
	}
	ui->customPlot->replot();
	updateMaxValueDiff();
}

void MainWindow::updateMaxValueDiff(){
	static const QString prefix = "max diff";
	QString str_val;
	if(mav_value_diff.size() <= 1){
		str_val = ":-";
	}else{
		for(int i = 1; i < mav_value_diff.size(); ++i){
			str_val += ":";
			str_val += QString::number(mav_value_diff[i]);
		}
	}
	ui->maximum_difference->setText(prefix + str_val);
}


void MainWindow::generatorDeleted(){
	points_generator_thread.wait();
	ui->customPlot->clearGraphs();
	ui->customPlot->replot();
	setDefaultBtnState();
	mav_value_diff.clear();
	updateMaxValueDiff();
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
