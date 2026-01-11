import Logger from "./Logger";

type ClientBroadcast = {
  event: "client-broadcast";
  payload: {
    ip: string;
  };
};

type SocketEvent = ClientBroadcast;

export class Packet {
  constructor(data: Buffer) {
    let json: SocketEvent;
    try {
      json = JSON.parse(data.toString());
    } catch (e) {
      Logger.error(`Failed to parse JSON body`, data.toString());
      throw new Error("Failed to parse JSON body.");
    }

    switch (json.event) {
      case "client-broadcast":
        console.log(event);
        break;
    }
  }
}
