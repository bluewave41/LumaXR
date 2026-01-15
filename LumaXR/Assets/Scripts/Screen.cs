using System.Collections.Generic;
using System.Threading.Tasks;
using UnityEngine;
using UnityEngine.XR.Interaction.Toolkit;
using UnityEngine.XR.Interaction.Toolkit.Interactables;
using UnityEngine.XR.Interaction.Toolkit.Interactors;

[System.Serializable]
public class ScreenResponse
{
    public int width;
    public int height;
    public int port;
    public string receiverPipeline;
}

public enum Direction { LEFT, RIGHT, TOP, BOTTOM, CENTER }

[RequireComponent(typeof(GStreamer))]
[RequireComponent(typeof(XRGrabInteractable))]
public class Screen : MonoBehaviour
{
    public int id;
    public Direction direction;
    public CreateScreenButton buttonPrefab;
    public GameObject buttonsContainer;
    public Transform displayQuad;
    public Stream stream;
    private HashSet<XRBaseInteractor> grabbers = new();
    private XRGrabInteractable grabInteractable;
    private Transform display;

    private readonly Dictionary<Direction, CreateScreenButton> buttons = new();
    private readonly Dictionary<Direction, Vector2> relativePositions = new()
    {
        { Direction.LEFT,  new Vector2(-1, 0) },
        { Direction.RIGHT, new Vector2(1, 0) },
        { Direction.TOP,   new Vector2(0, 1) },
        { Direction.BOTTOM,new Vector2(0, -1) }
    };

    float initialHandsDistance;
    Vector3 initialQuadScale;

    // Start is called once before the first execution of Update after the MonoBehaviour is created
    async void Awake()
    {
        display = transform.Find("Display");
        grabInteractable = GetComponent<XRGrabInteractable>();
        grabInteractable.selectEntered.AddListener(OnGrab);
        grabInteractable.selectExited.AddListener(OnRelease);
        gameObject.SetActive(false);
    }

    private void LateUpdate()
    {
        if (grabbers.Count != 2)
        {
            grabInteractable.trackPosition = true;
            grabInteractable.trackRotation = true;
            return;
        }

        XRBaseInteractor[] hands = new XRBaseInteractor[2];
        grabbers.CopyTo(hands);

        Vector3 hand0Pos = hands[0].attachTransform.position;
        Vector3 hand1Pos = hands[1].attachTransform.position;
        Debug.Log("0pos " + hand0Pos);
        Debug.Log("1pos " + hand1Pos);

        float currentDistance = Vector3.Distance(hand0Pos, hand1Pos);
        float scaleFactor = currentDistance / initialHandsDistance;

        Debug.Log("Distance " + currentDistance);
        Debug.Log("Scale " + scaleFactor);

        scaleFactor = Mathf.Clamp(scaleFactor, 0.2f, 5f);
        Debug.Log("Capped scale " + scaleFactor);

        display.localScale = initialQuadScale * scaleFactor;
    }

    private void OnGrab(SelectEnterEventArgs args)
    {
        XRBaseInteractor interactor = args.interactorObject as XRBaseInteractor;
        grabbers.Add(interactor);

        if(grabbers.Count == 2)
        {
            OnTwoHandGrab();
            // remove dropzones
            ScreenManager.Instance.ReleaseScreenGrab();
        }
    }

    private void OnRelease(SelectExitEventArgs args)
    {
        XRBaseInteractor interactor = args.interactorObject as XRBaseInteractor;
        grabbers.Remove(interactor);
    }

    private void OnTwoHandGrab()
    {
        Debug.Log("Two hand grab!");
        XRBaseInteractor[] hands = new XRBaseInteractor[2];
        grabbers.CopyTo(hands);

        initialHandsDistance = Vector3.Distance(hands[0].attachTransform.position, hands[1].attachTransform.position);
        initialQuadScale = transform.Find("Display").localScale;
        grabInteractable.trackPosition = false;
        grabInteractable.trackRotation = false;
    }

