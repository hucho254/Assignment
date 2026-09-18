# Concept Note Group 9: Hostel Allocation (C Implementation)

## Language substitution note

The brief specifies Python because it wants **lexically nested functions** with `nonlocal`/`global`. Standard C has no nested functions or closures. a C function's referencing environment is only *(its own locals/params) +* *(file-scope globals)*. We simulate the two Python mechanisms explicitly:

| Python mechanism | C simulation used here |
| - | - |
| `nonlocal` (reach into enclosing frame) | `localAllocCounter` in `main()` passed **by pointer** to `allocateStudent()` |
| `global` (reach program-wide state) | `totalAllocations`, a real C global : readable/writable from any function with no keyword |
| Nested inner function `confirm\_space()` | Ordinary top-level function `confirmSpace()`, called (not nested) : deliberately **cannot** see `allocateStudent()`'s locals |


This contrast (C's flat scoping vs. Python's lexical nesting) is itself answered as part of Task E below.

## Six key variables name, type, scope, lifetime

| Variable | Type | Scope | Lifetime |
| - | - | - | - |
| `totalAllocations` | `int` | File/global | Program start → program end (static storage) |
| `status` (outer) | `char\[20\]` | File/global | Program start → program end |
| `localAllocCounter` | `int` | Local to `main()` | `main()` entry → `main()` return (stack) |
| `room` (param) | `Room \*` | Local to `confirmSpace()` | Each call → each return |
| `selected\_room` | `int` | Local to `allocateStudent()` | Each call → each return |
| dynamically-scanned `rooms\[\]` array | `Room\[\]` | Local to `main()`, passed by reference | `main()` entry → `main()` return |


## Where each required concept appears

- **Named constants:** `MAX\_STUDENTS`, `MAX\_ROOMS`, `MIN\_VALID\_ID`, `PRIORITY\_CATEGORY`, `UNALLOCATED` (top of `hostel\_allocation.c`) replace all magic numbers/literals.

- **Referencing environments (checkpoints):** printed live at 3 labelled points inside `confirmSpace()`, inside `allocateStudent()` after the inner call, and in `main()`'s reporting section (`referencingEnvironmentCheckpoints()`), see console output.

- **Shadowing:** `shadowingDemo()`identifier `status` declared at global scope, then re-declared in a function-local block, then re-declared again in a nested `if\{\}` block; all three printed and explained.

- **Nonlocal-equivalent update:** `localAllocCounter` pointer parameter in `allocateStudent()`.

- **Global update:** `totalAllocations++` inside `allocateStudent()`.

- **Validation / exceptional cases (≥3):** invalid ID (`\< MIN\_VALID\_ID`), missing name, no room in requested block, room/block at full capacity all four exercised in the test dataset (students 1011, duplicate 1001, and ID `5`).

## Answers to reflection questions

**Static vs dynamic scoping (Task E):** Under C's (and Python's) static/lexical scoping, `confirmSpace()` if it referenced an undeclared `status` would resolve to the *global* `status`, because that is its lexical parent; there is no enclosing `allocateStudent()` frame to search since C functions aren't nested. Under hypothetical dynamic scoping, the same reference would instead resolve to whichever caller's `status` was most recently active on the call stack so the resolved value would depend on *who called whom at runtime*, not on where `confirmSpace()` is written in the source. This is why static scoping is more predictable and readable: the answer can be determined by reading the source text alone, without tracing execution.

**Nested-scope prediction:** In Python, `confirm\_space()` nested inside `allocate\_student()` would see `student`/`selected\_room` automatically (lexical closure). In C's flat model that visibility simply doesn't exist the same logical checkpoint that would show those names visible in Python shows them **absent** in C (see checkpoint 1 output), which is the clearest single piece of evidence for the language's scoping model.

## Test evidence

- Compiled with `gcc -Wall -Wextra -std=c11` zero warnings.

- 13 student records processed: 9 successful allocations, 4 rejections (block-full, nonexistent block `D`, capacity-full duplicate ID, ID below minimum).

- Full run captured in `test\_run\_output.txt`.

