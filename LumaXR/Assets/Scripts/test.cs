using UnityEngine;

public class test : MonoBehaviour
{
    // Start is called once before the first execution of Update after the MonoBehaviour is created
    void Start()
    {
        Vector3 localPivot = new(-0.5f, 0, 0); // left edge in local space
        Vector3 worldPivot = transform.TransformPoint(localPivot); // converts to world space
        GameObject pivotSphere = GameObject.CreatePrimitive(PrimitiveType.Sphere);
        pivotSphere.transform.position = worldPivot;
        pivotSphere.transform.localScale = Vector3.one * 0.05f;

        transform.RotateAround(worldPivot, transform.up, 45);
    }

    // Update is called once per frame
    void Update()
    {
    }
}
