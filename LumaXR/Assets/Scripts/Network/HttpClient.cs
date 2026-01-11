using UnityEngine;
using UnityEngine.Networking;
using System.Threading.Tasks;
using System.Text;

public class HttpClient : MonoBehaviour
{
    public static HttpClient Instance { get; private set; }    
    public NetworkConfig config;

    public string serverIP;
    public int serverPort;

    public string endpoint;

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

    public void StartClient(string IP, int port)
    {
        serverIP = IP;
        serverPort = port;
        endpoint = $"http://{serverIP}:{serverPort}";
    }

    public async Task<string> GET(string route)
    {
        string url = endpoint + route;
        Debug.Log(url);

        using UnityWebRequest request = UnityWebRequest.Get(url);
        var operation = request.SendWebRequest();

        while (!operation.isDone)
            await Task.Yield();

        if (request.result != UnityWebRequest.Result.Success)
        {
            Debug.LogError($"Error: {request.error}");
            return null;
        }

        Debug.Log($"Response: {request.downloadHandler.text}");
        return request.downloadHandler.text;
    }

    public async Task<string> POST(string route, string data)
    {
        string url = endpoint + route;
        Debug.Log(url);

        byte[] bytes = Encoding.UTF8.GetBytes(data);

        using UnityWebRequest request = new(url, "POST");
        request.uploadHandler = new UploadHandlerRaw(bytes);
        request.downloadHandler = new DownloadHandlerBuffer();
        request.SetRequestHeader("Content-Type", "application/json");
        
        var operation = request.SendWebRequest();

        while (!operation.isDone)
            await Task.Yield();

        if (request.result != UnityWebRequest.Result.Success)
        {
            Debug.LogError($"Error: {request.error}");
            return null;
        }

        Debug.Log($"Response: {request.downloadHandler.text}");
        return request.downloadHandler.text;
    }
}
