import { Variant, sessionBus } from "dbus-next";

interface Logger {
  info: (...message: any[]) => void;
  log: (...message: any[]) => void;
  debug: (...message: any[]) => void;
  error: (...message: any[]) => void;
}

const bus = (sessionBus as any)({ negotiateUnixFd: true });

module.exports = {
  key: "create-monitor",

  /**
   * This function should create a virtual monitor and return a pipewire
   * node coresponding to the stream to record.
   */
  async exec(logger: Logger) {
    logger.log("Plugin running!");
    const nodeId = await recordMonitor(true);
    logger.log("Piugin node: ", nodeId);
    return nodeId;
  },
};

async function createSession() {
  const screenCast = await bus.getProxyObject(
    "org.gnome.Mutter.ScreenCast",
    "/org/gnome/Mutter/ScreenCast"
  );

  const screenCastIface = screenCast.getInterface(
    "org.gnome.Mutter.ScreenCast"
  );

  const sessionPath = await screenCastIface.CreateSession({});

  const sessionObj = await bus.getProxyObject(
    "org.gnome.Mutter.ScreenCast",
    sessionPath
  );

  const sessionIface = sessionObj.getInterface(
    "org.gnome.Mutter.ScreenCast.Session"
  );

  return sessionIface;
}

async function recordMonitor(virtual: boolean) {
  const sessionIface = await createSession();
  let streamPath;

  if (virtual) {
    streamPath = await sessionIface.RecordVirtual({
      "cursor-mode": new Variant("u", 1),
    });
  } else {
    streamPath = await sessionIface.RecordMonitor("DP-1", {
      "cursor-mode": new Variant("u", 1),
    });
  }

  const streamObj = await bus.getProxyObject(
    "org.gnome.Mutter.ScreenCast",
    streamPath
  );

  const streamIface = streamObj.getInterface(
    "org.gnome.Mutter.ScreenCast.Stream"
  );

  const nodeIdPromise = new Promise((resolve, reject) => {
    const timeout = setTimeout(() => {
      reject(new Error("Timeout waiting for PipeWireStreamAdded signal"));
    }, 5000);

    streamIface.on("PipeWireStreamAdded", (nodeId: number) => {
      clearTimeout(timeout);
      resolve(nodeId);
    });
  });

  await sessionIface.Start();

  return (await nodeIdPromise) as number;
}
