using UnityEngine;

[System.Serializable]
public class CreateMonitorData
{
    public string direction;
}

[RequireComponent(typeof(Renderer))]
public class CreateScreenButton : MonoBehaviour
{
    public Direction direction;
    private Renderer rend;
    private Color originalColor;
    // Start is called once before the first execution of Update after the MonoBehaviour is created
    void Awake()
    {
        rend = GetComponent<Renderer>();
        originalColor = rend.material.color;
    }

    // Update is called once per frame
    void Update()
    {
    }

    public void OnHoverEnter()
    {
        rend.material.color = Color.blue;
    }

    public void OnHoverExit()
    {
        rend.material.color = originalColor;
    }

    public async void OnActivate()
    {
        ScreenManager.Instance.SpawnScreen(direction);
        Debug.Log("Pressed!");
    }
}
