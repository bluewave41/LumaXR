/*import { spawn } from "child_process";
import { Monitor } from "./Monitor";
import Logger from "./Logger";

export class InternalMonitorHandler {
  getMonitors = (): Promise<Monitor[]> => {
    let output = "";
    const monitors: Monitor[] = [];

    return new Promise((resolve, reject) => {
      const xrandr = spawn("xrandr", ["--listmonitors"]);
      xrandr.stdout.on("data", (d: Buffer) => (output += d.toString()));
      xrandr.on("close", () => {
        const regex = /(\d+)\/\d+x(\d+)\/\d+\+(\d+)\+(\d+)  (.*)/g;
        let match: any;
        while ((match = regex.exec(output))) {
          monitors.push(
            new Monitor(
              parseInt(match[1]),
              parseInt(match[2]),
              parseInt(match[3]),
              parseInt(match[4]),
              "60"
            )
          );
        }
        resolve(monitors);
      });
    });
  };

  getConnectedMonitors = (): Promise<Monitor[]> => {
    let output = "";
    const monitors: Monitor[] = [];

    return new Promise((resolve, reject) => {
      const xrandr = spawn("bash", ["-c", 'xrandr | grep " connected" -A1']);
      xrandr.stdout.on("data", (d: Buffer) => (output += d.toString()));
      xrandr.on("close", () => {
        const regex =
          /(\d+)x(\d+)\+(\d+)\+(\d+).*\n\s{3}\d+x\d+\s{5}(\d+\.\d+)/g;
        let match: any;
        while ((match = regex.exec(output))) {
          Logger.log(match);
          monitors.push(
            new Monitor(
              parseInt(match[1]),
              parseInt(match[2]),
              parseInt(match[3]),
              parseInt(match[4]),
              match[5]
            )
          );
        }
        Logger.log(JSON.stringify(monitors));
        resolve(monitors);
      });
    });
  };

  createVirtualMonitor(
    width: number,
    height: number,
    xOffset: number,
    yOffset: number
  ) {
    return {
      width,
      height,
      xOffset,
      yOffset,
    };
  }
}

const MonitorHandler = new InternalMonitorHandler();
export default MonitorHandler;*/
