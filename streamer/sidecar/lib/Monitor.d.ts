export declare class Monitor {
    connector: string;
    width: number;
    height: number;
    xOffset: number;
    yOffset: number;
    frameRate: number;
    isPrimary: boolean;
    constructor(connector: string, width: number, height: number, xOffset: number, yOffset: number, frameRate: number, isPrimary: boolean);
    update(newWidth: number, newHeight: number): void;
    equal(monitor: Monitor): boolean;
}
//# sourceMappingURL=Monitor.d.ts.map