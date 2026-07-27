// Structural attempt: dispatch CONTROL_CLOSE through the selected-building switch.
//
// Retail block B20 loads the widget ID into the switch temporary and the
// out-of-range chain compares it with 0x7800 before reaching this body.
// Moving the body from an early if into this case raised clean state from
// 96.670150% to 98.810940% with 215/215 relocations.
// Matrix: build/match-variants/townmain-close-switch-20260727b

switch (message.payload.widget.id) {
    // Building cases...
    case CONTROL_CLOSE:
        if (!quickView_k)
            SetCommandAndText(message);
        break;
}
