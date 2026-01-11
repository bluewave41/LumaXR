import * as dgram from "dgram";
export declare class Discovery {
    ip: string;
    port: number;
    socket: dgram.Socket;
    timer: NodeJS.Timeout | null;
    clients: string[];
    constructor(ip: string, port: number);
    stop(): void;
    private isMulticast;
}
//# sourceMappingURL=Discovery.d.ts.map