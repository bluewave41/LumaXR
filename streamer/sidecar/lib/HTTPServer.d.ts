import express from "express";
export declare class InternalHTTPServer {
    server: express.Express | undefined;
    constructor();
    createServer(): void;
}
declare const HTTPServer: InternalHTTPServer;
export default HTTPServer;
//# sourceMappingURL=HTTPServer.d.ts.map