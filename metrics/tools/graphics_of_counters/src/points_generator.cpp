#include "points_generator.h"

#include <QRandomGenerator>

#include <non_accurate_counter.h>
#include <accurate_counter.h>
#include <QMetaType>
#include <QSharedPointer>

PointsGenerator::PointsGenerator(std::initializer_list<std::shared_ptr<AbstractCounter> > counters, QObject *parent)
	: QObject(parent),
	  _counters(counters) {}

void PointsGenerator::pause(){
	stopTimer();
	emit paused();
}

void PointsGenerator::start(){
	stopTimer();
	timer_id = startTimer(113);
	emit started();
}

void PointsGenerator::stop(){
	stopTimer();
	emit stoped();
}

void PointsGenerator::timerEvent(QTimerEvent */*event*/) {
	uint32_t max_data_amount = 1;
	PointsGenerator::PlotData output(_counters.size(), max_data_amount);

	for(uint32_t i = 0; i < max_data_amount; ++i){
		uint64_t rand_v = QRandomGenerator::system()->generate64() % 300;
		auto time_point = std::chrono::steady_clock::now();
		auto time_point_double = double(time_point.time_since_epoch().count());

		for(int j = 0; j < _counters.size(); ++j){
			output.graphics_data[j].key[i] = time_point_double;
			output.graphics_data[j].value[i] = (*_counters[i])(rand_v, time_point);
		}
	}
	emit newPoint(std::move(output));
}

void PointsGenerator::stopTimer(){
	if(timer_id){
		killTimer(timer_id);
	}
	timer_id = 0;
}
