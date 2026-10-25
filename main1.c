#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PAGE_SIZE 256

int main (int arg, char** argv) {

    // Bring in all the necessary files
    FILE* addr_file = fopen(argv[1], "rt");
    FILE* store_file = fopen("BACKING_STORE.bin", "rb");
    FILE* out1 = fopen("out1.txt", "w");
    FILE* out2 = fopen("out2.txt", "w");
    FILE* out3 = fopen("out3.txt", "w");

    // Buffer for the current Address
    char curAddr[50];

    // Table array represents the page table
    int table[256];
    // Done so every element in page table starts off at -1 (Don't ask why {-1} doesn't work)
    memset(table, -1, sizeof(table));
    // Physical array represents physical memory
    int8_t physical[65536] = {0};
    // Keeps track of the number page faults (Not sure if we're supposed to print this out or not)
    int faults = 0;

    // use_next is a pointer which points to the next avaliable space in the physical memory
    int8_t* use_next = physical;
    // Keeps track of the frame number associated with the next free memory space
    int use_next_value = 0;
    while (fgets(curAddr, sizeof(curAddr), addr_file) != NULL) {
        // Store the logical address into n, and mask it's 16 rightmost bits
        unsigned int n = atoi(curAddr);
        n = (unsigned int)n & 0xFFFF;

        // Get the page number and offset for the logical address
        int page = n / PAGE_SIZE;
        int offset = n % PAGE_SIZE;

        // If there is not currently a logical to physical mapping in the page table for the logical
        // addresses page number, a page fault occurs
        if (table[page] == -1) {
            // Map the page number to the number of the frame that will now be used
            table[page] = use_next_value;
            // Use fseek to find the page in the backing store that is associated with the page number
            // 256 is used because each page if 256 bytes long
            fseek(store_file, (long)page * 256, SEEK_SET);
            // Copy each byte within the backing store into the frame, updating use_next so it knows
            // were the next avaliable memory spot is
            for (int i = 0; i < 256; i++) {
                *use_next = (int8_t)fgetc(store_file);
                use_next += sizeof(int8_t);
            }
            // Increment the number of the next free frame by 1
            use_next_value += 1;
        }
        // Calculate the physical address
        unsigned int physical_address = offset + (table[page] * 256);
        // Go into physical memory to retrieve the value
        int8_t value = physical[(256 * table[page]) + offset];
        // Print the information out to the correct files
        fprintf(out1, "%u\n", n);
        fprintf(out2, "%u\n", physical_address);
        fprintf(out3, "%d\n", value);
        // printf("Logical: %u, Physical: %u, Value: %d\n", n, physical_address, value);
    }
    return 0;
}