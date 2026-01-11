class InternalLogger {
  logs: any[] = [];

  constructor() {}
  info(...message: any[]) {
    this.logs = this.logs.concat(...message);
  }
  log(...message: any[]) {
    console.log(message);
    this.logs = this.logs.concat(...message);
  }
  debug(...message: any[]) {
    this.logs = this.logs.concat(...message);
  }
  error(...message: any[]) {
    this.logs = this.logs.concat(...message);
  }
  concat(str: any) {
    this.logs = this.logs.concat(str);
  }
  getLogs() {
    return this.logs.join("\n");
  }
}

const Logger = new InternalLogger();
export default Logger;
