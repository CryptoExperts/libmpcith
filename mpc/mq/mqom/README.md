# Signature Scheme MQOM - MPC Protocol

Implementation of the MPC protocol underlying the MQOM signature scheme. Compatible with the MPC-in-the-Head library.

Resources for SD-in-the-Head:
 * Website: https://mqom.org/
 * Specification: https://mqom.org/docs/mqom-v1.0.pdf
 * NIST Submission Package: https://csrc.nist.gov/csrc/media/Projects/pqc-dig-sig/documents/round-1/submission-pkg/mqom-submission.zip

## Available Configurations

All the available configurations are in the folder `config`: their naming syntaxes are
```bash
<variant>-<field>-<level>-<inst>
```
where
 * `<variant>` corresponds to the used MPCitH transformation: `traditional`, `hypercube`, or `threshold-ggm`;
 * `<field>` corresponds to the used field: `gf31` or `gf251`
 * `<level>` corresponds to the security level (according to the NIST categories): `l1`;
 * `<inst>` corresponds to the instance: `short` or `fast`;

## LICENSE

All the files of this folder are under the same license as the MPC-in-the-Head library.
