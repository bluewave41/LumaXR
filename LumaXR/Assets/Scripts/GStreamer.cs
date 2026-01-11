using System;
using UnityEngine;
using System.Runtime.InteropServices;
using Unity.XR.CompositionLayers;
using UnityEngine.XR.OpenXR.CompositionLayers;
using System.Threading;
using System.Collections;

[RequireComponent(typeof(Renderer))]
public class GStreamer : MonoBehaviour
{
    public int instanceId;
    public Material screenMaterial;
    public int width;
    public int height;
    public int port;
    public string pipeline;

    [DllImport("gstplugin")]
    private static extern int CreateGStreamerInstance(int width, int height, int port, string pipeline);

    [DllImport("gstplugin")]
    private static extern IntPtr GetRenderEventFunc();

    [DllImport("gstplugin")]
    private static extern IntPtr GetBackingTexture(int instanceId);

    [DllImport("gstplugin")]
    private static extern IntPtr GetOESBackingTexture(int instanceId);

    [DllImport("gstplugin")]
    private static extern IntPtr StopPipeline(int instanceId);

    [DllImport("gstplugin")]
    private static extern IntPtr SetNativeSurface(int instanceId, IntPtr surface);

    [DllImport("gstplugin")]
    private static extern void StopGStreamer(int instanceId);

    [DllImport("gstplugin")]
    private static extern void ReplayStream(int instanceId);

    private bool ready = false;
    private Texture2D texture;
    private static bool isInitialized = false;
    private readonly bool isAndroid = !Application.platform.ToString().ToLower().Contains("linux");

    public async void Begin(int width, int height, int port, string pipeline)
    {
        Debug.Log("Begin was called");
        this.width = width;
        this.height = height;
        this.port = port;
        this.pipeline = pipeline;

        if(isAndroid)
        {
            BeginAndroid();
        }
        else
        {
            BeginLinux();
            GL.IssuePluginEvent(GetRenderEventFunc(), instanceId + 2);

            Debug.Log("Ready");
            ready = true;
        }
    }

    void BeginLinux()
    {
        // on Linux we use an external texture as our compositor layer is bound to an Android surface which is unavailable on Linux
        instanceId = CreateGStreamerInstance(width, height, port, pipeline);
        Debug.Log("Instance ID: " + instanceId);

        GL.IssuePluginEvent(GetRenderEventFunc(), instanceId + 1);

        IntPtr backingTexture = GetBackingTexture(this.instanceId);
        Debug.Log("Texture ID: " + backingTexture);
        Debug.Log("Making external texture");
        Debug.Log(width);
        Debug.Log(height);

        texture = Texture2D.CreateExternalTexture(width, height, TextureFormat.RGBA32, false, false, backingTexture);

        Debug.Log("Made external texture: ", texture);
        Debug.Log(screenMaterial);

        screenMaterial.mainTexture = texture;
        GetComponent<Renderer>().material = new Material(screenMaterial);

        Debug.Log("Set textures");
    }

    void BeginAndroid()
    {
        if(!isInitialized)
        {
            using (AndroidJavaClass jc = new("com.bluewave41.lumaxr.NativeBridge"))
            {
                jc.CallStatic("setJavaVM");
            };

            AndroidJavaClass unityPlayer = new("com.unity3d.player.UnityPlayer");
            AndroidJavaObject activity = unityPlayer.GetStatic<AndroidJavaObject>("currentActivity");
            AndroidJavaClass gstClass = new("org.freedesktop.gstreamer.GStreamer");
            gstClass.CallStatic("init", activity);
            isInitialized = true;
        }

        instanceId = CreateGStreamerInstance(width, height, port, pipeline);
        Debug.Log("Instance ID: " + instanceId);

        IntPtr surface = IntPtr.Zero;

        StartCoroutine(WaitForSurface(surface =>
        {
            Debug.Log("Got a valid suface");
            SetNativeSurface(instanceId, surface);
            GL.IssuePluginEvent(GetRenderEventFunc(), instanceId + 1);
            Debug.Log("Finished event 1");

            GL.IssuePluginEvent(GetRenderEventFunc(), instanceId + 2);

            Debug.Log("Ready");
        }));
    }

    public void Destroy()
    {
        GL.IssuePluginEvent(GetRenderEventFunc(), instanceId + 4);
        Destroy(this);
    }

    void Update()
    {
        if(ready)
        {
            GL.IssuePluginEvent(GetRenderEventFunc(), instanceId + 3);
        }
    }

    /*void OnDestroy()
    {
        if (instanceId != 0) {
            StopPipeline(instanceId);
            instanceId = 0;
        }
    }*/

    void OnApplicationQuit()
    {
        if (instanceId != 0) {
            StopPipeline(instanceId);
            instanceId = 0;
        }
    }

    public void OnApplicationPause(bool pause)
    {
        if(pause)
        {
            StopPipeline(instanceId);
        }
        else if(!pause)
        {
            RestartStream();
        }
    }

    IEnumerator WaitForSurface(Action<IntPtr> onReady)
    {
        CompositionLayer layer = GetComponent<CompositionLayer>();
        IntPtr surface = IntPtr.Zero;
        yield return new WaitUntil(() =>
        {
            surface = OpenXRLayerUtility.GetLayerAndroidSurfaceObject(layer.GetInstanceID());
            return surface != IntPtr.Zero;

        });

        onReady.Invoke(surface);
    }

    void RestartStream()
    {
        IntPtr surface = IntPtr.Zero;

        StartCoroutine(WaitForSurface(surface =>
        {
            Debug.Log("Got a valid suface");
            SetNativeSurface(instanceId, surface);

            GL.IssuePluginEvent(GetRenderEventFunc(), instanceId + 3);

            Debug.Log("Restarted stream");
        }));
    }
}
