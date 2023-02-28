#include <criterion/criterion.h>
 #include "ft_ping.h"

t_ftping pingdata;
t_ftping *g_ping = &pingdata;

Test(suite1, failing, .description = "Just a failing test") {
    cr_assert(0);
}

Test(suite1, passing, .description = "Just a passing test") {
    cr_assert(1);
}
