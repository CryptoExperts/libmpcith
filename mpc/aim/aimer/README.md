# Signature Scheme AIMer - MPC Protocol

Implementation of the MPC protocol underlying the AIMer signature scheme. Compatible with the MPC-in-the-Head library.

Resources for AIMer:
 * Website: https://aimer-signature.org/
 * Specification: https://aimer-signature.org/docs/AIMer-NIST-Document.pdf
 * NIST Submission Package: https://csrc.nist.gov/csrc/media/projects/pqc-dig-sig/documents/round-1/submission-pkg/AIMer-submission.zip

## Available Configurations

All the available configurations are in the folder `config`: their naming syntaxes are
```bash
<variant>-<level>-<inst>
```
where
 * `<variant>` corresponds to the used MPCitH transformation: `traditional` or `hypercube`;
 * `<level>` corresponds to the security level (according to the NIST categories): `l1`, `l3` or `l5`;
 * `<inst>` corresponds to the used parameter sets: `inst1`, `inst2`, `inst3`, `inst4`.

The parameter sets correspond to those in the [AIMER specification](https://csrc.nist.gov/csrc/media/Projects/pqc-dig-sig/documents/round-1/spec-files/AIMer-spec-web.pdf). Let us outline that the hypercube transformation is available only when the number `N` of parties is a power of 2 (namely, for `inst1` and `inst3`).

## LICENSE

The following files came from the NIST submission package of AIMer:
 * `aim_common.c.i`
 * `aim.h`
 * `field.h`
 * All the files in `aim128`:
   * `aim128/aim128.c`
   * `aim128/aim128.h`
   * `aim128/field128.c`
   * `aim128/field128.c`
 * All the files in `aim192`:
   * `aim192/aim192.c`
   * `aim192/aim192.h`
   * `aim192/field192.c`
   * `aim192/field192.c`
 * All the files in `aim256`:
   * `aim256/aim256.c`
   * `aim256/aim256.h`
   * `aim256/field256.c`
   * `aim256/field256.c`

All these files are under the MIT license, as indicated in their headers. Some of these files **have been modified** to support the integration in the MPC-in-the-Head library.

The following files came from the NIST submission package of AIMer and are in the public domain as indicated in their headers:
 * `portable_endian.h`

Other files are under the same license as the MPC-in-the-Head library.
