#include "ft_ping.h"

void setTransmitted() {
	t_stats *ctrs;

	ctrs = &g_ping->counters;
	ctrs->transmitted++;
}

void setRecieved() {
	t_stats *ctrs;

	ctrs = &g_ping->counters;
	ctrs->recieved++;
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
	diff = g_ping->time.diff / 1000.0;

	if (ctrs->min > diff)
		ctrs->min = diff;
	if (ctrs->max < diff)
		ctrs->max = diff;
	ctrs->sum += diff;
	ctrs->sum2 += diff * diff;
	ctrs->avg = ctrs->sum / ctrs->recieved;
	ctrs->mdev = sqrt((ctrs->sum2 / ctrs->recieved) - (ctrs->avg * ctrs->avg));
}
