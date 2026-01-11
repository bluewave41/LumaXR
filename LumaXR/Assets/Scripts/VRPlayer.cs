using UnityEngine;
public class VRPlayer : MonoBehaviour
{
    public static VRPlayer Instance { get; private set; }

    public Transform Head;
    public GameObject LeftController;
    public GameObject RightController;
    public GameObject LeftHand;
    public GameObject RightHand;

    private void Awake()
    {
        if (Instance != null && Instance != this)
        {
            Destroy(gameObject);
            return;
        }
        Instance = this;
        DontDestroyOnLoad(gameObject);
    }
}
