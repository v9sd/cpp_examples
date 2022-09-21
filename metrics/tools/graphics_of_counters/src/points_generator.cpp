#include "points_generator.h"

#include <QRandomGenerator>

#include <non_accurate_counter.h>
#include <accurate_counter.h>

PointsGenerator::PointsGenerator(uint32_t max_vec_size, QObject *parent)
	: QObject(parent),
	  _start_point(std::chrono::steady_clock::now()),
	  _accurate_counter(std::chrono::seconds(1), _start_point),
	  _non_accurate_counter(std::chrono::seconds(1), _start_point),
	  max_data_porion_size(max_vec_size < 1 ? 1 : max_vec_size) {}

QVector<QString> PointsGenerator::graphicNames(){
	return {"accurate", "non accurate"};
}

void PointsGenerator::pause(){
	stopTimer();
	emit paused();
}

void PointsGenerator::start(){
	stopTimer();
	timer_id = startTimer(250);
	emit started();
}

void PointsGenerator::stop(){
	stopTimer();
	emit stoped();
}

void PointsGenerator::timerEvent(QTimerEvent */*event*/) {
	uint32_t max_data_amount = 1;
	PlotData output;
	output.append({QVector<double>(max_data_amount, 0), QVector<double>(max_data_amount, 0)});
	output.append({QVector<double>(max_data_amount, 0), QVector<double>(max_data_amount, 0)});

	for(uint32_t i = 0; i < max_data_amount; ++i){
		uint64_t value = QRandomGenerator::system()->generate64() % 10;
		auto time_point = std::chrono::steady_clock::now();
		auto time_point_double = double(time_point.time_since_epoch().count());
		output[0].first[i] = time_point_double;
		output[0].second[i] = (uint64_t)_accurate_counter.add(value, time_point);

		output[1].first[i] = time_point_double;
		output[1].second[i] = (uint64_t)_non_accurate_counter.add(value, time_point);
	}
	emit newPoint(std::move(output));
}

void PointsGenerator::stopTimer(){
	if(timer_id){
		killTimer(timer_id);
	}
	timer_id = 0;
}
