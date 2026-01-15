using System.Collections.Generic;
using UnityEngine;
using UnityEngine.Events;
using UnityEngine.XR.Interaction.Toolkit;
using UnityEngine.XR.Interaction.Toolkit.Interactables;
using UnityEngine.XR.Interaction.Toolkit.Interactors;

[RequireComponent(typeof(XRGrabInteractable))]
public class TwoHandResize : MonoBehaviour
{
    private readonly HashSet<XRBaseInteractor> grabbers = new();
    private XRGrabInteractable grabInteractable;
    private float initialHandsDistance;
    private Vector3 initialQuadScale;
    public Transform primitiveTransform;
    public UnityEvent OnTwoHandGrabStart;

    void Awake()
    {
        grabInteractable = GetComponent<XRGrabInteractable>();
        grabInteractable.selectEntered.AddListener(OnGrab);
        grabInteractable.selectExited.AddListener(OnRelease);
    }

    private void LateUpdate()
    {
        if (grabbers.Count < 2)
        {
            return;
        }

        XRBaseInteractor[] hands = new XRBaseInteractor[2];
        grabbers.CopyTo(hands);

        Vector3 hand0Pos = hands[0].attachTransform.position;
        Vector3 hand1Pos = hands[1].attachTransform.position;

        float currentDistance = Vector3.Distance(hand0Pos, hand1Pos);
        float scaleFactor = currentDistance / initialHandsDistance;

        scaleFactor = Mathf.Clamp(scaleFactor, 0.2f, 5f);

        primitiveTransform.localScale = initialQuadScale * scaleFactor;
    }
    public void OnGrab(SelectEnterEventArgs args)
    {
        XRBaseInteractor interactor = args.interactorObject as XRBaseInteractor;
        grabbers.Add(interactor);

        if(grabbers.Count >= 2)
        {
            XRBaseInteractor[] hands = new XRBaseInteractor[2];
            grabbers.CopyTo(hands);

            initialHandsDistance = Vector3.Distance(hands[0].attachTransform.position, hands[1].attachTransform.position);
            initialQuadScale = transform.Find("Display").localScale;
            grabInteractable.trackPosition = false;
            grabInteractable.trackRotation = false;

            OnTwoHandGrabStart.Invoke();
        }

    }
    private void OnRelease(SelectExitEventArgs args)
    {
        XRBaseInteractor interactor = args.interactorObject as XRBaseInteractor;
        grabbers.Remove(interactor);

        if(grabbers.Count == 0)
        {
            grabInteractable.trackPosition = true;
            grabInteractable.trackRotation = true;
        }
    }
}
