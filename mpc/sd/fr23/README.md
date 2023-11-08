# [FR23]'s MPC Protocol for SD problem

Implementation of the [FR23]'s MPC protocol for SD problem. Compatible with the MPC-in-the-Head library.

Resources:
 * [FR23] Feneuil, Rivain. Threshold Computation in the Head: Improved Framework for Post-Quantum Signatures and Zero-Knowledge Arguments. [ePrint 2023/1573](https://eprint.iacr.org/2023/1573)

## Available Configurations

All the available configurations are in the folder `config`: their naming syntaxes are
```bash
<variant>-<field>-<level>
```
where
 * `<variant>` corresponds to the used MPCitH transformation: `threshold-ggm` or `threshold-mt`;
 * `<field>` corresponds to the used field: `gf251`;
 * `<level>` corresponds to the security level (according to the NIST categories): `l1`, `l3` or `l5`;

## LICENSE

All the files of this folder are under the same license as the MPC-in-the-Head library.
