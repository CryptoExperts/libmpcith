# Signature Scheme Biscuit

Implementation of the MPC protocol underlying the Biscuit signature scheme. Compatible with the MPC-in-the-Head library.

Resources for Biscuit:
 * Website: https://www.biscuit-pqc.org/
 * Specification: https://csrc.nist.gov/csrc/media/Projects/pqc-dig-sig/documents/round-1/spec-files/Biscuit-spec-web.pdf
 * NIST Submission Package: https://csrc.nist.gov/csrc/media/Projects/pqc-dig-sig/documents/round-1/submission-pkg/Biscuit-submission.zip
 * Github: https://github.com/BiscuitTeam/Biscuit

## Available Configurations

All the available configurations are in the folder `config`: their naming syntaxes are
```bash
<variant>-<level>-<inst>
```
where
 * `<variant>` corresponds to the used MPCitH transformation: `traditional`, `hypercube` or `threshold-ggm`;
 * `<level>` corresponds to the security level (according to the NIST categories): `l1`;
 * `<inst>` corresponds to the used parameter sets: `short` or `fast`.

## LICENSE

All the files of this folder are under the same license as the MPC-in-the-Head library.
