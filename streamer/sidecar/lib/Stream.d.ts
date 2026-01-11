import { Direction } from "../interfaces/Direction";
import { Monitor } from "./Monitor";
import { EventEmitter } from "stream";
export declare class Stream extends EventEmitter {
    monitor: Monitor;
    senderPipeline: string;
    receiverPipeline: string;
    ip: string;
    port: number;
    direction: Direction;
    node: number;
    isVirtual: boolean;
    constructor(monitor: Monitor, senderPipeline: string, receiverPipeline: string, ip: string, port: number, direction: Direction, isVirtual: boolean);
    start(node: number): void;
    toJSON(): {
        width: number;
        height: number;
        senderPipeline: string;
        receiverPipeline: string;
        port: number;
    };
    update(monitor: Monitor): Promise<void>;
}
//# sourceMappingURL=Stream.d.ts.map