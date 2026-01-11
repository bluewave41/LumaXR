import Logger from "../lib/Logger";

export const parseJson = (data: string) => {
  try {
    return JSON.parse(data);
  } catch (e) {
    Logger.log("Failed to parse JSON", data);
    return null;
  }
};
