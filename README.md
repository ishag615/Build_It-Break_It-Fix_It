# Art Gallery Logging System

**Project Name:** Build It, Break It, Fix It - Art Gallery Tracking System

**Team Members:**
- Name 1 – Amitha Ajithkumari
- Name 2 – Isha Gupta

## Overview

This project implements a secure logging system for tracking the state of an art gallery, including guests and employees entering and leaving rooms. The system consists of two programs:

- **logappend** – appends new information to the log file with validation and state management
- **logread** – reads and displays the current state of the gallery

The project emphasizes security by considering potential attacks and implementing comprehensive mitigations against resource injection, unauthorized access, and invalid state transitions.

## Repository Structure

```
project-root/
├── src/
│   ├── logappend.cpp        # Main logging program
│   ├── logread.cpp          # Log query/display program
│   └── logappend.hpp        # Header file with class definitions
├── docs/
│   └── design_document.pdf  # Detailed security analysis and design
├── tests/
│   ├── test_logappend.cpp   # Unit tests for logappend
│   ├── test_logread.cpp     # Integration tests for logread
│   └── Makefile             # Build configuration
├── Makefile                 # Main project build file
└── README.md                # This file
```

## Building the Project

### Prerequisites
- C++17 compatible compiler (g++ or clang)
- Make utility

### Compile All Programs

```bash
make
```

This builds both `logappend` and `logread` executables.

### Compile Individual Programs

```bash
# Build only logappend
g++ -std=c++17 -Wall -Wextra -o logappend src/logappend.cpp

# Build only logread
g++ -std=c++17 -Wall -Wextra -o logread src/logread.cpp
```

## Usage

### logappend – Adding Entries to the Log

```bash
./logappend -T <timestamp> -K <token> (-E | -G) <name> (-A | -L) [-R <room_id>] <log_file>
```

**Arguments:**
- `-T <timestamp>` – Unix timestamp (must be greater than previous entry, range: 1 to 1,073,741,823)
- `-K <token>` – Security token for authentication (alphanumeric characters only)
- `-E <name>` – Employee name (max 19 alphabetic characters)
- `-G <name>` – Guest name (max 19 alphabetic characters)
- `-A` – Arrival event
- `-L` – Departure/Leave event
- `-R <room_id>` – Optional: specific room ID (0-7; -1 indicates gallery entrance/exit)
- `<log_file>` – Path to log file

**Examples:**

```bash
# Employee Alice arrives at gallery
./logappend -T 100 -K mytoken123 -E Alice -A gallery.log

# Employee Alice enters room 3
./logappend -T 105 -K mytoken123 -E Alice -A -R 3 gallery.log

# Guest Bob leaves room 5
./logappend -T 110 -K mytoken123 -G Bob -L -R 5 gallery.log

# Employee Alice exits gallery
./logappend -T 115 -K mytoken123 -E Alice -L gallery.log
```

***Demonstration of protections, please test in order:***
```bash
#First log
./logappend -T 1 -K mytoken123 -E Alice -A log.txt

#Resource injection attack
./logappend -T 1 -K mytoken123 -E Alice -A ..//log.txt

#Traversal Attack
./logappend -T 2 -K mytoken123 -E Alice -A ../../etc/passwd

#Normal room entry
./logappend -T 2 -K mytoken123 -E Alice -A -R 1 log.txt

#Invalid Token- checking input validation
./logappend -T 2 -K mytoken123#$% -E Alice -A -R -1 log.txt

#Invalid employee/ guest state
./logappend -T 3 -K mytoken123 -G Alice -A -R -1 log.txt

#Leaving incorrect room
./logappend -T 4 -K mytoken123 -E Alice -L -R 1 log.txt

#Name Buffer overflow
./logappend -T 4 -K mytoken123 -E AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA -A log.txt

#Room integer overflow
./logappend -T 4 -K mytoken123 -E Alice -L -R -28738728373872837 log.txt

#Name input validation check
./logappend -T 4 -K mytoken123 -E Alice123 -A log.txt


```

### logread – Querying the Log File

```bash
./logread -K <token> (-S | -R [-E | -G] <name>) <log_file>
```

