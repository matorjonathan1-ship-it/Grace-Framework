import json
import os
import subprocess

def run_gpm():
    print("🚀 Grace Packet Manager (GPM) v0.1")
    if not os.path.exists('grace.json'):
        manifest = {"name": "Grace-Project", "dependencies": {}}
        with open('grace.json', 'w') as f:
            json.dump(manifest, f, indent=4)
        print("📝 Created default grace.json")

    with open('grace.json', 'r') as f:
        data = json.load(f)

    deps = data.get("dependencies", {})
    os.makedirs("packets", exist_ok=True)

    for name, url in deps.items():
        path = os.path.join("packets", name)
        if not os.path.exists(path):
            print(f"📦 Installing {name}...")
            subprocess.run(["git", "clone", url, path])
        else:
            print(f"✅ {name} already installed.")

if __name__ == "__main__":
    run_gpm()
