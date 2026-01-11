import { execSync } from "child_process";
import fs from "fs";

const ext = process.platform === "win32" ? ".exe" : "";

const rustInfo = execSync("rustc -vV");
const regex = /host: (\S+)/g;
const target = regex.exec(rustInfo.toString());
if (!target) {
  process.exit();
}
const targetTriple = target[1];
if (!targetTriple) {
  console.error("Failed to determine platform target triple");
}
// TODO: create `src-tauri/binaries` dir
fs.renameSync(
  `sidecar${ext}`,
  `../src-tauri/binaries/sidecar-${targetTriple}${ext}`
);
