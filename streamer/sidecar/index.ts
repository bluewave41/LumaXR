import { Discovery } from "./lib/Discovery";
import Logger from "./lib/Logger";
import PluginManager from "./lib/PluginManager";
import Settings from "./lib/Settings";
import StreamManager from "./lib/StreamManager";

let discovery: Discovery | null = null;

PluginManager.loadPlugins();

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
      case "stream-update":
        const stream = StreamManager.getStream(json.port);
        const monitor = stream?.monitor;
        monitor?.update(json.newWidth, json.newHeight);
        Logger.log("Monitor: " + monitor?.width + "x" + monitor?.height);
        stream?.restart();
        break;
      case "get-plugins":
        sendResponse(json._id, { plugins: PluginManager.getPlugins() });
        break;
      case "get-env":
        const env = (
          process.env.XDG_CURRENT_DESKTOP ||
          process.env.XDG_SESSION_DESKTOP ||
          process.env.DESKTOP_SESSION ||
          ""
        )
          .split(":")
          .map((d) => d.toLowerCase());
        sendResponse(json._id, { env });
        break;
      case "update-active-plugins":
        PluginManager.activatePlugin(json.name);
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
    }),
  );
};
