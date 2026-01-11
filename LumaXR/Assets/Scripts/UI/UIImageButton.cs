using UnityEngine;
using UnityEngine.UI;
using UnityEngine.XR.Interaction.Toolkit;
using UnityEngine.XR.Interaction.Toolkit.Interactables;

[RequireComponent(typeof(Image))]
[RequireComponent(typeof(RectTransform))]
public class UIButton : MonoBehaviour
{
    private Image image;
    private Color originalColor;
    public Color hoverColor;
    void Awake()
    {
        image = GetComponent<Image>();
        originalColor = image.color;

        RectTransform rt = GetComponent<RectTransform>();
        BoxCollider collider = gameObject.AddComponent<BoxCollider>();
        collider.size = new Vector3(rt.rect.width, rt.rect.height, 0.01f);

        XRSimpleInteractable interactable = gameObject.AddComponent<XRSimpleInteractable>();
        interactable.hoverEntered.AddListener(OnHoverEnter);
        interactable.hoverExited.AddListener(OnHoverExit);

    }

    public void OnHoverEnter(HoverEnterEventArgs args)
    {
        Debug.Log("Entered");
        image.color = Color.aquamarine;
    }
    public void OnHoverExit(HoverExitEventArgs args)
    {
        image.color = originalColor;
    }
}
