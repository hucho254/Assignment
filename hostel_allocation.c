/* =====================================================================
 * GROUP 9 — UNIVERSITY HOSTEL ALLOCATION SYSTEM
 * Demonstration of Names, Bindings, Scopes, and Referencing Environments
 * ===================================================================== */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ---------------------------------------------------------------------
 * TASK A: NAMED CONSTANTS & DOMAIN RULES
 * Replaces magic numbers with explicitly scoped macro constants.
 * ------------------------------------------------------------------- */
#define MAX_STUDENTS        20
#define MAX_ROOMS           20
#define NAME_LEN            50
#define BLOCK_LEN           10
#define PRIORITY_CATEGORY   "Continuing"
#define MIN_VALID_ID        1000
#define UNALLOCATED         (-1)

/* ---------------------------------------------------------------------
 * DATA STRUCTURES
 * ------------------------------------------------------------------- */
typedef struct {
    int  studentId;
    char name[NAME_LEN];
    char category[NAME_LEN];
    char requestedBlock[BLOCK_LEN];
    int  allocatedRoomIndex;   /* Holds UNALLOCATED until room assignment */
} Student;

typedef struct {
    char roomNo[BLOCK_LEN];
    char block[BLOCK_LEN];
    int  capacity;
    int  occupied;
} Room;

/* ---------------------------------------------------------------------
 * GLOBAL SCOPE (Tasks B & D)
 * Static storage duration; visible throughout this translation unit.
 * ------------------------------------------------------------------- */
int  totalAllocations = 0;      /* Program-wide global counter */
char status[20] = "GLOBAL";     /* Base identifier used for shadowing tests */

/* Forward declarations */
int  findRoomIndex(Room rooms[], int roomCount, const char *block);
int  confirmSpace(Room *room);
int  allocateStudent(Student *s, Room rooms[], int roomCount,
                      int *localAllocCounter, char rejectReason[]);
void shadowingDemo(void);
void referencingEnvironmentCheckpoints(Room rooms[], int roomCount);
void printRoomTable(Room rooms[], int n);
void printAllocationReport(Student students[], int n, Room rooms[]);
void printSummary(Student students[], int n, Room rooms[], int roomCount,
                   int localAllocCounter, int rejectedCount);

/* =====================================================================
 * TASK C: REFERENCING ENVIRONMENT CHECKPOINT 1 (Inner Function)
 * In Python, an inner function accesses variables in its lexical outer frame.
 * In C, functions have flat scoping: confirmSpace sees ONLY its parameters
 * and true file-scope globals.
 * ===================================================================== */
int confirmSpace(Room *room) {
    printf("    [Checkpoint 1: inside confirmSpace()]\n"
           "      - Visible locals : parameter 'room'\n"
           "      - Visible globals: 'totalAllocations'=%d, 'status'=\"%s\"\n"
           "      - Inaccessible   : 'student' or 'selected_room' from allocateStudent()\n",
           totalAllocations, status);

    return (room->occupied < room->capacity) ? 1 : 0;
}

int findRoomIndex(Room rooms[], int roomCount, const char *block) {
    for (int i = 0; i < roomCount; i++) {
        if (strcmp(rooms[i].block, block) == 0 && confirmSpace(&rooms[i])) {
            return i;
        }
    }
    return -1;
}

/* =====================================================================
 * TASK B: OUTER ALLOCATION LOGIC & SIMULATED NONLOCAL SCOPE
 * 'localAllocCounter' represents Python's `nonlocal` state by receiving a 
 * pointer to main's stack-allocated variable.
 * ===================================================================== */
int allocateStudent(Student *s, Room rooms[], int roomCount,
                     int *localAllocCounter, char rejectReason[]) {
    int selected_room;

    /* Task A: Input Validation Rules */
    if (s->studentId < MIN_VALID_ID) {
        strcpy(rejectReason, "Invalid student ID (below minimum requirement)");
        return 0;
    }
    if (strlen(s->name) == 0) {
        strcpy(rejectReason, "Missing student name");
        return 0;
    }

    selected_room = findRoomIndex(rooms, roomCount, s->requestedBlock);

    if (selected_room == -1) {
        strcpy(rejectReason, "No available space in requested block");
        return 0;
    }

    /* Assign allocation */
    rooms[selected_room].occupied++;
    s->allocatedRoomIndex = selected_room;

    /* Mutate simulated nonlocal counter (via pointer) and global counter */
    (*localAllocCounter)++;
    totalAllocations++;

    /* Referencing Environment Checkpoint 2 */
    printf("    [Checkpoint 2: inside allocateStudent()]\n"
           "      - Visible locals : 's', 'rooms', 'roomCount', 'localAllocCounter', 'selected_room'\n"
           "      - Visible globals: 'totalAllocations'=%d, 'status'=\"%s\"\n",
           totalAllocations, status);

    return 1;
}

