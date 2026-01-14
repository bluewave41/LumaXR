import { promises as fs } from "fs";
import Logger from "./Logger";
import { build } from "esbuild";
import path from "path";
import Settings from "./Settings";

type PluginKeys = "create-monitor";
const pluginKeys: PluginKeys[] = ["create-monitor"];

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

export class InternalPluginManager {
  plugins: Partial<Record<PluginKeys, AnyPlugin>> = {};

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
    const distro = (
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
      Logger.log(distro, arch, method);
      if (!distro.includes(arch)) {
        Logger.log(`No matching arch found for: ${arch}`);
        continue;
      }
      try {
        const pluginPath = path.resolve(`../plugins/${folder}/index.ts`);
        const plugin: CreatePlugin = await this.loadTsPlugin(pluginPath);

        if (method === "create-monitor") {
          this.plugins["create-monitor"] = plugin;
          Logger.log("Added plugin!");
          Logger.log(JSON.stringify(this.plugins));
        } else {
          Logger.log(`Unknown methpd: ${method}`);
        }
      } catch (e) {
        Logger.log(`Failed to load plugin: ${folder}`);
        Logger.log(e);
      }
    }

    Logger.log("Done loading plugins");

    // TODO: check for missing plugins
  }
  async runPlugin<K extends PluginKeys>(key: K): Promise<PluginReturnType[K]> {
    Logger.log(`Running plugin: ${key}`);
    Logger.log(JSON.stringify(this.plugins));
    const plugin = this.plugins[key];
    if (plugin && "exec" in plugin) {
      return plugin.exec(Logger, Settings);
    }
    Logger.log(`No plugin found for: ${key}`);
    throw new Error();
  }
}

const PluginManager = new InternalPluginManager();
export default PluginManager;
