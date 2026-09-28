#include "cbapi.h"

#include <stdlib.h>
#include <string.h>

#include "biomes.h"
#include "finders.h"
#include "generator.h"

typedef struct
{
    int x, z;
} Item;

static int g_cx, g_cz;

static int cmpItem(const void *a, const void *b)
{
    const Item *ia = (const Item *) a;
    const Item *ib = (const Item *) b;
    long long dxa = (long long) ia->x - g_cx;
    long long dza = (long long) ia->z - g_cz;
    long long dxb = (long long) ib->x - g_cx;
    long long dzb = (long long) ib->z - g_cz;
    long long da = dxa * dxa + dza * dza;
    long long db = dxb * dxb + dzb * dzb;
    return (da > db) - (da < db);
}

static int structureId(const char *name)
{
    if (!name) return -1;
    if (!strcmp(name, "desert_pyramid"))      return Desert_Pyramid;
    if (!strcmp(name, "jungle_temple"))       return Jungle_Temple;
    if (!strcmp(name, "swamp_hut"))           return Swamp_Hut;
    if (!strcmp(name, "igloo"))               return Igloo;
    if (!strcmp(name, "village"))             return Village;
    if (!strcmp(name, "ocean_ruin"))          return Ocean_Ruin;
    if (!strcmp(name, "shipwreck"))           return Shipwreck;
    if (!strcmp(name, "monument"))            return Monument;
    if (!strcmp(name, "mansion"))             return Mansion;
    if (!strcmp(name, "pillager_outpost"))    return Outpost;
    if (!strcmp(name, "ruined_portal"))       return Ruined_Portal;
    if (!strcmp(name, "ruined_portal_nether"))return Ruined_Portal_N;
    if (!strcmp(name, "ancient_city"))        return Ancient_City;
    if (!strcmp(name, "buried_treasure"))     return Treasure;
    if (!strcmp(name, "mineshaft"))           return Mineshaft;
    if (!strcmp(name, "desert_well"))         return Desert_Well;
    if (!strcmp(name, "geode"))               return Geode;
    if (!strcmp(name, "fortress"))            return Fortress;
    if (!strcmp(name, "bastion"))             return Bastion;
    if (!strcmp(name, "end_city"))            return End_City;
    if (!strcmp(name, "trail_ruins"))         return Trail_Ruins;
    if (!strcmp(name, "trial_chambers"))      return Trial_Chambers;
    return -1;
}

int cb_find_strongholds(int mc, uint64_t seed, int centerX, int centerZ,
                        int limit, int *out)
{
    if (limit <= 0) return 0;

    Generator g;
    setupGenerator(&g, mc, 0);
    applySeed(&g, DIM_OVERWORLD, seed);

    StrongholdIter sh;
    initFirstStronghold(&sh, mc, seed);

    int max = 256;
    Item *items = (Item *) malloc(sizeof(Item) * max);
    if (!items) return -1;

    g_cx = centerX;
    g_cz = centerZ;

    int n = 0;
    int more = nextStronghold(&sh, &g);
    while (n < max) {
        items[n].x = sh.pos.x;
        items[n].z = sh.pos.z;
        n++;
        if (more <= 0) break;
        more = nextStronghold(&sh, &g);
    }

    qsort(items, n, sizeof(Item), cmpItem);
    int k = n < limit ? n : limit;
    for (int i = 0; i < k; i++) {
        out[2 * i] = items[i].x;
        out[2 * i + 1] = items[i].z;
    }
    free(items);
    return k;
}

int cb_find_structures(const char *structure, int mc, uint64_t seed,
                       int centerX, int centerZ, int radiusBlocks,
                       int limit, int *out)
{
    if (!structure || limit <= 0) return 0;
    if (!strcmp(structure, CB_STRONGHOLD_NAME))
        return cb_find_strongholds(mc, seed, centerX, centerZ, limit, out);

    int stype = structureId(structure);
    if (stype < 0) return -1;

    StructureConfig sc;
    if (!getStructureConfig(stype, mc, &sc)) return -1;

    int regionBlocks = sc.regionSize * 16;
    if (regionBlocks <= 0) return -1;

    int baseX = centerX / regionBlocks;
    int baseZ = centerZ / regionBlocks;
    int r = radiusBlocks / regionBlocks + 1;
    if (r > 128) r = 128;

    int max = 8192;
    Item *items = (Item *) malloc(sizeof(Item) * max);
    if (!items) return -1;

    g_cx = centerX;
    g_cz = centerZ;

    int n = 0;
    Pos p;
    for (int rx = baseX - r; rx <= baseX + r; rx++) {
        for (int rz = baseZ - r; rz <= baseZ + r; rz++) {
            if (getStructurePos(stype, mc, seed, rx, rz, &p)) {
                if (n < max) {
                    items[n].x = p.x;
                    items[n].z = p.z;
                    n++;
                }
            }
        }
    }

    qsort(items, n, sizeof(Item), cmpItem);
    int k = n < limit ? n : limit;
    for (int i = 0; i < k; i++) {
        out[2 * i] = items[i].x;
        out[2 * i + 1] = items[i].z;
    }
    free(items);
    return k;
}

int cb_get_structure_pos(const char *structure, int mc, uint64_t seed,
                         int regX, int regZ, int *outX, int *outZ)
{
    int stype = structureId(structure);
    if (stype < 0) return 0;
    Pos p;
    if (!getStructurePos(stype, mc, seed, regX, regZ, &p)) return 0;
    if (outX) *outX = p.x;
    if (outZ) *outZ = p.z;
    return 1;
}

int cb_version_id(const char *name)
{
    if (!name) return -1;
    if (!strcmp(name, "1.21.11") || !strcmp(name, "1.21.10") || !strcmp(name, "1.21.9")
        || !strcmp(name, "1.21.8") || !strcmp(name, "1.21.7") || !strcmp(name, "1.21.6")
        || !strcmp(name, "1.21.5"))
        return MC_1_21_11;
    if (!strcmp(name, "1.21.4")) return MC_1_21_4;
    if (!strcmp(name, "1.21.3") || !strcmp(name, "1.21.2")) return MC_1_21_3;
    if (!strcmp(name, "1.21.1") || !strcmp(name, "1.21")) return MC_1_21_1;
    if (!strcmp(name, "1.20")) return MC_1_20;
    if (!strcmp(name, "1.19")) return MC_1_19;
    if (!strcmp(name, "1.18")) return MC_1_18;
    if (!strcmp(name, "1.17")) return MC_1_17;
    if (!strcmp(name, "1.16")) return MC_1_16;
    return -1;
}

int cb_newest_version(void)
{
    return MC_NEWEST;
}
