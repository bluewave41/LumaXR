import { PUBLIC_DISCOVERY_IP, PUBLIC_DISCOVERY_PORT } from "$env/static/public";
import { Child, Command } from "@tauri-apps/plugin-shell";

class InternalSidecar {
  mainSidecar: Child | null = null;
  private nextRequestId = 1;
  private pendingPromises: Map<number, (data: string) => void> = new Map();

  async start() {
    if (!this.mainSidecar) {
      const command = Command.sidecar("binaries/sidecar", [
        "start-discovery",
        PUBLIC_DISCOVERY_IP,
        PUBLIC_DISCOVERY_PORT,
      ]);

      command.stdout.on("data", (d) => {
        const message = d.toString();
        try {
          const json = JSON.parse(message);
          if (json._id && this.pendingPromises.has(json._id)) {
            this.pendingPromises.get(json._id)!(json);
            this.pendingPromises.delete(json._id);
          }
        } catch (e) {}
      });
      command.stderr.on("data", (err) =>
        console.error("stderr:", err.toString())
      );

      this.mainSidecar = await command.spawn();
    }
  }
  get() {
    return this.mainSidecar;
  }
  write(type: string, args: Record<string, any> = {}): Promise<any> {
    if (this.mainSidecar === null) {
      return Promise.reject("Sidecar not initialized");
    }

    const id = this.nextRequestId++;
    const message = { _id: id, type, ...args };

    return new Promise((resolve) => {
      this.pendingPromises.set(id, resolve);
      this.mainSidecar!.write(JSON.stringify(message));
    });
  }
  stop() {
    if (this.mainSidecar) {
      this.mainSidecar.kill();
      this.mainSidecar = null;
    }
  }
}

const Sidecar = new InternalSidecar();
export default Sidecar;
