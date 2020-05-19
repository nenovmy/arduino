/*
 * (c) Matey Nenov (https://www.thinker-talk.com)
 *
 * Licensed under Creative Commons: By Attribution 3.0
 * http://creativecommons.org/licenses/by/3.0/
 *
 */

package com.nenoff.connecttoarduinoble;

public class RemoteControl {
    private final static byte START = 0x1;
    private final static byte HEARTBEAT = 0x2;
    private final static byte LED_COMMAND = 0x4;

    private final static byte VALUE_OFF = 0x0;
    private final static byte VALUE_ON = (byte)0xFF;

    private BLEController bleController;

    public RemoteControl(BLEController bleController) {
        this.bleController = bleController;
    }

    private byte [] createControlWord(byte type, byte ... args) {
        byte [] command = new byte[args.length + 3];
        command[0] = START;
        command[1] = type;
        command[2] = (byte)args.length;
        for(int i=0; i<args.length; i++)
            command[i+3] = args[i];

        return command;
    }

    public void switchLED(boolean on) {
        this.bleController.sendData(createControlWord(LED_COMMAND, on?VALUE_ON:VALUE_OFF));
    }

    public void heartbeat() {
        this.bleController.sendData(createControlWord(HEARTBEAT));
    }
}
