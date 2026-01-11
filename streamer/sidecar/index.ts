import { Discovery } from "./lib/Discovery";
import Logger from "./lib/Logger";
import Settings from "./lib/Settings";
import StreamManager from "./lib/StreamManager";

let discovery: Discovery | null = null;

const command = process.argv[2];
if (command === "start-discovery") {
  const ip = process.argv[3];
  const port = process.argv[4];
  if (ip !== undefined && port !== undefined) {
    discovery = new Discovery(ip, parseInt(port));
  }
}

process.stdin.on("data", (d) => {
  const message = d.toString();
  try {
    const json = JSON.parse(message);
    switch (json.type) {
      case "start-discovery":
        break;
      case "stop-discovery":
        if (discovery !== null) {
          (discovery as Discovery).stop();
        }
        break;

      case "read-logs":
        sendResponse(json._id, { logs: Logger.getLogs() });
        break;
      case "read-settings":
        Logger.log("Requested settings");
        Logger.log(JSON.stringify(Settings.getSettings()));
        sendResponse(json._id, {
          settings: Settings.getSettings(),
        });
        break;
      case "save-settings":
        Logger.log("Saved new settings");
        Logger.log(json.settings);
        Settings.saveSettings(json.settings);
        break;
      case "get-streams":
        sendResponse(json._id, { streams: StreamManager.getStreams() });
        break;
      case "debug-start-stream":
        fetch("http://localhost:4000/getPipeline");
        break;
    }
  } catch (e) {
    Logger.log("Non JSON request received: ", message);
  }
});

const sendResponse = (id: number, args: Record<string, any>) => {
  console.log(
    JSON.stringify({
      _id: id,
      ...args,
    })
  );
};