/* =====================================================================
 * TASK D: IDENTIFIER SHADOWING EXPERIMENT
 * Demonstrates binding resolution across nested block scopes `{ ... }`.
 * ===================================================================== */
void shadowingDemo(void) {
    printf("\n--- Task D: Shadowing Experiment on 'status' ---\n");
    printf("  Level 1 (Global scope)        : status = \"%s\"\n", status);

    {
        /* Outer local block scope */
        char status[20] = "OUTER-LOCAL"; 
        printf("  Level 2 (Function-local scope): status = \"%s\"\n", status);

        if (1) {
            /* Inner nested block scope */
            char status[20] = "INNER-BLOCK";
            printf("  Level 3 (Nested block scope)  : status = \"%s\"\n", status);
        }

        printf("  Back to Level 2 (Outer local) : status = \"%s\"\n", status);
    }

    printf("  Back to Level 1 (Global)      : status = \"%s\"\n", status);
    printf("  Resolution Rule: C resolves names by searching from the innermost "
           "enclosing block outward to file scope.\n");
}

/* =====================================================================
 * TASK C: REFERENCING ENVIRONMENT CHECKPOINT 3
 * ===================================================================== */
void referencingEnvironmentCheckpoints(Room rooms[], int roomCount) {
    (void)rooms; 
    (void)roomCount;
    printf("    [Checkpoint 3: Inside main() / Caller context]\n"
           "      - Visible locals : 'students[]', 'rooms[]', local counters\n"
           "      - Visible globals: 'totalAllocations'=%d, 'status'=\"%s\"\n"
           "      - Inaccessible   : Local variables of finished function calls\n",
           totalAllocations, status);
}

/* ---------------------------------------------------------------------
 * REPORTING & DISPLAY FUNCTIONS
 * ------------------------------------------------------------------- */
void printRoomTable(Room rooms[], int n) {
    printf("\nROOM AVAILABILITY TABLE\n");
    printf("%-6s %-6s %-9s %-9s %-10s\n", "Room", "Block", "Capacity", "Occupied", "Remaining");
    printf("--------------------------------------------------\n");
    for (int i = 0; i < n; i++) {
        printf("%-6s %-6s %-9d %-9d %-10d\n", 
               rooms[i].roomNo, rooms[i].block, rooms[i].capacity, 
               rooms[i].occupied, rooms[i].capacity - rooms[i].occupied);
    }
}

void printAllocationReport(Student students[], int n, Room rooms[]) {
    printf("\nSTUDENT ALLOCATION REPORT\n");
    printf("%-6s %-18s %-12s %-8s %-10s\n", "ID", "Name", "Category", "ReqBlk", "Room");
    printf("--------------------------------------------------\n");
    for (int i = 0; i < n; i++) {
        if (students[i].allocatedRoomIndex != UNALLOCATED) {
            printf("%-6d %-18s %-12s %-8s %-10s\n", 
                   students[i].studentId, students[i].name, students[i].category,
                   students[i].requestedBlock, rooms[students[i].allocatedRoomIndex].roomNo);
        }
    }
}

void printSummary(Student students[], int n, Room rooms[], int roomCount,
                   int localAllocCounter, int rejectedCount) {
    (void)students; (void)n;
    int blocksSeen = 0;
    char seenBlocks[MAX_ROOMS][BLOCK_LEN];

    printf("\nBLOCK OCCUPANCY TOTALS\n");
    for (int i = 0; i < roomCount; i++) {
        int alreadySeen = 0;
        for (int k = 0; k < blocksSeen; k++) {
            if (strcmp(seenBlocks[k], rooms[i].block) == 0) alreadySeen = 1;
        }
        if (alreadySeen) continue;
        strcpy(seenBlocks[blocksSeen++], rooms[i].block);

        int cap = 0, occ = 0;
        for (int j = 0; j < roomCount; j++) {
            if (strcmp(rooms[j].block, rooms[i].block) == 0) {
                cap += rooms[j].capacity;
                occ += rooms[j].occupied;
            }
        }
        printf("  Block %-4s | Capacity: %-3d | Occupied: %-3d | Remaining: %d\n",
               rooms[i].block, cap, occ, cap - occ);
    }

    printf("\nSYSTEM SUMMARY\n");
    printf("  Successful Allocations (Simulated Nonlocal Counter) : %d\n", localAllocCounter);
    printf("  Successful Allocations (Global Counter)             : %d\n", totalAllocations);
    printf("  Rejected Requests                                   : %d\n", rejectedCount);
}

