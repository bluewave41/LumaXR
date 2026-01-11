import dgram from "dgram";
import { Discovery } from "../lib/Discovery";

describe("Discovery", () => {
  let discovery: Discovery;

  afterEach(() => {
    discovery?.stop();
  });

  test("clients can connect to discovery server", (done) => {
    const multicastIp = "239.255.42.99";
    const port = 9999;

    discovery = new Discovery(multicastIp, port);

    const client = dgram.createSocket({ type: "udp4", reuseAddr: true });

    client.on("message", (msg) => {
      expect(msg.toString()).toBe("discovery");
      client.close();
      done();
    });

    client.bind(port, () => {
      client.addMembership(multicastIp);
    });
  }, 5000);
});
