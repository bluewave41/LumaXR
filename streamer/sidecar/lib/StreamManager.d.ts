import { EventEmitter } from "stream";
import { Direction } from "../interfaces/Direction";
import { Monitor } from "./Monitor";
import { Stream } from "./Stream";
export declare class InternalStreamManager extends EventEmitter {
    port: number;
    streams: Stream[];
    ws: WebSocket | null;
    resolutionEvents: EventEmitter | null;
    constructor();
    connect(): Promise<void>;
    createStream(monitor: Monitor, senderPipeline: string, receiverPipeline: string, ip: string, port: number, direction: Direction, isVirtual: boolean): Stream;
    getPort(): number;
    getStream(port: number): Stream | undefined;
    getStreams(): Stream[];
}
declare const StreamManager: InternalStreamManager;
export default StreamManager;
//# sourceMappingURL=StreamManager.d.ts.map