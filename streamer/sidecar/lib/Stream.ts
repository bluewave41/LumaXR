import { ChildProcessWithoutNullStreams, spawn } from "child_process";
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
  process: ChildProcessWithoutNullStreams | null = null;

  constructor(
    monitor: Monitor,
    senderPipeline: string,
    receiverPipeline: string,
    ip: string,
    port: number,
    direction: Direction,
    isVirtual: boolean,
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

    const args = this.senderPipeline
      .replace("{path}", node.toString())
      .replace("{port}", this.port.toString())
      .replace("{width}", this.monitor.width.toString())
      .replace("{height}", this.monitor.height.toString())
      .replace("{ip}", this.ip)
      .replace("{frameRate}", this.monitor.frameRate.toString())
      .split(" ");

    Logger.log("Starting GStreamer pipeline:", args);

    this.process = spawn(`gst-launch-1.0`, args, {
      stdio: "pipe",
    });

    this.process.stdout.on("data", (data) => {
      Logger.log(data.toString());
    });

    this.process.stderr.on("data", (data) => {
      Logger.error(data.toString());
    });

    this.process.on("exit", (code, signal) => {
      //this.emit("closed", this);
      Logger.log("Stream exited: ", this.direction);
      Logger.log(`GStreamer exited with code ${code}, signal ${signal}`);
    });

    this.process.on("error", (err) => {
      Logger.error("Failed to start GStreamer:", err);
    });
  }
  restart() {
    this.process?.kill("SIGTERM");
    this.start(this.node);
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
  update(monitor: Monitor) {
    this.monitor = monitor;
    this.senderPipeline = Settings.senderPipeline;
  }
}
