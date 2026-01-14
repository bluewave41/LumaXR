using System;
using TMPro;
using UnityEngine;

public class ServerInfo
{
    public string IP;

    public ServerInfo(string IP)
    {
        this.IP = IP;
    }
}

public class ServerCard : MonoBehaviour
{
    public string IP;
    public int Port;
    public static event Action<ServerCard> OnClicked;

    public void UpdateIP(string newIP)
    {
        IP = newIP;
        TMP_Text text = GetComponentInChildren<TMP_Text>();
        text.text = IP;
    }

    public void OnClick()
    {
        OnClicked?.Invoke(this);
    }
}