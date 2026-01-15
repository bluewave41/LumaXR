using System;
using System.Collections.Generic;
using System.Linq;
using System.Net.Sockets;
using System.Text;
using System.Threading.Tasks;
using UnityEngine;

public class ScreenManager : MonoBehaviour
{
    public Screen screenPrefab;
    public Material screenMaterial;
    public float screenWidth = 2f;
    public float horizontalFOV = 30f;
    public static int port = 3555;
    private static int nextScreenId = 1;
    public NetworkConfig config;
    private readonly Dictionary<Direction, Screen> screens = new();
    public static ScreenManager Instance { get; private set; }
    public DropZone dropZonePrefab;
    public GameObject zonesContainer;
    private readonly List<DropZone> dropZones = new();
    private bool isCenterSelected = false;
    public ServerPanel serverPanel;
    private Vector3 lastPosition;
    private Quaternion lastRotation;

    private void Awake()
    {
        if (Instance != null && Instance != this)
        {
            Destroy(gameObject);
            return;
        }
        Instance = this;
        DontDestroyOnLoad(gameObject);
    }

    private void OnEnable()
    {
        ServerCard.OnClicked += card => BeginStream(card);
    }
    
    public Screen GetScreen(int id)
    {
        return screens.Values.First(screen => screen.id == id);
    }

    private async Task BeginStream(ServerCard card)
    {
        if(!Settings.Instance.debug)
        {
            serverPanel.gameObject.SetActive(false);
            HttpClient.Instance.StartClient(card.IP, card.Port);
            byte[] data = Encoding.UTF8.GetBytes("start");
            using var client = new UdpClient();
            client.Send(data, data.Length, config.IP, config.Port);
        }

        SpawnScreen(Direction.CENTER);
    }

    public void RestartStream(int newWidth, int newHeight, int port, string pipeline)
    {
        Screen screen = screens.Values.First(screen => screen.stream.port == port);
        screen.ApplyScale(newWidth, newHeight);

        // destroy old stream
        GStreamer stream = screen.transform.GetComponentInChildren<GStreamer>();
        stream.Destroy();

        Debug.Log("Destroyed!");

        // create new stream
        Transform display = screen.transform.Find("Display");
        stream = display.gameObject.AddComponent<GStreamer>();
        stream.screenMaterial = screenMaterial;
        stream.Begin(newWidth, newHeight, port, pipeline);
    }

    public async Task SpawnScreen(Direction direction)
    {
        Debug.Log("Spawning screen");
        if (VRPlayer.Instance == null)
        {
            Debug.Log("No player");
            return;
        }

        Screen screen = Instantiate(screenPrefab, transform);
        screen.id = nextScreenId++;
        screen.direction = direction;

        await screen.Initialize();

        if(direction == Direction.CENTER)
        {
            float distance = 2.0f;

            // Position in front of player
            Transform playerHead = VRPlayer.Instance.Head;
            Vector3 forward = playerHead.forward;
            forward.y = 0;
            forward.Normalize();
            screen.transform.position = playerHead.position + forward * distance;

            // Look at player
            screen.transform.LookAt(playerHead.position);
            screen.transform.Rotate(0, 180, 0);

            lastPosition = screen.transform.position;
            lastRotation = screen.transform.rotation;
        }
        else
        {
            Screen center = screens[Direction.CENTER];
            Transform centerTransform = center.transform;
            Transform displayTransform = centerTransform.Find("Display");
            centerTransform.GetPositionAndRotation(out Vector3 pos, out Quaternion rot);
            screen.transform.SetPositionAndRotation(centerTransform.position, centerTransform.rotation);
            center.RemoveButton(direction);

            float centerWidth = displayTransform.lossyScale.x;
            float centerHeight = displayTransform.lossyScale.y;

            Vector3 pivot = new(0, 0, 0);
            Vector3 axis = new();
            float rotationAmount = 0;

            switch (direction)
            {
                case Direction.TOP:
                    pos += centerTransform.up * centerHeight;
                    pivot.y -= 0.5f;
                    rotationAmount = 45;
                    axis = displayTransform.right;
                    break;
                case Direction.LEFT:
                    pos -= centerTransform.right * centerWidth;
                    pivot.x += 0.5f;
                    rotationAmount = 45;
                    axis = displayTransform.up;
                    break;
                case Direction.RIGHT:
                    pos += centerTransform.right * centerWidth;
                    pivot.x -= 0.5f;
                    rotationAmount = -45;
                    axis = displayTransform.up;
                    break;
                case Direction.BOTTOM:
                    pos -= centerTransform.up * centerHeight;
                    pivot.y += 0.5f;
                    rotationAmount = -45;
                    break;
            }

            Vector3 worldPivot = displayTransform.TransformPoint(pivot);
            screen.transform.position = pos;
            screen.transform.RotateAround(worldPivot, axis, rotationAmount);

            Transform buttons = centerTransform.Find("Buttons");
            foreach (Transform button in buttons) {
                CreateScreenButton cb = button.GetComponent<CreateScreenButton>();
                if(cb.direction == direction)
                {
                    Destroy(button.gameObject);
                    break;
                }
            }
        }


        screens.Add(direction, screen);
    }

