declare class InternalSettings {
    senderPipeline: string;
    receiverPipeline: string;
    virtualOnly: boolean;
    linuxMode: boolean;
    clientIp: string;
    constructor();
    getSetting<K extends keyof InternalSettings>(setting: K): InternalSettings[K];
    getSettings(): {
        senderPipeline: string;
        receiverPipeline: string;
        virtualOnly: boolean;
        linuxMode: boolean;
        clientIp: string;
    };
    saveSettings(settings: any): void;
}
declare const Settings: InternalSettings;
export default Settings;
//# sourceMappingURL=Settings.d.ts.map