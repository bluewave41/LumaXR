declare class InternalLogger {
    logs: any[];
    constructor();
    info(...message: any[]): void;
    log(...message: any[]): void;
    debug(...message: any[]): void;
    error(...message: any[]): void;
    concat(str: any): void;
    getLogs(): string;
}
declare const Logger: InternalLogger;
export default Logger;
//# sourceMappingURL=Logger.d.ts.map