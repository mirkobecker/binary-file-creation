#!/usr/bin/env bash
set -euo pipefail

BINARY="./build/create_binary_file"
REFERENCE="tests/reference.bin"
OUTPUT="output.bin"
PASS=0
FAIL=0

trap 'rm -f "$OUTPUT"' EXIT

pass() { echo "PASS: $1"; PASS=$((PASS + 1)); }
fail() { echo "FAIL: $1"; FAIL=$((FAIL + 1)); }

check_fails() {
        local desc="$1"; shift
        rm -f "$OUTPUT"
        if "$BINARY" "$@" 2>/dev/null; then
                fail "$desc"
        elif [ -f "$OUTPUT" ]; then
                fail "$desc — output.bin must not exist on error"
                rm -f "$OUTPUT"
        else
                pass "$desc"
        fi
}

check_succeeds() {
        local desc="$1"; shift
        rm -f "$OUTPUT"
        if "$BINARY" "$@" 2>/dev/null; then
                if [ -f "$OUTPUT" ]; then
                        pass "$desc"
                else
                        fail "$desc — output.bin not created"
                fi
        else
                fail "$desc"
        fi
}

# --- Error cases: exit 1, no output.bin ---

check_fails "no args"
check_fails "1 arg"                                       00157E33AAFF
check_fails "2 args"                                      00157E33AAFF 00157E33AB00
check_fails "3 args"                                      00157E33AAFF 00157E33AB00 X550008
check_fails "4 args"                                      00157E33AAFF 00157E33AB00 X550008 09
check_fails "6 args"                                      00157E33AAFF 00157E33AB00 X550008 09 04 extra
check_fails "MAC1 too short"                              0015 00157E33AB00 X550008 09 04
check_fails "MAC1 invalid chars"                          00157E33AAGG 00157E33AB00 X550008 09 04
check_fails "MAC1 broadcast"                              FFFFFFFFFFFF 00157E33AB00 X550008 09 04
check_fails "MAC2 too short"                              00157E33AAFF X5500 X550008 09 04
check_fails "MAC2 invalid chars"                          00157E33AAFF 00157EGGAB00 X550008 09 04
check_fails "MAC2 broadcast"                              00157E33AAFF FFFFFFFFFFFF X550008 09 04
check_fails "serial too short"                            00157E33AAFF 00157E33AB00 X5500 09 04
check_fails "serial too long"                             00157E33AAFF 00157E33AB00 X5500089 09 04
check_fails "major non-digit"                             00157E33AAFF 00157E33AB00 X550008 9X 04
check_fails "major too long"                              00157E33AAFF 00157E33AB00 X550008 009 04
check_fails "minor non-digit"                             00157E33AAFF 00157E33AB00 X550008 09 0X
check_fails "minor too long"                              00157E33AAFF 00157E33AB00 X550008 09 004

# --- Happy path ---

check_succeeds "happy path exits 0 and creates output.bin" 00157E33AAFF 00157E33AB00 X550008 09 04

if [ -f "$OUTPUT" ]; then
        SIZE=$(wc -c < "$OUTPUT")
        if [ "$SIZE" -eq 1024 ]; then
                pass "file size = 1024 bytes"
        else
                fail "file size = 1024 bytes (got $SIZE)"
        fi

        if [ -f "$REFERENCE" ]; then
                if cmp -s "$OUTPUT" "$REFERENCE"; then
                        pass "byte-for-byte match with reference.bin"
                else
                        fail "byte-for-byte match with reference.bin"
                fi
        else
                echo "SKIP: $REFERENCE not found — skipping byte-for-byte comparison"
        fi
fi

# --- Summary ---
echo ""
echo "$PASS passed, $FAIL failed"
[ "$FAIL" -eq 0 ]
