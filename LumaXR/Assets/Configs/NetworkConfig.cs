using UnityEngine;

[CreateAssetMenu(fileName = "NetworkConfig", menuName = "Network/Config")]
public class NetworkConfig : ScriptableObject
{
    public string IP = "239.255.42.99";
    public int Port = 9999;
}
