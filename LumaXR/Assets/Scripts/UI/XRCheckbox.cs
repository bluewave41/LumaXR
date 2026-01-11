using UnityEngine;
using UnityEngine.UI;
using UnityEngine.XR.Interaction.Toolkit;
using UnityEngine.XR.Interaction.Toolkit.Interactables;

[RequireComponent(typeof(Image))]
public class XRCheckbox : MonoBehaviour
{
    public string setting;
    public bool isChecked;
    private Image image;
    private Color color;

    void Awake()
    {
        RectTransform rt = gameObject.GetComponent<RectTransform>();
        BoxCollider bc = gameObject.AddComponent<BoxCollider>();
        bc.size = new Vector3(rt.rect.width, rt.rect.height, 0.01f);

        XRSimpleInteractable si = gameObject.AddComponent<XRSimpleInteractable>();
        si.hoverEntered.AddListener(OnHoverEnter);
        si.hoverExited.AddListener(OnHoverExit);
        si.activated.AddListener(OnActivated);

        image = GetComponent<Image>();
        Debug.Log(Settings.Instance);
        isChecked = Settings.Instance.GetBoolSetting(setting);
        Debug.Log(isChecked);
        color = isChecked ? Color.green : Color.red;
        image.color = color;
    }

    void OnHoverEnter(HoverEnterEventArgs args)
    {
        color = Color.Lerp(color, Color.white, 0.5f);
        image.color = color;
    }

    void OnHoverExit(HoverExitEventArgs args)
    {
        color = isChecked ? Color.green : Color.red;
        image.color = color;
    }

    void OnActivated(ActivateEventArgs args)
    {
        isChecked = !isChecked;
        Settings.Instance.SetBoolSetting(setting, isChecked);
        color = isChecked ? Color.green : Color.red;
        OnHoverEnter(null);
    }
}
