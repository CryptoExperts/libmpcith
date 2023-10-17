# Signature Scheme RYDE - MPC Protocol

Implementation of the MPC protocol underlying the RYDE signature scheme. Compatible with the MPC-in-the-Head library.

Resources for RYDE:
 * Website: https://pqc-ryde.org/
 * Specification: https://pqc-ryde.org/assets/downloads/ryde_spec.pdf
 * NIST Submission Package: https://csrc.nist.gov/csrc/media/Projects/pqc-dig-sig/documents/round-1/submission-pkg/ryde-submission.zip

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

The following files came from the NIST submission package of RYDE:
 * All the files in `rbc-31`:
   * `rbc-31/rbc_31_elt.(c/h)`
   * `rbc-31/rbc_31_mat.(c/h)`
   * `rbc-31/rbc_31_qpoly.(c/h)`
   * `rbc-31/rbc_31_vec.(c/h)`
   * `rbc-31/rbc_31_vspace.(c/h)`
   * `rbc-31/rbc_31.h`
 * All the files in `rbc-37`:
   * `rbc-37/rbc_37_elt.(c/h)`
   * `rbc-37/rbc_37_mat.(c/h)`
   * `rbc-37/rbc_37_qpoly.(c/h)`
   * `rbc-37/rbc_37_vec.(c/h)`
   * `rbc-37/rbc_37_vspace.(c/h)`
   * `rbc-37/rbc_37.h`
 * All the files in `rbc-43`:
   * `rbc-43/rbc_43_elt.(c/h)`
   * `rbc-43/rbc_43_mat.(c/h)`
   * `rbc-43/rbc_43_qpoly.(c/h)`
   * `rbc-43/rbc_43_vec.(c/h)`
   * `rbc-43/rbc_43_vspace.(c/h)`
   * `rbc-43/rbc_43.h`

All these files are in the public domain, as indicated [here](https://csrc.nist.gov/csrc/media/Projects/pqc-dig-sig/documents/ip-statements/mira-ip-statements.pdf). Some of these files **have been modified** to support the integration in the MPC-in-the-Head library.

Other files are on the same license as the MPC-in-the-Head library.
