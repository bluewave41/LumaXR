using UnityEngine;
using UnityEngine.XR.Interaction.Toolkit;
using UnityEngine.XR.Interaction.Toolkit.Interactables;
using UnityEngine.XR.Interaction.Toolkit.Interactors;

[RequireComponent(typeof(XRSimpleInteractable))]
public class HandActivate : MonoBehaviour
{
    private XRSimpleInteractable interactable;
    private XRDirectInteractor hoveringHand;
    private bool donePinch = false;
    void Start()
    {
        interactable = GetComponent<XRSimpleInteractable>();
    }

    private void OnEnable()
    {
        interactable.hoverEntered.AddListener(OnHoverEnter);
        interactable.hoverExited.AddListener(OnHoverExit);
    }

    private void OnDisable()
    {
        interactable.hoverEntered.RemoveListener(OnHoverEnter);
        interactable.hoverExited.RemoveListener(OnHoverExit);
    }

    private void OnHoverEnter(HoverEnterEventArgs args)
    {
        hoveringHand = args.interactorObject as XRDirectInteractor;
    }

    private void OnHoverExit(HoverExitEventArgs args)
    {
        hoveringHand = null;
    }

    void Update()
    {
        if(hoveringHand == null)
        {
            return;
        }

        bool isPinching = hoveringHand.isSelectActive;

        if(isPinching && !donePinch)
        {
            interactable.activated.Invoke(new ActivateEventArgs { interactorObject = hoveringHand});
            donePinch = true;
        }

        if(!isPinching)
        {
            donePinch = false;
        }
    }
}
