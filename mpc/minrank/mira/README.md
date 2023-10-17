# Signature Scheme MIRA - MPC Protocol

Implementation of the MPC protocol underlying the MIRA signature scheme. Compatible with the MPC-in-the-Head library.

Resources for MIRA:
 * Website: https://pqc-mira.org/
 * Specification: https://pqc-mira.org/assets/downloads/mira_spec.pdf
 * NIST Submission Package: https://csrc.nist.gov/csrc/media/Projects/pqc-dig-sig/documents/round-1/submission-pkg/MIRA-submission.zip

## Available Configurations

All the available configurations are in the folder `config`: their naming syntaxes are
```bash
<variant>-<level>-<inst>
```
where
 * `<variant>` corresponds to the used MPCitH transformation: `traditional`, `hypercube` or `threshold-ggm`;
 * `<level>` corresponds to the security level (according to the NIST categories): `l1`, `l3` or `l5`;
 * `<inst>` corresponds to the used parameter sets: `short` or `fast`.

## LICENSE

The following files came from the NIST submission package of MIRA:
 * All the files in `mira128`:
   * `mira128/finite_fields.c`
   * `mira128/finite_fields.h`
 * All the files in `mira192`:
   * `mira192/finite_fields.c`
   * `mira192/finite_fields.h`
 * All the files in `mira256`:
   * `mira256/finite_fields.c`
   * `mira256/finite_fields.h`

All these files are in the public domain, as indicated [here](https://csrc.nist.gov/csrc/media/Projects/pqc-dig-sig/documents/ip-statements/mira-ip-statements.pdf). Some of these files **have been modified** to support the integration in the MPC-in-the-Head library.

Other files are under the same license as the MPC-in-the-Head library.