    public void HandleScreenGrab(Screen screen)
    {
        Transform display = screen.transform.Find("Display");
        float xScale = display.lossyScale.x / 2;
        float yScale = display.lossyScale.y / 2;
        if(screen.direction == Direction.CENTER)
        {
            isCenterSelected = true;
            foreach(Direction dir in Enum.GetValues(typeof(Direction))) {
                if(!screens.ContainsKey(dir))
                {
                    // create dropzone
                    DropZone zone = Instantiate(dropZonePrefab, zonesContainer.transform);
                    zone.transform.localPosition = Vector3.zero;
                    if(dir == Direction.TOP)
                    {
                        zone.transform.localPosition += new Vector3(0, yScale + 0.5f, 0);
                    }
                    if(dir == Direction.LEFT)
                    {
                        zone.transform.localPosition += new Vector3(-xScale - 0.5f, 0, 0);
                    }
                    if(dir == Direction.RIGHT)
                    {
                        zone.transform.localPosition += new Vector3(xScale + 0.5f, 0, 0);
                    }
                    if(dir == Direction.BOTTOM)
                    {
                        zone.transform.localPosition += new Vector3(0, -yScale - 0.5f, 0);
                    }
                    dropZones.Add(zone);
                }
            }
        }
    }

    private void DestroyDropZones()
    {
        foreach (DropZone zone in dropZones)
        {
            Destroy(zone.gameObject);
        }
        dropZones.Clear();
    }

    public void ReleaseScreenGrab()
    {
        isCenterSelected = false;
        DestroyDropZones();
    }

    public void HandleTwoHandGrab()
    {
        DestroyDropZones();
    }

    void Update()
    {
        if (!isCenterSelected) return;

        Screen centerScreen = screens[Direction.CENTER];
        Transform center = centerScreen.transform;

        Vector3 positionDelta = center.position - lastPosition;
        Quaternion rotationDelta = center.rotation * Quaternion.Inverse(lastRotation);

        foreach (var kvp in screens)
        {
            if (kvp.Key == Direction.CENTER) continue;

            Transform t = kvp.Value.transform;

            t.position = center.position + rotationDelta * (t.position - lastPosition);
            t.rotation = rotationDelta * t.rotation;
        }

        lastPosition = center.position;
        lastRotation = center.rotation;
    }

    public List<Direction> GetOpenDirections()
    {
        List<Direction> openDirections = new();
        foreach(Direction dir in Enum.GetValues(typeof(Direction)))
        {
            if(!screens.TryGetValue(dir, out Screen s))
            {
                openDirections.Add(dir);
            }
        }
        return openDirections;
    }
}
