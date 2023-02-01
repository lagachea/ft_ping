#include "ft_ping.h"

void setTransmitted() {
	g_ping->counters.transmitted++;
}

void setRecieved() {
	g_ping->counters.recieved++;
	setLoss();
}

void setLoss() {
	t_stats *ctrs;
	float ratio;

	ctrs = &g_ping->counters;
	ctrs->diff = ctrs->transmitted - ctrs->recieved;
	ratio = (float)ctrs->diff / ctrs->transmitted;
	ctrs->loss_percent = ratio * 100;
}

void updateStatistics() {
	t_stats *ctrs;
	double diff;
	
	ctrs = &g_ping->counters;
	diff = g_ping->time.diff_ms;

	if (ctrs->min > diff || ctrs->min == 0)
		ctrs->min = diff;
	if (ctrs->max < diff || ctrs->max == 0)
		ctrs->max = diff;
	ctrs->sum += diff;
	ctrs->sum2 += diff * diff;
	ctrs->avg = ctrs->sum / ctrs->recieved;
	ctrs->mdev = sqrt((ctrs->sum2 / ctrs->recieved) - (ctrs->avg * ctrs->avg));
}
