# Signature Scheme MiRitH - MPC Protocol

Implementation of the MPC protocol underlying the MiRitH signature scheme. Compatible with the MPC-in-the-Head library.

Resources for MiRitH:
 * Website: https://pqc-mirith.org/
 * Specification: https://pqc-mirith.org/assets/downloads/mirith_specifications_v1.0.0.pdf
 * NIST Submission Package: https://csrc.nist.gov/csrc/media/Projects/pqc-dig-sig/documents/round-1/submission-pkg/mirith-submission.zip
 * GitHub: https://github.com/Crypto-TII/mirith_nist_submission

## Available Configurations

All the available configurations are in the folder `config`: their naming syntaxes are
```bash
<variant>-<level>-<inst>
```
where
 * `<variant>` corresponds to the used MPCitH transformation: `traditional`, `hypercube` or `threshold-ggm`;
 * `<level>` corresponds to the security level (according to the NIST categories): `l1a`, `l1b`, `l3a`, `l3b`, `l5a` or `l5b`;
 * `<inst>` corresponds to the used parameter sets: `short` or `fast`.

## LICENSE

The following files came from the NIST submission package of MiRitH:
 * `matrix_constants.h`
 * `matrix.c`
 * `matrix.h`

All these files are under the Apache license (version 2.0), as indicated in their headers. Some of these files **have been modified** to support the integration in the MPC-in-the-Head library.

The following files came from the NIST submission package of MiRitH and are in the public domain as indicated in their headers:
 * `blas_avx2.h`

Other files use the same license as the MPC-in-the-Head library.