    public void ApplyScale(int width, int height)
    {
        stream.width = width;
        stream.height = height;

        float verticalAngleRad = 25 * Mathf.Deg2Rad;

        float aspect = (float)stream.width / stream.height;
        float quadHeight = 2f * 2f * Mathf.Tan(verticalAngleRad / 2f);

        displayQuad.localScale = new Vector3(aspect, quadHeight, 1);
    }

    public async Task Initialize()
    {
        if(!ScreenManager.Instance.zonesContainer)
        {
            GameObject zones = transform.Find("Zones").gameObject;
            if(!zones)
            {
                Debug.Log("Failed to find zones.");
            }
            ScreenManager.Instance.zonesContainer = zones;
        }

        if(!Settings.Instance.debug)
        {
            if(direction == Direction.CENTER)
            {
                // single physical
                string response = await HttpClient.Instance.GET("/getPipeline");
                ScreenResponse details = JsonUtility.FromJson<ScreenResponse>(response);
                stream = new Stream(details.width, details.height, details.port, details.receiverPipeline);
            }
            else
            {
                // virtual
                CreateMonitorData json = new() { direction = this.direction.ToString() };
                string response = await HttpClient.Instance.POST("/createMonitor", JsonUtility.ToJson(json));
                ScreenResponse details = JsonUtility.FromJson<ScreenResponse>(response);
                stream = new Stream(details.width, details.height, details.port, details.receiverPipeline);
            } 
        }
        else
        {
            Debug.Log("Debug");
            // Debug path
            stream = new Stream(1920, 1080, 0, "");
        }


        ApplyScale(stream.width, stream.height);

        Debug.Log("Set resolution to: " + stream.width + "x" + stream.height);

        if(direction == Direction.CENTER)
        {
            CreateButtons();
        }

        gameObject.SetActive(true);

        if(!Settings.Instance.debug)
        {
            GetComponentInChildren<GStreamer>().Begin(stream.width, stream.height, stream.port, stream.pipeline);
        }
    }

    public void CreateButtons()
    {
        // recreate buttons with new scale
        List<Direction> openDirections = ScreenManager.Instance.GetOpenDirections();
        Debug.Log(openDirections.Count);
        foreach (var kv in relativePositions)
        {
            Direction dir = kv.Key;

            if(!openDirections.Contains(dir))
            {
                Debug.Log("No: " + dir);
                continue;
            }

            Vector2 relativePosition = kv.Value;

            CreateScreenButton button = Instantiate(buttonPrefab, buttonsContainer.transform);
            button.direction = dir;

            float halfWidth  = displayQuad.localScale.x / 2f;
            float halfHeight = displayQuad.localScale.y / 2f;

            float paddingX = 1f;
            float paddingY = 0.5f;

            button.transform.localPosition = new Vector3(
                relativePosition.x == 0f ? 0f : Mathf.Sign(relativePosition.x) * (halfWidth + paddingX),
                relativePosition.y == 0f ? 0f : Mathf.Sign(relativePosition.y) * (halfHeight + paddingY),
                0f
            );
            buttons.Add(dir, button);
        }
    }

    public void DestroyButtons()
    {
        foreach (var kvp in buttons)
        {
            Destroy(kvp.Value.gameObject);
        }

        buttons.Clear();
    }

    public void OnGrab()
    {
        if(direction == Direction.CENTER)
        {
            DestroyButtons();
        }
        ScreenManager.Instance.HandleScreenGrab(this);
    }

    public void OnRelease()
    {
        if(direction == Direction.CENTER)
        {
            // TODO: This probably runs twice for two hand grab so lets just destroy buttons again
            DestroyButtons();
            CreateButtons();
        }
        ScreenManager.Instance.ReleaseScreenGrab();
    }
    public void RemoveButton(Direction dir)
    {
        buttons.Remove(dir);
    }
}
