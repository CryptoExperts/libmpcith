# Library MPC-in-the-Head

This library is a unified library for schemes relying on the MPC-in-the-Head paradigm. Its goal is to factorize as much as possible the code which is common to all the MPCitH-based schemes. As long as they respect the expected API, users just need to implement some specific parts of the scheme and they can then rely on the library to get the desired signature scheme. It enables us to speed up the development of the MPCitH-based schemes. The library presents some other advantages:
 * **Comparison**: it is not always easy to fairly compare the running times of two schemes in the literature since their implementations do not necessarily use the same primitives and/or the same level of optimizations. When two schemes are implemented via this library, we configure them such that all the common primitives rely on the same code, and it enables us to get a fairer comparison.
 * **Detailed benchmark**: the library integrates some benchmark tools. The latter can provide a detailed benchmark of the schemes, which includes for example the timings due to the commitments, the pseudo-randomness... Thanks to them, we can easily estimate the running times of implemented schemes and identify their computational bottlenecks.
 * **Versatility**: all the code blocks respect some API. It is thus easy to replace a primitive with another one and compare the impact on the running time, just by changing the configuration file.

## Quick Usage

To compile a specific instance of a scheme implemented by this library, you need to set the right environment variables. Then you can run the compilation using the Makefile of the library. In practice, each instance of a scheme is represented by a configuration file, which gathers all the required definitions of the environment variables. To list all the available configuration files of all the schemes implemented in the library, you can execute

```bash
$ make detect
mpc/mq/mqom/config/hypercube-gf31-l1-fast
mpc/mq/mqom/config/hypercube-gf31-l1-short
mpc/mq/mqom/config/hypercube-gf251-l1-fast
...
```

To use one configuration, you can run:

```bash
$ source mpc/mq/mqom/config/hypercube-gf251-l1-short
```

which sets all the environment variables in the current terminal. To check the loaded instance, you can run:

```bash
$ make print
```

The above command prints many information:
 * Available modes: it is the list of all the MPCitH transformations detected in the library
 * Current mode: it is the MPCitH transformation which is currently selected
 * The scheme URI: it is the path where the selected MPC protocol is implemented
 * Paths (symmetric, arithmetic, MPC protocols, core): they are the paths where the library will search the source files.
 * Sources (...): all the source files that the library will compile are listed there.

Now, you can compile the scheme, together with a small executable to test it:

```bash
$ make clean # To clean from previous compilations
$ make bench
```

From now, you can run the created executable with the command `./bench <nb>`, where `<nb>` is the number of times that the scheme is tested. Assuming that the scheme is a signature scheme, you will get something like:

```bash
$ ./bench 1000
===== MPCITH CONFIG =====
Label: mqom-251-L1
Variant: hypercube-7r
Selected security level: 128
Instruction Sets: SSE AVX
Debug: Off

===== SUMMARY =====
Validation: 1000/1000
Key Gen: 0.13 ms (std=0.00)
Sign:    7.52 ms (std=0.05)
Verify:  7.17 ms (std=0.03)
 -> Signature size (MAX): 7282 B (7.11 KB)
 -> Signature size: 6.42 KB (std=0.01)
 -> Signature size: 6574 B (std=14)
 -> PK size: 59 B
 -> SK size: 102 B
```

In this example, you can find some information:
 * The label of the tested scheme.
 * The used MPCitH transformation ("variant" field).
 * The security level.
 * The validation score, which corresponds to the number of generated signatures that pass the verification.
 * The running times of all the primitives (key generation, signing, verification).
 * The signature size and the key sizes.

Few options can be switched on for the compilation. As explained in the introduction, the library can be used to benchmark the different parts (pseudorandomness, commitments, MPC emulation, ...) in the scheme. To proceed, use the option `BENCH=1`:

```bash
$ BENCH=1 make bench
```

If you want to also have the timings in cycles (and not only in milliseconds), you can use the option `BENCH_CYCLES=1` (which can be used with the previous option). You can run the debug mode with the option `DEBUG=1`: it will add `-g` to the compilation and will run some debug tests (which will slow down the scheme). Finally, you can add the option `AGGRESSIVE=1` to make the compiler use very rigorous compilation options.

To generate some *Known Answer Tests*, you can compile the scheme with the target `kat_gen` and execute the produced executable `./kat_gen`:

```bash
$ make kat_gen
$ ./kat_gen
```

This produces two files `PQCsignKAT_<sks>.req` and `PQCsignKAT_<sks>.rsp` where `sks` is the size of the secret key in bytes.

## MakeFile Targets

* `make detect`: list all the configuration files which are in the folder `mpc`
* `make print`: print the current configuration
* `make bench`: compile the current configuration, together with a script to test it
* `make kat_gen`: compile the current configuration, together with a script to generate *Known Answer Tests*
* `make clean`: clean the compiled files from the current configuration
* `make release`: extract the current configuration from the library into a standalone scheme in the folder `build`

## How to contribute to the library

### Adding a new MPC protocol

All the MPC protocols are implemented in the `mpc` folder. Each subfolder in `mpc` corresponds to a security assumption (for example, `mq` corresponds to the scheme relying on the multivariate quadratic problem). Then, in each folder that corresponds to a security assumption, there are the folders that contain the different schemes and MPC protocols (one folder per MPC protocol).

To implement a new scheme with a specific MPC protocol, you need to create a dedicated subfolder in the folder that corresponds to the right security assumption/one-way function. In the dedicated folder, you can create a `config` folder where you will put all the configuration files for your scheme (which will be detected by the command `make detect`).

> :information_source: section to complete

### Adding a new MPCitH transformation

The MPCitH transformations are implemented in the `core` folder. The Makefile of the library is independent of all the implemented MPCitH transformations. To create a MPCitH which will be handled by the library, you should create a sub `Makefile` like
```
CURRENT_MODE_NAME:=<MODE_NAME>
MODE_LIST:= $(MODE_LIST) $(CURRENT_MODE_NAME)
MODE_$(CURRENT_MODE_NAME)_PATH=<PATHS>
```
where `<MODE_NAME>` is the tag of the MPCitH transformation and `<PATHS>` is the list of the paths where the sources of the MPCitH transformation are. For example,
```
CURRENT_MODE_NAME:=TCITH_MT_5R
MODE_LIST:= $(MODE_LIST) $(CURRENT_MODE_NAME)
MODE_$(CURRENT_MODE_NAME)_PATH=api-sign,linear-mpc/5rounds/threshold/standard,trees/hash
```
is the declaration of the MPCitH transformation which corresponds to TCitH-MT for a signature scheme with 5 rounds. The tag `TCITH_MT_5R` will be used by all the schemes that want to rely on this transformation. `api-sign,linear-mpc/5rounds/threshold/standard,trees/hash` is the list where the source files of the transformation are. We can see that this transformation uses the source files of the Merkle trees and uses the API of the signature (mandatory for a transformation which corresponds to a signature scheme).

> :information_source: section to complete

## Contact

Developed and maintained by [Thibauld Feneuil](https://www.thibauld-feneuil.fr/)

## License

This project is licensed under the terms of Apache License (version 2.0).

Some schemes implemented in this library (in the folders `mpc`) rely on source files from external projects. These files are identified in the README files of the underlying schemes, together with their licences.
