using System.Collections.Generic;
using UnityEngine;

public class ServerPanel : MonoBehaviour
{
    public List<ServerInfo> servers = new();
    private Transform scrollTransform;
    public ServerCard serverCardPrefab;

    void Start()
    {
        scrollTransform = transform.Find("Canvas/Scroll View/Viewport/Content");
    }

    public void AddServer(ServerInfo server)
    {
        Debug.Log(server.IP);
        servers.Add(server);
        ServerCard card = Instantiate(serverCardPrefab);
        card.transform.SetParent(scrollTransform, false);
        Debug.Log("Instantiate");
        card.IP = server.IP;
    }
    public bool HasServer(string IP)
    {
        return servers.Exists(server => server.IP == IP);
    }
}
