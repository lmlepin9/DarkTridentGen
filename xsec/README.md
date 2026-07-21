# Cross Section Generation

The `xsec` directory builds and runs the dark trident cross-section calculators used by GenExLight. The output files are ROOT files containing a graph named `gxsec`.

## Requirements

- A working ROOT environment.
- `root-config` must be available in your shell.
- ROOT must include the `Foam` and `EG` libraries.
- `GenExLight/TDecayTFoam/TDecay.o` must already be built.

Check ROOT with:

```bash
root-config --version
```

If `root-config` is not found, set up ROOT before compiling.

## Build the GenExLight Helper Object

Run this once from the repository top directory:

```bash
cd GenExLight/TDecayTFoam
make TDecay.o
```

This creates:

```text
GenExLight/TDecayTFoam/TDecay.o
```

## Compile a Cross-Section Executable

The compile script uses the `dm_type` variable in `compile_xsec.sh`.

For fermion dark matter:

```bash
dm_type="fermion"
```

For scalar dark matter:

```bash
dm_type="scalar"
```

Then return to the repository top directory and compile from `xsec`:

```bash
cd ../../xsec  # only needed if you are still in GenExLight/TDecayTFoam
bash compile_xsec.sh
```

Depending on `dm_type`, this produces one of:

```text
xsec/fermion_xsec.exe
xsec/scalar_xsec.exe
```

The script assumes it is run from inside `xsec` because it refers to source files and `../GenExLight/TDecayTFoam/TDecay.o` with relative paths.

## Generate Cross-Section ROOT Files

Edit `xsec_loop.sh` so `DM_TYPE` matches the executable you built:

```bash
DM_TYPE="fermion"
```

or:

```bash
DM_TYPE="scalar"
```

Then run:

```bash
bash xsec_loop.sh
```

The loop uses the mass points listed in `mass_points` and the value of `RATIO`.

Typical outputs are:

```text
cross_section_0.05_ratio_0.60_fermion_fix.root
cross_section_0.05_ratio_0.60_scalar_fix.root
```

These files are consumed by GenExLight through the `-x` argument.

## Single-Point Run

After compiling, a single mass point can be generated directly:

```bash
./fermion_xsec.exe -m 0.05 -r 0.60
```

or:

```bash
./scalar_xsec.exe -m 0.05 -r 0.60
```

Here `-m` is the dark photon mass and `-r` is the dark matter to dark photon mass ratio.

## Notes

- `compile_xsec.sh` and `xsec_loop.sh` are not executable by default, so run them with `bash`.
- `compile_xsec.sh` must succeed before `xsec_loop.sh` can run.
- Keep `compile_xsec.sh`'s `dm_type` and `xsec_loop.sh`'s `DM_TYPE` in sync.
