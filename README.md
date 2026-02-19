# Unreal ML Adapter Glue

This repository provides a minimal **glue layer** that connects Unreal Engine to external Machine‑Learning processes via a simple socket‑based RPC protocol. It demonstrates how to:

1. **Expose actor state** (transform, location, custom variables) from Unreal to a Python/ONNX process.
2. **Receive predictions** (e.g., movement vectors, animation parameters) back into Unreal and apply them to actors.
3. **Swap models** on‑the‑fly by restarting the external process – no need to rebuild the plugin.

---

## Repository Structure

```
├─ Plugin/                # Unreal Engine plugin source (C++)
│   ├─ Source/
│   │   ├─ MLAdapter/
│   │   │   ├─ MLAdapter.cpp
│   │   │   ├─ MLAdapter.h
│   │   │   └─ MLAdapterModule.cpp
│   │   └─ MLAdapter.Build.cs
│   └─ MLAdapter.uplugin   # Plugin descriptor
├─ external/               # Sample external script (Python)
│   └─ inference.py        # Simple JSON‑over‑TCP demo
└─ README.md
```

---

## Getting Started

### Prerequisites

- **Unreal Engine 5.4+** (tested on 5.4.1)
- **C++ Development Environment** (Xcode on macOS, Visual Studio on Windows)
- **Python 3.10+** (for the example inference script)
- **Git** and **GitHub CLI** (already configured on this machine)

### Step 1 – Install the Plugin in Your Project

1. Clone this repo into your project’s `Plugins/` folder:
   ```bash
   cd /path/to/YourUnrealProject/Plugins
   git clone https://github.com/AgileLens/unreal-ml-adapter-glue.git
   ```
2. Open your `.uproject` file in Unreal Editor and enable the **MLAdapter** plugin (Editor → Plugins → Installed → MLAdapter).
3. Restart the editor – the plugin will start a TCP server on `localhost:5555` the first time a level loads.

### Step 2 – Run the External Inference Process

```bash
cd /path/to/YourUnrealProject/Plugins/unreal-ml-adapter-glue/external
python3 inference.py
```
The script connects to the UE socket, receives actor JSON, performs a dummy “prediction” (adds a small random offset), and sends it back.

### Step 3 – Use the API in Blueprint or C++

The plugin exposes a Blueprint node **`SendActorState`** and a C++ helper `FMLAdapter::SendState`. Example Blueprint flow:

1. **Event Tick** → `SendActorState` (target actor). 
2. The node returns a **`Prediction`** struct you can apply to the actor’s location/rotation.

---

## Customizing the Protocol

The JSON schema is deliberately simple:
```json
{ "actor": "MyCharacter", "position": {"x":0,"y":0,"z":0}, "variables": {"health":100} }
```
You can extend it by editing `MLAdapter.h` – just keep the socket read/write logic unchanged.

## FAQ

- **Q:** *Do I need to rebuild the plugin for every model change?*
  **A:** No. The model runs outside UE; you only need to restart the external script.
- **Q:** *Is this production‑ready?*
  **A:** It’s a proof‑of‑concept. For production you’d want proper message framing, authentication, and error handling – easy to add on top of this scaffold.

---

## License

MIT – feel free to fork, improve, and share!
