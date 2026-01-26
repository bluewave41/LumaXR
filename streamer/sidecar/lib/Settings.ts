//pipewiresrc path={path} keepalive-time=30 ! video/x-raw,width={width},height={height},max-framerate=120/1 ! queue max-size-buffers=1 max-size-time=0 max-size-bytes=0 leaky=downstream ! nvh264enc bitrate=25000 gop-size=30 zerolatency=true preset=low-latency-hq rc-mode=cbr qp-min=10 qp-max=20 ! rtph264pay pt=96 mtu=1200 config-interval=1 aggregate-mode=1 ! udpsink host=192.168.0.230 port={port} sync=false async=false qos=false
//udpsrc port=%d buffer-size=4194304 caps="application/x-rtp, media=video, encoding-name=H264, payload=96" ! rtpjitterbuffer latency=30 ! queue ! rtph264depay ! h264parse ! amcviddec-c2qtiavcdecoderlowlatency ! video/x-raw(memory:GLMemory) ! glimagesink name=unity sync=false

import * as fs from "fs";
import Logger from "./Logger";
import { Plugin } from "./PluginManager";

class InternalSettings {
  senderPipeline: string;
  receiverPipeline: string;
  virtualOnly: boolean;
  linuxMode: boolean;
  clientIp: string = "";
  activePlugins: Plugin[] = [];

  constructor() {
    Logger.log("Settings initialized");
    let settings: any;
    try {
      settings = JSON.parse(fs.readFileSync("../settings.json").toString());
      Logger.log(settings.senderPipeline);
      this.senderPipeline = settings.senderPipeline;
      this.receiverPipeline = settings.receiverPipeline;
      this.virtualOnly = settings.virtualOnly;
      this.linuxMode = settings.linuxMode;
    } catch (e) {
      this.senderPipeline =
        "pipewiresrc path={path} keepalive-time=30 ! video/x-raw,width={width},height={height},max-framerate=120/1 ! queue max-size-buffers=1 max-size-time=0 max-size-bytes=0 leaky=downstream ! nvh264enc bitrate=25000 gop-size=30 zerolatency=true preset=low-latency-hq rc-mode=cbr qp-min=10 qp-max=20 ! rtph264pay pt=96 mtu=1200 config-interval=1 aggregate-mode=1 ! udpsink host=192.168.0.230 port={port} sync=false async=false qos=false";
      this.receiverPipeline = `udpsrc port=%d buffer-size=4194304 caps="application/x-rtp, media=video, encoding-name=H264, payload=96" ! rtpjitterbuffer latency=30 ! queue ! rtph264depay ! h264parse ! amcviddec-c2qtiavcdecoderlowlatency ! video/x-raw(memory:GLMemory) ! glimagesink name=unity sync=false`;
      this.virtualOnly = true;
      this.linuxMode = false;
      settings = JSON.stringify({
        senderPipeline: this.senderPipeline,
        receiverPipeline: this.receiverPipeline,
        virtualOnly: this.virtualOnly,
        linuxMode: this.linuxMode,
      });
      this.saveSettings(settings);
    }
  }
  getSetting<K extends keyof InternalSettings>(
    setting: K,
  ): InternalSettings[K] {
    return this[setting];
  }
  getSettings() {
    return {
      senderPipeline: this.senderPipeline,
      receiverPipeline: this.receiverPipeline,
      virtualOnly: this.virtualOnly,
      linuxMode: this.linuxMode,
      clientIp: this.clientIp,
    };
  }
  saveSettings(settings: any) {
    const json = JSON.parse(settings);
    this.senderPipeline = json.senderPipeline;
    this.receiverPipeline = json.receiverPipeline;
    this.virtualOnly = json.virtualOnly;
    this.linuxMode = json.linuxMode;

    fs.writeFileSync("../settings.json", settings);
  }
}

const Settings = new InternalSettings();
export default Settings;
