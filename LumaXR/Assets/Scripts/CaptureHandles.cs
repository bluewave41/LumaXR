#if UNITY_EDITOR
using UnityEditor.XR.OpenXR.Features;
#endif
using UnityEngine;
using UnityEngine.XR.OpenXR.Features;
using UnityEditor;
using System.Runtime.InteropServices;

#if UNITY_EDITOR
[OpenXRFeature(
    UiName = "Capture Unity OpenXR Instance/Session",
    BuildTargetGroups = new [] { BuildTargetGroup.Standalone, BuildTargetGroup.Android },
    Company = "YourCompany",
    Desc = "Captures the XRInstance and XRSession from Unity and sends to native plugin",
    Version = "1.0.0",
    FeatureId = featureId,
    OpenxrExtensionStrings = "")]
#endif

public class CaptureXRHandlesFeature : OpenXRFeature
{
    [DllImport("gstplugin")]
    private static extern void SetXRInstance(ulong instance);
    [DllImport("gstplugin")]
    private static extern void SetXRSession(ulong session);
    public const string featureId = "com.bluewave41.lumaxr.capturehandles";

    // Called when the XR instance is created by Unity
    protected override bool OnInstanceCreate(ulong xrInstance)
    {
        Debug.Log($"Unity XRInstance: {xrInstance}");
        SetXRInstance(xrInstance);
        return base.OnInstanceCreate(xrInstance);
    }

    // Called when the XR session is created by Unity
    protected override void OnSessionCreate(ulong xrSession)
    {
        Debug.Log($"Unity XRSession: {xrSession}");
        SetXRSession(xrSession);
        base.OnSessionCreate(xrSession);
    }

    // Called when the session ends / is destroyed
    protected override void OnSessionDestroy(ulong xrSession)
    {
        //Native_ClearSession();
        base.OnSessionDestroy(xrSession);
    }
}
