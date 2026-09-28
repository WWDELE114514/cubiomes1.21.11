/* Smoke test for the flat cbapi wrapper. */
#include <stdio.h>

#include "cbapi.h"

int main(void)
{
    int out[64];
    int mc = cb_version_id("1.21.11");
    printf("mc id for 1.21.11 = %d (newest = %d)\n", mc, cb_newest_version());

    int n = cb_find_strongholds(mc, 12, 0, 0, 5, out);
    printf("strongholds: %d\n", n);
    for (int i = 0; i < n; i++)
        printf("  %d %d\n", out[2 * i], out[2 * i + 1]);

    const char *types[] = { "village", "end_city", "monument", "mansion", "bastion" };
    for (int t = 0; t < 5; t++) {
        n = cb_find_structures(types[t], mc, 12, 0, 0, 4000, 3, out);
        printf("%s: %d\n", types[t], n);
        for (int i = 0; i < n; i++)
            printf("  %d %d\n", out[2 * i], out[2 * i + 1]);
    }
    return 0;
}
