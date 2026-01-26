import Logger from "./Logger";
import Settings from "./Settings";
type PluginKeys = "create-monitor";
export interface Plugin {
    key: PluginKeys;
    env: string;
    exec: (logger: typeof Logger, settings: typeof Settings) => Promise<number>;
    activated: boolean;
}
type PluginReturnType = {
    "create-monitor": number;
};
export declare class InternalPluginManager {
    plugins: Plugin[];
    constructor();
    loadTsPlugin(path: string): Promise<any>;
    loadPlugins(): Promise<void>;
    runPlugin<K extends PluginKeys>(key: K): Promise<PluginReturnType[K]>;
    getPlugins(): Plugin[];
    activatePlugin(name: string): void;
}
declare const PluginManager: InternalPluginManager;
export default PluginManager;
//# sourceMappingURL=PluginManager.d.ts.map