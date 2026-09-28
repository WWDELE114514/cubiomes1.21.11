/*
 * Simple flat C API on top of Cubiomes, meant to be called from Java via JNA.
 * All structures are returned as pairs of block coordinates (x, z).
 */
#ifndef CBAPI_H
#define CBAPI_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Special structure name handled by the stronghold iterator. */
#define CB_STRONGHOLD_NAME "stronghold"

/*
 * Finds up to `limit` structures of the given type near (centerX, centerZ).
 * `structure` is a name such as "village", "end_city", "stronghold".
 * `out` must have room for 2*limit ints and receives x,z pairs sorted by
 * distance to the center.
 *
 * Returns the number of results, or -1 if the structure is unknown or has no
 * configuration for this version.
 */
int cb_find_structures(const char *structure, int mc, uint64_t seed,
                       int centerX, int centerZ, int radiusBlocks,
                       int limit, int *out);

/* Finds up to `limit` strongholds sorted by distance to (centerX, centerZ). */
int cb_find_strongholds(int mc, uint64_t seed, int centerX, int centerZ,
                        int limit, int *out);

/* Writes the position of the structure in one region. Returns 1 on success. */
int cb_get_structure_pos(const char *structure, int mc, uint64_t seed,
                         int regX, int regZ, int *outX, int *outZ);

/* Maps a version string such as "1.21.11" to the internal id, -1 if unknown. */
int cb_version_id(const char *name);

/* Internal id of the newest supported version. */
int cb_newest_version(void);

#ifdef __cplusplus
}
#endif

#endif
