import { Variant, sessionBus } from "dbus-next";
import Logger from "./Logger";
import StreamManager from "./StreamManager";
import { Monitor } from "./Monitor";
import { EventEmitter } from "stream";

//https://gitlab.gnome.org/GNOME/mutter/-/blob/gnome-40/src/org.gnome.Mutter.ScreenCast.xml

const bus = (sessionBus as any)({ negotiateUnixFd: true });

let monitors: Monitor[] = [];

export async function createSession() {
  const screenCast = await bus.getProxyObject(
    "org.gnome.Mutter.ScreenCast",
    "/org/gnome/Mutter/ScreenCast"
  );

  const screenCastIface = screenCast.getInterface(
    "org.gnome.Mutter.ScreenCast"
  );

  const sessionPath = await screenCastIface.CreateSession({});
  Logger.log("Session path:", sessionPath);

  const sessionObj = await bus.getProxyObject(
    "org.gnome.Mutter.ScreenCast",
    sessionPath
  );

  const sessionIface = sessionObj.getInterface(
    "org.gnome.Mutter.ScreenCast.Session"
  );

  return sessionIface;
}

export async function recordMonitor(virtual: boolean, port: number) {
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

  // Get the stream object
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
      Logger.log("PipeWireStreamAdded signal received with node ID:", nodeId);
      clearTimeout(timeout);
      resolve(nodeId);
    });
  });

  await sessionIface.Start();
  Logger.log("Recording started, waiting for signal...");

  const nodeId = (await nodeIdPromise) as number;
  Logger.log("PipeWire node ID:", nodeId);

  const stream = StreamManager.getStream(port);
  stream?.start(nodeId);
}

export async function watchResolutionChanges() {
  const emitter = new EventEmitter();
  // set the initial monitors so we can detect which changed
  monitors = await getMonitors();

  const displayConfig = await bus.getProxyObject(
    "org.gnome.Mutter.DisplayConfig",
    "/org/gnome/Mutter/DisplayConfig"
  );

  const displayIFace = displayConfig.getInterface(
    "org.gnome.Mutter.DisplayConfig"
  );

  displayIFace.on("MonitorsChanged", () => {
    console.log("MonitorsChanged fired");

    displayIFace.GetCurrentState().then((state: any) => {
      let tempMonitors: Monitor[] = [];
      let newMonitors = state[1];
      for (const monitor of newMonitors) {
        Logger.log("Monitor", JSON.stringify(monitor));
        const info = monitor[1].find(
          (el: any) => el[6]["is-current"] !== undefined
        );
        if (info) {
          tempMonitors.push(
            new Monitor(monitor[0][0], info[1], info[2], 0, 0, info[3], false)
          );
        }
      }
      const activeMonitors = state[2];
      for (var i = 0; i < activeMonitors.length; i++) {
        const monitor = tempMonitors[i];
        if (monitor) {
          monitor.xOffset = activeMonitors[i][0];
          monitor.yOffset = activeMonitors[i][1];
          monitor.isPrimary = activeMonitors[i][4];
        }
      }

      const changedMonitors: Monitor[] = [];
      for (const monitor of tempMonitors) {
        const index = monitors.findIndex(
          (el) => el.connector === monitor.connector
        );
        const oldMonitor = monitors[index];
        if (oldMonitor && !monitor.equal(oldMonitor)) {
          changedMonitors.push(monitor);
          monitors[index] = monitor;
        }
      }

      if (changedMonitors.length > 0) {
        emitter.emit("changed", changedMonitors);
      }
    });
  });

  return emitter;
}

export async function getMonitors() {
  const displayConfig = await bus.getProxyObject(
    "org.gnome.Mutter.DisplayConfig",
    "/org/gnome/Mutter/DisplayConfig"
  );

  const displayIFace = displayConfig.getInterface(
    "org.gnome.Mutter.DisplayConfig"
  );

  const newMonitors = await displayIFace.GetCurrentState();

  let tempMonitors: Monitor[] = [];
  for (const monitor of newMonitors[1]) {
    const info = monitor[1][0];
    tempMonitors.push(
      new Monitor(monitor[0][0], info[1], info[2], 0, 0, info[3], false)
    );
  }

  const activeMonitors = newMonitors[2];
  for (var i = 0; i < activeMonitors.length; i++) {
    const monitor = tempMonitors[i];
    if (monitor) {
      monitor.xOffset = activeMonitors[i][0];
      monitor.yOffset = activeMonitors[i][1];
      monitor.isPrimary = activeMonitors[i][4];
    }
  }

  return tempMonitors;
}

export { bus };
