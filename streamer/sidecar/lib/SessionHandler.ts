let requestTokenCounter = 0;
let sessionTokenCounter = 0;

/**
 * Creates a new request token
 * @param sessionName
 * @returns
 */
export function newRequestPath(portal: any) {
  const sessionName = portal.bus.name.slice(1).replaceAll(".", "_");
  requestTokenCounter++;
  const requestToken = `u${requestTokenCounter}`;
  const requestPath = `/org/freedesktop/portal/desktop/request/${sessionName}/${requestToken}`;
  return { requestToken, requestPath };
}

/**
 * Creates a new sesion token
 * @param sessionName
 * @returns
 */
export function newSessionPath(portal: any) {
  const sessionName = portal.bus.name.slice(1).replaceAll(".", "_");
  sessionTokenCounter++;
  const sessionToken = `u${sessionTokenCounter}`;
  const sessionPath = `/org/freedesktop/portal/desktop/session/${sessionName}/${sessionToken}`;
  return { sessionToken, sessionPath };
}
