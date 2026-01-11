import * as dgram from "dgram";
import Logger from "./Logger";
import HTTPServer from "./HTTPServer";
import StreamManager from "./StreamManager";
import Settings from "./Settings";

export class Discovery {
  ip: string;
  port: number;
  socket: dgram.Socket;
  timer: NodeJS.Timeout | null = null;
  clients: string[] = [];

  constructor(ip: string, port: number) {
    this.ip = ip;
    this.port = port;
    this.socket = dgram.createSocket({ type: "udp4", reuseAddr: true });

    this.socket.on("listening", () => {
      if (this.isMulticast(this.ip)) {
        this.socket.addMembership(this.ip);
      }
    });

    this.socket.bind(this.port, () => {
      this.socket.setMulticastTTL(1);
      this.socket.setMulticastLoopback(true);
      this.timer = setInterval(() => {
        Logger.info(`Sending discovery message at ${Date.now()}`);
        this.socket.send("discovery", this.port, this.ip);
      }, 1000);
    });

    HTTPServer.createServer();

    this.socket.on("message", (data, rInfo) => {
      if (data.toString() === "discovery") {
        return;
      }

      const event = data.toString();
      Logger.log("Got event: ", event);

      if (event === "start") {
        StreamManager.connect();
        Logger.log("Address: ", rInfo.address);
        Settings.clientIp = rInfo.address;
        Logger.info(`${rInfo.address} requested we start streaming`);
        this.stop();
        Logger.info(
          `Discovery service has been stopped. Communication is now over HTTP.`
        );
      }
    });
  }
  stop() {
    if (this.timer) {
      clearInterval(this.timer);
      this.timer = null;
    }
    this.socket.close();
  }
  private isMulticast(ip: string) {
    const split = ip.split(".");
    if (!split[0]) {
      return false;
    }
    const first = parseInt(split[0], 10);
    return first >= 224 && first <= 239;
  }
}