**Arguments:**
- `-K <token>` – Security token (must match token in log file)
- `-S` – Display current state (all employees, guests, and room occupancy)
- `-R` – Display room history for a specific person
- `-E <name>` – Query employee (used with `-R`)
- `-G <name>` – Query guest (used with `-R`)
- `<log_file>` – Path to log file to read

**Examples:**

```bash
# Display current gallery state (use log.txt/ any file decalred in logappend)
./logread -K mytoken123 -S gallery.log

# Display room history for employee Alice
./logread -K mytoken123 -R -E Alice gallery.log

# Display room history for guest Bob
./logread -K mytoken123 -R -G Bob gallery.log
```

## State Validation

The system enforces valid state transitions for gallery visitors:

- **Entering Gallery:** Person must not already be in gallery
- **Entering Room:** Person must first be in gallery and not in another room
- **Leaving Room:** Person must be in that specific room
- **Leaving Gallery:** Person must be in gallery and not in any room
- **Timestamps:** Must be strictly increasing; no duplicate or earlier timestamps allowed
- **Identity:** A person cannot be both an employee and a guest

## Log File Format

The log file uses a simple CSV format:

```
<token>
<timestamp>,<name>,<type>,<action>,<room_id>
<timestamp>,<name>,<type>,<action>,<room_id>
...
```

**Field Definitions:**
- `token` – First line contains the security token
- `timestamp` – Long integer representing time of event
- `name` – Person's name (alphabetic characters only)
- `type` – "E" for employee, "G" for guest
- `action` – "A" for arrival, "L" for departure/leave
- `room_id` – Room number (0-7) or -1 for gallery

**Example Log File:**

```
mytoken123
100,Alice,E,A,-1
105,Alice,E,A,3
110,Bob,G,A,-1
115,Bob,G,A,5
120,Alice,E,L,3
```

## Security Considerations

### Implemented Mitigations

1. **Token-Based Authentication**
   - Each log file is protected with a token
   - Token is validated on read and write operations
   - Prevents unauthorized access to gallery logs

2. **Resource Injection Prevention**
   - Filenames are validated to block path traversal attacks (`..`, `//`, `\`)
   - Prevents attackers from writing logs outside intended directories

3. **Input Validation**
   - Names: Only alphabetic characters, maximum 19 characters
   - Tokens: Only alphanumeric characters
   - Timestamps: Range validated (1 to 1,073,741,823)
   - Room IDs: Valid range (-1 to 7)
   - Arguments: Bounds checking and type validation

4. **State Machine Enforcement**
   - Validates logical state transitions for gallery visitors
   - Prevents impossible sequences (e.g., entering room without entering gallery first)

5. **File Locking**
   - Uses `flock()` for concurrent file access protection
   - Ensures atomic read/write operations

### Threat Model

The system considers three main classes of attacks:

1. **Unauthorized Access** – Mitigated through token authentication
2. **Resource Injection** – Mitigated through filename validation
3. **Invalid State Transitions** – Mitigated through state validation logic

See `docs/design_document.pdf` for detailed threat analysis and implementation details.

## Testing

### Run All Tests

```bash
make test
```

This runs both unit tests for logappend and integration tests for logread.

### Run Specific Tests

```bash
# Unit tests only
make test_logappend

# Integration tests only
make test_logread
```

### Clean Test Artifacts

```bash
make clean
```

## Project Highlights

- **Secure by Design:** Input validation at every step
- **Comprehensive Testing:** 10 integration tests covering normal and edge cases
- **State Machine Logic:** Enforces valid visitor state transitions
- **Zero Compiler Warnings:** Code compiles cleanly with `-Wall -Wextra`
- **Well-Documented:** Detailed design document included

## Error Handling

Both programs return exit codes:

- `0` – Success
- `255` – Invalid arguments or operation failed

Programs output "invalid" or specific error messages to indicate problems.

## Limitations and Future Work

- Single-threaded design (concurrent access protection via file locking)
- No persistent token storage (token must be provided per command)
- No audit logging of failed access attempts
- Room IDs hardcoded to 0-7 range
---
