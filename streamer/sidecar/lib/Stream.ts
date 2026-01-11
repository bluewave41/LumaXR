import { spawn } from "child_process";
import { Direction } from "../interfaces/Direction";
import Logger from "./Logger";
import { Monitor } from "./Monitor";
import { EventEmitter } from "stream";
import Settings from "./Settings";

export class Stream extends EventEmitter {
  monitor: Monitor;
  senderPipeline: string;
  receiverPipeline: string;
  ip: string;
  port: number;
  direction: Direction;
  node: number;
  isVirtual: boolean;

  constructor(
    monitor: Monitor,
    senderPipeline: string,
    receiverPipeline: string,
    ip: string,
    port: number,
    direction: Direction,
    isVirtual: boolean
  ) {
    super();
    this.monitor = monitor;
    this.senderPipeline = senderPipeline;
    this.receiverPipeline = receiverPipeline;
    this.ip = ip;
    this.port = port;
    this.direction = direction;
    this.node = -1;
    this.isVirtual = isVirtual;
  }
  start(node: number) {
    Logger.log("Start called with: ", node);
    this.node = node;

    this.senderPipeline = this.senderPipeline
      .replace("{path}", node.toString())
      .replace("{port}", this.port.toString())
      .replace("{width}", this.monitor.width.toString())
      .replace("{height}", this.monitor.height.toString())
      .replace("{ip}", this.ip)
      .replace("{frameRate}", this.monitor.frameRate.toString());

    Logger.log("Starting GStreamer pipeline:", this.senderPipeline);

    const gstProcess = spawn(`gst-launch-1.0 ${this.senderPipeline}`, {
      stdio: "pipe",
      shell: true,
    });

    gstProcess.stdout.on("data", (data) => {
      Logger.log(data.toString());
    });

    gstProcess.stderr.on("data", (data) => {
      Logger.error(data.toString());
    });

    gstProcess.on("exit", (code, signal) => {
      //this.emit("closed", this);
      Logger.log("Stream exited: ", this.direction);
      Logger.log(`GStreamer exited with code ${code}, signal ${signal}`);
    });

    gstProcess.on("error", (err) => {
      Logger.error("Failed to start GStreamer:", err);
    });
  }
  toJSON() {
    return {
      width: this.monitor.width,
      height: this.monitor.height,
      senderPipeline: this.senderPipeline,
      receiverPipeline: this.receiverPipeline,
      port: this.port,
    };
  }
  async update(monitor: Monitor) {
    this.monitor = monitor;
    this.senderPipeline = Settings.senderPipeline;
  }
}