/* =====================================================================
 * MAIN PROGRAM ENTRY POINT
 * ===================================================================== */
int main(void) {
    Room rooms[MAX_ROOMS] = {
        {"R1", "A", 1, 0}, {"R2", "A", 1, 0},
        {"R3", "B", 2, 0}, {"R4", "B", 2, 0},
        {"R5", "C", 3, 0}, {"R6", "C", 2, 0}
    };
    int roomCount = 6;

    Student students[MAX_STUDENTS] = {
        {1001, "Alice Amina",     PRIORITY_CATEGORY, "A", UNALLOCATED},
        {1002, "Brian Otieno",    PRIORITY_CATEGORY, "A", UNALLOCATED},
        {1003, "Cynthia Wanjiru", PRIORITY_CATEGORY, "A", UNALLOCATED}, /* Expected: Reject (Block A full) */
        {1004, "David Mwangi",    PRIORITY_CATEGORY, "B", UNALLOCATED},
        {1005, "Grace Nyambura",  PRIORITY_CATEGORY, "B", UNALLOCATED},
        {1006, "Peter Kamau",     PRIORITY_CATEGORY, "B", UNALLOCATED},
        {1007, "Susan Chebet",    PRIORITY_CATEGORY, "B", UNALLOCATED},
        {1008, "Kevin Otieno",    PRIORITY_CATEGORY, "C", UNALLOCATED},
        {1009, "Lucy Wambui",     PRIORITY_CATEGORY, "C", UNALLOCATED},
        {1010, "Mark Kiptoo",     PRIORITY_CATEGORY, "C", UNALLOCATED},
        {1011, "Nancy Akinyi",    PRIORITY_CATEGORY, "D", UNALLOCATED}, /* Expected: Reject (No Block D) */
        {1001, "Duplicate Test",  PRIORITY_CATEGORY, "B", UNALLOCATED},
        {5,    "Bad Id Test",     PRIORITY_CATEGORY, "C", UNALLOCATED}  /* Expected: Reject (ID < MIN_VALID_ID) */
    };
    int studentCount = 13;

    int localAllocCounter = 0; /* Simulated nonlocal counter */
    int rejectedCount = 0;

    printf("================================================================\n");
    printf(" GROUP 9 - HOSTEL ALLOCATION SYSTEM (C Execution)\n");
    printf("================================================================\n");

    printf("\n--- Task B & C: Processing Allocations ---\n");

    for (int i = 0; i < studentCount; i++) {
        char reason[80] = "";
        printf("\nProcessing Student %d (%s), Block %s:\n",
               students[i].studentId, students[i].name, students[i].requestedBlock);

        int blockExists = 0;
        for (int b = 0; b < roomCount; b++) {
            if (strcmp(rooms[b].block, students[i].requestedBlock) == 0)
                blockExists = 1;
        }

        if (!blockExists) {
            printf("  REJECTED: Requested block '%s' does not exist\n", students[i].requestedBlock);
            rejectedCount++;
            continue;
        }

        if (allocateStudent(&students[i], rooms, roomCount, &localAllocCounter, reason)) {
            printf("  ALLOCATED: Assigned to room %s\n", rooms[students[i].allocatedRoomIndex].roomNo);
        } else {
            printf("  REJECTED: %s\n", reason);
            rejectedCount++;
        }
    }

    printf("\n--- Task C: Final Referencing Environment Checkpoint ---\n");
    referencingEnvironmentCheckpoints(rooms, roomCount);

    shadowingDemo();

    printRoomTable(rooms, roomCount);
    printAllocationReport(students, studentCount, rooms);
    printSummary(students, studentCount, rooms, roomCount, localAllocCounter, rejectedCount);

    printf("\n--- Task E: Static vs. Dynamic Scoping Analysis ---\n");
    printf(
      "  In C (Static/Lexical Scoping), 'status' inside confirmSpace() always\n"
      "  binds to the global variable at compile time based on code geometry.\n"
      "  Under Dynamic Scoping, confirmSpace() would resolve 'status' by\n"
      "  inspecting the dynamic call stack at runtime, picking up local variables\n"
      "  declared inside allocateStudent() or main() if they existed.\n");

    return 0;
}