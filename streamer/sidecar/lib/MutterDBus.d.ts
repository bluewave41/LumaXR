interface Stream {
    port: number;
    stream: any;
    fd: number;
    restoreToken: string;
    width?: number;
    height?: number;
    physical: boolean;
}
declare const streams: Stream[];
declare const bus: any;
export declare function createSession(): Promise<any>;
export declare function recordMonitor(virtual: boolean, port: number): Promise<void>;
export { bus, streams };
//# sourceMappingURL=MutterDBus.d.ts.map