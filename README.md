# outbreak
The high-level idea of this project is to create a small interactive web-based experience that simulates a population over several weeks to months given a few user-determined parameters that control the behavior of individuals and the government.

More details:
https://docs.google.com/document/d/1zyOM_xf-3XBRUZb9_MQ0FgydpXCSgonVsBYxfLFbJo0/edit#heading=h.cb12u9pln7y

## How to build and run
tl;dr: clone the repository, `cd` into it, run `run_server.sh`, and navigate to `http://localhost:8000`.

### A tiny bit more detail
I don't have deep knowledge about build systems outside of my employer's ecosystem, so for now here are two shell scripts to get up and running quickly:
* `build_cc.sh` rebuilds `gen/index.wasm` and `gen/index.js` based on the C++ simulation code in `src/cc`. Because that can potentially be a pain (you'll need to install emscripten, etc.), a version of these files is already checked in.
* `run_server.sh` copies everything from `src/web` (plus some third-party dependencies in the `contrib` folder) to the `gen` directory, and then runs a Python SimpleHTTPServer within that directory.
