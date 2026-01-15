using UnityEngine;

public class Portal : MonoBehaviour
{
    private bool isActivated = false;
    public void OnActivate()
    {
        isActivated = !isActivated;
        transform.Find("Close").gameObject.SetActive(isActivated);
    }

    public void OnClose()
    {
        Debug.Log("Close!");
        Destroy(gameObject);
    }
}
