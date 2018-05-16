#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include "mbr.h"

typedef struct {
    uint32_t startLBA;//tLBA
    uint32_t sectorCount;
    tLARGE startCHS;//tIDECHS
    tLARGE endCHS;
    uint8_t active;
    uint8_t code;
} tSTLine;

typedef struct {
    tSTLine row[4];
} tST;

uint64_t get_disk_size(tIDECHS *ide) {
    uint64_t ide_head, ide_cylinder, ide_sector;
    printf("Enter count headers (ide geometry): "); scanf("%"PRIu64, &ide_head);
    printf("Enter count cylinders (ide geometry): "); scanf("%"PRIu64, &ide_cylinder);
    printf("Enter count sectors (ide geometry): "); scanf("%"PRIu64, &ide_sector);
    if (ide_head > 0x10 || ide_cylinder > 0x10000 || ide_sector > 0xFF) {
        printf("Error params!\n");
        return 0;
    }
    ide->head = ide_head - 1;
    ide->cylinder = ide_cylinder - 1;
    ide->sector = ide_sector;
    uint64_t size = ide_head * ide_cylinder * ide_sector * 512;
    printf("Max size: %.2f GB\n", size / (1024.0 * 1024.0 * 1024.0));
    return size;
}

void insert_line(tSTLine *row, uint32_t lba, uint64_t size, uint8_t type, uint8_t active, tLARGE geomerty) {
    row->startLBA = lba;
    row->sectorCount = size;
    row->active = active;
    row->code = type;
    a_lba2large(geomerty, lba, &row->startCHS);
    a_lba2large(geomerty, lba + size - 1, &row->endCHS);
    //a_lba2idechs(geomerty, lba, &row->startCHS);
    //a_lba2idechs(geomerty, lba - 1, &row->endCHS);
}

tST* create_section_table(uint64_t max_size_bytes, const tIDECHS *idegeomery) {
    double sectionSizeMB;
    uint32_t lbaStart = 1, maxLBA;//tLBA
    uint16_t tableID = 0, rowID = 0;
    uint8_t sectionType, isActive, activeIsLock = 0;
    tLARGE geomerty;
    g_idechs2ilarge(*idegeomery, &geomerty);
    g_large2lba(geomerty, &maxLBA);
    tST *table = (tST*)calloc(sizeof(tST), 1);
    --max_size_bytes;//- main mbr
    while (max_size_bytes != 0) {
        isActive = 0;
        sectionType = 0;
        
        printf("Enter section size (MB): ");
        scanf("%lf", &sectionSizeMB);
        if (sectionSizeMB == 0)
            break;
        printf("Enter type section: ");
        scanf("%hhu", &sectionType);
        if (activeIsLock == 0) {
            getchar();
            printf("Section is active (y/n): ");
            isActive = tolower(getchar()) == 'y';
            activeIsLock = isActive;
        }
        uint64_t sizeUsed = ceil(sectionSizeMB * 1024 * 1024);
        if (sizeUsed % 512)
            sizeUsed += 512 - (sizeUsed % 512);
        if (sizeUsed > max_size_bytes)
            sizeUsed = max_size_bytes;
        
        if ((rowID >= 3 && (max_size_bytes - sizeUsed) != 0) || lbaStart >= maxLBA /* && rowID != 0*/) {
            tST *new = (tST*)realloc(table, sizeof(tST) * (tableID + 2));
            if (new == NULL)
                return table;
            table = new;
            insert_line(table[tableID].row + rowID, lbaStart, max_size_bytes / 512, 5, 0, geomerty);
            rowID = 0;
            ++tableID;
        }
        
        insert_line(table[tableID].row + rowID, lbaStart, sizeUsed / 512, sectionType, isActive, geomerty);

        lbaStart += table[tableID].row[rowID].sectorCount;
        max_size_bytes -= sizeUsed;
        ++rowID;
    }
    return table;
}

void printTable(const tST* table) {
    int id = 0;
    while (id == 0) {
        printf("\nAct |     CHS start    |  OS |      CHS end     |  LBA start | Section count\n");
        printf("----------------------------------------------------------------------------\n");
        for (; id < 4; ++id) {
            const tSTLine *row = table->row + id;
            printf("%02Xh | %4hu | %3hhu | %3hhu | %02Xh | %4hu | %3hhu | %3hhu | %10u | %10u\n",
               row->active, row->startCHS.cylinder, row->startCHS.head, row->startCHS.sector,
               row->code, row->endCHS.cylinder, row->endCHS.head, row->endCHS.sector,
               row->startLBA, row->sectorCount);
            if (row->code == 5) {
                ++table;
                id = 0;
                break;
            }
        }
    }
}

int main() {
    tIDECHS ide;
    uint64_t size = get_disk_size(&ide);
    tST *tables = create_section_table(size, &ide);
    printTable(tables);
    free(tables);
    return 0;
}
