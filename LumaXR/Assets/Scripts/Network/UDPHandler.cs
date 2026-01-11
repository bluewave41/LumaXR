using System;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Threading.Tasks;
using UnityEngine;

public class UdpHandler : MonoBehaviour
{
    public NetworkConfig config;
    public ServerPanel serverPanel;
    public string multicastIP;
    public int port;

    private UdpClient client;
    private bool listening;

    private void Start()
    {
        multicastIP = config.IP;
        port = config.Port;
        StartListening();
    }

    private void OnDestroy()
    {
        StopListening();
    }

    private void StartListening()
    {
        client = new UdpClient(port);
        client.JoinMulticastGroup(IPAddress.Parse(multicastIP));
        listening = true;

        // Fire-and-forget async loop
        _ = ListenLoop();
    }

    private void StopListening()
    {
        listening = false;
        client?.DropMulticastGroup(IPAddress.Parse(multicastIP));
        client?.Close();
        client = null;
    }

    private async Task ListenLoop()
    {
        while (listening)
        {
            try
            {
                UdpReceiveResult result = await client.ReceiveAsync();
                string message = Encoding.UTF8.GetString(result.Buffer);
                if(message == "discovery")
                {
                    string IP = result.RemoteEndPoint.Address.ToString();
                    if(!serverPanel.HasServer(IP))
                    {
                        serverPanel.AddServer(new ServerInfo(IP));
                        Debug.Log($"Received from {result.RemoteEndPoint.Address}: {message}");
                    }
                }
            }
            catch (Exception ex)
            {
                Debug.LogError($"UDP Error: {ex}");
            }
        }
    }
}
