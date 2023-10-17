# [Fen22]'s MPC Protocol based on Linearized Polynomials

Implementation of the [Fen22]'s MPC protocol relying on linearized polynomials for the MinRank problem. Compatible with the MPC-in-the-Head library.

Resources:
 * [Fen22] Feneuil. Building MPCitH-based Signatures from MQ, MinRank, Rank SD and PKP. [ePrint 2022/1512](https://eprint.iacr.org/2022/1512)

## Available Configurations

All the available configurations are in the folder `config`: their naming syntaxes are
```bash
<variant>-<field>-<level>
```
where:
 * `<variant>` corresponds to the used MPCitH transformation: `traditional`, `hypercube`, `threshold-ggm` or `threshold-mt`;
 * `<field>` corresponds to the used field: `gf16`, `gf251` or `gf256`;
 * `<level>` corresponds to the security level (according to the NIST categories): `l1`, `l3` or `l5`.

All the above configuration involved MPC protocols with 256 protocols. Some additional configurations with only 32 parties are available and used with the suffix `-n32`.

## LICENSE

All the files of this folder are under the same license as the MPC-in-the-Head library.
