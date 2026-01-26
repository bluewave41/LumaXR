import { Variant, sessionBus, Message } from "dbus-next";

interface Logger {
  info: (...message: any[]) => void;
  log: (...message: any[]) => void;
  debug: (...message: any[]) => void;
  error: (...message: any[]) => void;
}

interface Settings {
  senderPipeline: string;
  receiverPipeline: string;
  virtualOnly: boolean;
  linuxMode: boolean;
}

const bus = (sessionBus as any)({ negotiateUnixFd: true });
let requestTokenCounter = 0;
let sessionTokenCounter = 0;

module.exports = {
  key: "create-monitor",

  /**
   * This function should create a virtual monitor and return a pipewire
   * node coresponding to the stream to record.
   */
  async exec(logger: Logger, settings: Settings) {
    logger.log("Plugin running!");
    logger.log("Hello from any!");
    logger.log("Settings: ", settings.virtualOnly);
    const nodeId = await recordMonitor();
    logger.log("Piugin node: ", nodeId);
    return nodeId;
  },
};

async function createSession() {
  const portal = await bus.getProxyObject(
    "org.freedesktop.portal.Desktop",
    "/org/freedesktop/portal/desktop",
  );

  const { sessionToken } = newSessionPath(
    (portal.bus as any).name.slice(1).replaceAll(".", "_"),
  );
  const { requestToken } = newRequestPath(
    (portal.bus as any).name.slice(1).replaceAll(".", "_"),
  );

  const screenCastIface = portal.getInterface(
    "org.freedesktop.portal.ScreenCast",
  );

  await screenCastIface.CreateSession({
    handle_token: new Variant("s", requestToken),
    session_handle_token: new Variant("s", sessionToken),
    cursor_mode: new Variant("u", 2),
  });

  const session = await listenForMessage(
    "org.freedesktop.portal.Request",
    "Response",
  );

  return {
    screenCastIface,
    session: session.body[1].session_handle.value,
    requestToken,
    sessionToken,
  };
}

async function recordMonitor() {
  const { screenCastIface, session, requestToken } = await createSession();

  await screenCastIface.SelectSources(session, {
    handleToken: new Variant("s", requestToken),
    types: new Variant("u", 4), // 4 = virtual
  });

  await screenCastIface.Start(session, "", {
    handle_token: new Variant("s", requestToken),
  });

  const streamsResponse = await listenForMessage(
    "org.freedesktop.portal.Request",
    "Response",
  );

  return streamsResponse.body[1].streams.value[0][0];
}

async function listenForMessage(iface: string, member: string) {
  return new Promise<Message>((resolve, reject) => {
    bus.once("message", (msg: any) => {
      if (msg.type === 4 && msg.interface === iface && msg.member === member) {
        resolve(msg);
      }
    });
  });
}

function newRequestPath(sessionName: string) {
  requestTokenCounter++;
  const requestToken = `u${requestTokenCounter}`;
  const requestPath = `/org/freedesktop/portal/desktop/request/${sessionName}/${requestToken}`;
  return { requestToken, requestPath };
}

function newSessionPath(sessionName: string) {
  sessionTokenCounter++;
  const sessionToken = `u${sessionTokenCounter}`;
  const sessionPath = `/org/freedesktop/portal/desktop/session/${sessionName}/${sessionToken}`;
  return { sessionToken, sessionPath };
}
