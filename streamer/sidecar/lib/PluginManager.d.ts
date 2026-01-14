import Logger from "./Logger";
import Settings from "./Settings";
type PluginKeys = "create-monitor";
interface Plugin {
    key: PluginKeys;
}
interface CreatePlugin extends Plugin {
    key: "create-monitor";
    exec: (logger: typeof Logger, settings: typeof Settings) => Promise<number>;
}
type AnyPlugin = CreatePlugin;
type PluginReturnType = {
    "create-monitor": number;
};
export declare class InternalPluginManager {
    plugins: Partial<Record<PluginKeys, AnyPlugin>>;
    constructor();
    loadTsPlugin(path: string): Promise<any>;
    loadPlugins(): Promise<void>;
    runPlugin<K extends PluginKeys>(key: K): Promise<PluginReturnType[K]>;
}
declare const PluginManager: InternalPluginManager;
export default PluginManager;
//# sourceMappingURL=PluginManager.d.ts.map