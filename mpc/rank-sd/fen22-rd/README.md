# [Fen22]'s MPC Protocol based on Rank Decomposition

Implementation of the [Fen22]'s MPC protocol relying on rank decomposition for the rank syndrome decoding problem. Compatible with the MPC-in-the-Head library.

Resources:
 * [Fen22] Feneuil. Building MPCitH-based Signatures from MQ, MinRank, Rank SD and PKP. [ePrint 2022/1512](https://eprint.iacr.org/2022/1512)

## Available Configurations

All the available configurations are in the folder `config`: their naming syntaxes are
```bash
<variant>-<field>
```
where
 * `<variant>` corresponds to the used MPCitH transformation: `traditional` or `hypercube`;
 * `<field>` corresponds to the used field: `gf2to31`;

All the above configuration involved MPC protocols with 256 protocols. Some additional configurations with only 32 parties are available and used the suffix `-n32`.

## LICENSE

All the files of this folder are on the same license as the MPC-in-the-Head library.
