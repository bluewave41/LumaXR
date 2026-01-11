import { EventEmitter } from "stream";
import { Direction } from "../interfaces/Direction";
import Logger from "./Logger";
import { Monitor } from "./Monitor";
import { Stream } from "./Stream";
import { recordMonitor, watchResolutionChanges } from "./DBus";

export class InternalStreamManager extends EventEmitter {
  port = 3555;
  streams: Stream[] = [];
  ws: WebSocket | null = null;
  resolutionEvents: EventEmitter | null = null;

  constructor() {
    super();
  }
  async connect() {
    this.ws = new WebSocket("ws://127.0.0.1:8572/stream-close");
    this.ws.onopen = () => {
      Logger.log("Close path open!");
    };
    this.resolutionEvents = await watchResolutionChanges();
    this.resolutionEvents.on("changed", (newMonitors: Monitor[]) => {
      // update the stream
      // get a new pipewire node
      // start the new stream

      for (const newMonitor of newMonitors) {
        const stream = this.streams.find(
          (stream) => stream.monitor.connector == newMonitor.connector
        );
        if (stream && this.ws) {
          stream.update(newMonitor);
          recordMonitor(stream.isVirtual, stream.port);
          const message = {
            width: stream.monitor.width,
            height: stream.monitor.height,
            port: stream.port,
            pipeline: stream.receiverPipeline,
          };
          this.ws.send(JSON.stringify(message));
        }
      }
      Logger.log("Stream manager got changes", JSON.stringify(newMonitors));
    });
  }
  createStream(
    monitor: Monitor,
    senderPipeline: string,
    receiverPipeline: string,
    ip: string,
    port: number,
    direction: Direction,
    isVirtual: boolean
  ) {
    Logger.log("Created a stream");
    const stream = new Stream(
      monitor,
      senderPipeline,
      receiverPipeline,
      ip,
      port,
      direction,
      isVirtual
    );
    this.streams.push(stream);
    return stream;
  }
  getPort() {
    return this.port++;
  }
  getStream(port: number) {
    return this.streams.find((stream) => stream.port === port);
  }
  getStreams() {
    Logger.log("Streams: ", this.streams);
    return this.streams;
  }
}

const StreamManager = new InternalStreamManager();
export default StreamManager;
