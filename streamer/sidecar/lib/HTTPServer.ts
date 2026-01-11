import express from "express";
import Settings from "./Settings";
import { getMonitors, recordMonitor } from "./DBus";
import StreamManager from "./StreamManager";
import Logger from "./Logger";
import { Monitor } from "./Monitor";

export class InternalHTTPServer {
  server: express.Express | undefined;

  constructor() {}
  createServer() {
    this.server = express();
    this.server.use(express.json());

    this.server.get("/getPipeline", async (req, res) => {
      let clientIp = req.ip as string;
      if (clientIp.startsWith("::ffff:")) {
        clientIp = clientIp.replace("::ffff:", "");
      }

      Logger.log(`Get pipeline hit from: ${clientIp}`);
      Logger.log(`Linux mode: ${Settings.linuxMode}`);

      let monitor;
      if (Settings.virtualOnly) {
        monitor = new Monitor("any", 1920, 1080, 0, 0, 120, true);
      } else {
        const monitors = await getMonitors();
        monitor = monitors[0];
        if (monitor === undefined) {
          return res.end();
        }
      }

      let receiverPipeline = Settings.receiverPipeline;
      if (Settings.linuxMode) {
        Logger.log("Fixing stream for linux mode...");
        receiverPipeline = `udpsrc port=%d buffer-size=4194304 caps="application/x-rtp, media=video, encoding-name=H264, payload=96" ! rtpjitterbuffer latency=30 ! queue ! rtph264depay ! h264parse ! nvh264dec ! videoconvert ! video/x-raw,format=RGBA ! appsink name=unity emit-signals=true`;
      }

      const stream = StreamManager.createStream(
        monitor,
        Settings.senderPipeline,
        receiverPipeline,
        clientIp,
        StreamManager.getPort(),
        "CENTER",
        Settings.virtualOnly
      );

      Logger.log("Virtual?: ", Settings.virtualOnly);

      recordMonitor(stream.isVirtual, stream.port);
      res.json(stream.toJSON());
    });

    this.server.post("/createMonitor", async (req, res) => {
      let clientIp = req.ip as string;
      if (clientIp.startsWith("::ffff:")) {
        clientIp = clientIp.replace("::ffff:", "");
      }

      const direction = req.body.direction;
      const monitor = new Monitor("any", 1920, 1080, 0, 0, 120, false);

      Logger.log("120 FPS START");

      const center = StreamManager.getStream(3555);
      if (!center) {
        return;
      }

      /*switch (direction) {
        case "LEFT":
          monitor.xOffset -= center?.width;
          break;
        case "RIGHT":
          monitor.xOffset += center?.width;
          break;
      }*/

      let receiverPipeline = Settings.receiverPipeline;
      if (Settings.linuxMode) {
        Logger.log("Fixing stream for linux mode...");
        receiverPipeline = `udpsrc port=%d buffer-size=4194304 caps="application/x-rtp, media=video, encoding-name=H264, payload=96" ! rtpjitterbuffer latency=30 ! queue ! rtph264depay ! h264parse ! nvh264dec ! videoconvert ! video/x-raw,format=RGBA ! appsink name=unity emit-signals=true`;
      }

      Logger.log(JSON.stringify(monitor));

      Logger.log("Received create request: " + direction);
      const stream = StreamManager.createStream(
        monitor,
        Settings.senderPipeline,
        receiverPipeline,
        clientIp,
        StreamManager.getPort(),
        direction,
        true
      );
      recordMonitor(stream.isVirtual, stream.port);
      res.json(stream.toJSON());
    });

    this.server.listen(4000);
  }
}

const HTTPServer = new InternalHTTPServer();
export default HTTPServer;
