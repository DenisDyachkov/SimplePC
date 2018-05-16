#include <stdio.h>
#include <stdlib.h>
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

tST* create_section_table(uint64_t max_size_bytes, const tIDECHS *geomer) {
    uint64_t maxSizeSection;
    uint8_t sectionType, isActive, activeIsLock = 0;
    uint32_t lbaStart = 1;//tLBA
    tLARGE geomerty;
    g_idechs2ilarge(*geomer, &geomerty);
    printf(" %hu %hhu %hhu -> %hu %hhu %hhu",
        geomer->cylinder, geomer->head, geomer->sector,
        geomerty.cylinder, geomerty.head, geomerty.sector
    );
    tST table;// = //(tST*)malloc(sizeof(tST));
    --max_size_bytes;//- main mbr
    while (max_size_bytes != 0) {
        isActive = 0;
        sectionType = 0;
        
        printf("Enter max size section: ");
        scanf("%"PRIu64, &maxSizeSection);
        if (maxSizeSection == 0)
            break;
        printf("Enter type section: ");
        scanf("%hhu", &sectionType);
        if (activeIsLock == 0) {
            printf("Section is active: ");
            scanf("%hhu", &isActive);
            activeIsLock = isActive;
        }
        if (maxSizeSection > max_size_bytes)
            maxSizeSection = max_size_bytes;
        
        tSTLine *row = table.row + 0;
        row->startLBA = lbaStart;
        row->sectorCount = maxSizeSection / 512 + (maxSizeSection % 512 != 0);
        a_lba2large(geomerty, lbaStart, &row->startCHS);
        //a_lba2idechs(geomerty, lbaStart, &row->startCHS);
        lbaStart += row->sectorCount;
        a_lba2large(geomerty, lbaStart - 1, &row->endCHS);
        //a_lba2idechs(geomerty, lbaStart - 1, &row->endCHS);
        row->active = isActive;
        row->code = sectionType;
        
        printf("%hhu | start: %hu/%hhu/%hhu | %hhu | end: %hu/%hhu/%hhu | %u | %u\n",
               row->active, row->startCHS.cylinder, row->startCHS.head, row->startCHS.sector,
               row->code, row->endCHS.cylinder, row->endCHS.head, row->endCHS.sector,
               row->startLBA, row->sectorCount);
        
        max_size_bytes -= maxSizeSection;
    }
}

int main() {
    tIDECHS ide;
    uint64_t size = get_disk_size(&ide);
    create_section_table(size, &ide);
    return 0;
}
