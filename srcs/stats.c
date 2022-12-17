#include "ft_ping.h"

void set_transmitted() {
	t_stats *ctrs;

	ctrs = &g_ping->counters;
	ctrs->transmitted++;
}

void set_recieved() {
	t_stats *ctrs;

	ctrs = &g_ping->counters;
	ctrs->recieved++;
	set_loss();
}

void set_loss() {
	t_stats *ctrs;
	float ratio;

	ctrs = &g_ping->counters;
	ctrs->loss = ctrs->transmitted - ctrs->recieved;
	ratio = (float)ctrs->loss / ctrs->transmitted;
	ctrs->loss_percent = ratio * 100;
}
