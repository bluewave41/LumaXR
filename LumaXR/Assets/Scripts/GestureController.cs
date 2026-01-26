using UnityEngine;

public class GestureController : MonoBehaviour
{
    private VRPlayer player;
    // Start is called once before the first execution of Update after the MonoBehaviour is created
    void Start()
    {
        player = VRPlayer.Instance;
    }

    // Update is called once per frame
    void Update()
    {
        
    }

    public void TurnRed()
    {
        Debug.Log("Activated");
        SkinnedMeshRenderer renderer = player.RightHand.GetComponentInChildren<SkinnedMeshRenderer>();
        renderer.material.color = Color.red;
    }
}
