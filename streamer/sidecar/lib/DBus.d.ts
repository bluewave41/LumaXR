import { Monitor } from "./Monitor";
import { EventEmitter } from "stream";
declare const bus: any;
export declare function createSession(): Promise<any>;
export declare function recordMonitor(virtual: boolean, port: number): Promise<void>;
export declare function watchResolutionChanges(): Promise<EventEmitter<[never]>>;
export declare function getMonitors(): Promise<Monitor[]>;
export { bus };
//# sourceMappingURL=DBus.d.ts.map