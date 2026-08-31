#include "world/entity/player.h"
#include "world/inventory/inventory.h"
#include "world/item/bonemeal_item.h"
#include "world/level/world.h"
#include "world/level/tile/tile_behavior.h"
#include <stdlib.h>

static bool grassWalk(World* w, int& x, int& y, int& z, int steps) {
    for (int i = 0; i < steps / 16; i++) {
        x += rand() % 3 - 1;

        y += (rand() % 3) * (rand() % 3 - 1) / 2;
        z += rand() % 3 - 1;
        if (worldBlock(w, x, y - 1, z) != BLOCK_GRASS || isOpaque(worldBlock(w, x, y, z)))
            return false;
    }
    return steps > 15;
}

static void bonemealGrass(World* w, int x, int y, int z) {
    for (int i = 16; i != 64; ++i) {
        int xx = x, yy = y + 1, zz = z;
        if (!grassWalk(w, xx, yy, zz, i)) continue;
        if (worldBlock(w, xx, yy, zz) != BLOCK_AIR) continue;

        const int roll = rand() & 0xF;
        unsigned char id, data;
        if (roll == 0)      { id = BLOCK_FLOWER;    data = 0; }
        else if (roll == 1) { id = BLOCK_ROSE;      data = 0; }
        else if (roll == 2) { id = BLOCK_TALLGRASS; data = TG_FERN; }
        else                { id = BLOCK_TALLGRASS; data = TG_TALL_GRASS; }

        if (bushCanSurviveWith(w, id, data, xx, yy, zz))
            worldSetTileUpdate(w, xx, yy, zz, id, data);
    }
}

static bool bonemealReed(World* w, int x, int y, int z) {
    int below = y - 1;
    while (below > 0 && worldBlock(w, x, below, z) == BLOCK_REEDS) below--;
    int above = y + 1;
    while (above <= WORLD_H - 1 && worldBlock(w, x, above, z) == BLOCK_REEDS) above++;

    const int grow = below - (above - 1) + 3;
    if (grow <= 0) return false;

    const int top = above - 1 + grow;
    for (int yy = above; yy <= top; yy++)
        worldSetTileUpdate(w, x, yy, z, BLOCK_REEDS, 0);
    return true;
}

bool BonemealItem::useOn(ItemInstance* item, Player* player, World* world, int x, int y, int z, int face, float, float, float) {

    if (item->data != DYE_WHITE) return false;
    switch (worldBlock(world, x, y, z)) {

        case BLOCK_SAPLING:
            saplingGrow(world, x, y, z);
            worldUpdateLights(world);

            worldRebuildAroundNow(world, x, y, z);
            if (player) player->inventory->consumeSelected();
            return true;

        case BLOCK_WHEAT:
        case BLOCK_MELON_STEM: {
            int age = worldData(world, x, y, z) + 2 + rand() % 3;
            if (age >= 7) age = 7;
            worldSetData(world, x, y, z, (unsigned char)age);
            if (player) player->inventory->consumeSelected();
            return true;
        }
        case BLOCK_GRASS:
            bonemealGrass(world, x, y, z);
            if (player) player->inventory->consumeSelected();
            return true;
        case BLOCK_REEDS:
            if (!bonemealReed(world, x, y, z)) return false;
            if (player) player->inventory->consumeSelected();
            return true;
    }
    return false;
}
