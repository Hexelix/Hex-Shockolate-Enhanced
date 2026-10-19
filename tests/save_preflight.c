// Exercise real save preflight without opening the global resource manager.
#include "gamewrap.h"
#include "player.h"
#include "res.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
extern const ResLayout PlayerLayout_M32, PlayerLayout_M64;

FILE *fopen_caseless(const char *path, const char *mode) {
    return fopen(path, mode);
}

static void fixture(const char *path, unsigned size, int truncate) {
    ResFileHeader header = {0};
    ResDirHeader directory = {0};
    ResDirEntry entry = {0};
    memcpy(header.signature, "LG Res File v2\r\n", 16);
    header.dirOffset = sizeof(header);
    directory.numEntries = 1;
    entry.id = SAVE_GAME_ID_BASE + 1;
    entry.size = size;
    FILE *file = fopen(path, "wb");
    assert(file);
    assert(fwrite(&header, sizeof(header), 1, file) == 1);
    assert(fwrite(&directory, sizeof(directory), 1, file) == 1);
    if (!truncate)
        assert(fwrite(&entry, sizeof(entry), 1, file) == 1);
    fclose(file);
}

int main(void) {
    char path[] = "/tmp/hex-save-preflight-XXXXXX";
    int fd = mkstemp(path);
    assert(fd >= 0);
    close(fd);
    fixture(path, PlayerLayout_M64.dsize, 0);
    assert(validate_player_save(path) == OK);
    fixture(path, PlayerLayout_M32.dsize, 0);
    assert(validate_player_save(path) == OK);
    // Nine-weapon saves differ by exactly one five-byte weapon record.
    fixture(path, PlayerLayout_M64.dsize - 5, 0);
    assert(validate_player_save(path) != OK);
    fixture(path, PlayerLayout_M32.dsize - 5, 0);
    assert(validate_player_save(path) != OK);
    fixture(path, PlayerLayout_M64.dsize - 50, 0);
    assert(validate_player_save(path) != OK);
    fixture(path, PlayerLayout_M64.dsize, 1);
    assert(validate_player_save(path) != OK);
    FILE *file = fopen(path, "wb");
    fputs("not a resource archive", file);
    fclose(file);
    assert(validate_player_save(path) != OK);
    unlink(path);
    assert(validate_player_save(path) != OK);
    puts("Save preflight: compatible, incompatible, truncated, corrupt and missing files passed");
}
