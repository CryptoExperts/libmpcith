# Signature Scheme SDitH - MPC Protocol

Implementation of the MPC protocol underlying the SD-in-the-Head signature scheme. Compatible with the MPC-in-the-Head library.

Resources for SD-in-the-Head:
 * Website: https://sdith.org/
 * Specification: https://sdith.org/docs/sdith-v1.0.pdf
 * NIST Submission Package: https://csrc.nist.gov/csrc/media/Projects/pqc-dig-sig/documents/round-1/submission-pkg/SDitH-submission.zip

## Available Configurations

All the available configurations are in the folder `config`: their naming syntaxes are
```bash
<variant>-<field>-<level>
```
where
 * `<variant>` corresponds to the used MPCitH transformation: `traditional`, `hypercube`, `threshold-ggm`, `threshold-mt`;
 * `<field>` corresponds to the used field: `gf251` or `gf256`
 * `<level>` corresponds to the security level (according to the NIST categories): `l1`, `l3` or `l5`;

## LICENSE

All the files of this folder are under the same license as the MPC-in-the-Head library.
