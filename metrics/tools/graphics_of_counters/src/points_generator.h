#ifndef POINTS_GENERATOR_H
#define POINTS_GENERATOR_H

#include <QObject>

#include <chrono>
#include <memory>

class PointsGenerator : public QObject{
		Q_OBJECT
	public:
		class AbstractCounter{
			public:
				virtual uint64_t operator()(uint64_t amount,
							    std::chrono::steady_clock::time_point current_time) = 0;
		};

		template<class T>
		class CurrentCounter: public AbstractCounter{
			public:
				CurrentCounter(std::chrono::steady_clock::duration interval = std::chrono::seconds(1),
					       std::chrono::steady_clock::time_point current_time = std::chrono::steady_clock::now());
				uint64_t operator()(uint64_t amount,
						    std::chrono::steady_clock::time_point current_time) override;
			private:
				T _counter;
		};


		PointsGenerator(std::initializer_list<std::shared_ptr<AbstractCounter> > counters, QObject *parent = nullptr);
		struct PlotData{
				struct GraphicData{

						GraphicData(QVector<double>::size_type max_data_amount) : key(max_data_amount, 0), value(max_data_amount, 0) {}
						QVector<double> key;
						QVector<double> value;
						uint64_t diff = 0;
				};
				PlotData() = default;
				PlotData(QVector<GraphicData>::size_type graphics_amount,
					 QVector<double>::size_type max_data_amount) :
					graphics_data(graphics_amount, GraphicData(max_data_amount)) {}

				QVector<GraphicData> graphics_data;
		};

	public slots:
		void pause();
		void start();
		void stop();
	signals:
		void newPoint(PointsGenerator::PlotData);
		void paused();
		void started();
		void stoped();
	protected:
		void timerEvent(QTimerEvent *event) override;
		void stopTimer();
	private:
		QVector<std::shared_ptr<AbstractCounter> > _counters;
		int timer_id = 0;
};


template<class T>
PointsGenerator::CurrentCounter<T>::CurrentCounter(std::chrono::steady_clock::duration interval,
						   std::chrono::steady_clock::time_point current_time
						   ) : _counter(interval, current_time) {}

template<class T>
uint64_t PointsGenerator::CurrentCounter<T>::operator()(uint64_t amount,
							std::chrono::steady_clock::time_point current_time){
	return _counter.add(amount, current_time);
}




#endif
