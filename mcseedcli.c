/*
 * Small command line front-end for Cubiomes, used to verify structure and
 * stronghold positions for a given world seed.
 *
 * Build:
 *   make libcubiomes
 *   cc -O2 -o mcseedcli mcseedcli.c -L. -lcubiomes -lm
 *
 * Usage:
 *   mcseedcli stronghold <seed> [version]
 *   mcseedcli <structure> <seed> <centerX> <centerZ> <radiusBlocks> [version]
 *
 * Examples:
 *   mcseedcli stronghold 12 1.21.11
 *   mcseedcli village 12 -49869 -13702 2000 1.21.11
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "finders.h"
#include "generator.h"
#include "biomes.h"

static int versionId(const char *s)
{
    if (!s || !*s) return MC_1_21_11;
    if (!strcmp(s, "1.21.11") || !strcmp(s, "1.21.10") || !strcmp(s, "1.21.9")
        || !strcmp(s, "1.21.8") || !strcmp(s, "1.21.7") || !strcmp(s, "1.21.6")
        || !strcmp(s, "1.21.5"))
        return MC_1_21_11;
    if (!strcmp(s, "1.21.4")) return MC_1_21_4;
    if (!strcmp(s, "1.21.3") || !strcmp(s, "1.21.2")) return MC_1_21_3;
    if (!strcmp(s, "1.21.1") || !strcmp(s, "1.21")) return MC_1_21_1;
    if (!strcmp(s, "1.20")) return MC_1_20;
    if (!strcmp(s, "1.19")) return MC_1_19;
    if (!strcmp(s, "1.18")) return MC_1_18;
    if (!strcmp(s, "1.17")) return MC_1_17;
    if (!strcmp(s, "1.16")) return MC_1_16;
    return atoi(s);
}

static int structureId(const char *name)
{
    if (!strcmp(name, "village"))          return Village;
    if (!strcmp(name, "desert_pyramid"))   return Desert_Pyramid;
    if (!strcmp(name, "jungle_temple"))    return Jungle_Temple;
    if (!strcmp(name, "swamp_hut"))        return Swamp_Hut;
    if (!strcmp(name, "igloo"))            return Igloo;
    if (!strcmp(name, "ocean_ruin"))       return Ocean_Ruin;
    if (!strcmp(name, "shipwreck"))        return Shipwreck;
    if (!strcmp(name, "monument"))         return Monument;
    if (!strcmp(name, "mansion"))          return Mansion;
    if (!strcmp(name, "pillager_outpost")) return Outpost;
    if (!strcmp(name, "ruined_portal"))    return Ruined_Portal;
    if (!strcmp(name, "ruined_portal_nether")) return Ruined_Portal_N;
    if (!strcmp(name, "ancient_city"))     return Ancient_City;
    if (!strcmp(name, "buried_treasure"))  return Treasure;
    if (!strcmp(name, "mineshaft"))        return Mineshaft;
    if (!strcmp(name, "desert_well"))      return Desert_Well;
    if (!strcmp(name, "geode"))            return Geode;
    if (!strcmp(name, "fortress"))         return Fortress;
    if (!strcmp(name, "bastion"))          return Bastion;
    if (!strcmp(name, "end_city"))         return End_City;
    if (!strcmp(name, "trail_ruins"))      return Trail_Ruins;
    if (!strcmp(name, "trial_chambers"))   return Trial_Chambers;
    return -1;
}

int main(int argc, char **argv)
{
    if (argc < 3) {
        fprintf(stderr,
            "usage: mcseedcli stronghold <seed> [version]\n"
            "       mcseedcli <structure> <seed> <centerX> <centerZ> <radiusBlocks> [version]\n");
        return 1;
    }

    const char *cmd = argv[1];
    uint64_t seed = strtoull(argv[2], NULL, 10);

    if (!strcmp(cmd, "stronghold")) {
        int mc = versionId(argc >= 4 ? argv[3] : NULL);
        StrongholdIter sh;
        Generator g;
        initFirstStronghold(&sh, mc, seed);
        setupGenerator(&g, mc, 0);
        applySeed(&g, DIM_OVERWORLD, seed);
        int i = 0;
        while (nextStronghold(&sh, &g) > 0 && i < 200) {
            printf("stronghold %d %d %d\n", i, sh.pos.x, sh.pos.z);
            i++;
        }
        printf("total %d\n", i);
        return 0;
    }

    int stype = structureId(cmd);
    if (stype < 0) {
        fprintf(stderr, "unknown structure: %s\n", cmd);
        return 1;
    }
    if (argc < 6) {
        fprintf(stderr, "need <centerX> <centerZ> <radiusBlocks>\n");
        return 1;
    }
    int cx = atoi(argv[3]);
    int cz = atoi(argv[4]);
    int radius = atoi(argv[5]);
    int mc = versionId(argc >= 7 ? argv[6] : NULL);

    StructureConfig sc;
    if (!getStructureConfig(stype, mc, &sc)) {
        fprintf(stderr, "no structure config for this version\n");
        return 1;
    }

    int regionBlocks = sc.regionSize * 16;
    int baseX = cx / regionBlocks;
    int baseZ = cz / regionBlocks;
    int r = radius / regionBlocks + 1;

    Pos p;
    int count = 0;
    for (int rx = baseX - r; rx <= baseX + r; rx++) {
        for (int rz = baseZ - r; rz <= baseZ + r; rz++) {
            if (getStructurePos(stype, mc, seed, rx, rz, &p)) {
                printf("%s %d %d %d\n", cmd, count++, p.x, p.z);
            }
        }
    }
    printf("total %d\n", count);
    return 0;
}
