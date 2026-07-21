# GenExLight Compilation

GenExLight builds the `evgen.exe` executable used to convert BdNMC dark matter flux into final-state dark trident events.

## Requirements

- A working ROOT environment.
- `root-config` must be available in your shell.
- ROOT must include the `Foam` and `EG` libraries.
- A C++17-capable compiler, such as `g++`.

Check the ROOT setup with:

```bash
root-config --version
```

If this command is not found, set up ROOT first before compiling GenExLight.

## Compile

Run these commands from the repository top directory:

```bash
cd GenExLight/TDecayTFoam
make TDecay.o

cd ..
bash compile.sh
```

This produces:

```text
GenExLight/evgen.exe
```

The `compile.sh` script assumes it is run from inside the `GenExLight` directory because it refers to `evgen.cxx` and `./TDecayTFoam/TDecay.o` with relative paths.

## Notes

- `compile.sh` is not executable by default, so use `bash compile.sh`.
- If `./TDecayTFoam/TDecay.o` is missing, build it first with `make TDecay.o` inside `GenExLight/TDecayTFoam`.
- The helper script `run_evgen.sh` contains hard-coded `/uboone/...` paths. Update those paths before using it in a local checkout.
