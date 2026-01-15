using UnityEngine;

[RequireComponent(typeof(Renderer))]
public class HoverFeedback : MonoBehaviour
{
    private Renderer targetRenderer;
    private Color originalColor;
    public Color hoverColor;
    // Start is called once before the first execution of Update after the MonoBehaviour is created

    void Awake()
    {
        targetRenderer = GetComponent<Renderer>();
        originalColor = targetRenderer.material.color;
    }
    public void OnHoverEnter()
    {
        targetRenderer.material.color = hoverColor;
    }

    public void OnHoverExit()
    {
        targetRenderer.material.color = originalColor;
    }
}
