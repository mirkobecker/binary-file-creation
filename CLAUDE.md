# Binary File Creation — Claude Instructions

A C command-line tool that produces a 1024-byte binary file. See `README.md` for the full binary layout, field definitions, and the reference example.

## Language

All identifiers, comments, commit messages, and documentation are written in **English**.

## Implementation Decisions

- **Argument parsing:** Direct `argc`/`argv` — no `getopt`; 5 positional args only
- **Endianness:** `htons()`/`htonl()` — no custom byte-swap
  - Linux: `<arpa/inet.h>` · Windows: `<winsock2.h>`
- **CRC-16/CCITT:** Custom implementation — Init=`0xFFFF`, Poly=`0x1021`
- **Test framework:** Unity — vendored at `tests/unity/` (`unity.c`, `unity.h`, `unity_internals.h`); do not use a submodule or download script
- **Dev environment:** `.devcontainer/Dockerfile` (base `mcr.microsoft.com/devcontainers/cpp:1-bookworm`) + `devcontainer.json`

## Error Handling

- All error messages → `stderr`
- Exit `EXIT_FAILURE` (1) on any error; no output file is written
- Exit `EXIT_SUCCESS` (0) on success

## Build

| Target    | Description                                   |
|-----------|-----------------------------------------------|
| `all`     | Build the Linux binary (`create_binary_file`) |
| `windows` | Cross-compile for Windows via mingw-w64       |
| `test`    | Build and run all tests                       |
| `clean`   | Remove all build artefacts                    |

## Test Strategy

- **Unit tests** (Unity): argument parsing, CRC calculation, serial number encoding, MAC validation
- **Integration tests:** invoke the binary with known inputs and compare `output.bin` byte-for-byte against a reference binary
  - Reference: `create_binary_file 00157E33AAFF 00157E33AB00 X550008 09 04`
