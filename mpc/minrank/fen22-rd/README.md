# [Fen22]'s MPC Protocol based on Rank Decomposition

Implementation of the [Fen22]'s MPC protocol relying on rank decomposition for the MinRank problem. Compatible with the MPC-in-the-Head library.

Resources:
 * [Fen22] Feneuil. Building MPCitH-based Signatures from MQ, MinRank, Rank SD and PKP. [ePrint 2022/1512](https://eprint.iacr.org/2022/1512)

## Available Configurations

All the available configurations are in the folder `config`: their naming syntaxes are
```bash
<variant>-<field>-<level>[suffix]
```
where
 * `<variant>` corresponds to the used MPCitH transformation: `traditional` or `hypercube`;
 * `<field>` corresponds to the used field: `gf16`;
 * `<level>` corresponds to the security level (according to the NIST categories): `l1`;
 * `[suffix]`: by default, the MPC protocol involves 256 parties. Adding the `-n32` to the configuration name changes this number to 32. Moreover, adding the suffix `-alt` activates an alternative MPC protocol relying on non-correlated Beaver triples.

## LICENSE

All the files of this folder are under the same license as the MPC-in-the-Head library.
