# [FR23]'s MPC Protocol for SD problem for Ring Signatures

Implementation of the [FR23]'s MPC protocol for SD problem to build a ring signature scheme. Compatible with the MPC-in-the-Head library.

Resources:
 * [FR23] Feneuil, Rivain. Threshold Computation in the Head: Improved Framework for Post-Quantum Signatures and Zero-Knowledge Arguments. [ePrint 2023/1573](https://eprint.iacr.org/2023/1573)

## Available Configurations

All the available configurations are in the folder `config`: their naming syntaxes are
```bash
<variant>-<field>-<level>-<seldim>
```
where
 * `<variant>` corresponds to the used MPCitH transformation: `threshold-ggm`;
 * `<field>` corresponds to the used field: `gf251`;
 * `<level>` corresponds to the security level (according to the NIST categories): `l1`;
 * `<seldim>` corresponds to the dimension of the selector (the part of the witness which encodes which user is signing): `sel1`, `sel2`, `sel3` or `sel4`.

## LICENSE

All the files of this folder are under the same license as the MPC-in-the-Head library.
