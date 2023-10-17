# Library MPCitH - Snapshot

## Source

LibMPCitH - https://github.com/CryptoExperts/libmpcith

## How to use it?

To run some signatures and have some benchmark:
```bash
make bench
./bench <N>
```
where `<N>` is the number of signing to perform.

To generate the KAT files:
```bash
make PQCgenKAT_sign
./PQCgenKAT_sign
```
then the generated files will be `PQCsignKAT_***.req` and `PQCsignKAT_***.rsp`.

## Configuration of the snapshot

 - `SELECTED_MODE={{SELECTED_MODE}}`
 - `USER_FLAGS={{USER_FLAGS}}`
 - `SYM_SRC={{SYM_SRC}}`
 - `ARITH_PATHS={{ARITH_PATHS}}`
 - `ARITH_SRC={{ARITH_SRC}}`
 - `SPE_PATHS={{SPE_PATHS}}`
 - `MPC_SRC={{MPC_SRC}}`
 - `MODE_PATHS={{MODE_PATHS}}`
 - `CORE_SRC={{CORE_SRC}}`
