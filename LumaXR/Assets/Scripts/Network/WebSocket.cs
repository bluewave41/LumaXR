using PimDeWitte.UnityMainThreadDispatcher;
using UnityEngine;
using WebSocketSharp;
using WebSocketSharp.Server;

public class WebSocketServerUnity : MonoBehaviour
{
    public ScreenManager screenManager;
    private WebSocketServer server;

    void Start()
    {
        server = new WebSocketServer(8572);
        server.AddWebSocketService<GetPosition>("/position", behavior =>
        {
            behavior.screenManager = screenManager;
        });
        server.AddWebSocketService<StreamClose>("/stream-close", Behaviour =>
        {
            
        });
        server.Start();
        Debug.Log("WebSocket server started on ws://localhost:8572");
    }

    void OnApplicationQuit()
    {
        server.Stop();
    }
}

[System.Serializable]
public class PositionRequest
{
    public int id;
    public TransformData? position;
}

[System.Serializable]
public class StreamCloseData
{
    public int width;
    public int height;
    public int port;
    public string pipeline;
}

[System.Serializable]
public class TransformData
{
    public float[] pos;
    public float[] rot;
    public float[] scale;

    public TransformData(Transform screen, Transform display)
    {
        pos = new float[] { screen.localPosition.x, screen.localPosition.y, screen.localPosition.z };
        rot = new float[] { display.localRotation.eulerAngles.x, display.localRotation.eulerAngles.y, display.localRotation.eulerAngles.z };
        scale = new float[] { display.localScale.x, display.localScale.y, display.localScale.z };
    }
}

public class StreamClose: WebSocketBehavior
{
    protected override void OnMessage(MessageEventArgs e)
    {
        string msg = e.Data;
        Debug.Log("Got data " + msg);
        UnityMainThreadDispatcher.Instance().Enqueue(() =>
        {
            HandleMessage(msg);
        });
    }  

    private void HandleMessage(string data)
    {
        StreamCloseData json = JsonUtility.FromJson<StreamCloseData>(data);
        Debug.Log("Changing screen size: " + json.width + " " + json.height);
        ScreenManager.Instance.RestartStream(json.width, json.height, json.port, json.pipeline);
    }
}

public class GetPosition : WebSocketBehavior
{
    public ScreenManager screenManager;
    protected override void OnMessage(MessageEventArgs e)
{
    // Capture the message data
    string msg = e.Data;

    // Schedule a main-thread action
    UnityMainThreadDispatcher.Instance().Enqueue(() =>
    {
        HandleMessage(msg);
    });
}

    // This runs on Unity main thread
    private void HandleMessage(string data)
    {
        Debug.Log("Got a message");

        PositionRequest request = JsonUtility.FromJson<PositionRequest>(data);
        Screen screen = screenManager.GetScreen(request.id);

        if (screen == null)
        {
            Debug.Log("No screen found");
            Send("{}");
            return;
        }

        // we scale the display separately so we want to send its scale
        Transform screenTransform = screen.transform;
        Transform displayTransform = screenTransform.Find("Display");

        Debug.Log(request.position.pos);

        if(request.position.pos != null && request.position.rot != null && request.position.scale != null)
        {
            Debug.Log("Received coordinate update request");
            screenTransform.localPosition = new Vector3(request.position.pos[0], request.position.pos[1], request.position.pos[2]);
            displayTransform.localRotation = Quaternion.Euler(
                request.position.rot[0],
                request.position.rot[1],
                request.position.rot[2]
            );
            displayTransform.localScale = new Vector3(request.position.scale[0], request.position.scale[1], request.position.scale[2]);
            return;
        }

        TransformData td = new(screenTransform, displayTransform);

        Send(JsonUtility.ToJson(td));
    }
}
