using UnityEngine;

public class PortalOptions : MonoBehaviour
{
    public Portal portalPrefab;
    public void CreatePortal()
    {
        VRPlayer player = VRPlayer.Instance;
        Vector3 spawnPos = player.Head.position + player.Head.forward * 2;
        Quaternion spawnRot = Quaternion.LookRotation(player.Head.forward);
        Instantiate(portalPrefab, spawnPos, spawnRot);
    }
}
