#ifndef POINTS_GENERATOR_H
#define POINTS_GENERATOR_H

#include <QObject>

#include <non_accurate_counter.h>
#include <accurate_counter.h>

class PointsGenerator : public QObject{
		Q_OBJECT
	public:
		typedef QVector<QPair<QVector<double>, QVector<double> > > PlotData;
		PointsGenerator(uint32_t max_vec_size, QObject *parent = nullptr);
		QVector<QString> graphicNames();
	public slots:
		void pause();
		void start();
		void stop();
	signals:
		void newPoint(PlotData);
		void paused();
		void started();
		void stoped();
	protected:
		void timerEvent(QTimerEvent *event) override;
		void stopTimer();
	private:
		std::chrono::steady_clock::time_point _start_point;
		NonAccurateCounter     _accurate_counter;
		NonAccurateCounter _non_accurate_counter;

		const uint32_t max_data_porion_size;
		int timer_id = 0;
};


#endif
