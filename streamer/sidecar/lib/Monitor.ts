export class Monitor {
  connector: string;
  width: number;
  height: number;
  xOffset: number;
  yOffset: number;
  frameRate: number;
  isPrimary: boolean;

  constructor(
    connector: string,
    width: number,
    height: number,
    xOffset: number,
    yOffset: number,
    frameRate: number,
    isPrimary: boolean,
  ) {
    this.connector = connector;
    this.width = width;
    this.height = height;
    this.xOffset = xOffset;
    this.yOffset = yOffset;
    this.frameRate = frameRate;
    this.isPrimary = isPrimary;
  }
  update(newWidth: number, newHeight: number) {
    this.width = newWidth;
    this.height = newHeight;
  }
  // TODO: one line this
  equal(monitor: Monitor) {
    if (
      this.width !== monitor.width ||
      this.height !== monitor.height ||
      this.frameRate !== monitor.frameRate
    ) {
      return false;
    }
    return true;
  }
}
