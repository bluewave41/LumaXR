using UnityEngine;
using UnityEngine.XR;

public class Settings : MonoBehaviour
{
    public static Settings Instance { get; private set; }

    public bool debug = false;
    public bool handsEnabled = true;

    void Start()
    {
        XRSettings.useOcclusionMesh = false;
    }
    void Awake()
    {
        if (Instance != null && Instance != this)
        {
            Destroy(gameObject);
            return;
        }

        Instance = this;
        DontDestroyOnLoad(gameObject);
    }
    public bool GetBoolSetting(string setting)
    {
        if(setting.Equals("debug"))
        {
            return debug;
        }
        else if(setting.Equals("handsEnabled")) {
            return handsEnabled;
        }
        throw new System.Exception("Unknown setting requested.");
    }

    public void SetBoolSetting(string setting, bool value)
    {
        if(setting.Equals("debug"))
        {
            debug = value;
        }
        else if(setting.Equals("handsEnabled"))
        {
            handsEnabled = value;
            if(value)
            {
                VRPlayer.Instance.LeftHand.SetActive(true);
                VRPlayer.Instance.RightHand.SetActive(true);
            }
            else
            {
                VRPlayer.Instance.LeftHand.SetActive(false);
                VRPlayer.Instance.RightHand.SetActive(false);
            }
        }
    }

    public void Update()
    {
        // temporary hack to keep the hands disabled
        if(!handsEnabled)
        {
            VRPlayer.Instance.LeftHand.SetActive(false);
            VRPlayer.Instance.RightHand.SetActive(false);
        }
    }
}
