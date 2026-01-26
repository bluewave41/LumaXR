import { promises as fs } from "fs";
import Logger from "./Logger";
import { build } from "esbuild";
import path from "path";
import Settings from "./Settings";

type PluginKeys = "create-monitor";

const keys: PluginKeys[] = ["create-monitor"];

export interface Plugin {
  key: PluginKeys;
  env: string;
  exec: (logger: typeof Logger, settings: typeof Settings) => Promise<number>;
  activated: boolean;
}

type PluginReturnType = {
  "create-monitor": number;
};

export class InternalPluginManager {
  plugins: Plugin[] = [];

  constructor() {}
  async loadTsPlugin(path: string) {
    const jsPath = path.replace(".ts", ".js");
    await build({
      entryPoints: [path],
      bundle: true,
      minify: true,
      target: "es2020",
      format: "cjs",
      platform: "node",
      outfile: jsPath,
    });

    return require(jsPath);
  }
  async loadPlugins() {
    Logger.log("Loading plugins");
    const env = (
      process.env.XDG_CURRENT_DESKTOP ||
      process.env.XDG_SESSION_DESKTOP ||
      process.env.DESKTOP_SESSION ||
      ""
    )
      .split(":")
      .map((d) => d.toLowerCase());
    const folders = await fs.readdir("../plugins");
    for (const folder of folders) {
      const split = folder.split("-");
      const arch = split[0] as string;
      const method = split.slice(1).join("-");
      Logger.log(env, arch, method);
      try {
        const pluginPath = path.resolve(`../plugins/${folder}/index.ts`);
        const plugin: Plugin = await this.loadTsPlugin(pluginPath);
        plugin.env = arch;

        if (method === "create-monitor") {
          this.plugins.push(plugin);
          Logger.log("Added plugin!");
          Logger.log(JSON.stringify(this.plugins));
        } else {
          Logger.log(`Unknown method: ${method}`);
        }
      } catch (e) {
        Logger.log(`Failed to load plugin: ${folder}`);
        Logger.log(e);
      }
    }

    Logger.log("Done loading plugins");

    if (Settings.activePlugins.length === 0) {
      // activate relevant plugins
      for (const key of keys) {
        let plugin = this.plugins.find(
          (plugin) => plugin.key === key && plugin.env === env[1],
        );
        if (!plugin) {
          plugin = this.plugins.find(
            (plugin) => plugin.key === key && plugin.env === "any",
          );
        }
        if (plugin) {
          plugin.activated = true;
          Settings.activePlugins.push(plugin);
        }
      }
    }
  }
  async runPlugin<K extends PluginKeys>(key: K): Promise<PluginReturnType[K]> {
    Logger.log(`Running plugin: ${key}`);
    Logger.log(JSON.stringify(this.plugins));
    const plugin = Settings.activePlugins.find((plugin) => plugin.key === key);
    if (plugin && "exec" in plugin) {
      return plugin.exec(Logger, Settings);
    }
    Logger.log(`No plugin found for: ${key}`);
    throw new Error();
  }
  getPlugins() {
    return this.plugins;
  }
  activatePlugin(name: string) {
    const split = name.split("-");
    const env = split[0];
    const key = split.slice(1).join("-");

    Settings.activePlugins = Settings.activePlugins.filter(
      (plugin) => plugin.key !== key,
    );

    const plugin = this.plugins.find(
      (plugin) => plugin.key === key && plugin.env === env,
    );
    if (plugin) {
      Settings.activePlugins.push(plugin);
    }
  }
}

const PluginManager = new InternalPluginManager();
export default PluginManager;
